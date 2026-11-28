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

#ifndef YANDEX_MUSIC_H
#define YANDEX_MUSIC_H

#include <yandex_music/c/account.h>
#include <yandex_music/c/api.h>
#include <yandex_music/c/catalogue.h>
#include <yandex_music/c/client.h>
#include <yandex_music/c/download.h>
#include <yandex_music/c/error.h>
#include <yandex_music/c/export.h>
#include <yandex_music/c/playlists.h>
#include <yandex_music/c/recommendations.h>
#include <yandex_music/c/request.h>
#include <yandex_music/c/response.h>
#include <yandex_music/c/result.h>
#include <yandex_music/c/search.h>
#include <yandex_music/c/types.h>
#include <yandex_music/c/wave.h>

#ifdef __cplusplus
extern "C" {
#endif

YM_API const char* YM_CALL ym_version(void) YM_NOEXCEPT;

YM_API const char* YM_CALL ym_build_info(void) YM_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif
