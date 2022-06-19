#include <iostream>
#include <random>

#include "Yaml.hpp"
#include "config.h"
#include "job.h"
#include "schedule.h"

#define USAGE()                                                        \
    do {                                                               \
        std::cerr << "\tWobble " << VER << std::endl;                  \
        std::cerr << "\tUsage: " << argv[0] << " CONFIG" << std::endl; \
        exit(1);                                                       \
    } while (false)

int main(int argc, char **argv) {
    // TODO: add logging
    // TODO: handle SIGHUP on POSIX
    if (argc != 2) USAGE();
#undef USAGE

    std::random_device rd;
    std::mt19937_64 rng;
    rng.seed(rd());

    auto cfg = Config::from_file(argv[1]);
    std::uniform_int_distribution<time_t> d(0, cfg.cycle_s);
    JobQueue q;
    for (auto j : cfg.jobs) q.insert(Job::from_file(j), d(rng) + time(nullptr));

    // TODO: main loop
    // TODO: limit parallelism
}
