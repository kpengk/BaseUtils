#pragma once
#include <cassert>
#include <vector>

/*!
 * \class RecentlyBuffer
 *
 * \ingroup Glasssix
 *
 * \brief Cache a fixed length of recent data.
 *
 * The total size of the cache is fixed, and the length of the most recent data that needs to be cached is fixed.
 * And ensure that each access is continuous memory data.
 *
 * +------------------+------------------+
 * |  readable datas  |  writable datas  |
 * |     (CONTENT)    |                  |
 * +------------------+------------------+
 * |                  |                  |
 * 0      <=     writerIndex    <=     size
 *
 * \note
 *
 * \author kp
 *
 * \version 1.0
 *
 * \date 2024/03/14
 *
 * Contact: kangpeng@glasssix.com
 *
 */

template <typename Ty>
class RecentlyBuffer {
public:
    using element_type = Ty;
    using value_type = typename std::remove_cv<Ty>::type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = element_type*;
    using const_pointer = const element_type*;
    using reference = element_type&;
    using const_reference = const element_type&;
    using iterator = element_type*;
    using const_iterator = const element_type*;

    explicit RecentlyBuffer(size_t size = 1024, size_t capacity = SIZE_MAX) : cache_size_{size}, writer_index_{} {
        resize_cache(size, capacity);
    }

    inline std::size_t cache_size() const {
        return cache_size_;
    }

    inline void resize_cache(size_t size, size_t capacity = SIZE_MAX) {
        assert(capacity > size);
        cache_size_ = size;
        reserve(capacity == 0 || capacity == SIZE_MAX ? size * 1.5 : capacity);
    }

    inline void reserve(std::size_t n) {
        assert(n > cache_size_);
        buffer_.resize(n);
        buffer_.shrink_to_fit();
        if (writer_index_ > n) {
            writer_index_ = n;
        }
    }

    inline std::size_t capacity() const {
        return buffer_.size();
    }

    inline std::size_t size() const {
        return writer_index_;
    }

    inline bool empty() const {
        return writer_index_ == 0;
    }

    inline void clear() {
        writer_index_ = 0;
    }

    inline size_t readable() const {
        return writer_index_;
    }

    inline size_t writable() const {
        return buffer_.size() - writer_index_;
    }

    inline void erase_back(size_t n) {
        if (writer_index_ >= n) {
            writer_index_ -= n;
        } else {
            writer_index_ = 0;
        }
    }

    inline void push_back(const Ty& val) {
        ensure_writable(1);
        buffer_[writer_index_] = val;
        ++writer_index_;
    }

    inline void push_back(Ty&& val) {
        ensure_writable(1);
        buffer_[writer_index_] = std::move(val);
        ++writer_index_;
    }

    constexpr const_reference front() const {
        assert(!empty());
        return *data();
    }

    constexpr const_reference back() const {
        assert(!empty());
        return *(data() + (size() - 1));
    }

    constexpr const_iterator begin() const noexcept {
        return data();
    }

    constexpr iterator begin() noexcept {
        return data();
    }

    constexpr const_iterator end() const noexcept {
        return data() + size();
    }

    constexpr iterator end() noexcept {
        return data() + size();
    }

    constexpr const_pointer data() const noexcept {
        return buffer_.data();
    }

    constexpr pointer data() noexcept {
        return buffer_.data();
    }

    constexpr const_reference operator[](const size_t pos) const noexcept {
        return buffer_[pos];
    }

    constexpr reference operator[](const size_t pos) noexcept {
        return buffer_[pos];
    }

    static constexpr size_t move_count(size_t n, size_t cache, size_t capacity) {
        return n <= capacity ? 0 : (n - cache) / ((capacity - cache) + 1);
    }

private:
    void ensure_writable(size_t len) {
        assert(len < buffer_.size());
        if (writable() < len) {
            move_space();
        }
    }

    void move_space() {
        // move readable data to the front, make space inside buffer
        const size_t move_len = cache_size_ - 1;
        const size_t valid_start = writer_index_ - move_len;
        for (size_t i = 0; i < move_len; ++i) {
            buffer_[i] = std::move(buffer_[valid_start + i]);
        }
        writer_index_ = move_len;
    }

    size_t cache_size_;
    std::vector<Ty> buffer_;
    size_t writer_index_;
};
