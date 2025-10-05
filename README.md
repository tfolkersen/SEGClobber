# SEGClobber ("Simplest Equal Game" Clobber)
A linear (1D) Clobber solver using Combinatorial Game Theory.

## Basic Usage
Go to the `src` directory. To build, run `make`. This will produce a
`segclobber` executable.

Usage format:
```
./segclobber [options] <board> <first player>
```

The board is a contiguous string where each character is one of:
- 'B' black stone
- 'W' white stone
- '.' empty space

The first player is either 'B' or 'W'.

Example invocation to solve the board `BWBWBW.BBW` for black to play first:
```
./segclobber BWBWBW.BBW B
```

You can change the transposition table size by editing the `TTABLE_BITS` macro
in `src/options.h`. If you uncomment the `PRINT_TTABLE_SIZE` macro, the program
will print the size of the transposition table (in MB) and then quit, right
before where it would normally use `calloc` to allocate memory for it. With the
default 26 bits, the size should be around 5.6 GB.


## CLI Options
Options are:
- `--persist` Read input from stdin (until EOF), instead of the command line.
  Much faster for solving more than 1 position. Avoids startup cost between
  runs, and doesn't clear the transposition table between runs.
- `--altmove` Root node move ordering for (BW)^n and black first player. At the
  root node, plays 12-13 (on 0-indexed board) first if possible.
- `--no-id` Don't use iterative deepening.
- `--altdb` Instead of using database3.bin, use database3_alt.bin. The latter's
  subgame substitutions are based on 
- `--no-links` Don't substitute groups of subgames with "simpler" groups of
  subgames from the database.
- `--no-delete-subgames` No speculative subgame removal.
- `--no-delete-dominated` Don't prune dominated moves.

## Other Scripts

## Other Makefile Targets


## Known Bugs
The makefile doesn't track dependencies, so sometimes you may need to `make
clean` before rebuilding, if you edit the source code.
