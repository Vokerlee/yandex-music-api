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
#include <yandex_music/yandex_music.h>

int main(void)
{
    if (strcmp(ym_version(), "0.1.0") != 0 || strstr(ym_build_info(), "C++23") == NULL) {
        fputs("Invalid C version/build contract\n", stderr);
        return 1;
    }
    ym_client* client = NULL;
    ym_error error;
    if (ym_client_create(NULL, &client, &error) != YM_OK)
        return 2;
    if (ym_client_language(client).size != 2)
        return 3;
    ym_client_destroy(client);
    const char body[] = "{\"result\":true}";
    ym_result* response = NULL;
    uint32_t flag = 0;
    if (ym_response_parse(200, (ym_string_view) { body, sizeof(body) - 1 }, NULL, &response, &error)
        != YM_OK)
        return 4;
    if (ym_value_bool(ym_result_root(response), &flag, &error) != YM_OK || flag != 1)
        return 5;
    ym_result_destroy(response);
    return 0;
}
