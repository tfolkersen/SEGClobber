"""
Utility for finding the SEGClobber executable.

- `build_name` is the name of the build directory i.e. "build"
- CLI usage:
        python3 segclobber_path.py
    Or:
        python3 segclobber_path.py build
- Or import the `get_segclobber_path` function

"""
import os
import sys
from pathlib import Path

def get_segclobber_path(build_name):
    assert type(build_name) is str
    this_file = os.path.realpath(__file__)
    project_base = Path(this_file).parent.parent
    return project_base / build_name / "segclobber"

if __name__ == "__main__":
    assert 1 <= len(sys.argv) and len(sys.argv) <= 2

    build_name = "build"

    if len(sys.argv) > 1:
        build_name = sys.argv[1]

    print(get_segclobber_path(build_name))
