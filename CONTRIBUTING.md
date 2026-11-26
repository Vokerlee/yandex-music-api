# Contributing

Each behavior or infrastructure improvement has its own PR, including the tests and
documentation necessary to use it. Keep commits buildable and PRs focused. Use an
area-prefixed title, for example `build: export installed library targets`.

Build/test commands and library consumption are in [README.md](README.md). C++ source
uses C++23; public C headers must also compile as C11. Keep warnings and compiler
options private to the SDK. First-party source carries the GPLv3 notice.

Install pinned formatters with `python3 -m pip install -r requirements-dev.txt`:

```sh
python3 scripts/format.py
python3 scripts/format.py --check
python3 scripts/format.py --check --files src/c/version.cpp
python3 -m unittest discover -s scripts/tests
```

Only existing tracked files are formatted. Explicit paths must be tracked and relative
to the repository root. CI uses the same command and tool pins. Missing tools, incorrect
versions and formatting failures fail the check. Stage newly created files before
formatting them. An optional `--install-hook` installs the check without overwriting a
custom hook; it checks working-tree contents, including partially staged files.

Keep credentials and local research/plans out of commits. Do not force-add `.cache/`.

Every PR declares `Upstream-Reuse: none` or
`Upstream-Reuse: MarshalX/yandex-music-api@<40-hex-SHA>`. Ported behavior, models,
fixtures and schemas need pinned source/native paths in `third_party/provenance.json`,
preserved notices, and this commit trailer:

```text
Co-authored-by: Ilya Siamionau <15520314+MarshalX@users.noreply.github.com>
```

Preserve that trailer in the final squash/merge. It credits adapted upstream work;
it does not imply the upstream author reviewed or endorsed this port. Co-authorship
does not replace license obligations. Independently authored infrastructure does
not receive fabricated credit. The metadata checker cannot detect undeclared copying;
review the actual diff and provenance. Run it locally with
`python3 scripts/check_attribution.py --repository Vokerlee/yandex-music-api --pr NUMBER`.
