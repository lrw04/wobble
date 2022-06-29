# wobble

wobble is a utility for scheduling tasks. It reads configuration in JSON
that specifies global configuration and points to task configuration files,
and runs them, writing a status report to a JSON file. The program is
supposed to work on Windows (MSYS2) and POSIX.

## Installation

On Windows, the program requires the [IANA timezone database](https://www.iana.org/time-zones)
to be installed in ~/Downloads with timezone mapping from
[Unicode CLDR](https://cldr.unicode.org/index/downloads).

## License

The program is licensed under the MIT License. The codebase also includes these
opensource projects in-tree:

- [fmtlib](https://github.com/fmtlib/fmt) 8.1.1 in /include/fmt licensed under MIT
- [date.h](https://github.com/HowardHinnant/date) 3.0.1 in /include/date and /date
licensed under MIT
- [nlohmann/json](https://github.com/nlohmann/json) 3.10.5 as /include/json.hpp
licensed under MIT
- [tiny-process-library](https://gitlab.com/eidheim/tiny-process-library) 2.0.4 as
/include/process.hpp and /tiny-process-library licensed under MIT
- [argagg](https://github.com/vietjtnguyen/argagg) 0.4.6 as /argagg.hpp licensed under MIT
- [loguru](https://github.com/emilk/loguru) master as /include/loguru.hpp and in /loguru
in public domain, changed
