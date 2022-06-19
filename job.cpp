#include "job.h"

#include <string>

#include "Yaml.hpp"

Job Job::from_file(std::filesystem::path c) {
    Yaml::Node r;
    Yaml::Parse(r, c.c_str());
    Job res;
    res.cfg = c;
    res.exe = r["exe"].As<std::string>();
    return res;
}
