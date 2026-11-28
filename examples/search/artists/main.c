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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yandex_music/yandex_music.h>

static ym_string_view view(const char* text)
{
    return (ym_string_view){text, strlen(text)};
}

static const ym_value* member(const ym_value* object, const char* name)
{
    const ym_value* value = NULL;
    ym_value_member(object, view(name), &value, NULL);
    return value;
}

static void print_value(const ym_value* value)
{
    ym_string_view text = {0};
    uint64_t number = 0;
    if (ym_value_string(value, &text, NULL) == YM_OK) {
        if (text.size)
            fwrite(text.data, 1, text.size, stdout);
    } else if (ym_value_uint64(value, &number, NULL) == YM_OK)
        printf("%" PRIu64, number);
}

int main(int argc, char** argv)
{
    const char* token = getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 2 || !token || !*token) {
        fprintf(stderr, "Usage: %s QUERY\nSet YANDEX_MUSIC_TOKEN first.\n", argv[0]);
        return EXIT_FAILURE;
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
    if (status == YM_OK)
        status = ym_search(client, view(argv[1]), YM_SEARCH_ARTIST, 0, NULL, &request, &error);
    if (status == YM_OK)
        status = ym_client_wait(client, request, 30000, &response, &error);

    if (status != YM_OK)
        fprintf(stderr, "%s\n", error.message);
    else {
        const ym_value* root = ym_result_root(response);
        const ym_value* items = member(member(root, "artists"), "results");
        size_t count = 0;
        ym_value_size(items, &count, NULL);
        for (size_t i = 0; i < count; ++i) {
            const ym_value* item = NULL;
            ym_value_at(items, i, &item, NULL);
            const ym_value* record = item;
            print_value(member(record, "id"));
            fputc('\t', stdout);
            print_value(member(record, "name"));
            fputc('\n', stdout);
        }
    }

    ym_result_destroy(response);
    ym_request_destroy(request);
    ym_client_destroy(client);
    return status == YM_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
