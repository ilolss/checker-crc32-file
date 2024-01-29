#ifndef GET_CRC32_HPP_
#define GET_CRC32_HPP_

#include <filesystem>
#include <fstream>
#include "exception.hpp"

namespace fs = std::filesystem;

#include <boost/crc.hpp>

#define BUFFER_SIZE 1024

namespace PlatformV {
inline unsigned int get_crc32(const fs::path &path) {
    if (!fs::exists(path)) {
        throw FileNotFoundExceptions("FILE NOT FOUND");
    }

    boost::crc_32_type result;

    std::ifstream ifs(path, std::ios_base::binary);

    if (ifs.is_open()) {
        do {
            char buffer[BUFFER_SIZE];

            ifs.read(buffer, BUFFER_SIZE);
            result.process_bytes(buffer, ifs.gcount());
        } while (ifs);
    } else {
        throw FileNotOpenExceptions("FILE NOT OPEN");
    }

    return result.checksum();
}
}  // namespace PlatformV

#endif  // GET_CRC32_HPP_