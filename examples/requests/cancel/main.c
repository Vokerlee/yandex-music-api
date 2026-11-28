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

#include <yandex_music/c/yandex_music.h>

int main(void)
{
    ym_client* client = NULL;
    ym_request* request = NULL;
    ym_error error;
    ym_status status = ym_client_create(NULL, &client, &error);
    if (status == YM_OK)
        status = ym_search(client, (ym_string_view){"music", 5}, YM_SEARCH_TRACK, 0, NULL, &request,
            &error);
    if (status == YM_OK)
        status = ym_request_cancel(request, &error);
    if (status == YM_OK)
        puts(ym_status_name(ym_request_get_status(request)));
    else
        fprintf(stderr, "%s\n", error.message);
    ym_request_destroy(request);
    ym_client_destroy(client);
    return status == YM_OK ? 0 : 1;
}
