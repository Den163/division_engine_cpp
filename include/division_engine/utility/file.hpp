#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <division_engine/core/exception.hpp>

namespace division_engine::utility::file
{
template<typename PathType>
std::string read_text(PathType& path)
{
    std::ifstream file { path };
    if (!file.is_open())
    {
        throw core::Exception { "Failed to open a file at path " + std::string { path } };
    }

    std::ostringstream output {};

    output << file.rdbuf();

    return output.str();
}
}