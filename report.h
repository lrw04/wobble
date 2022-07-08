#ifndef REPORT_H
#define REPORT_H

#include <chrono>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "job.h"

enum JobStatus { RUNNING, WAITING, SKIPPED, FAILED, NF };
const std::vector<std::string> status_str = {"running", "waiting", "skipped",
                                             "failed", "not-found"};

struct JobEntry {
    Job job;
    JobStatus status;
    std::chrono::time_point<std::chrono::system_clock> update;
    int pid, ret;
};

struct Report {
    std::filesystem::path f;
    std::mutex m;
    std::map<std::string, JobEntry> cont;
    std::chrono::system_clock clk;

    Report(const std::filesystem::path& p);
    void reg(const Job& job);
    void update(const std::string& name, JobStatus st, int p, int ret);
    void clear();
};

#endif
