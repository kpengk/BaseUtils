#pragma once

#include <thread>

class SpinLock {
public:
    explicit SpinLock(std::int32_t count = 1024) noexcept : spin_count_{count}, locked_{false} {}

    bool try_lock() noexcept {
        return !locked_.exchange(true, std::memory_order_acquire);
    }

    void lock() noexcept {
        std::int32_t counter = spin_count_;
        while (!try_lock()) {
            while (locked_.load(std::memory_order_relaxed)) {
                if (counter-- <= 0) {
                    std::this_thread::yield();
                    counter = spin_count_;
                }
            }
        }
    }

    void unlock() noexcept {
        locked_.store(false, std::memory_order_release);
    }

private:
    const std::int32_t spin_count_;
    std::atomic<bool> locked_;
};

class ScopedSpinLock {
public:
    explicit ScopedSpinLock(SpinLock& lock) : lock_{lock} {
        lock_.lock();
    }
    ~ScopedSpinLock() {
        lock_.unlock();
    }
    ScopedSpinLock(const ScopedSpinLock&) = delete;
    ScopedSpinLock& operator=(const ScopedSpinLock&) = delete;

private:
    SpinLock& lock_;
};
