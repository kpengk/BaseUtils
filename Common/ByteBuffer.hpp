#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

namespace glasssix {
    /*!
     * \class ByteBuffer
     *
     * \ingroup Glasssix
     *
     * \brief A byte buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
     *
     * +-------------------+------------------+------------------+
     * | prependable bytes |  readable bytes  |  writable bytes  |
     * |                   |     (CONTENT)    |                  |
     * +-------------------+------------------+------------------+
     * |                   |                  |                  |
     * 0      <=      readerIndex   <=   writerIndex    <=     size
     *
     * \note
     *
     * \author kp
     *
     * \version 1.0
     *
     * \date 2024/03/13
     *
     * Contact: kangpeng@glasssix.com
     *
     */
    class ByteBuffer {
    public:
        static const size_t CheapPrepend = 8;
        static const size_t InitialSize = 1024;

        explicit ByteBuffer(size_t size = InitialSize)
            : buffer_(CheapPrepend + size), reader_index_{CheapPrepend}, writer_index_{CheapPrepend} {
            assert(readable_bytes() == 0);
            assert(writable_bytes() == size);
            assert(prependable_bytes() == CheapPrepend);
        }

        void swap(ByteBuffer& rhs) {
            buffer_.swap(rhs.buffer_);
            std::swap(reader_index_, rhs.reader_index_);
            std::swap(writer_index_, rhs.writer_index_);
        }

        size_t readable_bytes() const {
            return writer_index_ - reader_index_;
        }

        size_t writable_bytes() const {
            return buffer_.size() - writer_index_;
        }

        size_t prependable_bytes() const {
            return reader_index_;
        }

        const char* peek() const {
            return begin() + reader_index_;
        }

        void skip(size_t len) {
            assert(len <= readable_bytes());
            if (len < readable_bytes()) {
                reader_index_ += len;
            } else {
                skip_all();
            }
        }

        template <typename Tv, std::enable_if_t<std::is_integral_v<Tv>>* = nullptr>
        void skip_val() {
            skip(sizeof(Tv));
        }

        void skip_all() {
            reader_index_ = CheapPrepend;
            writer_index_ = CheapPrepend;
        }

        std::string read_all_as_string() {
            return read_as_string(readable_bytes());
        }

        std::string read_as_string(size_t len) {
            assert(len <= readable_bytes());
            std::string result(peek(), len);
            skip(len);
            return result;
        }

        std::string_view to_string_view() const {
            return std::string_view(peek(), readable_bytes());
        }

        void append(std::string_view str) {
            append(str.data(), str.size());
        }

        void append(const char* data, size_t len) {
            ensure_writable_bytes(len);
            std::copy(data, data + len, begin_write());
            has_written(len);
        }

        void append(const void* data, size_t len) {
            append(static_cast<const char*>(data), len);
        }

        void ensure_writable_bytes(size_t len) {
            if (writable_bytes() < len) {
                make_space(len);
            }
            assert(writable_bytes() >= len);
        }

        char* begin_write() {
            return begin() + writer_index_;
        }

        const char* begin_write() const {
            return begin() + writer_index_;
        }

        void has_written(size_t len) {
            assert(len <= writable_bytes());
            writer_index_ += len;
        }

        void unwrite(size_t len) {
            assert(len <= readable_bytes());
            writer_index_ -= len;
        }

        template <typename Tv, std::enable_if_t<std::is_integral_v<Tv>>* = nullptr>
        void append_val(Tv x) {
            append(&x, sizeof x);
        }

        // Require: buf->readableBytes() >= sizeof(Tv)
        template <typename Tv, std::enable_if_t<std::is_integral_v<Tv>>* = nullptr>
        Tv peek_val() const {
            assert(readable_bytes() >= sizeof(Tv));
            Tv x = 0;
            ::memcpy(&x, peek(), sizeof x);
            return x;
        }

        // Require: buf->readableBytes() >= sizeof(Tv)
        template <typename Tv, std::enable_if_t<std::is_integral_v<Tv>>* = nullptr>
        Tv read_val() {
            Tv result = peek_val<Tv>();
            skip_val<Tv>();
            return result;
        }

        template <typename Tv, std::enable_if_t<std::is_integral_v<Tv>>* = nullptr>
        void prepend_val(Tv x) {
            prepend(&x, sizeof x);
        }

        void prepend(const void* data, size_t len) {
            assert(len <= prependable_bytes());
            reader_index_ -= len;
            const char* d = static_cast<const char*>(data);
            std::copy(d, d + len, begin() + reader_index_);
        }

        void shrink(size_t reserve) {
            ByteBuffer other;
            other.ensure_writable_bytes(readable_bytes() + reserve);
            other.append(to_string_view());
            swap(other);
        }

        size_t internal_capacity() const {
            return buffer_.capacity();
        }

    private:
        char* begin() {
            return buffer_.data();
        }

        const char* begin() const {
            return buffer_.data();
        }

        void make_space(size_t len) {
            if (writable_bytes() + prependable_bytes() < len + CheapPrepend) {
                // FIXME: move readable data
                buffer_.resize(writer_index_ + len);
            } else {
                // move readable data to the front, make space inside buffer
                assert(CheapPrepend < reader_index_);
                size_t readable = readable_bytes();
                std::copy(begin() + reader_index_, begin() + writer_index_, begin() + CheapPrepend);
                reader_index_ = CheapPrepend;
                writer_index_ = reader_index_ + readable;
                assert(readable == readable_bytes());
            }
        }

    private:
        std::vector<char> buffer_;
        size_t reader_index_;
        size_t writer_index_;
    };

} // namespace glasssix
