#ifndef THREAD_SAFE_MAP_HPP_
#define THREAD_SAFE_MAP_HPP_

#include <filesystem>
#include <iostream>
#include <mutex>
#include <unordered_map>

namespace fs = std::filesystem;

namespace PlatformV {
class ThreadSafeMap {
public:
    void add(const fs::path &path, unsigned int value);
    bool get(const fs::path &path, unsigned int &value) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<fs::path, unsigned int> map_;
};
}  // namespace PlatformV

#endif  // THREAD_SAFE_MAP_HPP_