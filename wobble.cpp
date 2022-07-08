#include <fmt/format.h>

#include <argagg.hpp>
#include <chrono>
#include <iostream>
#include <loguru.hpp>
#include <random>
#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>
#include <thread>
#include <vector>

#include "config.h"
#include "job.h"
#include "report.h"
#include "schedule.h"

class reproc_log_sink {
   public:
    reproc_log_sink(const std::string& str) : str(str) {}
    std::error_code operator()(reproc::stream stream, const uint8_t* buffer,
                               std::size_t size) {
        if (size) {
            LOG_F(INFO, "message from %s: %s", str.c_str(),
                  std::string(reinterpret_cast<const char*>(buffer),
                              buffer[size - 1] == '\n' ? size - 1 : size)
                      .c_str());
        }
        return {};
    }

   private:
    std::string str;
};

void run_job(Job job, Report& rep) {
    loguru::set_thread_name((job.name + " runner").c_str());
    LOG_F(INFO, "Started job %s", job.name.c_str());

    // run job.exec with argument
    reproc::options op;
    op.working_directory = job.cfg.parent_path().string().c_str();
    reproc::process proc;
    std::vector<std::string> cmdline = {job.exe};
    if (job.use_args) {
        for (const auto& arg : job.args) cmdline.push_back(arg);
    } else {
        cmdline.push_back(job.cfg.string());
    }
    auto ec = proc.start(cmdline, op);
    if (ec == std::errc::no_such_file_or_directory) {
        LOG_F(ERROR, "Executable for job %s was not found", job.name.c_str());
        rep.update(job.name, JobStatus::NF, -1, -1);
        return;
    }
    ec = reproc::drain(proc, reproc_log_sink("stdout"),
                       reproc_log_sink("stderr"));
    if (ec) {
        LOG_F(ERROR, "Failed to intercept streams: %s", ec.message().c_str());
        rep.update(job.name, JobStatus::FAILED, -1, -1);
        return;
    }

    int sta = 0;
    std::tie(sta, ec) = proc.wait(reproc::infinite);
    if (ec) {
        LOG_F(ERROR, "Job %s failed: %s", job.name.c_str(),
              ec.message().c_str());
        rep.update(job.name, JobStatus::FAILED, -1, -1);
    } else if (sta) {
        LOG_F(ERROR, "Job %s failed with return code %d", job.name.c_str(),
              sta);
        rep.update(job.name, JobStatus::FAILED, -1, sta);
    } else {
        LOG_F(INFO, "Job %s succeeded", job.name.c_str());
        rep.update(job.name, JobStatus::WAITING, -1, sta);
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

        // TODO: Fix tasks limiting
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
