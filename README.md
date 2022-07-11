# wobble

wobble is a utility for scheduling tasks. It reads configuration in JSON
that specifies global configuration and points to task configuration files,
and runs them, writing a status report to a JSON file. The program is
supposed to work on Windows (MinGW64) and POSIX.

## Installation

On Windows, the program requires the [IANA timezone database](https://www.iana.org/time-zones)
to be installed in ~/Downloads with timezone mapping from
[Unicode CLDR](https://cldr.unicode.org/index/downloads). The repository includes
it in /tzdata.zip, so you can run the following in PowerShell under the root to install:

```powershell
Expand-Archive tzdata.zip -DestinationPath $HOME\Downloads\tzdata
```

Binaries for Windows will be published for releases. To run wobble on POSIX, you will
need to compile it from source:

```shell
make
```

## Usage

Specify the path to your configuration file when running:

```shell
wobble config.json
```

The configuration must be in JSON, and comments are accepted. wobble needs the following keys:

- `nproc`: int, number of tasks allowed to run at the same time.
- `report`: path to report JSON output.
- `logs.enable_file`: whether to log to a file.
- `logs.path`: path to log file.
- `logs.enable_syslog`: whether to log to syslog. This will be ignored on Windows.
- `logs.syslog_name`: name of application when logging to syslog.
- `logs.verbosity`: logging verbosity. Can be "error", "fatal", "info" or "warning".
- `rand`: int, randomness for time of start for jobs. Jobs will be delayed by a random amount of time
chosen uniformly in the range of (0, rand) ms. The default is 0.
- `jobs`: list of paths to job configuration files.

An example for a configuration file:

```json
{
    "nproc": 4,
    "report": "rep.json",
    "rand": 0.5,
    "jobs": [
        "job1.json",
        "job2.json",
        "job3.json",
        "job4.json"
    ],
    "logs": {
        "enable_file": true,
        "path": "logs.log",
        "verbosity": "info",
        "enable_syslog": true,
        "syslog_name": "test"
    }
}

```

wobble needs the following keys for a job configuration:

- `exe`: command to execute or a path to one.
- `rel`: optional bool. When `exe` is a relative path, this must be true.
- `name`: name of job. Names cannot be the same for multiple jobs.
- `cycle`: number of seconds of a cycle of execution.
- `delay`: number of seconds of the delay from wobble starting and this job being
executing.
- `args`: optional. When this is present, arguments for the executable. If this is
not, the only argument will be path to the configuration.

An example:

```json
{
    "exe": "ping",
    "cycle": 10,
    "name": "job1",
    "args": [
        "127.0.0.1"
    ]
}

```

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
