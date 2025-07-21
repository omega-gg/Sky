#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

ffmpeg="$PWD/bin/ffmpeg"

yuv="yuv420p"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 4 -o $# -gt 6 ] || [ $# = 5 -a "$5" != "precise" ]; then

    echo "Usage: cut <video> <timeA> <timeB> <output> [precise]"
    echo "           [codec | lossless]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $# -lt 6 ]; then

    codec="-codec:v libx264 -crf 15 -preset slow"

elif [ "$6" = "lossless" ]; then

    codec="-codec:v libx264 -preset veryslow -qp 0 -pix_fmt $yuv"
else
    codec="$6"
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

# NOTE: The order of -i parameter matters and the frame cut is not perfect.
#       https://stackoverflow.com/questions/18444194/cutting-multimedia-files-based-on-start-and-end-time-using-ffmpeg

if [ "$5" = "precise" ]; then

    "$ffmpeg" -y -i "$1" -ss "$2" -to "$3" $codec -c:a copy "$4"
else
    "$ffmpeg" -y -ss "$2" -to "$3" -i "$1" -c copy "$4"
fi
