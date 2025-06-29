#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include "Queue.hpp"

class Printer
{
public:
    Printer(int threadCount, int queueSize)
        : isTerminationRequested_(false)
        , queue_(queueSize)
    {
        for (int n = 0; n < threadCount; n++) {
            threads_.emplace_back(std::thread(main_, n));
        }
    }
    ~Printer() {
        {
            std::unique_lock<std::mutex> ul(mutex_);
            isTerminationRequested_ = true;
        }
        cv_.notify_all();
        const int size = threads_.size();
        for (int n = 0; n < size; n++) {
            threads_.at(n).join();
        }
    }
    bool append(std::string&& str) {
        {
            std::unique_lock<std::mutex> ul(mutex_);
            if (!queue_.put(std::move(str))) { return false; }
        }
        cv_.notify_all();
        return true;
    }
    bool append(const std::string& str) {
        {
            std::unique_lock<std::mutex> ul(mutex_);
            if (!queue_.put(str)) { return false; }
        }
        cv_.notify_all();
        return true;
    }
private:
    std::function<void(int)> main_ = [this](int num)
        {
            while (1) {
                std::string str;
                {
                    std::unique_lock<std::mutex> ul(mutex_);
                    while (queue_.empty()) {
                        if (isTerminationRequested_) { return; }
                        cv_.wait(ul);
                    }
                    const bool result = queue_.get(str);
                    assert(result);
                }
                std::cout << "num=" << num << " " << str << std::endl;
            }
        };
    bool isTerminationRequested_;
    Queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<std::thread> threads_;
};