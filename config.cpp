#include "config.h"

#include <nlohmann/json.hpp>
#include <thread>
#include <unordered_map>

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
        r = nlohmann::json::parse(s, nullptr, true, true);
    } catch (const std::exception& e) {
        ABORT_F("Failed to read configuration: %s", e.what());
    }

    Config res;
    try {
        CHECK_F((!r.contains("nproc")) || r.at("nproc").is_number_unsigned(),
                "'nproc' is not an unsigned integer");
        res.nproc = r.value("nproc", std::thread::hardware_concurrency());

        CHECK_F(r.contains("report") && r.at("report").is_string(),
                "'report' is missing or not a string");
        res.report = std::filesystem::path(f.parent_path() / r.at("report"));

        CHECK_F(r.contains("logs") && r.at("logs").is_object(),
                "'logs' is missing or not an object");
        CHECK_F(r.at("logs").contains("enable_file") &&
                    r.at("logs").at("enable_file").is_boolean(),
                "'logs.enable_file' is missing or not boolean");
        res.do_log_file = r.at("logs").at("enable_file");

        if (res.do_log_file) {
            CHECK_F(r.at("logs").contains("path") &&
                        r.at("logs").at("path").is_string(),
                    "'logs.path' is missing or not a string");
            res.logs = std::filesystem::path(f.parent_path() /
                                             r.at("logs").at("path"));
        }

        CHECK_F(r.at("logs").contains("enable_syslog") &&
                    r.at("logs").at("enable_syslog").is_boolean(),
                "'logs.enable_syslog' is missing or not boolean");
        res.do_syslog = r.at("logs").at("enable_syslog");

        if (res.do_syslog) {
            CHECK_F(r.at("logs").contains("syslog_name") &&
                        r.at("logs").at("syslog_name").is_string(),
                    "'logs.syslog_name' is missing or not a string");
            res.syslog_name = r.at("logs").at("syslog_name");
        }
        if (res.do_log_file || res.do_syslog) {
            CHECK_F(r.at("logs").contains("verbosity") &&
                        r.at("logs").at("verbosity").is_string(),
                    "'logs.verbosity' is missing or not a string");
            std::string verbosity = r.at("logs").at("verbosity");
            if (!stov.count(verbosity))
                ABORT_F("Invalid verbosity: %s", verbosity.c_str());
            res.verbosity = stov[verbosity];
        }

        CHECK_F((!r.contains("rand")) || r.at("rand").is_number_unsigned(),
                "'rand' is not an unsigned integer");
        res.rand = r.value("rand", 0);
        for (auto j : r.at("jobs")) {
            CHECK_F(j.is_string(), "Expected string for every job entry");
            res.jobs.push_back(f.parent_path() / j);
        }
    } catch (const nlohmann::json::basic_json::out_of_range& e) {
        ABORT_F("Invalid configuration: %s", e.what());
    }
    return res;
}
