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

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yandex_music/c/yandex_music.h>

static ym_string_view view(const char* text)
{
    return (ym_string_view){text, strlen(text)};
}

static int YM_CALL progress(uint64_t received, uint64_t total, void* userdata)
{
    uint64_t limit = *(const uint64_t*)userdata;
    fprintf(stderr, "\r%" PRIu64 " / %" PRIu64 " bytes", received, total);
    return limit != 0 && received >= limit;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        fprintf(stderr,
            "Usage: %s HTTPS_URL FILENAME CANCEL_AFTER_BYTES\nUse 0 to download without a byte "
            "limit.\n",
            argv[0]);
        return 1;
    }
    errno = 0;
    char* end = NULL;
    uint64_t limit = strtoull(argv[3], &end, 10);
    if (errno || !*argv[3] || *argv[3] == '-' || *end) {
        fputs("CANCEL_AFTER_BYTES must be an unsigned 64-bit integer.\n", stderr);
        return 1;
    }

    ym_error error;
    ym_status status =
        ym_file_download(view(argv[1]), view(argv[2]), 30000, progress, &limit, &error);
    fputc('\n', stderr);
    if (status == YM_CANCELLED) {
        puts("Cancelled; partial output removed.");
        return 0;
    }
    if (status != YM_OK) {
        fprintf(stderr, "%s\n", error.message);
        return 1;
    }
    printf("Saved %s\n", argv[2]);
    return 0;
}
