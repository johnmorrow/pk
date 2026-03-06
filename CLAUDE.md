# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What is pk?

`pk` is a C99 command-line field extraction utility — a middle ground between `cut` and `awk` for selecting columns from input streams. It handles variable whitespace, quoted/escaped delimiters, empty fields, field ranges, and exclude patterns.

## Build Commands

This is an autotools (autoconf/automake) project. Requires `argp.h` (GNU argp).

On macOS: `brew install autoconf automake argp-standalone`, then pass Homebrew paths to configure:
```shell
./configure CFLAGS="-I/opt/homebrew/include" LDFLAGS="-L/opt/homebrew/lib"
```

```shell
# Bootstrap (generate configure script)
./autogen

# Standard build
./configure && make

# Debug build (strict warnings + -Werror)
./debug configure
./debug make

# Run tests
make test
# Or with debug build:
./debug test
```

The `debug` script builds into `bin/debug/` and passes strict compiler flags (`-Wall -Werror -Wextra` etc.).

## Testing

Tests are system/integration tests in `test/system/`. Each test case is a triplet:
- `<name>.cmd` — shell command invoking `$PK` with flags
- `<name>.in` — input fed via stdin
- `<name>.out` — expected output

The `test/system/run` script iterates all `.cmd` files, compares output via md5/md5sum. To run a single test manually:

```shell
cd test/system && . <name>.cmd < <name>.in
```

Set `PK` env var to point to the binary if not using the default `../../src/pk` path.

## Architecture

The pipeline in `main.c` is: **configuration → tokenizer → fieldprinter**.

- **configuration** (`configuration.c/h`) — Parses CLI args via argp into a `CONFIGURATION` struct. Also handles the `PK_EXCLUDES` environment variable.
- **tokenizer** (`tokenizer.c/h`) — Splits input lines into a `STRINGLIST` of tokens. Handles delimiters, quoting, backslash escaping, empty fields, trimming, and exclude string removal.
- **fieldprinter** (`fieldprinter.c/h`) — Takes parsed field specs (indices, ranges, literal strings) and a token list, prints selected fields to stdout.
- **stringlist** (`stringlist.c/h`) — Dynamic string array used throughout for token lists and field specs.
- **wrappers** (`wrappers.c/h`) — Checked wrappers around malloc/fopen/etc that abort on failure.
- **compat** (`compat.c/h`) — Portability shims for `getline` and `asprintf` on systems lacking them.

Types are typedef'd as uppercase names (e.g., `TOKENIZER`, `STRINGLIST`, `CONFIGURATION`, `FIELDPRINTER`). The opaque struct pattern is used for tokenizer and fieldprinter (public typedef, private struct definition).
