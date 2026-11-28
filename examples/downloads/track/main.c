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
#include <stdlib.h>
#include <string.h>

#include <yandex_music/c/yandex_music.h>

static ym_string_view view(const char* text)
{
    return (ym_string_view){text, strlen(text)};
}

int main(int argc, char** argv)
{
    const char* token = getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 3 || !token || !*token) {
        fprintf(stderr, "Usage: %s TRACK_ID FILENAME\nSet YANDEX_MUSIC_TOKEN first.\n", argv[0]);
        return 1;
    }
    ym_client_options options;
    ym_client_options_init(&options);
    options.token = view(token);
    options.language = view("en");
    ym_client* client = NULL;
    ym_error error;
    ym_status status = ym_client_create(&options, &client, &error);
    if (status == YM_OK)
        status = ym_track_download(client, view(argv[1]), view(argv[2]), view("hq"), view("mp3"),
            30000, NULL, NULL, &error);
    if (status == YM_OK)
        printf("Saved %s\n", argv[2]);
    else
        fprintf(stderr, "%s\n", error.message);
    ym_client_destroy(client);
    return status == YM_OK ? 0 : 1;
}
