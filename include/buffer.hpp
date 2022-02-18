#pragma once

#include "slice.hpp"

#include <algorithm>
#include <cassert>

class Buffer {
public:
    explicit Buffer(size_t initial_size = 1024, size_t reserved_prepend_size = 8)
        : capacity_(reserved_prepend_size + initial_size)
        , read_index_(reserved_prepend_size)
        , write_index_(reserved_prepend_size)
        , reserved_prepend_size_(reserved_prepend_size) {
        buffer_ = new char[capacity_];
        assert(length() == 0);
        assert(WritableBytes() == initial_size);
        assert(PrependableBytes() == reserved_prepend_size);
    }

    ~Buffer() {
        delete[] buffer_;
        buffer_ = nullptr;
        capacity_ = 0;
    }

    void Swap(Buffer& rhs) {
        std::swap(buffer_, rhs.buffer_);
        std::swap(capacity_, rhs.capacity_);
        std::swap(read_index_, rhs.read_index_);
        std::swap(write_index_, rhs.write_index_);
        std::swap(reserved_prepend_size_, rhs.reserved_prepend_size_);
    }

    // Skip advances the reading index of the buffer
    void Skip(size_t len) {
        if (len < length()) {
            read_index_ += len;
        } else {
            Reset();
        }
    }

    // Retrieve advances the reading index of the buffer
    // Retrieve it the same as Skip.
    void Retrieve(size_t len) {
        Skip(len);
    }

    // Truncate discards all but the first n unread bytes from the buffer
    // but continues to use the same allocated storage.
    // It does nothing if n is greater than the length of the buffer.
    void Truncate(size_t n) {
        if (n == 0) {
            read_index_ = reserved_prepend_size_;
            write_index_ = reserved_prepend_size_;
        } else if (write_index_ > read_index_ + n) {
            write_index_ = read_index_ + n;
        }
    }

    // Reset resets the buffer to be empty,
    // but it retains the underlying storage for use by future writes.
    // Reset is the same as Truncate(0).
    void Reset() {
        Truncate(0);
    }

    // Increase the capacity of the container to a value that's greater
    // or equal to len. If len is greater than the current capacity(),
    // new storage is allocated, otherwise the method does nothing.
    void Reserve(size_t len) {
        if (capacity_ >= len + reserved_prepend_size_) {
            return;
        }

        // TODO add the implementation logic here
        grow(len + reserved_prepend_size_);
    }

    // Make sure there is enough memory space to append more data with length len
    void EnsureWritableBytes(size_t len) {
        if (WritableBytes() < len) {
            grow(len);
        }

        assert(WritableBytes() >= len);
    }

    // Write
public:
    void Write(const void* d, size_t len) {
        EnsureWritableBytes(len);
        memcpy(WriteBegin(), d, len);
        assert(write_index_ + len <= capacity_);
        write_index_ += len;
    }

    void Append(const Slice& str) {
        Write(str.data(), str.size());
    }

    void Append(const char* d, size_t len) {
        Write(d, len);
    }

    void Append(const void* d, size_t len) {
        Write(d, len);
    }

    // Insert content, specified by the parameter, into the front of reading index
    void Prepend(const void* d, size_t len) {
        assert(len <= PrependableBytes());
        read_index_ -= len;
        const char* p = static_cast<const char*>(d);
        memcpy(buffer_ + read_index_, p, len);
    }

    void UnwriteBytes(size_t n) {
        assert(n <= length());
        write_index_ -= n;
    }

    void WriteBytes(size_t n) {
        assert(n <= WritableBytes());
        write_index_ += n;
    }

    //Read
public:
    Slice ToSlice() const {
        return Slice(data(), length());
    }

    std::string ToString() const {
        return std::string(data(), length());
    }

    void Shrink(size_t reserve) {
        Buffer other(length() + reserve);
        other.Append(ToSlice());
        Swap(other);
    }

    // Next returns a slice containing the next n bytes from the buffer,
    // advancing the buffer as if the bytes had been returned by Read.
    // If there are fewer than n bytes in the buffer, Next returns the entire buffer.
    // The slice is only valid until the next call to a read or write method.
    Slice Next(size_t len) {
        if (len < length()) {
            Slice result(data(), len);
            read_index_ += len;
            return result;
        }

        return NextAll();
    }

    // NextAll returns a slice containing all the unread portion of the buffer,
    // advancing the buffer as if the bytes had been returned by Read.
    Slice NextAll() {
        Slice result(data(), length());
        Reset();
        return result;
    }

    std::string NextString(size_t len) {
        Slice s = Next(len);
        return std::string(s.data(), s.size());
    }

    std::string NextAllString() {
        Slice s = NextAll();
        return std::string(s.data(), s.size());
    }

    // ReadByte reads and returns the next byte from the buffer.
    // If no byte is available, it returns '\0'.
    char ReadByte() {
        assert(length() >= 1);

        if (length() == 0) {
            return '\0';
        }

        return buffer_[read_index_++];
    }

    // UnreadBytes unreads the last n bytes returned
    // by the most recent read operation.
    void UnreadBytes(size_t n) {
        assert(n < read_index_);
        read_index_ -= n;
    }

public:
    // data returns a pointer of length Buffer.length() holding the unread portion of the buffer.
    // The data is valid for use only until the next buffer modification (that is,
    // only until the next call to a method like Read, Write, Reset, or Truncate).
    // The data aliases the buffer content at least until the next buffer modification,
    // so immediate changes to the slice will affect the result of future reads.
    const char* data() const {
        return buffer_ + read_index_;
    }

    char* WriteBegin() {
        return buffer_ + write_index_;
    }

    const char* WriteBegin() const {
        return buffer_ + write_index_;
    }

    // length returns the number of bytes of the unread portion of the buffer
    size_t length() const {
        assert(write_index_ >= read_index_);
        return write_index_ - read_index_;
    }

    // size returns the number of bytes of the unread portion of the buffer.
    // It is the same as length().
    size_t size() const {
        return length();
    }

    // capacity returns the capacity of the buffer's underlying byte slice, that is, the
    // total space allocated for the buffer's data.
    size_t capacity() const {
        return capacity_;
    }

    size_t WritableBytes() const {
        assert(capacity_ >= write_index_);
        return capacity_ - write_index_;
    }

    size_t PrependableBytes() const {
        return read_index_;
    }

private:
    void grow(size_t len) {
        if (WritableBytes() + PrependableBytes() < len + reserved_prepend_size_) {
            //grow the capacity
            size_t n = (capacity_ << 1) + len;
            size_t m = length();
            char* d = new char[n];
            memcpy(d + reserved_prepend_size_, buffer_ + read_index_, m);
            write_index_ = m + reserved_prepend_size_;
            read_index_ = reserved_prepend_size_;
            capacity_ = n;
            delete[] buffer_;
            buffer_ = d;
        } else {
            // move readable data to the front, make space inside buffer
            assert(reserved_prepend_size_ < read_index_);
            size_t readable = length();
            memmove(buffer_ + reserved_prepend_size_, buffer_ + read_index_, length());
            read_index_ = reserved_prepend_size_;
            write_index_ = read_index_ + readable;
            assert(readable == length());
            assert(WritableBytes() >= len);
        }
    }

private:
    char* buffer_;
    size_t capacity_;
    size_t read_index_;
    size_t write_index_;
    size_t reserved_prepend_size_;
    static const char kCRLF[];
};
