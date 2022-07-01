#ifndef JOB_H
#define JOB_H

#include <filesystem>
#include <string>
#include <vector>

struct Job {
    std::string exe;
    std::filesystem::path cfg;
    std::string name;
    int cycle;
    std::vector<std::string> args;
    bool use_args;
    static Job from_file(const std::filesystem::path& c);
};

#endif
