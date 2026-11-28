# Getting your Yandex Music token

The methods below come from the Python reference project's token guide. Its Device Flow examples use the **Python client**; pass the resulting `access_token` to this library's C or C++ client.

## Using the Python reference: OAuth Device Flow

Install the reference client:

```sh
python -m pip install --upgrade yandex-music
```

`device_auth` waits until you authorize the application on Yandex. It returns an `OAuthToken` with `access_token`, `refresh_token`, `expires_in`, and `token_type`.

### Synchronous

```python
from yandex_music import Client


def on_code(code):
    print(f"Open {code.verification_url} and enter: {code.user_code}")


client = Client()
token = client.device_auth(on_code=on_code)

# Save the access token so you do not need to sign in every time.
print(f"access_token: {token.access_token}")
print(f"refresh_token: {token.refresh_token}")
print(f"expires_in: {token.expires_in}")

client.init()
print(client.me.account.login)
```

### Asynchronous

```python
import asyncio

from yandex_music import ClientAsync


async def main():
    def on_code(code):
        print(f"Open {code.verification_url} and enter: {code.user_code}")

    client = ClientAsync()
    token = await client.device_auth(on_code=on_code)

    print(f"access_token: {token.access_token}")
    print(f"refresh_token: {token.refresh_token}")
    print(f"expires_in: {token.expires_in}")

    await client.init()
    print(client.me.account.login)


asyncio.run(main())
```

Store the token yourself, for example in an environment variable or a private configuration file. The reference client does not save it to disk or automatically refresh it when `expires_in` elapses.

## Using a browser: implicit OAuth

If Device Flow is unsuitable, obtain a token directly in your browser:

1. Optionally open Developer Tools → **Network** and enable network throttling. The Music page redirects quickly; slowing the connection gives you time to copy the address.
2. Open [Yandex Music authorization](https://oauth.yandex.ru/authorize?response_type=token&client_id=23cabbbdc6cd418abb4b39c32c41195d).
3. Sign in to Yandex and approve the application's access.
4. Copy the redirect URL before the Music page redirects again. It looks like this:

   ```text
   https://music.yandex.ru/#access_token=YOUR_TOKEN&token_type=bearer&expires_in=31535645
   ```

5. Your token is the value after `access_token=` and before the next `&`.

Network throttling instructions: [Chrome](https://developer.chrome.com/docs/devtools/network/reference#throttling), [Firefox](https://firefox-source-docs.mozilla.org/devtools-user/network_monitor/throttling/index.html).

## Alternative methods

The upstream guide uses existing Music client applications rather than a new custom OAuth application. Its alternatives include:

- [Browser token helper](https://ym-token.marshal.dev/).
- [Android application releases](https://github.com/MarshalX/yandex-music-token/releases).
- [Chrome extension](https://chrome.google.com/webstore/detail/yandex-music-token/lcbjeookjibfhjjopieifgjnhlegmkib).
- [Firefox extension](https://addons.mozilla.org/en-US/firefox/addon/yandex-music-token/).

The helper's [source code](https://github.com/MarshalX/yandex-music-token) is public. You can also consult the upstream-linked [Yandex Station authentication implementation](https://github.com/AlexxIT/YandexStation/blob/master/custom_components/yandex_station/core/yandex_session.py).

## Use the token with this library

Keep the token private. Set `YANDEX_MUSIC_TOKEN` in the terminal where you will run your program. In Bash, this reads it without echoing it or placing it in the command text:

```sh
read -rsp "Yandex Music token: " YANDEX_MUSIC_TOKEN
export YANDEX_MUSIC_TOKEN
printf '\n'
```

Read it with `getenv("YANDEX_MUSIC_TOKEN")` in C or `std::getenv("YANDEX_MUSIC_TOKEN")` in C++. Pass it as the client's `token` option. See [creating a client](../README.md#create-a-client).

---

English adaptation of [the upstream token guide](https://github.com/MarshalX/yandex-music-api/blob/9ca132e5c18a72aab779c1fc68c169380a9357d0/docs/source/token.md). Original copyright © 2019–2026 Ilya (Marshal); translation and C/C++ integration notes © 2026 Vokerlee. This guide retains [LGPLv3](../LICENSES/MarshalX-yandex-music-LGPL-3.0.txt).
