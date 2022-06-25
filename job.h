#ifndef JOB_H
#define JOB_H

#include <filesystem>
#include <string>

struct Job {
    std::filesystem::path exe, cfg;
    std::string name;
    int cycle;
    static Job from_file(const std::filesystem::path& c);
};

#endif
