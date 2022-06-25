#include "config.h"
#include "json.hpp"
#include "util.h"

Config Config::from_file(const std::filesystem::path& f) {
    auto s = read_file(f);
    auto r = nlohmann::json::parse(s);
    Config res;
    res.nproc = r.value("nproc", 1);
    res.report = std::filesystem::path(r.at("report"));
    res.rand = r.value("rand", 0);
    for (auto j : r.at("jobs")) {
        res.jobs.push_back(j);
    }
    return res;
}

std::ostream& operator<<(std::ostream& s, const Config& c) {
    s << "Run with " << c.nproc << " workers\n";
    s << "Report to " << c.report << "\n";
    s << "Jobs:\n";
    for (const auto& j : c.jobs) {
        s << "\t" << j << "\n";
    }
    return s;
}
