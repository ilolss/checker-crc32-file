#ifndef THREAD_SAFE_QUEUE_HPP_
#define THREAD_SAFE_QUEUE_HPP_

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace PlatformV {

#define DEFAULT_CAPACITY 20

template <typename T>
class ThreadSafeQueue {
    using type_name = T;

public:
    void push(type_name &&value) {
        std::unique_lock<std::mutex> lock(mutex_);
        isNotQueueFull_.wait(lock, [this]() {
            return queue_.size() < capacity_;
        });

        queue_.push(std::forward<type_name>(value));

        isNotQueueEmpty_.notify_one();
    }

    void push(const type_name &value) {
        std::unique_lock<std::mutex> lock(mutex_);
        isNotQueueFull_.wait(lock, [this]() {
            return queue_.size() < capacity_;
        });

        queue_.push(value);

        isNotQueueEmpty_.notify_one();
    }

    bool try_pop(type_name &value) {
        std::unique_lock<std::mutex> lock(mutex_);

        if (!isNotQueueEmpty_.wait_for(
                lock, std::chrono::milliseconds(100),
                [this]() { return !queue_.empty(); }
            )) {
            return false;
        }

        value = std::move(queue_.front());

        queue_.pop();
        isNotQueueFull_.notify_one();

        return true;
    }

    void set_stop(bool value) {
        stop_ = value;
    }

    bool is_stop() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return stop_ && queue_.empty();
    }

private:
    std::queue<type_name> queue_;
    mutable std::mutex mutex_;
    std::condition_variable isNotQueueEmpty_;
    std::condition_variable isNotQueueFull_;
    std::atomic<bool> stop_;
    const int capacity_ = DEFAULT_CAPACITY;
};

}  // namespace PlatformV

#endif  // THREAD_SAFE_QUEUE_HPP_