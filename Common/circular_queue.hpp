#pragma once

#include <cassert>
#include <vector>

/// <summary>
/// Non-thread-safe cyclic queue based on std::vector.
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T>
class circular_queue {
public:
    using value_type = T;

    /// <summary>
    /// disabled queue with no elements allocated at all
    /// </summary>
    circular_queue() = delete;

    explicit circular_queue(std::size_t max_items)
        : max_items_(max_items + 1)
        , head_{0} // Keep an item as a marker for the full vec_.
        , tail_{0}
        , overrun_counter_{0}
        , vec_(max_items_) {
        assert(max_items_ > 0);
    }

    circular_queue(const circular_queue&) = default;
    circular_queue& operator=(const circular_queue&) = default;

    circular_queue(circular_queue&& other) noexcept { copy_moveable(std::move(other)); }

    circular_queue& operator=(circular_queue&& other) noexcept {
        copy_moveable(std::move(other));
        return *this;
    }

    /// <summary>
    /// push, overrun (oldest) item if no room left.
    /// </summary>
    /// <param name="item"></param>
    void push(const T& item) {
        vec_[tail_] = item;
        tail_ = (tail_ + 1) % max_items_;

        if (tail_ == head_) // overrun last item if full
        {
            head_ = (head_ + 1) % max_items_;
            ++overrun_counter_;
        }
    }

    /// <summary>
    /// push, overrun (oldest) item if no room left.
    /// </summary>
    /// <param name="item"></param>
    void push(T&& item) {
        vec_[tail_] = std::move(item);
        tail_ = (tail_ + 1) % max_items_;

        if (tail_ == head_) // overrun last item if full
        {
            head_ = (head_ + 1) % max_items_;
            ++overrun_counter_;
        }
    }

    /// <summary>
    /// Return reference to the front item. If there are no elements in the container, the behavior is undefined.
    /// </summary>
    /// <returns>item</returns>
    const T& front() const { return vec_[head_]; }

    T& front() { return vec_[head_]; }

    /// <summary>
    /// Return number of elements actually stored.
    /// </summary>
    /// <returns></returns>
    size_t size() const {
        if (tail_ >= head_) {
            return tail_ - head_;
        } else {
            return max_items_ - (head_ - tail_);
        }
    }

    /// <summary>
    /// // Return const reference to item by index. Index is out of range 0…size()-1, the behavior is undefined.
    /// </summary>
    /// <param name="i">Index</param>
    /// <returns>item</returns>
    const T& at(size_t i) const {
        assert(i < size());
        return vec_[(head_ + i) % max_items_];
    }

    /// <summary>
    /// Pop item from front. If there are no elements in the container, the behavior is undefined.
    /// </summary>
    void pop() { head_ = (head_ + 1) % max_items_; }

    bool empty() const { return tail_ == head_; }

    bool full() const {
        // head is ahead of the tail by 1
        return ((tail_ + 1) % max_items_) == head_;
    }

    std::size_t overrun_counter() const { return overrun_counter_; }

    void clear() {
        head_ = 0;
        tail_ = 0;
        overrun_counter_ = 0;
    }

private:
    // copy from other&& and reset it to disabled state
    void copy_moveable(circular_queue&& other) noexcept {
        max_items_ = other.max_items_;
        head_ = other.head_;
        tail_ = other.tail_;
        overrun_counter_ = other.overrun_counter_;
        vec_ = std::move(other.vec_);

        // put &&other in disabled, but valid state
        other.max_items_ = 0;
        other.head_ = other.tail_ = 0;
        other.overrun_counter_ = 0;
    }

private:
    const std::size_t max_items_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t overrun_counter_;
    std::vector<T> vec_;
};
