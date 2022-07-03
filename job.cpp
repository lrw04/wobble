#include "job.h"

#include <loguru.hpp>
#include <nlohmann/json.hpp>
#include <string>

#include "util.h"

Job Job::from_file(const std::filesystem::path& c) {
    std::string s;
    nlohmann::json r;
    try {
        s = read_file(c);
        r = nlohmann::json::parse(s, nullptr, true, true);
    } catch (const std::exception& e) {
        ABORT_F("Failed to read job configuration: %s", e.what());
    }

    Job res;
    res.cfg = c;
    try {
        if (r.contains("rel")) {
            CHECK_F(r.at("rel").is_boolean(), "Expected boolean 'rel'");
            if (r.at("rel")) res.exe = (c.parent_path() / r.at("exe")).string();
        } else {
            res.exe = r.at("exe");
        }
        res.name = r.at("name");
        res.cycle = r.at("cycle");
        res.use_args = false;
        if (r.contains("args")) {
            CHECK_F(r.at("args").is_array(), "Expected array 'args'");
            res.use_args = true;
            for (const auto& arg : r.at("args")) res.args.push_back(arg);
        }
    } catch (const nlohmann::json::basic_json::out_of_range& e) {
        ABORT_F("Invalid job configuration: %s", e.what());
    }
    return res;
}
