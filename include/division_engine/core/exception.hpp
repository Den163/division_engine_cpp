#pragma once

#include <exception>
#include <string>

namespace division_engine::core
{
class Exception : public std::exception
{
public:
    Exception(const std::string& message)
      : _message(message)
    {
    }

    virtual ~Exception() throw() {}

    const char* what() const throw() override { return _message.c_str(); }

private:
    std::string _message;
};
}