#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_

#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include "base_task.hpp"
#include "thread_safe_queue.hpp"

namespace PlatformV {
class ThreadPool {
public:
    ThreadPool();

    void submit(std::unique_ptr<BaseTask> task_ptr);

    ~ThreadPool();

private:
    ThreadSafeQueue<std::unique_ptr<BaseTask>> tasks_;
    std::atomic<bool> is_stop_;
    std::vector<std::thread> threads_;
};
}  // namespace PlatformV

#endif  // THREAD_POOL_HPP_