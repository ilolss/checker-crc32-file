#include "../include/thread_pool.hpp"
#include <memory>
#include <utility>
#include "../include/base_task.hpp"

PlatformV::ThreadPool::ThreadPool() : is_stop_(false) {
    const size_t count_threads = std::thread::hardware_concurrency();
    threads_.reserve(count_threads);
    for (int i = 0; i < count_threads; i++) {
        threads_.emplace_back([this]() {
            while (!is_stop_) {
                std::unique_ptr<BaseTask> task_ptr;
                if (!tasks_.try_pop(task_ptr)) {
                    continue;
                }
                task_ptr->operator()();
            }
        });
    }
}

void PlatformV::ThreadPool::submit(std::unique_ptr<BaseTask> task_ptr) {
    tasks_.push(std::move(task_ptr));
}

PlatformV::ThreadPool::~ThreadPool() {
    is_stop_ = true;
    for (auto &thread : threads_) {
        thread.join();
    }
}