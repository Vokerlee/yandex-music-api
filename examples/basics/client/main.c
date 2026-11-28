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

#include <yandex_music/yandex_music.h>

int main(void)
{
    ym_client_options options;
    ym_client_options_init(&options);
    options.language = (ym_string_view){"en", 2};
    ym_client* client = NULL;
    ym_error error;
    ym_status status = ym_client_create(&options, &client, &error);
    if (status != YM_OK) {
        fprintf(stderr, "%s\n", error.message);
        return 1;
    }

    ym_string_view language = ym_client_language(client);
    fwrite(language.data, 1, language.size, stdout);
    fputc('\n', stdout);
    status = ym_client_close(client, &error);
    if (status != YM_OK)
        fprintf(stderr, "%s\n", error.message);
    ym_client_destroy(client);
    return status == YM_OK ? 0 : 1;
}
