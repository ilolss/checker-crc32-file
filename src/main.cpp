#include <chrono>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <queue>
#include "../include/checksum.hpp"
#include "../include/exception.hpp"
#include "../include/logger.hpp"
#include "../include/thread_pool.hpp"
#include "../include/thread_safe_map.hpp"

namespace fs = std::filesystem;

#include <boost/crc.hpp>  // for boost::crc_32_type

namespace PlatformV {

std::unique_ptr<CheckSum> checksum_ptr;

void handleSIGTERM(int signal) {
    throw ReturnExceptions();
}

void handleSIGUSR1(int signal) {
    checksum_ptr->start();
}

}  // namespace PlatformV

int main(int argc, char *argv[]) {
    fs::path path;
    long long period;

    if (argc > 2) {
        path = fs::path(argv[1]);
        period = std::atoi(argv[2]);
    } else {
        char *value;
        value = std::getenv("DIR_PATH");
        if (value == NULL) {
            PlatformV::Logger log;
            log.write_ERR("Invalid path");
            return EXIT_FAILURE;
        }
        path = fs::path(value);
        value = std::getenv("PERIOD_MILLISECONDS");
        if (value == NULL) {
            PlatformV::Logger log;
            log.write_ERR("Invalid period");
            return EXIT_FAILURE;
        }
        period = std::atoi(value);
    }

    PlatformV::checksum_ptr = std::make_unique<PlatformV::CheckSum>(path);

    try {
        // Ignore the signals
        std::signal(SIGQUIT, SIG_IGN);
        std::signal(SIGINT, SIG_IGN);
        std::signal(SIGHUP, SIG_IGN);
        std::signal(SIGSTOP, SIG_IGN);
        std::signal(SIGCONT, SIG_IGN);

        // Register custom signals handlers
        std::signal(SIGTERM, PlatformV::handleSIGTERM);

        std::signal(SIGUSR1, PlatformV::handleSIGUSR1);

        while (true) {
            PlatformV::checksum_ptr->start();
            std::this_thread::sleep_for(std::chrono::milliseconds(period));
        }
    } catch (const PlatformV::ReturnExceptions &e) {
        return EXIT_SUCCESS;
    }
}
