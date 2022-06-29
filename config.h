#ifndef CONFIG_H
#define CONFIG_H

#ifndef VER
#define VER "master"
#endif

#include <ctime>
#include <filesystem>
#include <ostream>
#include <string>
#include <vector>
#include <loguru.hpp>

struct Config {
    std::vector<std::filesystem::path> jobs;
    int nproc, rand;
    std::filesystem::path report, logs;
    bool do_log_file, do_syslog;
    loguru::Verbosity verbosity;
    std::string syslog_name;

    static Config from_file(const std::filesystem::path& f);
};

std::ostream& operator<<(std::ostream& s, const Config& c);

#endif
