#!/bin/sh

# A dwm_bar function to display information regarding system memory, CPU temperature, and storage
# Joe Standring <git@joestandring.com>
# GNU GPLv3

dwm_resources () {
    # Used and total memory
    MEMUSED=$(free -h | awk '(NR == 2) {print $3}')
    MEMTOT=$(free -h |awk '(NR == 2) {print $2}')
    # CPU temperature
    CPU=$(sysctl -n hw.sensors.cpu0.temp0 | cut -d. -f1)
    # Used and total storage in /home (rounded to 1024B)
    STOUSED=$(df -h | grep '/$' | awk '{print $3}')
    STOTOT=$(df -h | grep '/$' | awk '{print $2}')
    STOPER=$(df -h | grep '/$' | awk '{print $5}')

    printf "%s" "$SEP1"
    if [ "$IDENTIFIER" = "unicode" ]; then
        printf "💻%s/%s STO %s/%s: %s" "$MEMUSED" "$MEMTOT" "$STOUSED" "$STOTOT" "$STOPER"
    else
        printf "MEM %s/%s STO %s/%s: %s" "$MEMUSED" "$MEMTOT" "$STOUSED" "$STOTOT" "$STOPER"
    fi
    printf "%s\n" "$SEP2"
}

dwm_resources
