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
    if (argc != 4 || !token || !*token) {
        fprintf(stderr,
            "Usage: %s USER_ID TRACK_IDS like_or_unlike\nSet YANDEX_MUSIC_TOKEN first.\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[3], "like") && strcmp(argv[3], "unlike")) {
        fputs("Choose like or unlike\n", stderr);
        return EXIT_FAILURE;
    }
    uint32_t liked = strcmp(argv[3], "like") == 0;
    ym_client_options options;
    ym_client_options_init(&options);
    options.token = view(token);
    options.language = view("en");
    ym_client* client = NULL;
    ym_request* request = NULL;
    ym_result* response = NULL;
    ym_error error;

    ym_status status = ym_client_create(&options, &client, &error);
    if (status == YM_OK)
        status = ym_set_like(client, view(argv[1]), YM_LIBRARY_TRACK, view(argv[2]), liked, NULL,
            &request, &error);
    if (status == YM_OK)
        status = ym_client_wait(client, request, 30000, &response, &error);

    if (status != YM_OK)
        fprintf(stderr, "%s\n", error.message);
    else {
        puts("Updated");
    }

    ym_result_destroy(response);
    ym_request_destroy(request);
    ym_client_destroy(client);
    return status == YM_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
