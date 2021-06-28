#!/bin/sh

# A dwm_bar function that shows the current artist, track, duration, and status from Spotify using playerctl
# Joe Standring <git@joestandring.com>
# GNU GPLv3

# Dependencies: spotify/spotifyd, playerctl

# NOTE: The official spotify client does not provide the track position or shuffle status through playerctl. This does work through spotifyd however.

dwm_spotify() {
	if ps -C spotify >/dev/null; then
		PLAYER="spotify"
	elif ps -C spotifyd >/dev/null; then
		PLAYER="spotifyd"
	fi

	if [ "$PLAYER" = "spotify" ] || [ "$PLAYER" = "spotifyd" ]; then
		ARTIST=$(playerctl metadata artist)
		TRACK=$(playerctl metadata title)
		STATUS=$(playerctl status)

		STATUS="⏯"
		printf "%s%s - %s%s" "$STATUS" "$ARTIST" "$TRACK" "$SEP2"
	fi
}

dwm_spotify
