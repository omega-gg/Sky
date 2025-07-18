#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

ffmpeg="$PWD/bin/ffmpeg"

ffprobe="$PWD/bin/ffprobe"

width="3840"
height="2160"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

getFps()
{
    "$ffprobe" -v 0 -select_streams v:0 -show_entries stream=r_frame_rate -of csv=p=0 "$1" \
    | awk -F/ '{ printf "%.3f", $1 / $2 }'
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 3 -o $# -gt 4 ] || [ $# = 4 -a "$4" != "lossless" ]; then

    echo "Usage: concatZoom <image> <video> <output> [lossless]"
    echo ""
    echo "This command output is usefull to generate a wide background and turn 16:9 into 21:9 \
with a generative tool like Luma."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

fps=$(getFps "$2")

if [ "$4" = "lossless" ]; then

    codec="-codec:v libx264 -preset veryslow -qp 0"
else
    codec="-codec:v libx264 -crf 15 -preset slow"
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

ratio=$(awk "BEGIN { printf \"%.6f\", $width / $height }")

frames=$(awk "BEGIN { print int($fps * 1) }")

zoom=$(awk "BEGIN { print (1.333 - 1) / $frames }")

echo "concatZoom: Generating input image frames."

mkdir -p frames

for i in $(seq 0 $((frames - 1))); do

    scale=$(awk "BEGIN { print 1 + ($i * $zoom) }")

    "$ffmpeg" -y -loop 1 -t 1 -i "$1" -vf "\
             scale=if(gt(a\,${ratio})\,${width}\,-1):if(gt(a\,${ratio})\,-1\,${height}):flags=lanczos, \
             pad=${width}:${height}:(ow-iw)/2:(oh-ih)/2, \
             scale=iw*${scale}:ih*${scale}:flags=lanczos, \
             crop=${width}:${height}:(in_w-${width})/2:(in_h-${height})/2" \
            -frames:v 1 "frames/frame$(printf "%04d" "$i").png"
done

"$ffmpeg" -y -framerate "$fps" -i frames/frame%04d.png \
          -c:v libx264 -preset veryslow -qp 0 -pix_fmt yuv444p "temp.mp4"

rm -rf frames

echo "file 'temp.mp4'" >  videos.txt
echo "file '$2'"       >> videos.txt

"$ffmpeg" -y -f concat -safe 0 -i videos.txt $codec -c:a copy "$3"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

rm temp.mp4

rm videos.txt
