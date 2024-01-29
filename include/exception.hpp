#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "logger.hpp"

namespace fs = std::filesystem;

namespace PlatformV {
class Exceptions {
public:
    Exceptions(std::string &&message)
        : message_(std::forward<std::string>(message)) {
    }

    virtual std::string get_message() const {
        return message_;
    }

    virtual ~Exceptions() {
    }

protected:
    // fs::path path_;
    std::string message_;
};

class FileNotOpenExceptions : public Exceptions {
public:
    FileNotOpenExceptions(std::string &&message)
        : Exceptions(std::forward<std::string>(message)) {
    }
};

class FileNotFoundExceptions : public Exceptions {
public:
    FileNotFoundExceptions(std::string &&message)
        : Exceptions(std::forward<std::string>(message)) {
    }
};

class ReturnExceptions {};

}  // namespace PlatformV

#endif  // EXCEPTION_HPP_