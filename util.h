#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <string>

std::string read_file(const std::filesystem::path& p);
void write_file(const std::filesystem::path& p, const std::string& s);

#endif
