#pragma once

#include "circular_queue.hpp"

#include <mutex>
#include <condition_variable>

template<typename T>
class bounded_blocking_queue
{
public:
	explicit bounded_blocking_queue(unsigned max_size)
		: queue_(max_size)
	{
	}

	void enqueue(T&& item)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			pop_cv_.wait(lock, [this] { return !this->queue_.full(); });
			queue_.push_back(std::move(item));
		}
		push_cv_.notify_one();
	}

	bool enqueue_for(T&& item, std::chrono::milliseconds wait_duration)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (!pop_cv_.wait_for(lock, wait_duration, [this] { return !this->queue_.full(); }))
			{
				return false;
			}
			queue_.push_back(std::move(item));
		}
		push_cv_.notify_one();

		return true;
	}

	// enqueue immediately. overrun oldest message in the queue if no room left.
	void enqueue_nowait(T&& item)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			queue_.push_back(std::move(item));
		}
		push_cv_.notify_one();
	}

	void dequeue(T& popped_item)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			push_cv_.wait(lock, [this] { return !this->queue_.empty(); });
			popped_item = std::move(queue_.front());
			queue_.pop_front();
		}
		pop_cv_.notify_one();
	}

	// try to dequeue item. if no item found. wait up to timeout and try again
	// Return true, if succeeded dequeue item, false otherwise
	bool dequeue_for(T& popped_item, std::chrono::milliseconds wait_duration)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (!push_cv_.wait_for(lock, wait_duration, [this] { return !this->queue_.empty(); }))
			{
				return false;
			}
			popped_item = std::move(queue_.front());
			queue_.pop_front();
		}
		pop_cv_.notify_one();
		return true;
	}

	bool empty()
	{
		std::scoped_lock lock{ mutex_ };
		return queue_.empty();
	}

	bool full()
	{
		std::scoped_lock lock{ mutex_ };
		return queue_.full();
	}

	std::size_t size()
	{
		std::scoped_lock lock{ mutex_ };
		return queue_.size();
	}

	std::size_t overrun_counter()
	{
		std::scoped_lock lock{ mutex_ };
		return queue_.overrun_counter();
	}

private:
	std::mutex mutex_;
	std::condition_variable push_cv_;
	std::condition_variable pop_cv_;
	circular_queue<T> queue_;
};
