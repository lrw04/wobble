#ifndef CONFIG_H
#define CONFIG_H

#ifndef VER
#define VER "master"
#endif

#include <filesystem>
#include <ctime>
#include <string>
#include <vector>
#include <ostream>

struct Config {
    std::vector<std::filesystem::path> jobs;
    int nproc;
    time_t cycle_s;
    std::filesystem::path report;

    static Config from_file(const std::filesystem::path& f);
};

std::ostream& operator<<(std::ostream& s, const Config& c);

#endif
