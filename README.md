# Quidnuncd

Quidnuncd is a portable, event-driven monitoring server. It was designed for
relatively old Unixes without the modern infrastructure that would allow similar
tools, like nagios or collectd, to be installed. The primary use case is for
internal servers behind a firewall.

Inspired by Redis and beanstalkd, I created an ASCII-encoded and line-oriented
protocol to talk to the server. This makes it trivial to use telnet to gather
statistics about a server without needing a special client.

Written only using ANSI C, Quidnuncd uses [libstatgrab](http://www.i-scream.org/libstatgrab/)
and [libev](http://software.schmorp.de/pkg/libev.html). Both libraries work
seamlessly on all systems that I need to support: Solaris 8+ (Sparc/x64),
RHEL 5+, Ubuntu, OS X, and FreeBSD.

A trivial test on my 2014 MacBook Pro 2.8GHz Quad-core Intel i7 using 10 clients
benchmarked an average 49K requests/sec. This is overkill for my typical usage,
but it at least demonstrates the server performs well.

![Build Status](https://travis-ci.org/jbcrail/quidnuncd.png)

## Installation

It's as simple as:

```
$ ./configure
$ make
$ make test
```

## Usage

To quickly run quidnuncd with the default configuration:

```
$ cd src
$ ./qnd
```

To start quidnuncd on another port, try:

```
$ ./qnd --port 9999
```

To see all available options, try:

```
$ ./qnd --help
Usage: qnd [options]

  --help                       show this help
  --version                    show version information
  --host <address>             listen on address (default is 0.0.0.0)
  --port <int>                 listen on port (default is 3230)
  --heartbeat-interval <int>   issue a heartbeat once every <int> seconds
```

## Interacting with quidnuncd

You can use any telnet client to play with quidnuncd. Start a quidnuncd instance;
then in another terminal, try:

```
$ telnet localhost 3230
PING
pong

TIME
time.secs=1422942771
time.usecs=661028

INFO
server.os_name=Darwin
server.os_release=13.4.0
server.os_platform=x86_64
server.hostname=localhost
server.uptime_in_seconds=22539
server.uptime_in_days=0
server.cpu_max=8
server.cpu_available=8
server.arch_bits=64
time.secs=1422942823
time.usecs=707045

QUIT
Connection closed by foreign host.
```

## Protocol

Command      | Description
------------ | -------------------------------------------
HELP         | List all available commands
INFO fs      | Return metrics for all mounted filesystems
INFO load    | Return metrics for system load
INFO memory  | Return metrics for system memory
INFO network | Return metrics for system network
INFO page    | Return metrics for paging system
INFO stats   | Return metrics for Quidnuncd clients
INFO swap    | Return metrics for swap space
PING         | Return `pong`; meant as a sanity check
QUIT         | Close the current connection
TIME         | Return the system time

Most responses return multiple lines that follow this format: `KEY=VALUE`.
This makes it simple to parse the responses and easier to read for humans.

## Supported platforms

Quidnuncd was compiled and tested on the following OSes:

Architecture | Operating System        | Compiler
------------ | ----------------------- | -----------------------
64-bit x86   | RHEL 5                  | gcc 4.1.2
64-bit x86   | Ubuntu 14.10            | gcc 4.9.1
32-bit x86   | Ubuntu 14.10            | gcc 4.8.2
64-bit x86   | Ubuntu 12.04            | gcc 4.6.3
64-bit x86   | OS X 10.9.5 (Mavericks) | clang 3.5
64-bit x86   | OS X 10.9.5 (Mavericks) | gcc 4.2.1
64-bit x86   | Solaris 10              | gcc 3.4.6
32-bit Sparc | Solaris 8               | gcc 3.4.3
64-bit x86   | FreeBSD 10.1            | clang 3.4.1

## Future work

1. Add more tests using [minunit](https://github.com/siu/minunit).
1. Add Riemann support (in-progress via [nano-riemann](https://github.com/jbcrail/nano-riemann)).
1. Add sequence diagrams to documentation.
1. Collect additional statistics on each command.
1. Associate custom data with a watcher to eliminate hash.
1. Given a resolution, cache statistics to limit the number of system calls.
1. Experiment with fuzzing tools to test security.

## License

MIT. See [LICENSE.md](https://github.com/jbcrail/quidnuncd/blob/master/LICENSE.md).
