#pragma once
#include <cstdlib>
#include <memory>
#include <cassert>

class CircularBuffer
{
public:
	CircularBuffer(size_t capacity)
		: buffer_{ new char[capacity] }
		, capacity_{ capacity }
		, read_index_{}
		, data_len_{}
	{
	}


	~CircularBuffer()
	{
		delete[] buffer_;
	}

	bool write(const char* data, size_t len) {
		if (len > capacity_ - data_len_ || data == nullptr) {
			return false;
		}

		// copy data to the end
		const size_t back_index = (read_index_ + data_len_) % capacity_;
		if (const size_t back_len = capacity_ - back_index; len > back_len) {
			memcpy(buffer_ + back_index, data, back_len);
			memcpy(buffer_, data + back_len, len - back_len);
		}
		else {
			memcpy(buffer_ + back_index, data, len);
		}
		data_len_ += len;
		return true;
	}

	bool read(char* data, size_t len) {
		if (len > data_len_ || data == nullptr) {
			return false;
		}

		// copy data
		if (const size_t back_len = capacity_ - read_index_; len > back_len) {
			memcpy(data, buffer_ + read_index_, back_len);
			memcpy(data + back_len, buffer_, len - back_len);
			read_index_ = len - back_len;
		}
		else {
			memcpy(data, buffer_ + read_index_, len);
			read_index_ += len;
		}
		read_index_ = read_index_ >= capacity_ ? 0 : read_index_;
		data_len_ -= len;
		return true;
	}

	bool peek(char* data, size_t len) {
		if (len > data_len_ || data == nullptr) {
			return false;
		}

		// copy data
		if (const size_t back_len = capacity_ - read_index_; len > back_len) {
			memcpy(data, buffer_ + read_index_, back_len);
			memcpy(data + back_len, buffer_, len - back_len);
		}
		else {
			memcpy(data, buffer_ + read_index_, len);
		}
		return true;
	}

	char peek_front() {
		return buffer_[read_index_];
	}

	char peek_back() {
		const size_t index = (read_index_ + data_len_ - 1) % capacity_;
		return buffer_[index];
	}

	void skip(size_t len) {
		if (len < data_len_) {
			if (const size_t back_len = capacity_ - read_index_; len > back_len) {
				read_index_ = len - back_len;
			}
			else {
				read_index_ += len;
			}
			read_index_ = read_index_ >= capacity_ ? 0 : read_index_;
			data_len_ -= len;
		}
		else {
			clear();
		}
	}

	size_t size() {
		return data_len_;
	}

	size_t capacity() const {
		return buffer_ == nullptr ? 0 : capacity_;
	}

	size_t writable_bytes() const {
		return capacity_ - data_len_;
	}

	bool empty() {
		return data_len_ == 0;
	}

	void clear() {
		read_index_ = 0;
		data_len_ = 0;
	}

private:
	char* const buffer_;
	const size_t capacity_;
	size_t read_index_;
	size_t data_len_;
};
