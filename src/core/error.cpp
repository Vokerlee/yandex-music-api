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

#include <yandex_music/c/error.h>

void YM_CALL ym_error_init(ym_error* error) YM_NOEXCEPT
{
    if (error) {
        *error = {};
        error->retry_after_ms = -1;
    }
}

const char* YM_CALL ym_status_name(ym_status status) YM_NOEXCEPT
{
    switch (status) {
#define YM_CASE(value)                                                                             \
    case value:                                                                                    \
        return #value
        YM_CASE(YM_OK);
        YM_CASE(YM_INVALID_ARGUMENT);
        YM_CASE(YM_OUT_OF_MEMORY);
        YM_CASE(YM_CLOSED);
        YM_CASE(YM_PENDING);
        YM_CASE(YM_CANCELLED);
        YM_CASE(YM_DEADLINE_EXCEEDED);
        YM_CASE(YM_RESOURCE_LIMIT);
        YM_CASE(YM_SCHEMA_ERROR);
        YM_CASE(YM_SERVICE_ERROR);
        YM_CASE(YM_UNAUTHORIZED);
        YM_CASE(YM_FORBIDDEN);
        YM_CASE(YM_NOT_FOUND);
        YM_CASE(YM_RATE_LIMITED);
        YM_CASE(YM_CONFLICT);
        YM_CASE(YM_NETWORK_ERROR);
        YM_CASE(YM_TLS_ERROR);
        YM_CASE(YM_UNSUPPORTED);
        YM_CASE(YM_OUTCOME_UNKNOWN);
        YM_CASE(YM_INTERNAL_ERROR);
        YM_CASE(YM_INVALID_STATE);
        YM_CASE(YM_REENTRANT_CALL);
        YM_CASE(YM_IO_ERROR);
#undef YM_CASE
    default:
        return "YM_UNKNOWN_STATUS";
    }
}
