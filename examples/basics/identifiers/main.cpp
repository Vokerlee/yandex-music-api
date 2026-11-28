/*
 * Copyright (c) 2026 Vokerlee <vokerlee@gmail.com>
 *
 * Yandex Music Native is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * Yandex Music Native is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <yandex_music/cpp/yandex_music.hpp>

int main(int argc, char** argv)
{
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " TRACK_ID ALBUM_ID USER_ID PLAYLIST_KIND\n";
        return 1;
    }
    auto track = ym::TrackId::create(argv[1]);
    auto album = ym::AlbumId::create(argv[2]);
    auto owner = ym::UserId::create(argv[3]);
    auto kind = ym::PlaylistKind::create(argv[4]);
    if (!track || !album || !owner || !kind) {
        std::cerr << "Could not create identifiers.\n";
        return 1;
    }
    ym::TrackReference reference{*track, *album};
    ym::PlaylistKey playlist{*owner, *kind};
    std::cout << "Track: " << reference.id.value() << ", album: " << reference.album->value()
              << '\n';
    std::cout << "Playlist: " << playlist.owner.value() << " / " << playlist.kind.value() << '\n';
}
