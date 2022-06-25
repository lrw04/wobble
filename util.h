#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <filesystem>

std::string read_file(const std::filesystem::path& p);
void write_file(const std::filesystem::path& p, const std::string& s);

#endif
