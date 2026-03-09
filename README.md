<p align="center">
  <img src="logo.png" alt="The Citadel System" width="220"/>
</p>

<h1 align="center">The Citadel System</h1>

<p align="center">
  <strong>Operating Systems Project : Year 2025/2026</strong><br/>
  <strong>La Salle Universitat Ramon Llull</strong><br/>
  <strong>Authors:</strong> Anas TAGUI | Abderrahmen MESKINE
</p>

---

## Overview

The Citadel System is a distributed network simulation inspired by the world of Westeros. The project models a decentralized inter-realm communication system where independent realms (Great Houses) can forge alliances and conduct trade through a network of **Maester** processes.

Each Maester process acts as the local agent of a realm  reading its configuration, managing its inventory, and eventually communicating with other realms over the network. The project is developed incrementally across multiple phases, each introducing new OS concepts.

---

## Project Structure

```
OS_Project_citadel/
├── README.md
├── logo.png
└── Phase1/
    ├── Phase1_Design_Report.pdf         # Full design and implementation report
    └── Files/
        ├── main.c                       # Entry point: arg parsing, signal setup, command loop
        ├── main.h
        ├── globals.h                    # Shared constants, system includes, struct definitions
        ├── io_utils.c                   # I/O utilities (read/write syscalls only — no printf/scanf)
        ├── io_utils.h
        ├── config.c                     # maester.dat parser
        ├── config.h
        ├── inventory.c                  # stock.db binary reader
        ├── inventory.h
        ├── commands.c                   # Command parser and dispatcher
        ├── commands.h
        ├── Makefile
        ├── maester.dat                  # Sample configuration file
        ├── stock.db                     # Sample binary inventory database
        ├── Gx_F1.tar                    # Phase 1 submission archive
        ├── gitignore
        └── dragonmont/                  # Maester working directory (trade files output)
            └── gitkeep
```

---

## Build & Run

### Requirements

- GCC with C99 or later
- GNU Make
- Linux / POSIX-compatible environment

### Compilation

```bash
cd Phase1/Files
make
```

The Makefile uses strict flags: `-Wall -Wextra -Werror -pedantic`. The build produces zero warnings.

### Running

```bash
./Maester <config.dat> <stock.db>
```

**Example:**

```bash
cd Phase1/Files
./Maester maester.dat stock.db
```

**Expected output:**

```
Maester of <RealmName> initialized. The board is set.
$
```

### Clean

```bash
make clean
```

---

## File Formats

### `maester.dat` : Configuration File (plain text)

```
<RealmName>
<FolderPath>
<NumEnvoys>
<IP>
<Port>
--- ROUTES ---
<RealmName|DEFAULT> <IP> <Port>
...
```

**Example:**

```
Driftmark
./dragonmont
3
192.168.1.3
9003
--- ROUTES ---
DEFAULT 192.168.1.5 9002
KingsLanding 192.168.1.5 9002
TheVale 192.168.1.3 9002
Driftmark *.*.*.* 0
```

### `stock.db` : Binary Inventory Database

Each record is exactly **108 bytes**, laid out sequentially with no separators:

| Field  | Type    | Size     |
|--------|---------|----------|
| `name` | `char[100]` | 100 bytes |
| `amount` | `int`  | 4 bytes  |
| `weight` | `float` | 4 bytes  |

---

## Command Reference

All commands are **case-insensitive** and validated by exact word count.

### Main Prompt (`$ `)

| Command | Arguments | Phase 1 Behavior |
|---------|-----------|-----------------|
| `LIST REALMS` | — | Lists all realms from the routing table |
| `LIST PRODUCTS` | — | Displays the local inventory table |
| `LIST PRODUCTS <realm>` | realm name | Prints `Command OK` |
| `PLEDGE <realm> <sigil>` | realm, sigil file | Prints `Command OK` |
| `PLEDGE RESPOND <realm> ACCEPT\|REJECT` | realm, decision | Prints `Command OK` |
| `PLEDGE STATUS` | — | Prints `Command OK` |
| `ENVOY STATUS` | — | Prints `Command OK` |
| `START TRADE <realm>` | realm name | Enters trade sub-menu |
| `EXIT` | — | Graceful shutdown |

### Trade Sub-Menu Prompt (`(trade)> `)

Activated by `START TRADE <realm>`. The prompt changes from `$ ` to `(trade)> `.

| Command | Arguments | Behavior |
|---------|-----------|----------|
| `add <product> <amount>` | product name (can be multi-word), amount | Adds items to the trade list |
| `remove <product> <amount>` | product name, amount | Removes items from the trade list |
| `send` | — | Writes trade list to file and exits sub-menu |
| `cancel` | — | Discards the trade list and exits sub-menu |

On `send`, a plain text file named `trade_<realm>.txt` is written to the Maester's working directory (defined in `maester.dat`). Each line contains `<product_name> <amount>`.

**Example trade session:**

```
$ START TRADE TheVale
(trade)> add Myrish Lace 10
(trade)> add Sweetwine 5
(trade)> send
Trade list sent to TheVale.
$
```

---

## Architecture

### Module Dependency Chain

```
main → commands → config / inventory → io_utils → globals
```

Each module only includes headers it directly depends on. `globals.h` is the single point for all system includes and type definitions.

### Key Design Decisions

- **No `printf`/`scanf`**: All I/O is done exclusively through `read()` and `write()` system calls. Integer and float formatting are implemented manually in `io_utils`.
- **Dynamic inventory array**: `malloc`/`realloc` with doubling strategy (initial capacity 16) — the number of products is not known at compile time.
- **Fixed-size route array**: Up to 50 routes, the number of realms in the network is expected to be small and known at startup.
- **Stack-allocated trade list**: The `TradeList` lives only for the duration of the trade sub-menu, so no dynamic memory is needed.
- **SIGINT handling via `sigaction()`**: More reliable than `signal()`. The handler frees all dynamic memory and calls `_exit(0)`. Global config and inventory are accessible to the handler through file-scope static variables.
- **Strict command validation**: Word count is checked for every command. Commands with extra trailing arguments are rejected as `Unknown command`.

### Execution Flow

1. Validate command-line arguments (exactly 2 required)
2. Install `SIGINT` handler via `sigaction()`
3. Parse `maester.dat` into `MaesterConfig`
4. Load `stock.db` into `Inventory`
5. Print initialization message and enter interactive command loop
6. On `EXIT` or `CTRL+C`: free all memory and exit cleanly

---

## Signals & Graceful Shutdown

The process handles `SIGINT` (Ctrl+C) and the `EXIT` command identically:

1. Print farewell message: `The Maester of <realm> signs off. The ravens rest.`
2. Free inventory (`freeInventory()`)
3. Free config (`freeConfig()`)
4. Exit with code 0

No memory leaks are present (verified with Valgrind).

---

## Progress : Where We Are

### Phase 1: *Valar Compilis*  **COMPLETE**

Phase 1 establishes the local foundations of the Maester process. No network communication is implemented at this stage.

**All 16 test cases from the official Phase 1 testing document pass**, including:

| Test Input | Result |
|------------|--------|
| `LIST REALMS` | PASS |
| `PLEDGE TheVale arryn.png` | PASS |
| `PLEDGE RESPOND TheVale ACCEPT` | PASS |
| `PLEDGE RESPOND TheVale REJECT` | PASS |
| `LIST PRODUCTS KingsLanding` | PASS |
| `LIST PRODUCTS` (own inventory) | PASS |
| `START TRADE Driftmark` (sub-menu) | PASS |
| `PLEDGE STATUS` | PASS |
| `ENVOY STATUS` | PASS |
| `EXIT` | PASS |
| `LiSt REAlms` (case-insensitive) | PASS |
| `PLedgE TheVale arryn.png` (extra spaces) | PASS |
| `START TRADE` (missing args) | PASS |
| `CHECK MY REALMS` (unknown command) | PASS |
| `START TRADE TheVale now` (extra args) | PASS |
| `LIST REALMS please` (extra args) | PASS |

### Phase 2 and beyond : Planned

Future phases will introduce:

- **Phase 2**: Multi-process architecture with `fork()`/`exec()`, pipes/sockets, and the Envoy processes
- **Phase 3**: Network communication between realms via sockets; trade files transmitted over the network
- **Phase 4+**: Full distributed Citadel network with pledge/alliance protocol, concurrent envoy management

---

## Authors

| Name | Role |
|------|------|
| **Anas TAGUI** | Development |
| **Abderrahmen MESKINE** | Development |

**Course:** Operating Systems  2025/2026

**Institution:** La Salle Universitat Ramon Llull
**Date:** March 2026
