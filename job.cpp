#include "job.h"

#include <json.hpp>
#include <loguru.hpp>
#include <string>

#include "util.h"

Job Job::from_file(const std::filesystem::path& c) {
    std::string s;
    nlohmann::json r;
    try {
        s = read_file(c);
        r = nlohmann::json::parse(s);
    } catch (const std::exception& e) {
        ABORT_F("Failed to read job configuration: %s", e.what());
    }

    Job res;
    res.cfg = c;
    try {
        res.exe = std::filesystem::path(r.at("exe"));
        res.name = r.at("name");
        res.cycle = r.at("cycle");
    } catch (const nlohmann::json::basic_json::out_of_range& e) {
        ABORT_F("Invalid job configuration: %s", e.what());
    }
    return res;
}
