#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#define FMT_HEADER_ONLY 1
#include "fmt/format.h"
#include "config.h"
#include "job.h"
#include "report.h"
#include "schedule.h"

void run_job(Job job, Report& rep) {
    // TODO: maintain the running job
    // TODO: add PID entry
    rep.update(job.name, JobStatus::RUNNING);
    std::cout << job.name << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    rep.update(job.name, JobStatus::WAITING);
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
    for (auto j : cfg.jobs) jobs.push_back(Job::from_file(j));
    std::chrono::steady_clock clk;
    JobQueue q(cfg.rand);
    for (auto j : jobs) q.insert(j, clk.now());

    std::vector<std::thread> ts;
    Report r(cfg.report);
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
            r.update(j.name, JobStatus::SKIPPED);
            continue;
        }
        // run
        ts.emplace_back(run_job, j, std::ref(r));
        ts.back().detach();
    }
}
