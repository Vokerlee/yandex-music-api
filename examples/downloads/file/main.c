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

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s HTTPS_URL FILENAME\n", argv[0]);
        return 1;
    }
    ym_error error;
    ym_status status = ym_file_download(view(argv[1]), view(argv[2]), 30000, NULL, NULL, &error);
    if (status != YM_OK) {
        fprintf(stderr, "%s\n", error.message);
        return 1;
    }
    printf("Saved %s\n", argv[2]);
    return 0;
}
