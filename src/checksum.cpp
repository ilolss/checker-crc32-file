#include "../include/checksum.hpp"

PlatformV::CheckSum::CheckSum(const fs::path &dir) : dir_(dir) {
    const size_t thread_count = std::thread::hardware_concurrency();
    ThreadSafeQueue<fs::path> queue;
    std::vector<std::thread> threads_;
    threads_.reserve(thread_count);

    threads_.emplace_back([&]() {
        for (const auto &entry : fs::directory_iterator(dir_)) {
            queue.push(entry.path());
            files_path_.push_back(entry.path());
        }

        queue.set_stop(true);
    });

    for (size_t i = 0; i < thread_count; ++i) {
        threads_.emplace_back([this, &queue]() {
            while (!queue.is_stop()) {
                fs::path path;
                if (!queue.try_pop(path)) {
                    continue;
                }
                try {
                    unsigned int x = get_crc32(path);
                    checksum_map_.add(path, x);
                } catch (const Exceptions &e) {
                    Logger log;
                    log.write_ERR(
                        "Integrity check: FAIL (\"" + path.string() + "\" - " +
                        e.get_message() + ")"
                    );
                } catch (const std::exception &e) {
                    Logger log;
                    log.write_ERR(e.what());
                }
            }
        });
    }

    for (auto &thread : threads_) {
        thread.join();
    }
}

PlatformV::CheckSum::CheckSum(fs::path &&dir)
    : dir_(std::forward<fs::path>(dir)) {
    const size_t thread_count = std::thread::hardware_concurrency();
    ThreadSafeQueue<fs::path> queue;
    std::vector<std::thread> threads_;
    threads_.reserve(thread_count);

    threads_.emplace_back([&]() {
        for (const auto &entry : fs::directory_iterator(dir_)) {
            queue.push(entry.path());
            files_path_.push_back(entry.path());
        }

        queue.set_stop(true);
    });

    for (size_t i = 0; i < thread_count; ++i) {
        threads_.emplace_back([this, &queue]() {
            while (!queue.is_stop()) {
                fs::path path;
                if (!queue.try_pop(path)) {
                    continue;
                }
                try {
                    unsigned int x = get_crc32(path);
                    checksum_map_.add(path, x);
                } catch (const Exceptions &e) {
                    Logger log;
                    log.write_ERR(
                        "Integrity check: FAIL (\"" + path.string() + "\" - " +
                        e.get_message() + ")"
                    );
                } catch (const std::exception &e) {
                    Logger log;
                    log.write_ERR(e.what());
                }
            }
        });
    }

    for (auto &thread : threads_) {
        thread.join();
    }
}

void PlatformV::CheckSum::start() {
    std::vector<std::future<Message>> futures;
    std::unordered_map<fs::path, bool> used_path;

    for (const auto &path : files_path_) {
        if (used_path[path] == true) {
            continue;
        }
        std::unique_ptr<Task> task_ptr =
            std::make_unique<Task>(path, checksum_map_);

        futures.push_back(task_ptr->get_future());
        thread_pool_.submit(std::move(task_ptr));

        used_path[path] = true;
    }

    for (const auto &entry : fs::directory_iterator(dir_)) {
        if (used_path[entry.path()] == true) {
            continue;
        }
        std::unique_ptr<Task> task_ptr =
            std::make_unique<Task>(entry.path(), checksum_map_);

        futures.push_back(task_ptr->get_future());
        thread_pool_.submit(std::move(task_ptr));

        used_path[entry.path()] = true;
    }

    bool is_ok = true;

    for (auto &task : futures) {
        Message message = task.get();
        if (message.is_ok == true) {
            continue;
        }
        is_ok = false;
        if (message.status == "FAIL") {
            Logger log;
            log.write_INFO(
                "Integrity check: FAIL (\"" + message.path.string() + "\" - <" +
                std::to_string(message.etalon_crc32) + ", " +
                std::to_string(message.result_crc32) + ">)"
            );
        } else {
            Logger log;
            log.write_INFO(
                "Integrity check: FAIL (\"" + message.path.string() + "\" - " +
                message.status + ")"
            );
        }
    }

    if (is_ok) {
        Logger log;
        log.write_INFO("Integrity check OK");
    }
}
