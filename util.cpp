#include "util.h"

#include <fmt/format.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

std::string read_file(const std::filesystem::path& p) {
    std::ifstream st(p);
    if (!st) {
        throw std::invalid_argument(
            fmt::format("failed to read file {}", p.string()));
    }
    std::stringstream s;
    s << st.rdbuf();
    return s.str();
}

void write_file(const std::filesystem::path& p, const std::string& s) {
    std::ofstream st(p);
    if (!st) {
        throw std::invalid_argument(
            fmt::format("failed to write file {}", p.string()));
    }
    st << s;
}
