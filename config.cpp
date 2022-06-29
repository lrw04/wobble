#include "config.h"

#include <json.hpp>
#include <unordered_map>
#include <thread>

#include "util.h"

static std::unordered_map<std::string, loguru::Verbosity> stov{
    {"error", loguru::Verbosity_ERROR},
    {"fatal", loguru::Verbosity_FATAL},
    {"info", loguru::Verbosity_INFO},
    {"warning", loguru::Verbosity_WARNING}};

Config Config::from_file(const std::filesystem::path& f) {
    std::string s;
    nlohmann::json r;
    try {
        s = read_file(f);
        r = nlohmann::json::parse(s);
    } catch (const std::exception& e) {
        ABORT_F("Failed to read configuration: %s", e.what());
    }

    Config res;
    res.nproc = r.value("nproc", std::thread::hardware_concurrency());
    try {
        res.report = std::filesystem::path(f.parent_path() / r.at("report"));
        res.do_log_file = r.at("logs").at("enable_file");
        if (res.do_log_file) {
            res.logs = std::filesystem::path(f.parent_path() /
                                             r.at("logs").at("path"));
            std::string verbosity = r.at("logs").at("verbosity");
            res.verbosity = stov[verbosity];
        }
        res.do_syslog = r.at("logs").at("enable_syslog");
        if (res.do_syslog) {
            res.syslog_name = r.at("logs").at("syslog_name");
        }
        res.rand = r.value("rand", 0);
        for (auto j : r.at("jobs")) {
            res.jobs.push_back(f.parent_path() / j);
        }
    } catch (const nlohmann::json::basic_json::out_of_range& e) {
        ABORT_F("Invalid configuration: %s", e.what());
    }
    return res;
}
