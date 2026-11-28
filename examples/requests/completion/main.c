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

static void YM_CALL complete(ym_request* request, ym_status status, void* userdata)
{
    (void)request;
    int* done = userdata;
    *done = 1;
    printf("Completed: %s\n", ym_status_name(status));
}

int main(int argc, char** argv)
{
    const char* token = getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 2 || !token || !*token) {
        fprintf(stderr, "Usage: %s QUERY\nSet YANDEX_MUSIC_TOKEN first.\n", argv[0]);
        return EXIT_FAILURE;
    }

    int done = 0;
    ym_request_options request_options;
    ym_request_options_init(&request_options);
    request_options.on_complete = complete;
    request_options.userdata = &done;
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
        status = ym_search(client, view(argv[1]), YM_SEARCH_TRACK, 0, &request_options, &request,
            &error);
    if (status == YM_OK)
        status = ym_client_wait(client, request, 30000, &response, &error);

    if (status != YM_OK)
        fprintf(stderr, "%s\n", error.message);
    else {
        const ym_value* root = ym_result_root(response);
        printf("HTTP %u\n", ym_result_http_status(response));
        printf("Callback delivered: %s\n", done ? "yes" : "no");
        (void)root;
    }

    ym_result_destroy(response);
    ym_request_destroy(request);
    ym_client_destroy(client);
    return status == YM_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
