#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

ffmpeg="$PWD/bin/ffmpeg"

ffprobe="$PWD/bin/ffprobe"

duration="60"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

getWidth()
{
    "$ffprobe" -v error -select_streams v:0 -show_entries stream=width -of csv=p=0 "$1"
}

getHeight()
{
    "$ffprobe" -v error -select_streams v:0 -show_entries stream=height -of csv=p=0 "$1"
}

getFps()
{
    "$ffprobe" -v 0 -select_streams v:0 -show_entries stream=r_frame_rate -of csv=p=0 "$1" \
    | awk -F/ '{ printf "%.3f", $1 / $2 }'
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 3 ]; then

    echo "Usage: concatZoom <image> <video> <output>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

width=$(getWidth "$2")

height=$(getHeight "$2")

fps=$(getFps "$2")

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

"$ffmpeg" -y -loop 1 -i "$1" -t 1 -vf "\
    scale='if(gt(a,21/9),$width,-1)':'if(gt(a,21/9),-1,$height)', \
    pad=$width:$height:(ow-iw)/2:(oh-ih)/2, \
    fps=$fps" \
    -c:v libx264 -preset veryslow -qp 0 -pix_fmt yuv444p "temp.mp4"

frames=$(awk "BEGIN { print int($fps * 1) }")

zoom=$(awk "BEGIN { print (1.333 - 1) / $frames }")

"$ffmpeg" -y -i "temp.mp4" -vf "\
    zoompan=z='1+$zoom*on':x='iw/2-(iw/zoom/2)':y='ih/2-(ih/zoom/2)':\
    s=${width}x${height}:fps=$fps" \
    -frames:v "$frames" \
    -c:v libx264 -preset veryslow -qp 0 -pix_fmt yuv444p "temp2.mp4"

rm temp.mp4

echo "file 'temp2.mp4'" >  videos.txt
echo "file '$2'"        >> videos.txt

codec="-codec:v libx264 -crf 15 -preset slow"

"$ffmpeg" -y -f concat -safe 0 -i videos.txt $codec -c:a copy "$3"

#"$ffmpeg" -y -f concat -safe 0 -i videos.txt -c copy "$3"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

rm temp2.mp4

rm videos.txt
