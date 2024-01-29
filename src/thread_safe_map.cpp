#include "../include/thread_safe_map.hpp"

void PlatformV::ThreadSafeMap::add(const fs::path &path, unsigned int value) {
    std::lock_guard<std::mutex> lock(mutex_);
    map_[path] = value;
}

bool PlatformV::ThreadSafeMap::get(const fs::path &path, unsigned int &value)
    const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = map_.find(path);

    if (it == map_.end()) {
        return false;
    }

    value = it->second;
    return true;
}