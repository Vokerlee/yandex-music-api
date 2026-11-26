# Third-party notices

The private JSON implementation uses [nlohmann/json](https://github.com/nlohmann/json),
qualified at v3.12.0. Copyright (c) 2013-2025 Niels Lohmann. Its MIT license is
retained in [LICENSES/nlohmann-json-MIT.txt](LICENSES/nlohmann-json-MIT.txt).
The source archive is pinned and hash-verified in `cmake/dependencies.cmake`.

Response envelope handling and associated conformance cases are adapted from
[MarshalX/yandex-music-api](https://github.com/MarshalX/yandex-music-api) at
`9ca132e5c18a72aab779c1fc68c169380a9357d0` (request_base.py, response.py and
test_request_base.py). Copyright (C) 2019-2026 Ilya (Marshal). The adapted native
files retain LGPL-3.0-only notices; their exact paths and source paths are in
`third_party/provenance.json`. The original LGPLv3 text is retained in
`LICENSES/MarshalX-yandex-music-LGPL-3.0.txt`, with GPLv3 in `LICENSE`.
This attribution does not imply upstream review or endorsement.
