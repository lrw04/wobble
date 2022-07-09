# wobble

wobble is a utility for scheduling tasks. It reads configuration in JSON
that specifies global configuration and points to task configuration files,
and runs them, writing a status report to a JSON file. The program is
supposed to work on Windows (MinGW64) and POSIX.

## Installation

On Windows, the program requires the [IANA timezone database](https://www.iana.org/time-zones)
to be installed in ~/Downloads with timezone mapping from
[Unicode CLDR](https://cldr.unicode.org/index/downloads).

## License

The program is licensed under the MIT License. The codebase also includes these
opensource projects in-tree:

- [argagg](https://github.com/vietjtnguyen/argagg) 0.4.6 in /argagg licensed under MIT
- [date.h](https://github.com/HowardHinnant/date) 3.0.1 in /date
licensed under MIT
- [loguru](https://github.com/emilk/loguru) master in /loguru
in public domain, changed

and the following will be fetched during configuration:

- [fmtlib](https://github.com/fmtlib/fmt) 8.1.1 licensed under MIT
- [nlohmann/json](https://github.com/nlohmann/json) 3.10.5
licensed under MIT
- [reproc](https://github.com/DaanDeMeyer/reproc) 14.2.4
licensed under MIT
