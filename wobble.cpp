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
    reproc_log_sink(const std::string& job, const std::string& str)
        : job(job), str(str) {}
    std::error_code operator()(reproc::stream stream, const uint8_t* buffer,
                               std::size_t size) {
        if (size) {
            while (buffer[size - 1] == '\n' || buffer[size - 1] == '\r') size--;
            LOG_F(INFO, "Message from %s of %s: %s", str.c_str(), job.c_str(),
                  std::string(reinterpret_cast<const char*>(buffer), size)
                      .c_str());
        }
        return {};
    }

   private:
    std::string job, str;
};

void run_job(Job job, Report& rep) {
    LOG_F(INFO, "Starting %s", job.name.c_str());
    loguru::set_thread_name((job.name + " runner").c_str());

    // run job.exec with argument
    reproc::options op;
    std::string wd = job.cfg.parent_path().string();
    op.working_directory = wd.c_str();
    reproc::process proc;
    std::vector<std::string> cmdline = {job.exe};
    if (job.use_args) {
        for (const auto& arg : job.args) cmdline.push_back(arg);
    } else {
        cmdline.push_back(job.cfg.string());
    }
    auto ec = proc.start(cmdline, op);
    if (ec == std::errc::no_such_file_or_directory) {
        LOG_F(ERROR, "Executable for %s was not found", job.name.c_str());
        rep.update(job.name, JobStatus::NF, -1, -1);
        return;
    }
    LOG_F(INFO, "Started %s", job.name.c_str());
    rep.update(job.name, JobStatus::RUNNING, proc.pid().first, -1);
    ec = reproc::drain(proc, reproc_log_sink(job.name, "stdout"),
                       reproc_log_sink(job.name, "stderr"));
    if (ec) {
        LOG_F(ERROR, "Failed to intercept streams for %s: %s", job.name.c_str(),
              ec.message().c_str());
        rep.update(job.name, JobStatus::FAILED, -1, -1);
        return;
    }

    int sta = 0;
    std::tie(sta, ec) = proc.wait(reproc::infinite);
    if (ec) {
        LOG_F(ERROR, "%s failed: %s", job.name.c_str(), ec.message().c_str());
        rep.update(job.name, JobStatus::FAILED, -1, -1);
    } else if (sta) {
        LOG_F(ERROR, "%s failed with return code %d", job.name.c_str(), sta);
        rep.update(job.name, JobStatus::FAILED, -1, sta);
    } else {
        LOG_F(INFO, "%s succeeded", job.name.c_str());
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

    Report r(cfg.report, cfg.nproc);
    for (auto j : jobs) r.reg(j);

    while (true) {
        auto jo = q.top();
        std::this_thread::sleep_until(jo.second);
        auto j = jo.first;

        q.reenter();
        if (!r.can_run(j.name)) {
            // skip
            LOG_F(INFO, "Skipped %s", j.name.c_str());
            r.update(j.name, JobStatus::SKIPPED, -1, -1);
            continue;
        }
        // run
        std::thread t(run_job, j, std::ref(r));
        r.add_running_job(j.name);
        t.detach();
    }
}
