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

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yandex_music/c/yandex_music.h>

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

static int same_name(ym_string_view value, const char* wanted)
{
    if (value.size != strlen(wanted))
        return 0;
    for (size_t i = 0; i < value.size; ++i)
        if (tolower((unsigned char)value.data[i]) != tolower((unsigned char)wanted[i]))
            return 0;
    return 1;
}

int main(int argc, char** argv)
{
    const char* token = getenv("YANDEX_MUSIC_TOKEN");
    if (argc != 2 || !token || !*token) {
        fprintf(stderr, "Usage: %s GENRE_NAME_OR_TAG\nSet YANDEX_MUSIC_TOKEN first.\n", argv[0]);
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
        status = ym_wave_stations(client, NULL, &request, &error);
    if (status == YM_OK)
        status = ym_client_wait(client, request, 30000, &response, &error);

    if (status != YM_OK)
        fprintf(stderr, "%s\n", error.message);
    else {
        const ym_value* root = ym_result_root(response);
        ym_string_view selected = {0};
        size_t count = 0;
        ym_value_size(root, &count, NULL);
        for (size_t i = 0; i < count; ++i) {
            const ym_value* item = NULL;
            ym_value_at(root, i, &item, NULL);
            const ym_value* station = member(item, "station");
            const ym_value* id = member(station, "id");
            ym_string_view type = {0}, tag = {0}, name = {0};
            ym_value_string(member(id, "type"), &type, NULL);
            ym_value_string(member(id, "tag"), &tag, NULL);
            ym_value_string(member(station, "name"), &name, NULL);
            if (same_name(type, "genre") && (same_name(tag, argv[1]) || same_name(name, argv[1]))) {
                selected = tag;
                break;
            }
        }
        if (!selected.size) {
            fputs("Genre unavailable; run wave/stations to find its current name or tag.\n",
                stderr);
            status = YM_NOT_FOUND;
        } else {
            size_t size = sizeof("genre:") + selected.size;
            char* seed = malloc(size);
            if (!seed) {
                fputs("Could not allocate a seed\n", stderr);
                status = YM_OUT_OF_MEMORY;
            } else {
                memcpy(seed, "genre:", 6);
                memcpy(seed + 6, selected.data, selected.size);
                seed[size - 1] = '\0';
                ym_request_destroy(request);
                request = NULL;
                status = ym_wave_start(client, view(seed), NULL, &request, &error);
                free(seed);
                ym_result* wave = NULL;
                if (status == YM_OK)
                    status = ym_client_wait(client, request, 30000, &wave, &error);
                if (status != YM_OK)
                    fprintf(stderr, "%s\n", error.message);
                else {
                    const ym_value* wave_root = ym_result_root(wave);
                    print_value(member(wave_root, "radioSessionId"));
                    fputc('\n', stdout);
                    const ym_value* items = member(wave_root, "sequence");
                    size_t count = 0;
                    ym_value_size(items, &count, NULL);
                    for (size_t i = 0; i < count; ++i) {
                        const ym_value* item = NULL;
                        ym_value_at(items, i, &item, NULL);
                        const ym_value* record = member(item, "track");
                        print_value(member(record, "id"));
                        fputc('\t', stdout);
                        print_value(member(record, "title"));
                        fputc('\n', stdout);
                    }
                }
                ym_result_destroy(wave);
            }
        }
    }

    ym_result_destroy(response);
    ym_request_destroy(request);
    ym_client_destroy(client);
    return status == YM_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
