#include "report.h"

#include <sstream>
#include <stdexcept>

#include "date.h"

#define FMT_HEADER_ONLY 1
#include "fmt/format.h"
#include "json.hpp"
#include "util.h"

using namespace date;

Report::Report(const std::filesystem::path& p) { f = p; }

void Report::reg(const Job& job) {
    const std::lock_guard<std::mutex> lock(m);
    if (cont.count(job.name)) {
        throw std::invalid_argument(
            fmt::format("Job names collided between {} and {}.",
                        cont[job.name].job.cfg.string(), job.cfg.string()));
    }
    cont[job.name] = {job, JobStatus::WAITING, clk.now()};
}

void Report::update(const std::string& name, JobStatus st) {
    const std::lock_guard<std::mutex> lock(m);
    if (!cont.count(name))
        throw std::invalid_argument(fmt::format("Job not registered: {}.", name));
    cont[name].status = st;
    cont[name].update = clk.now();
    nlohmann::json r;
    for (const auto& [k, v] : cont) {
        r[k] = nlohmann::json::object();
        r[k]["name"] = cont[k].job.name;
        r[k]["status"] = status_str[cont[k].status];
        std::stringstream ss;
        ss << cont[k].update;
        r[k]["update"] = ss.str();
    }
    write_file(f, r.dump());
}
