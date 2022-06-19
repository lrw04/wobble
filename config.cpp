#include "config.h"
#include "Yaml.hpp"

Config Config::from_file(const std::filesystem::path& f) {
    Yaml::Node r;
    Yaml::Parse(r, f.c_str()); // To make lib read the file
    Config res;
    res.nproc = r["nproc"].As<int>();
    res.report = r["report"].As<std::string>();
    res.cycle_s = r["cycle"].As<int>();
    Yaml::Node& jobs = r["jobs"];
    for (auto it = jobs.Begin(); it != jobs.End(); it++) {
        res.jobs.push_back((*it).second.As<std::string>());
    }
    return res;
}

std::ostream& operator<<(std::ostream& s, const Config& c) {
    s << "Run with " << c.nproc << " workers\n";
    s << "Report to " << c.report << "\n";
    s << "Update every " << c.cycle_s << " second(s)\n";
    s << "Jobs:\n";
    for (const auto& j : c.jobs) {
        s << "\t" << j << "\n";
    }
    return s;
}
