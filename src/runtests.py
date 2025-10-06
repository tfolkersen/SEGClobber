import subprocess
import sys
import os
import pathlib

############################################################ Constants/variables
TEST_DIR = "tests"

COLOR_RED = 31
COLOR_GREEN = 32
COLOR_RESET = 0

COLORS = [
    COLOR_RED,
    COLOR_GREEN,
    COLOR_RESET,
]

reset = False
proc = None

############################################################ Helper functions
def is_valid_format(test_case_field):
    assert type(test_case_field) is str

    if len(test_case_field) == 0:
        return False

    for c in test_case_field:
        if c not in ['.', 'B', 'W']:
            return False

    return True

def run_test_reset_proc(board, player, expected):
    for field in [board, player, expected]:
        assert is_valid_format(field)
    assert len(player) == 1 and len(expected) == 1

    command = f"./segclobber {board} {player}"

    proc = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    result = proc.stdout.readline().decode("UTF-8")
    proc.wait()

    winner = result.split()[0]

    assert type(winner) is str and len(winner) == 1
    return winner == expected

def run_test_persist_proc(board, player, expected):
    for field in [board, player, expected]:
        assert is_valid_format(field)
    assert len(player) == 1 and len(expected) == 1

    global proc

    if proc is None:
        command = "./segclobber --persist"
        proc = subprocess.Popen(
            command.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    proc.stdin.write(f"{board} {player}\n".encode("UTF-8"))
    proc.stdin.flush()

    result = proc.stdout.readline().decode("UTF-8")

    winner = result.split()[0]

    assert type(winner) is str and len(winner) == 1
    return winner == expected


def set_color(color):
    assert color in COLORS and type(color) is int
    print("\x1b[" + str(color) + "m", end="")


############################################################ Main logic
run_test_fn = run_test_reset_proc

assert len(sys.argv) in [1, 2]
if "--persist" in sys.argv:
    run_test_fn = run_test_persist_proc

test_dir = pathlib.Path(TEST_DIR)

tests_passed = 0
tests_failed = 0

for test_filename in test_dir.iterdir():
    print(test_filename)
    assert os.path.isfile(test_filename)

    infile = open(test_filename, "r")
    for line in infile:
        line = line.strip()
        if len(line) == 0:
            continue

        test_case = line.split()
        assert len(test_case) == 3

        board, player, expected = test_case

        result = run_test_fn(board, player, expected)
        assert type(result) is bool

        if result:
            tests_passed += 1
        else:
            tests_failed += 1

        set_color(COLOR_GREEN if result else COLOR_RED)
        result_str = "PASS" if result else "FAIL"

        print(f"{board} {player} {result_str} (expected {expected}) ")
        set_color(COLOR_RESET)
    infile.close()

tests_total = tests_passed + tests_failed
set_color(COLOR_GREEN if (tests_failed == 0) else COLOR_RED)
print("\n" + ("=" * 20))
print(f"Total tests: {tests_total}")
print(f"Tests passed: {tests_passed}")
print(f"Tests failed: {tests_failed}")
set_color(COLOR_RESET)
