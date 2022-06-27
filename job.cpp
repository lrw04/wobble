#include "job.h"

#include <json.hpp>
#include <string>

#include "util.h"

Job Job::from_file(const std::filesystem::path& c) {
    auto s = read_file(c);
    auto r = nlohmann::json::parse(s);
    Job res;
    res.cfg = c;
    res.exe = std::filesystem::path(r.at("exe"));
    res.name = r.at("name");
    res.cycle = r.at("cycle");
    return res;
}
