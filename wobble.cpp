#include <iostream>

#include "Yaml.hpp"
#include "config.h"

#define USAGE()                                                        \
    do {                                                               \
        std::cerr << "\tWobble " << VER << std::endl;                    \
        std::cerr << "\tUsage: " << argv[0] << " CONFIG" << std::endl; \
        exit(1);                                                       \
    } while (false)

int main(int argc, char **argv) {
    if (argc != 2) USAGE();
#undef USAGE

    auto cfg = Config::from_file(argv[1]);
    std::cerr << cfg << std::endl;
}
