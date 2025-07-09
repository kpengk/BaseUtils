#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>

template <typename T>
class blocking_queue {
public:
    using value_type = T;

    explicit blocking_queue() : d_{std::make_unique<queue_private>()} {}

    void enqueue(const T& item) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            d_->elements.push_back(item);
        }
        d_->push_cv.notify_one();
    }

    void enqueue(T&& item) {
        {
            std::unique_lock<std::mutex> lock(d_->mtx);
            d_->elements.push_back(std::move(item));
        }
        d_->push_cv.notify_one();
    }

    void dequeue(T& popped_item) {
        std::unique_lock<std::mutex> lock(d_->mtx);
        d_->push_cv.wait(lock, [this] { return !d_->elements.empty(); });
        popped_item = std::move(d_->elements.front());
        d_->elements.pop_front();
    }

    // try to dequeue item. if no item found. wait up to timeout and try again
    // Return true, if succeeded dequeue item, false otherwise
    template <typename Rep, typename Period>
    bool dequeue_for(T& popped_item, const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(d_->mtx);
        if (!d_->push_cv.wait_for(lock, timeout, [this] { return !d_->elements.empty(); })) {
            return false;
        }
        popped_item = std::move(d_->elements.front());
        d_->elements.pop_front();
        return true;
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(d_->mtx);
        return d_->elements.empty();
    }

    std::size_t size() {
        std::unique_lock<std::mutex> lock(d_->mtx);
        return d_->elements.size();
    }

    void clear() {
        std::unique_lock<std::mutex> lock(d_->mtx);
        d_->elements.clear();
    }

private:
    struct queue_private {
        std::mutex mtx;
        std::condition_variable push_cv;
        std::deque<T> elements;
    };

    std::unique_ptr<queue_private> d_;
};
