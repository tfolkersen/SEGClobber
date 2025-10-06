# SEGClobber ("Simplest Equal Game" Clobber)
A linear (1D) Clobber solver implementing many optimizations based on
Combinatorial Game Theory.

Taylor Folkersen, Zahra Bashir, Fatemeh Tavakoli, Martin Müller

Enhancements over a basic minimax-based solver:
- Transposition table
- Board normalization (at both the individual subgame, and full board levels)
- Database containing both subgames and sums of games, enabling many
  optimizations
- Static evaluation rules (based on outcome classes and bounds)
- Subgame substitution (replacing sums with their "simplest" equal sums)
    - Uses an enhanced complexity score estimating the number of
      "sensible" (non-strictly-dominated) moves in the game tree of a sum
- Dominated move pruning
- Several move ordering rules
- Speculative subgame removal
- Iterative deepening with a heuristic evaluation function for positions, to
  improve move ordering

For more details about our algorithmic techniques, see our forthcoming paper,
"SEGClobber - A Linear Clobber Solver" (TODO properly reference this)

## Basic Usage
To build, go to the `src` directory and run `make`. This will produce a
`segclobber` executable.

Usage format:
```
./segclobber [options] <board> <first player>
```

The board is a contiguous string where each character is one of:
- 'B' black stone
- 'W' white stone
- '.' empty space

The first player is given by one character (either 'B' or 'W').

Example invocation to solve the board `BWBWBW.BBW` for black to play first:
```
./segclobber BWBWBW.BBW B
```

You can change the transposition table size by editing the `TTABLE_BITS` macro
in `src/options.h`. If you uncomment the `PRINT_TTABLE_SIZE` macro, the program
will print the size of the transposition table (in MB) and then quit, right
before when it would normally use `calloc` to allocate memory for it. With the
default 26 bits, the size should be around 5.6 GB.

## SEGClobber CLI Options
`[options]` is 0 or more of the following:
- `--persist` Read input from stdin (until EOF), instead of the command line.
  Much faster for solving multiple positions. Avoids startup cost between
  runs, and doesn't clear the transposition table between runs.
- `--no-delete-dominated` Don't prune dominated moves.
- `--no-links` Don't substitute groups of subgames with "simpler" groups of
  subgames from the database.
- `--no-delete-subgames` No speculative subgame removal.
- `--altdb` Instead of using database3.bin, use database3_alt.bin. The latter's
  subgame substitutions are based on complexity score 3 instead of complexity
  score 4 (see paper for details). Essentially, complexity score 3 only counts
  the immediate sensible moves of a sum, whereas complexity score 4 counts both
  immediate sensible moves, and sensible moves in the game
  tree which are reachable by only playing sensible moves.
- `--no-id` Don't use iterative deepening.
- `--altmove` Root node move ordering for (BW)^n and black first player. At the
  root node, plays 12-13 (on 0-indexed board) first if possible.

## Other Makefile Targets
- `db` Builds the `DBManage4` executable, then runs it to re-compute the
  database files (`database3.bin` and `database3_alt.bin`). May take about
  10-20 minutes.
- `clean` Deletes all `.o` files, and the `segclobber` and `DBManage4`
  executables.
- `test` Uses `runtests.py` to run the game tests in the `src/tests` directory.
  Restarts the `segclobber` executable between runs, so this may take a few minutes
  to complete.
- `ftest` Like the `test` target, but uses `segclobber --persist` to greatly
  speed up run time.

## Other Scripts
- `pattern.py` (See output of `python3 -h pattern.py`) Used to run `segclobber`
  for various purposes:
    - `BW` and `BBW` conjectures
    - Generate linear Clobber test cases for MCGS, another CGT solver which is
    much more general.
    - Run on random boards with various parameters
- `explore.py` (See output of `python3 explore.py -h` and see
  `exploreExample.txt`) Uses `segclobber --persist` to interactively find
  winning moves of given positions. Input is not processed from stdin, but
  rather from a named pipe `inpipe`. Output is colorized.
- `runtests.py` Runs game tests from `src/tests` directory. A test is a single
  line in the format `<board> <first player> <expected winning player>`.
- `diagram.py` and `conjecture_diagram.py` Diagram image generators (using
  matplotlib) for our ACG 2025 paper.

## Known Bugs
The makefile doesn't track dependencies, so sometimes you may need to `make
clean` before rebuilding, if you edit the source code.

## Future Work
Some potential future improvements:
- Smaller transposition table entries, allowing for more entries
- Database: make more compact, and generate for larger boards
- Parallelization of search
- Improve game substitution, based on a better understanding of avoiding
  cycles (in search) resulting from substitution
- Better heuristic evaluation function
- Dynamically generate database entries for subgames during search
- Use bounds for some kind of alpha-beta-like pruning
