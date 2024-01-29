#include "../include/logger.hpp"

PlatformV::Logger::Logger() {
    openlog("checker_crc32_file", LOG_PID, LOG_USER);
}

void PlatformV::Logger::write_INFO(const std::string &message) const {
    syslog(LOG_INFO, message.c_str());
}

void PlatformV::Logger::write_ERR(const std::string &message) const {
    syslog(LOG_ERR, message.c_str());
}

PlatformV::Logger::~Logger() {
    closelog();
}