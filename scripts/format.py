#!/usr/bin/env python3
"""Format tracked C/C++ and CMake files with the pinned development tools."""

import argparse
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--check', action='store_true')
args = parser.parse_args()
root = Path(__file__).resolve().parent.parent
files = subprocess.check_output(['git', 'ls-files', '-z'], cwd=root).decode().split('\0')
cpp = [p for p in files if Path(p).suffix in {'.c', '.h', '.cpp', '.hpp'}]
cmake = [p for p in files if p.endswith('.cmake') or Path(p).name == 'CMakeLists.txt']

groups = [
    ('clang-format', ['--dry-run', '--Werror'] if args.check else ['-i'], cpp),
    ('gersemi', ['--check'] if args.check else ['--in-place'], cmake),
]
for tool, options, paths in groups:
    if paths:
        subprocess.run([tool, *options, *paths], cwd=root, check=True)
