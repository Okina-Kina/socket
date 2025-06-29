#pragma once

#include <deque>

template <typename T>
class Queue
{
public:
	Queue(int size)
		: size_(size)
	{
	}

	bool Put(T&& data) {
		if (size_ <= deque_.size()) {
			return false;
		}
		deque_.emplace_back(std::move(data));
		return true;
	}
	bool Put(const T& data) {
		if (size_ <= deque_.size()) {
			return false;
		}
		deque_.emplace_back(data);
		return true;
	}
	bool Get(T& data) {
		if (deque_.empty()) {
			return false;
		}
		data = std::move(deque_.front());
		deque_.pop_front();
		return true;
	}
	bool Empty() const {
		return deque_.empty();
	}
private:
	int size_;
	std::deque<T> deque_;
};