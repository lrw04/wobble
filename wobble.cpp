#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#define FMT_HEADER_ONLY 1
#include <fmt/format.h>

#include "config.h"
#include "job.h"
#include "process.hpp"
#include "report.h"
#include "schedule.h"

void run_job(Job job, Report& rep) {
    // run job.exec with argument job.cfg
    TinyProcessLib::Process proc(
        std::vector<std::string>{job.exe.string(), job.cfg.string()},
        job.cfg.parent_path().string(),
        [](const char* bytes, size_t n) {
            // TODO: logging stdout
        },
        [](const char* bytes, size_t n) {
            // TODO: logging stderr
        });
    rep.update(job.name, JobStatus::RUNNING, proc.get_id(), -1);
    auto sta = proc.get_exit_status();
    rep.update(job.name, sta ? JobStatus::FAILED : JobStatus::WAITING, -1, sta);
}

#define USAGE()                                                        \
    do {                                                               \
        std::cerr << "\tWobble " << VER << std::endl;                  \
        std::cerr << "\tUsage: " << argv[0] << " CONFIG" << std::endl; \
        exit(1);                                                       \
    } while (false)

int main(int argc, char** argv) {
    // TODO: add logging
    // TODO: graceful exit
    if (argc != 2) USAGE();
#undef USAGE

    auto cfg = Config::from_file(argv[1]);
    std::vector<Job> jobs;
    for (auto j : cfg.jobs)
        jobs.push_back(
            Job::from_file(std::filesystem::path(argv[1]).parent_path() / j));
    std::chrono::steady_clock clk;
    JobQueue q(cfg.rand);
    for (auto j : jobs) q.insert(j, clk.now());

    std::vector<std::thread> ts;
    Report r(std::filesystem::absolute(
        std::filesystem::path(argv[1]).parent_path() / cfg.report));
    for (auto j : jobs) r.reg(j);

    while (true) {
        auto jo = q.top();
        std::this_thread::sleep_until(jo.second);
        auto j = jo.first;

        // cleanup ended tasks
        for (auto it = ts.begin(); it != ts.end();) {
            if (!it->joinable()) {
                it = ts.erase(it);
            } else {
                it++;
            }
        }
        q.reenter();
        if (ts.size() == (std::size_t)cfg.nproc) {
            // skip
            r.update(j.name, JobStatus::SKIPPED, -1, -1);
            continue;
        }
        // run
        ts.emplace_back(run_job, j, std::ref(r));
        ts.back().detach();
    }
}
