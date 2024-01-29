#ifndef ASYNC_TASK_HPP_
#define ASYNC_TASK_HPP_

#include <filesystem>
#include <functional>
#include <future>
#include <string>
#include "base_task.hpp"
#include "exception.hpp"
#include "get_crc32.hpp"
#include "logger.hpp"
#include "thread_safe_map.hpp"

namespace fs = std::filesystem;

namespace PlatformV {

template <typename T = void>
class AsyncTask : public BaseTask {
public:
    AsyncTask(AsyncTask &&) = default;
    AsyncTask &operator=(AsyncTask &&) = default;

    template <typename Function, typename... Args>
    AsyncTask(Function &&function, Args &&...args)
        : function_(std::bind(
              std::forward<Function>(function),
              std::forward<Args>(args)...
          )) {
    }

    std::future<T> get_future() {
        return promise_.get_future();
    }

    void operator()() override {
        promise_.set_value(run());
    }

    ~AsyncTask() override {
    }

protected:
    AsyncTask() = default;

    virtual T run() {
        return function_();
    }

private:
    std::promise<T> promise_;
    std::function<T()> function_;
};

template <>
class AsyncTask<void> : public BaseTask {
public:
    AsyncTask(AsyncTask &&) = default;
    AsyncTask &operator=(AsyncTask &&) = default;

    template <class Function, class... Args>
    AsyncTask(Function &&function, Args &&...args)
        : function_(std::bind(
              std::forward<Function>(function),
              std::forward<Args>(args)...
          )) {
    }

    std::future<void> get_future() {
        return promise_.get_future();
    }

    void operator()() override {
        run();
        promise_.set_value();
    }

    ~AsyncTask() override {
    }

protected:
    AsyncTask() = default;

    virtual void run() {
        function_();
    }

private:
    std::promise<void> promise_;
    std::function<void()> function_;
};

struct Message {
    bool is_ok;
    unsigned int etalon_crc32;
    unsigned int result_crc32;
    fs::path path;
    std::string status;
};

class Task : public AsyncTask<Message> {
public:
    Task(fs::path &&path, const ThreadSafeMap &checksum_map)
        : path_(std::forward<fs::path>(path)), checksum_map_(checksum_map) {
    }

    Task(const fs::path &path, const ThreadSafeMap &checksum_map)
        : path_(path), checksum_map_(checksum_map) {
    }

protected:
    Message run() override {
        unsigned int checksum;
        unsigned int true_checksum;
        try {
            checksum = get_crc32(path_);
            if (!checksum_map_.get(path_, true_checksum)) {
                return Message(false, 0, checksum, path_, "NEW");
            }
            if (true_checksum != checksum) {
                return Message(false, true_checksum, checksum, path_, "FAIL");
            }
            return Message(true, true_checksum, checksum, path_, "OK");
        } catch (const Exceptions &e) {
            return Message(false, 0, 0, path_, e.get_message());
        } catch (const std::exception &e) {
            Logger log;
            log.write_ERR(e.what());
        }
        return Message(false, 0, 0, path_, "SOMETHING WENT WRONG");
    }

private:
    fs::path path_;
    const ThreadSafeMap &checksum_map_;
};
}  // namespace PlatformV

#endif  // ASYNC_TASK_HPP_