#include "report.h"

#include <fmt/format.h>

#include <loguru.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>

#include "util.h"

Report::Report(const std::filesystem::path& p, int np) {
    f = p;
    nproc = np;
}

void Report::reg(const Job& job) {
    const std::lock_guard<std::mutex> lock(m);
    if (cont.count(job.name)) {
        ABORT_F("Job names collided between %s and %s",
                cont[job.name].job.cfg.string().c_str(),
                job.cfg.string().c_str());
    }
    cont[job.name] = {job, JobStatus::WAITING, clk.now(), -1};
    rn[job.name] = false;
}

void Report::update(const std::string& name, JobStatus st, int p, int ret) {
    const std::lock_guard<std::mutex> lock(m);
    if (!cont.count(name)) {
        ABORT_F("Job not registered: %s", name.c_str());
    }
    if (st != JobStatus::RUNNING && cont[name].status == JobStatus::RUNNING) {
        running--;
        rn[name] = false;
    }
    cont[name].status = st;
    cont[name].update = clk.now();
    cont[name].pid = p;
    cont[name].ret = ret;
    nlohmann::json r;
    for (const auto& [k, v] : cont) {
        r[k] = nlohmann::json::object();
        r[k]["name"] = cont[k].job.name;
        r[k]["status"] = status_str[cont[k].status];
        r[k]["pid"] = cont[k].pid;
        r[k]["return_code"] = cont[k].ret;
        r[k]["update"] = std::chrono::round<std::chrono::milliseconds>(
                             cont[k].update.time_since_epoch())
                             .count();
    }
    write_file(f, r.dump() + "\n");
}

void Report::add_running_job(const std::string& name) {
    running++;
    rn[name] = true;
}

bool Report::can_run(const std::string& name) {
    return running < nproc && (!rn[name]);
}
