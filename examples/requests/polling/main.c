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

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif

#include <yandex_music/c/yandex_music.h>

static ym_string_view view(const char* text)
{
    return (ym_string_view){text, strlen(text)};
}

int main(int argc, char** argv)
{
    const char* token = getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 2 || !token || !*token) {
        fprintf(stderr, "Usage: %s QUERY\nSet YANDEX_MUSIC_TOKEN first.\n", argv[0]);
        return 1;
    }
    ym_client_options options;
    ym_client_options_init(&options);
    options.token = view(token);
    options.language = view("en");
    ym_client* client = NULL;
    ym_request* request = NULL;
    ym_result* response = NULL;
    ym_error error;
    ym_status status = ym_client_create(&options, &client, &error);
    ym_request_options request_options;
    ym_request_options_init(&request_options);
    request_options.deadline_ms = ym_monotonic_ms() + 30000;
    if (status == YM_OK)
        status = ym_search(client, view(argv[1]), YM_SEARCH_TRACK, 0, &request_options, &request,
            &error);

    while (status == YM_OK && ym_request_get_status(request) == YM_PENDING) {
        status = ym_client_poll(client, ym_monotonic_ms(), 64, &error);
#ifdef _WIN32
        Sleep(1);
#else
        const struct timespec delay = {0, 1000000};
        nanosleep(&delay, NULL);
#endif
    }
    if (status == YM_OK)
        status = ym_request_take_result(request, &response, &error);
    if (status == YM_OK)
        printf("HTTP %u\n", ym_result_http_status(response));
    else
        fprintf(stderr, "%s\n", error.message);
    ym_result_destroy(response);
    ym_request_destroy(request);
    ym_client_destroy(client);
    return status == YM_OK ? 0 : 1;
}
