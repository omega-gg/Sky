#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

ffmpeg="$PWD/../ffmpeg/bin/ffmpeg"

ffprobe="$PWD/../ffmpeg/bin/ffprobe"

width="3840"
height="2160"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

# NOTE: Apparently lanczos and bicubic scaling are glitching LivePortrait.

upscale()
{
    "$ffmpeg" -y -i "$1" -vf "scale=${width}:${height}:flags=bilinear,fps=$fps" \
              -fps_mode:v cfr \
              -c:v libx264 -preset veryslow -qp 0 -pix_fmt yuv444p \
              -c:a copy "$2"
}

# NOTE: Bicubic seems to yield better results in particular on character faces.

downscale()
{
    "$ffmpeg" -y -i "$1" -vf "scale=${input_width}:${input_height}:flags=bicubic,fps=$fps" \
              -fps_mode:v cfr \
              $codec -c:a copy "$2"
}

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

if [ $# -lt 3 -o $# -gt 7 ] \
   || \
   [ $# = 7 -a "$7" != "lossless" ]; then

    echo "Usage: sync <input path> <input name> <output path> [crop_x] [width] [height] [lossless]"
    echo ""
    echo "This command upscales the input videos, runs the synchronization, downscales the output \
video at the original size and moves everything in output folder. This maximizes the rendering \
quality."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $# = 5 ]; then

    width="$5"

elif [ $# -gt 5 ]; then

    width="$5"
    height="$6"
fi

input="$1/$2"

input_width=$(getWidth "$input.mp4")

input_height=$(getHeight "$input.mp4")

fps=$(getFps "$input.mp4")

ratio_input=$(awk "BEGIN { printf \"%.6f\", $input_width / $input_height }")

ratio_target=$(awk "BEGIN { printf \"%.6f\", $width / $height }")

if [ "$ratio_input" != "$ratio_target" ]; then

    echo "sync: Invalid input video ratio."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

echo "---------"
echo "UPSCALING"
echo "---------"

upscale "$input.mp4" "temp.mp4"

cp "$input-sync.mp4" "temp-sync.mp4"

if [ $# = 4 ]; then

    sh run.sh "$PWD/temp.mp4" "$PWD/temp-sync.mp4" "$4"
else
    sh run.sh "$PWD/temp.mp4" "$PWD/temp-sync.mp4"
fi

rm "temp-sync.mp4"

name="output/temp--temp-sync"

mv "$name".mp4        "temp.mp4"
mv "$name"_concat.mp4 "temp-sync.mp4"

echo "-----------"
echo "DOWNSCALING"
echo "-----------"

if [ "$7" = "lossless" ]; then

    codec="-codec:v libx264 -preset veryslow -qp 0 -pix_fmt yuv444p"
else
    codec="-codec:v libx264 -crf 15 -preset slow"
fi

downscale "temp.mp4" "$3/$2.mp4"

mv "temp-sync.mp4" "$3/$2-sync.mp4"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

rm "temp.mp4"
