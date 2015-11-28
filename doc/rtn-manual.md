RandomTest.net
==============

Building
--------

The following build commands are supported:

- make: builds Java and C/C++ probes
- make doc: builds HTML documentation in doc subdirectory
- make test: runs tests of the software (some special setup might be needed,
  though)

randomtest-server.php
---------------------

This is a PHP script that has the following duties:

- collects crash reports from devices
  - **?stacktrace=...&version=...** - (POST) adds stacktrace to the database
    (multiple reports are merged into one), version information is preserved
    there
- allows to reset it's state
  - **?reset=<PASSWORD>** - (GET) request database reset, PASSWORD is embedded into
    the script (to restrict state change operation to a trusted list of
    persons / programs)
- presents aggregated report of the crashes
  - no arguments passed

randomtest-probe.c
------------------

This is a probe for C, C++ family of languages. The following environment
variables are recognized:

- RANDOMTEST\_FILE: the file where crash stacktraces must be appended,
  optional
- RANDOMTEST\_URL: the URL of randomtest-server.php where crash reports
  should be sent
- RANDOMTEST\_VERSION: the version of the software (string) that is currently
  tested, it's used in the stacktrace aggretor to collect version
  information, optional (if not set: ampty version is used)


