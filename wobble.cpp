#include <fmt/format.h>

#include <argagg.hpp>
#include <chrono>
#include <iostream>
#include <loguru.hpp>
#include <random>
#include <thread>
#include <vector>

#include "config.h"
#include "job.h"
#include "process.hpp"
#include "report.h"
#include "schedule.h"

void run_job(Job job, Report& rep) {
    loguru::set_thread_name((job.name + " runner").c_str());
    LOG_F(INFO, "Started job %s", job.name.c_str());
    // run job.exec with argument job.cfg
    std::vector<std::string> cmdline = {job.exe};
    if (job.use_args) {
        for (const auto& arg : job.args) cmdline.push_back(arg);
    } else {
        cmdline.push_back(job.cfg.string());
    }

    TinyProcessLib::Process proc(
        cmdline, job.cfg.parent_path().string(),
        [](const char* bytes, size_t n) {
            LOG_F(INFO, "message from process stdout: %s",
                  std::string(bytes, n).c_str());
        },
        [](const char* bytes, size_t n) {
            LOG_F(INFO, "message from process stderr: %s",
                  std::string(bytes, n).c_str());
        });
    rep.update(job.name, JobStatus::RUNNING, proc.get_id(), -1);
    auto sta = proc.get_exit_status();
    rep.update(job.name, sta ? JobStatus::FAILED : JobStatus::WAITING, -1, sta);
    if (sta) {
        LOG_F(ERROR, "Job %s failed with return code %d", job.name.c_str(),
              sta);
    } else {
        LOG_F(INFO, "Job %s finished", job.name.c_str());
    }
}

int main(int argc, char** argv) {
    // TODO: graceful exit

    loguru::init(argc, argv);
    argagg::parser arg{{}};
    argagg::parser_results argr;
    try {
        argr = arg.parse(argc, argv);
    } catch (const std::exception& e) {
        ABORT_F("Invalid argument: %s", e.what());
    }

    if (argr.pos.size() > 1) {
        fmt::print("Usage: {} CONFIG\n", argv[0]);
        return EXIT_FAILURE;
    }
    std::string cfg_f;
    if (argr.pos.size()) {
        cfg_f = argr.pos[0];
    } else {
        LOG_F(WARNING, "Defaulting to config file config.json");
        cfg_f = "config.json";
    }
    auto cfg = Config::from_file(cfg_f);
    if (cfg.do_log_file)
        loguru::add_file(cfg.logs.string().c_str(), loguru::Append,
                         cfg.verbosity);
    if (cfg.do_syslog)
        loguru::add_syslog(cfg.syslog_name.c_str(), cfg.verbosity);
    std::vector<Job> jobs;
    for (auto j : cfg.jobs) jobs.push_back(Job::from_file(j));
    std::chrono::steady_clock clk;
    JobQueue q(cfg.rand);
    for (const auto& j : jobs) q.insert(j, clk.now());

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
            LOG_F(INFO, "Skipped job %s", j.name.c_str());
            r.update(j.name, JobStatus::SKIPPED, -1, -1);
            continue;
        }
        // run
        ts.emplace_back(run_job, j, std::ref(r));
        ts.back().detach();
    }
}
