#!/bin/sh

dwm_layout() {
	LAYOUT=$(xkblayout-state print "%s")
	printf "%s" "$SEP1"
	if [ "$IDENTIFIER" = "unicode" ]; then
		printf "⌨%s" "$LAYOUT"
	else
		printf "LAY %s" "$LAYOUT"
	fi
	printf "%s\n" "$SEP2"
}

dwm_layout
