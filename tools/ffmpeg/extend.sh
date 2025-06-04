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

if [ $# != 3 ]; then

    echo "Usage: extend <video> <reference video> <output>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

durationA=$(getDuration "$1")
durationB=$(getDuration "$2")

duration=$(awk "BEGIN { print $durationB - $durationA }")

check=$(awk "BEGIN { print ($duration <= 0) }")

if [ "$check" = 1 ]; then

    echo "No extension needed, input is longer or equal."

    cp "$1" "$3"

    exit 0
fi

"$ffmpeg" -y -i "$1" -vf "tpad=stop_mode=clone:stop_duration=$duration" -codec:v libx264 -crf 15 -preset slow -c:a copy "$3"
