#ifndef CHECKSUM_HPP_
#define CHECKSUM_HPP_

#include <atomic>
#include <filesystem>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include "async_task.hpp"
#include "exception.hpp"
#include "get_crc32.hpp"
#include "logger.hpp"
#include "thread_pool.hpp"
#include "thread_safe_map.hpp"
#include "thread_safe_queue.hpp"

namespace fs = std::filesystem;

namespace PlatformV {

class CheckSum {
public:
    CheckSum(const fs::path &dir);
    CheckSum(fs::path &&dir);

    void start();

private:
    fs::path dir_;
    std::vector<fs::path> files_path_;
    ThreadSafeMap checksum_map_;
    ThreadPool thread_pool_;
};
}  // namespace PlatformV

#endif  // CHECKSUM_HPP_