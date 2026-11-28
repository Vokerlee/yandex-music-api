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

#include <stdio.h>
#include <string.h>

#include <yandex_music/c/yandex_music.h>

static ym_string_view view(const char* text)
{
    return (ym_string_view){text, strlen(text)};
}

int main(int argc, char** argv)
{
    if (argc != 5) {
        fprintf(stderr, "Usage: %s TRACK_ID ALBUM_ID USER_ID PLAYLIST_KIND\n", argv[0]);
        return 1;
    }
    ym_string_view track = view(argv[1]);
    ym_string_view album = view(argv[2]);
    ym_string_view owner = view(argv[3]);
    ym_string_view kind = view(argv[4]);
    printf("Track: %.*s, album: %.*s\n", (int)track.size, track.data, (int)album.size, album.data);
    printf("Playlist: %.*s / %.*s\n", (int)owner.size, owner.data, (int)kind.size, kind.data);
    return 0;
}
