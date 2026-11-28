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

int main(void)
{
    const char* json = "{\"result\":{\"tracks\":[{\"id\":42,\"title\":\"Example\"}],\"available\":"
                       "true,\"rating\":4.5}}";
    ym_result* response = NULL;
    ym_error error;
    ym_status status = ym_response_parse(200, view(json), NULL, &response, &error);
    if (status != YM_OK) {
        fprintf(stderr, "%s\n", error.message);
        return 1;
    }

    const ym_value* root = ym_result_root(response);
    const ym_value* track = NULL;
    ym_value_at(member(root, "tracks"), 0, &track, NULL);
    uint64_t id = 0;
    ym_string_view title = {0};
    uint32_t available = 0;
    double rating = 0;
    ym_value_uint64(member(track, "id"), &id, NULL);
    ym_value_string(member(track, "title"), &title, NULL);
    ym_value_bool(member(root, "available"), &available, NULL);
    ym_value_double(member(root, "rating"), &rating, NULL);
    printf("%" PRIu64 "\t%.*s\n", id, (int)title.size, title.data);
    printf("Available: %s\nRating: %.1f\n", available ? "yes" : "no", rating);
    printf("Optional field: %s\n",
        ym_value_typeof(member(root, "optional")) == YM_VALUE_MISSING ? "missing" : "present");

    ym_result_destroy(response);
    return 0;
}
