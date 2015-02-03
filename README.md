# Quidnuncd

![Build Status](https://travis-ci.org/jbcrail/quidnuncd.png)

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
