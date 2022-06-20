#include "report.h"

#include <sstream>
#include <stdexcept>

#include "Yaml.hpp"
#include "date.h"

using namespace date;

Report::Report(const std::filesystem::path& p) { f = p; }

void Report::reg(const Job& job) {
    const std::lock_guard<std::mutex> lock(m);
    cont[job.name] = {job, JobStatus::WAITING, clk.now()};
}

void Report::update(const std::string& name, JobStatus st) {
    const std::lock_guard<std::mutex> lock(m);
    if (!cont.count(name)) throw std::invalid_argument("Job was not registered");
    cont[name].status = st;
    cont[name].update = clk.now();
    // TODO: write file
    Yaml::Node root;
    for (const auto& [k, v] : cont) {
        root[k] = Yaml::Node();
        root[k]["name"] = cont[k].job.name;
        root[k]["status"] = status_str[cont[k].status];
        std::stringstream ss;
        ss << cont[k].update;
        root[k]["update"] = ss.str();
    }
    Yaml::Serialize(root, f.c_str());
}
