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

int main(void)
{
    ym_result* original = NULL;
    ym_result* saved = NULL;
    ym_error error;
    ym_status status =
        ym_response_parse(200, view("{\"result\":\"Saved value\"}"), NULL, &original, &error);
    if (status == YM_OK)
        status = ym_result_clone(original, &saved, &error);
    ym_result_destroy(original);

    if (status != YM_OK) {
        fprintf(stderr, "%s\n", error.message);
        return 1;
    }
    ym_string_view text = {0};
    status = ym_value_string(ym_result_root(saved), &text, &error);
    if (status == YM_OK)
        printf("%.*s\n", (int)text.size, text.data);
    else
        fprintf(stderr, "%s\n", error.message);
    ym_result_destroy(saved);
    return status == YM_OK ? 0 : 1;
}
