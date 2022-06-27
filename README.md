# wobble

wobble is a utility for scheduling tasks. It reads configuration in JSON
that specifies global configuration and points to task configuration files,
and runs them, writing a status report to a JSON file. The program is
supposed to work on Windows and POSIX.

## License

The program is licensed under the MIT License. The codebase also uses these
opensource projects in-tree:

- [fmtlib](https://github.com/fmtlib/fmt) in /fmt licensed under MIT
- [date.h](https://github.com/HowardHinnant/date) as /date.h licensed under MIT
- [nlohmann/json](https://github.com/nlohmann/json) as /json.hpp licensed under MIT
- [tiny-process-library](https://gitlab.com/eidheim/tiny-process-library) as /process* except process_platform.cpp, licensed under MIT
