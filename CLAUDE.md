# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What is pk?

`pk` is a Rust command-line field extraction utility — a middle ground between `cut` and `awk` for selecting columns from input streams. It handles variable whitespace, quoted/escaped delimiters, empty fields, field ranges, and exclude patterns.

## Build Commands

Requires Rust toolchain (`cargo`).

```shell
# Build (debug)
cargo build

# Build (release, optimized + stripped)
cargo build --release

# Run tests
cargo build && test/system/run

# Run a single test manually
cd test/system && PK=../../target/debug/pk . <name>.cmd < <name>.in
```

## Testing

Tests are system/integration tests in `test/system/`. Each test case is a triplet:
- `<name>.cmd` — shell command invoking `$PK` with flags
- `<name>.in` — input fed via stdin
- `<name>.out` — expected output

The `test/system/run` script iterates all `.cmd` files, compares output via md5/md5sum. Set `PK` env var to override the binary path (default: `../../target/debug/pk`).

## Architecture

Single-file implementation in `src/main.rs`. The pipeline is: **CLI parsing → tokenizer → field printer**.

- **CLI / Config** — Uses clap for arg parsing. Optional-value flags (`-E`, `-N`, `-q`) are handled by argv preprocessing before clap, since argp-style "attached value only" semantics aren't natively supported by clap.
- **Tokenizer** (`tokenize()`) — Splits input lines into a `Vec<String>` of tokens. Handles delimiters, quoting, backslash escaping, empty fields, trimming, and exclude string removal.
- **Field Printer** (`print_fields()`) — Takes parsed field specs (indices, ranges, literal strings) and a token list, prints selected fields to stdout via buffered writer.
- **Field Spec** — Enum with `Number(usize)`, `Range(usize, Option<usize>)`, and `Literal(String)` variants.
