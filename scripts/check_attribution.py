#!/usr/bin/env python3
"""Validate declared upstream reuse using metadata only; never execute PR code."""

import argparse
import base64
import json
import pathlib
import re
import subprocess
import sys

UPSTREAM = 'MarshalX/yandex-music-api'
TRAILER = 'Co-authored-by: Ilya Siamionau <15520314+MarshalX@users.noreply.github.com>'
MANIFEST = 'third_party/provenance.json'


def check(data):
    declarations = re.findall(r'^Upstream-Reuse:\s*(\S+)\s*$', data['body'], re.MULTILINE)
    if len(declarations) != 1:
        raise ValueError('Supply exactly one Upstream-Reuse: none or MarshalX/yandex-music-api@<40-hex-SHA> declaration')
    manifest = data['provenance']
    if manifest.get('version') != 1 or not isinstance(manifest.get('adaptations'), list):
        raise ValueError('Unsupported provenance manifest')
    files = set(data['files'])
    adaptations = manifest['adaptations']
    for entry in adaptations:
        if entry.get('repository') != UPSTREAM or not re.fullmatch(r'[0-9a-f]{40}', entry.get('revision', '')):
            raise ValueError('Invalid pinned upstream identity')
        for key in ('source_paths', 'native_paths'):
            paths = entry.get(key)
            if not isinstance(paths, list) or not paths:
                raise ValueError(f'Missing {key}')
            for path in paths:
                if not isinstance(path, str) or not path or path.startswith('/') or '..' in path.split('/') or '\\' in path:
                    raise ValueError('Provenance paths must be repository-relative')
        if entry.get('license') != 'LGPL-3.0-only':
            raise ValueError('Preserve the upstream LGPL-3.0-only license declaration')
    affected = [entry for entry in adaptations if files.intersection(entry['native_paths'])]
    declaration = declarations[0]
    if declaration == 'none':
        if affected:
            raise ValueError('Changes to an adapted file require upstream credit')
        return
    match = re.fullmatch(re.escape(UPSTREAM) + r'@([0-9a-f]{40})', declaration)
    if not match:
        raise ValueError('Upstream-Reuse must pin the supported upstream repository and revision')
    if not any(entry['revision'] == match[1] for entry in affected):
        raise ValueError('Declared reuse needs a provenance entry covering changed native files')
    if MANIFEST not in files:
        raise ValueError('A reuse PR must update the provenance manifest')
    required = {'LICENSES/MarshalX-yandex-music-LGPL-3.0.txt', 'THIRD_PARTY_NOTICES.md'}
    if not required.issubset(set(data['available_files'])):
        raise ValueError('Missing upstream license or third-party notice')
    if not data['commits']:
        raise ValueError('No commits to attribute')
    for message in data['commits']:
        trailers = message.strip().rsplit('\n\n', 1)[-1].splitlines()
        if TRAILER not in trailers or not all(re.match(r'^[A-Za-z][A-Za-z-]*: ', line) for line in trailers):
            raise ValueError('Every commit in a declared reuse PR must retain the MarshalX co-author trailer')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', type=pathlib.Path, required=True)
    args = parser.parse_args()
    check(json.loads(args.input.read_text()))
    print('Upstream attribution metadata passes')

if __name__ == '__main__':
    try:
        main()
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
