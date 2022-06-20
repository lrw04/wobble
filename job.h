#ifndef JOB_H
#define JOB_H

#include <filesystem>
#include <string>

struct Job {
    std::filesystem::path exe, cfg;
    std::string name;
    static Job from_file(std::filesystem::path c);
};

#endif
