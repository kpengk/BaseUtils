#pragma once
#include "circular_queue.hpp"

#include <condition_variable>
#include <mutex>

template <typename T>
class bounded_blocking_queue {
public:
    using value_type = T;

    explicit bounded_blocking_queue(unsigned max_size = 15) : d_{std::make_unique<queue_private>(max_size)} {}

    void enqueue(const T& item) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            d_->pop_cv.wait(lock, [this] { return !d_->elements.full(); });
            d_->elements.push(item);
        }
        d_->push_cv.notify_one();
    }

    void enqueue(T&& item) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            d_->pop_cv.wait(lock, [this] { return !d_->elements.full(); });
            d_->elements.push(std::move(item));
        }
        d_->push_cv.notify_one();
    }

    template <typename Rep, typename Period>
    bool enqueue_for(T&& item, const std::chrono::duration<Rep, Period>& timeout) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            if (!d_->pop_cv.wait_for(lock, timeout, [this] { return !d_->elements.full(); })) {
                return false;
            }
            d_->elements.push(std::move(item));
        }
        d_->push_cv.notify_one();

        return true;
    }

    // enqueue immediately. overrun oldest message in the queue if no room left.
    void enqueue_nowait(T&& item) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            d_->elements.push(std::move(item));
        }
        d_->push_cv.notify_one();
    }

    void dequeue(T& popped_item) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            d_->push_cv.wait(lock, [this] { return !d_->elements.empty(); });
            popped_item = std::move(d_->elements.front());
            d_->elements.pop();
        }
        d_->pop_cv.notify_one();
    }

    // try to dequeue item. if no item found. wait up to timeout and try again
    // Return true, if succeeded dequeue item, false otherwise
    template <typename Rep, typename Period>
    bool dequeue_for(T& popped_item, const std::chrono::duration<Rep, Period>& timeout) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            if (!d_->push_cv.wait_for(lock, timeout, [this] { return !this->d_->elements.empty(); })) {
                return false;
            }
            popped_item = std::move(d_->elements.front());
            d_->elements.pop();
        }
        d_->pop_cv.notify_one();
        return true;
    }

    bool empty() const {
        std::lock_guard lock{d_->mtx};
        return d_->elements.empty();
    }

    bool full() const {
        std::lock_guard lock{d_->mtx};
        return d_->elements.full();
    }

    std::size_t size() const {
        std::lock_guard lock{d_->mtx};
        return d_->elements.size();
    }

    std::size_t overrun_counter() const {
        std::lock_guard lock{d_->mtx};
        return d_->elements.overrun_counter();
    }

    void clear() {
        {
            std::lock_guard lock{d_->mtx};
            d_->elements.clear();
        }
        d_->pop_cv.notify_all();
    }

private:
    struct queue_private {
        explicit queue_private(unsigned max_size) : elements{max_size} {}

        mutable std::mutex mtx;
        std::condition_variable push_cv;
        std::condition_variable pop_cv;
        circular_queue<T> elements;
    };

    std::unique_ptr<queue_private> d_;
};
