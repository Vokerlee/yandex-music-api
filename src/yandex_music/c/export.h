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

#ifndef YANDEX_MUSIC_EXPORT_H
#define YANDEX_MUSIC_EXPORT_H

#if defined(_WIN32) && !defined(YM_STATIC)
#if defined(YM_BUILDING_LIBRARY)
#define YM_API __declspec(dllexport)
#else
#define YM_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && !defined(YM_STATIC)
#define YM_API __attribute__((visibility("default")))
#else
#define YM_API
#endif

#if defined(_WIN32)
#define YM_CALL __cdecl
#else
#define YM_CALL
#endif

#ifdef __cplusplus
#define YM_NOEXCEPT noexcept
#else
#define YM_NOEXCEPT
#endif

#endif
