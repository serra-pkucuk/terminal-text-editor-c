# Terminal-Based Text Editor

**CSE 232 — Systems Programming | Yeditepe University | Team project**

A command-driven text editor written in C with an `ncurses` terminal interface. The course project uses a fixed-size array of nodes linked by indices to represent the active lines of a text file.

The full team project was completed and submitted. This repository currently contains **Serra Pehlivanküçük's garbage-collection and command-loop module**, a shared header, a build configuration, and a test plan. It is not yet a complete standalone copy of the submitted application.

## Technologies and concepts

- C and `ncurses`
- GNU Make / GCC
- Array-backed doubly linked lists
- Buffer compaction and memory reuse
- Command dispatch and module integration
- Unit, edge-case, and integration test planning

## Design

The editor represents lines using a fixed-capacity array (`MAX_LINES = 100`). Each node stores up to 39 text characters and two indices, `next` and `prev`. The `head` and `tail` indices identify the active list. Deleting a line unlinks its node, but does not immediately reclaim the array slot.

### Garbage collection

The `garbageCollection()` function in `gc_main.c`:

1. Traverses the active list from `head`.
2. Copies reachable nodes to a temporary buffer.
3. Compacts those nodes at the beginning of the main array.
4. Rebuilds `next` / `prev` links and updates `head`, `tail`, and `freeIdx`.
5. Resets the operation counter.

Garbage collection can be requested explicitly with `G`, and the command loop triggers it when the operation threshold or buffer capacity is reached.

### Command interface

| Command | Intended action |
| --- | --- |
| `E` | Open a text file |
| `I` | Insert a line |
| `D` | Delete a line |
| `R` | Replace a character |
| `P` | Print / refresh text |
| `S` | Save |
| `G` | Compact active nodes |
| `Q` | Quit (with confirmation) |

## Repository contents

```text
.
├── gc_main.c       # Garbage collection, command loop, and entry point
├── editor.h        # Shared data types and function declarations
├── Makefile        # Build configuration for the team modules
├── TEST_PLANI.md   # Test scenarios (in Turkish)
└── mytext.txt      # Sample text
```

## Building

The intended Linux build uses GCC and the `ncurses` development headers:

```bash
make
./texteditor
```

**Important:** The current GitHub snapshot is **incomplete**. Its Makefile also references `editor_core.c`, `ui_ncurses.c`, `insert_delete.c`, and `replace_print.c`, which have not been uploaded to this repository. The commands above will not successfully build this snapshot until the missing modules are added, and the code is reconciled with the final submitted version.

## Testing

See [TEST_PLANI.md](TEST_PLANI.md) for proposed test cases covering empty buffers, repeated deletions, buffer capacity, garbage collection, commands, and integration. A test plan is not evidence that all cases have been executed or passed.

## My contribution

**Serra Pehlivanküçük — garbage collection and integration**

- Implemented the array-compaction garbage collector and updated links and buffer indices.
- Wrote the main command-dispatch loop connecting the editor's functionality.
- Prepared test cases and integration considerations for the team project.

The text editor was developed collaboratively; other modules were assigned to other team members.

## Status

**Course project submitted; public repository incomplete.** This repository preserves the current portion of the submission. Completing the public snapshot and verifying a clean build are remaining documentation/repository tasks.
