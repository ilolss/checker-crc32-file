#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <syslog.h>
#include <string>

namespace PlatformV {
class Logger {
public:
    Logger();

    void write_INFO(const std::string &message) const;
    void write_ERR(const std::string &message) const;

    ~Logger();
};
}  // namespace PlatformV

#endif  // LOGGER_HPP_