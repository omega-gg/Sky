#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

root="$PWD"

ffmpeg="$PWD/../ffmpeg"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 3 -o $# -gt 4 ]; then

    echo "Usage: reframeWide <image> <video> <output>"
    echo "                   [codec | lossless]"
    echo ""
    echo "This command output is usefull to turn 16:9 into 21:9 with Luma reframe."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

echo "----------"
echo "GENERATING"
echo "----------"

cd "$ffmpeg"

# NOTE: We don't use a generic name to avoid caching issues with Amazon S3 hosting.
filename=$(basename -- "$2")

# NOTE: Luma does not support lossless video input for now.
sh concatZoom.sh "$1" "$2" "$root/$filename"

cd -

echo "---------"
echo "REFRAMING"
echo "---------"

sh reframe.sh "$filename" temp.mp4 21:9

rm "$filename"

echo "--------"
echo "RESIZING"
echo "--------"

cd "$ffmpeg"

if [ $# -lt 4 ]; then

    sh resize.sh "$root/temp.mp4" "$2" "$3" 1
else
    sh resize.sh "$root/temp.mp4" "$2" "$3" 1 0 "$4"
fi

cd -

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

rm temp.mp4
