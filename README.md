# Igbo Programming Language Interpreter

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/your/repo/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

An experimental interpreter for a small programming language written in the Igbo language. The goal of this project is to explore compiler design concepts while promoting cultural preservation through the use of the Igbo language.

## Why Igbo?

Igbo is one of the major languages spoken in Nigeria. This project aims to provide a fun way to learn about interpreters while also celebrating the Igbo language.

## Installation

1. Ensure a C compiler (such as `gcc`) is installed.
2. Clone this repository and change into the directory:
   ```bash
   git clone https://github.com/your/repo.git
   cd igbo-programming-language
   ```
3. Build the interpreter using `make`:
   ```bash
   make
   ```
   This creates an executable named `igbo`.

## Quick Start

Run one of the programs in the `examples/` folder:

```bash
./igbo examples/hello.igbo
```

which prints:

```text
Ndewo, uwa!
```

### More Examples

See the [examples](examples/) directory for additional sample programs written in the language.

## Language Reference

The following keywords are recognised:

| Keyword | Meaning          |
|---------|------------------|
| `dee`   | variable declaration ("write") |
| `gosi`  | print            |
| `ma`    | if               |
| `mana`  | else             |
| `mgbe`  | while            |
| `eziokwu` | boolean `true` |
| `ụgha` | boolean `false` |

Tokens for basic arithmetic operators (`+`, `-`, `*`, `/`) and comparison (`==`, `!=`, `<`, `>`, `<=`, `>=`) are also supported.

## Contributing

Contributions are welcome! Please open issues or pull requests. Ensure code is well formatted and documented.

## License

This project is licensed under the [MIT License](LICENSE).

---
Repository keywords: `programming-language`, `igbo`, `interpreter`, `c`, `cultural-preservation`. Visit our website at <https://example.com> for updates.
