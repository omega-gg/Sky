#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

ffmpeg="$PWD/bin/ffmpeg"

ffprobe="$PWD/bin/ffprobe"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

getDuration()
{
    "$ffprobe" -i "$1" -show_entries format=duration -v quiet -of csv="p=0"
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 3 -o $# -gt 5 ]; then

    echo "Usage: resize <video> <reference video> <output> [skip=0] [chop=0]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

durationA=$(getDuration "$1")
durationB=$(getDuration "$2")

if [ $# = 4 -o $# = 5 ]; then

    duration=$(awk "BEGIN { print $durationB - $durationA + $4 }")

    if [ "$4" = "0" ]; then

        skip=""
    else
        skip="-ss $4"
    fi
else
    duration=$(awk "BEGIN { print $durationB - $durationA }")

    skip=""
fi

check=$(awk "BEGIN { print ($duration <= 0) }")

if [ "$check" = 1 ]; then

    if [ $# = 5 ]; then

        duration=$(awk "BEGIN { print $durationB - $5 }")
    else
        duration="$durationB"
    fi

    "$ffmpeg" -y -i "$1" $skip -t "$duration" -codec:v libx264 -crf 15 -preset slow -c:a copy "$3"
else
    if [ $# = 5 ]; then

        duration=$(awk "BEGIN { print $duration - $5 }")
    fi

    "$ffmpeg" -y -i "$1" $skip -vf "tpad=stop_mode=clone:stop_duration=$duration" -codec:v libx264 -crf 15 -preset slow -c:a copy "$3"
fi
