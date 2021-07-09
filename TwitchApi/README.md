# arduino_twitch_api
A wrapper for the [Twitch API](https://dev.twitch.tv/docs/api/reference/) for Arduino

Interact with the Twitch API directly on your Arduino Device. Works on ESP8266 (tested), ESP32(not tested, but should) and probably anything else that supports HTTPS clients.

![Example](https://i.imgur.com/xMxX4YD.png)

If you are new the ESP8266, [check out this video](https://www.youtube.com/watch?v=AFUAMVFzpWw)

## Implemented Endpoints

- users (See userAndFollowerData example)
- follower (For getting follower count)
- stream (For getting live viewer count - See getStreamInfo)