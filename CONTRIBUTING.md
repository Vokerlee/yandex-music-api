# Contributing

Keep each implementation PR focused on one reviewable responsibility, with relevant tests.
Local research/plans stay ignored. Install pinned formatters from requirements-dev.txt.
Run `python3 scripts/format.py` or `python3 scripts/format.py --check`.
Only tracked files are selected; explicit --files paths must be relative and tracked.
Missing tools, incorrect versions and formatter failures fail the check.

`python3 scripts/format.py --install-hook` installs an optional pre-commit check.
Custom hooks are preserved; checks inspect working-tree contents, including partial stages.

Each PR declares Upstream-Reuse: none or MarshalX/yandex-music-api@<full SHA>.
Adapted work must retain source/native paths, LGPL notices and this commit trailer:

Co-authored-by: Ilya Siamionau <15520314+MarshalX@users.noreply.github.com>

Preserve that credit in the final merge. It does not imply upstream review/endorsement.
