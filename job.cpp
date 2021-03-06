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
            if (r.at("rel")) res.exe = std::filesystem::absolute((c.parent_path() / r.at("exe"))).string();
        } else {
            res.exe = r.at("exe");
        }
        CHECK_F(r.contains("name") && r.at("name").is_string(),
                "'name' is missing or not a string");
        res.name = r.at("name");
        CHECK_F(r.contains("cycle") && r.at("cycle").is_number_unsigned(),
                "'cycle' is missing or not an unsigned integer");
        res.cycle = r.at("cycle");
        CHECK_F((!r.contains("delay")) || r.at("delay").is_number_unsigned(),
                "'delay' is not an unsigned integer");
        res.delay = r.value("delay", 0);
        res.use_args = false;
        if (r.contains("args")) {
            CHECK_F(r.at("args").is_array(), "Expected array 'args'");
            res.use_args = true;
            for (const auto& arg : r.at("args")) {
                CHECK_F(arg.is_string(),
                        "Expected string for every args entry");
                res.args.push_back(arg);
            }
        }
    } catch (const nlohmann::json::basic_json::out_of_range& e) {
        ABORT_F("Invalid job configuration: %s", e.what());
    }
    return res;
}
