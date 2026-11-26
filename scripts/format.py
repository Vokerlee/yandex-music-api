#!/usr/bin/env python3
"""Shared local/CI formatting. Only tracked first-party source files are selected."""

import argparse
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
HOOK = '#!/bin/sh\n# yandex-music-format-hook\nexec python3 scripts/format.py --check\n'


def tracked_files(root=ROOT, explicit=None):
    output = subprocess.check_output(['git', 'ls-files', '-z'], cwd=root)
    tracked = set(output.decode().split('\0')) - {''}
    selected = tracked if explicit is None else set(explicit)
    if selected - tracked:
        raise ValueError('Files must be tracked paths relative to the repository root: '
                         + ', '.join(sorted(selected - tracked)))
    return sorted(p for p in selected if (root / p).is_file())




def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--files', nargs='+')
    args = parser.parse_args()
    files = tracked_files(explicit=args.files)
    pins = dict(line.strip().split('==') for line in
                (ROOT / 'requirements-dev.txt').read_text().splitlines() if '==' in line)
    groups = {
        'clang-format': [p for p in files if pathlib.Path(p).suffix in {'.c', '.h', '.cpp', '.hpp'}],
        'gersemi': [p for p in files if p.endswith('.cmake') or pathlib.Path(p).name == 'CMakeLists.txt'],
    }
    for tool, selected in groups.items():
        if not selected:
            continue
        version = subprocess.check_output([tool, '--version'], text=True)
        if not re.search(r'(?<!\d)' + re.escape(pins[tool]) + r'(?!\d)', version):
            raise ValueError(f'{tool} must be {pins[tool]}; install requirements-dev.txt')
        if tool == 'clang-format':
            options = ['--dry-run', '--Werror'] if args.check else ['-i']
        else:
            options = ['--check'] if args.check else ['--in-place']
        subprocess.run([tool, *options, *selected], cwd=ROOT, check=True)


if __name__ == '__main__':
    try:
        main()
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
