#ifndef JOB_H
#define JOB_H

#include <filesystem>

struct Job {
    std::filesystem::path exe, cfg;
    static Job from_file(std::filesystem::path c);
};

#endif
