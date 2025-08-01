#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

bin="$PWD/bin"

topaz="C:\Program Files\Topaz Labs LLC\Topaz Video AI"

model="C:\ProgramData\Topaz Labs LLC\Topaz Video AI\models"

width="3840"
height="2160"

interpolation="chr-2"

yuv="yuv420p"

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

getDuration()
{
    "$ffprobe" -i "$1" -show_entries format=duration -v quiet -of csv="p=0"
}

getFps()
{
    "$ffprobe" -v 0 -select_streams v:0 -show_entries stream=r_frame_rate -of csv=p=0 "$1" \
    | awk -F/ '{ printf "%.3f", $1 / $2 }'
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 3 -o $# -gt 8 ] \
   || \
   [ $3 != "prob-4" -a $3 != "iris-3" -a $3 != "rhea-1" ] \
   || \
   [ $# -gt 3 \
     -a \
     "$4" != "default" -a "$4" != "letterbox" -a "$4" != "crop" -a "$4" != "wide" ]; then

    echo "Usage: topaz <input> <output> <prob-4 | iris-3 | rhea-1>"
    echo "             [default | letterbox | crop | wide]"
    echo "             [width = $width] [height = $height]"
    echo "             [fps = default]"
    echo "             [codec | lossless]"

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

ffmpeg="$topaz/ffmpeg"

ffprobe="$topaz/ffprobe"

export TVAI_MODEL_DIR="$model"

export TVAI_MODEL_DATA_DIR="$model"

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

input_width=$(getWidth "$1")

input_height=$(getHeight "$1")

if [ "$input_width" != "$width" ] || [ "$input_height" != "$height" ]; then

    filter="tvai_up=model=$3:scale=0:w=$width:h=$height:preblur=0:noise=0:details=0:halo=0:blur=0:compression=0:estimate=8:blend=0.2:device=0:vram=1:instances=1,scale=w=$width:h=$height:flags=lanczos:threads=0"

    if [ "$4" = "letterbox" ]; then

        filter="$filter:force_original_aspect_ratio=decrease,pad=$width:$height:-1:-1:color=black"

    elif [ "$4" = "crop" ]; then

        filter="$filter:force_original_aspect_ratio=increase,crop=$width:$height"
    fi
else
    echo "Video size is already ${width}x${height}, no upscaling will be applied."
fi

if [ $# -gt 6 ]; then

    input_fps=$(getFps "$1")

    target_fps=$(printf "%.3f" "$7")

    if [ "$input_fps" = "$target_fps" ]; then

        echo "Video framerate is already $7, no interpolation will be applied."

        fps="-fps_mode:v passthrough"
    else
        echo "Input framerate: $input_fps, target: $7"

        filter="tvai_fi=model=$interpolation:slowmo=1:rdt=0.01:fps=$7:device=0:vram=1:instances=1,$filter"

        fps="-fps_mode:v cfr"
    fi
else
    fps="-fps_mode:v passthrough"
fi

echo "Filter [$filter]"

movflags="frag_keyframe+empty_moov+delay_moov+use_metadata_tags+write_colr"

metadata="videoai=Enhanced using $3; mode: auto; revert compression at 0; recover details at 0; sharpen at 0; reduce noise at 0; dehalo at 0; anti-alias/deblur at 0; focus fix Off; and recover original detail at 20. Changed resolution to ${width}x${height}"

# FIXME Topaz: Sometimes the output duration differs from the source. This is usually related to
#              fps interpolation but not always. This does not seem to happen with the UI.
#
# NOTE: This line comes from Topaz Video AI (right click > FFmpeg command).

if [ -n "$filter" ]; then

    "$ffmpeg" -y "-hide_banner" "-nostdin" "-i" "$1" \
    "-sws_flags" "spline+accurate_rnd+full_chroma_int" -filter_complex "$filter" "-level" "3" \
    "-c:v" "ffv1" "-pix_fmt" "$yuv" "-slices" "4" "-slicecrc" "1" "-g" "1" "-an" \
    "-map_metadata" "0" "-map_metadata:s:v" "0:s:v" $fps "-map" "0:s?" "-c:s" "copy"  \
    "-movflags" "$movflags" "-bf" "0" "-metadata" "$metadata" "temp.mkv"
else
    "$ffmpeg" -y "-hide_banner" "-nostdin" "-i" "$1" \
    "-sws_flags" "spline+accurate_rnd+full_chroma_int" "-level" "3" \
    "-c:v" "ffv1" "-pix_fmt" "$yuv" "-slices" "4" "-slicecrc" "1" "-g" "1" "-an" \
    "-map_metadata" "0" "-map_metadata:s:v" "0:s:v" $fps "-map" "0:s?" "-c:s" "copy"  \
    "-movflags" "$movflags" "-bf" "0" "-metadata" "$metadata" "temp.mkv"
fi

if [ "$4" = "wide" ]; then

    mv temp.mkv temp2.mkv

    sh wide.sh "temp2.mkv" "temp.mkv" lossless

    rm temp2.mkv
fi

# NOTE: This part makes sure that we have the proper duration and encoding.

if [ $# -lt 8 ]; then

    codec="-codec:v libx265 -crf 12 -preset slow"

    sh resize.sh "temp.mkv" "$1" "temp.mp4" 0 0 "$codec"

elif [ "$8" = "lossless" ]; then

    codec="-codec:v libx265 -preset veryslow -x265-params lossless=1 -pix_fmt $yuv"

    sh resize.sh "temp.mkv" "$1" "temp.mp4" 0 0 "$codec"
else
    sh resize.sh "temp.mkv" "$1" "temp.mp4" 0 0 "$8"
fi

#--------------------------------------------------------------------------------------------------

# NOTE: This line comes from Topaz Video AI logs (Help > logging > Open current log).
"$ffmpeg" -y "-hide_banner" "-nostdin" "-i" "temp.mp4" "-i" "$1" "-c:v" "copy" "-c:a" "copy" \
"-map" "0:v" "-map" "1:a?" "-map_metadata" "0" "-movflags" "use_metadata_tags" \
"-fps_mode" "passthrough" "$2"

#--------------------------------------------------------------------------------------------------
# Check
#--------------------------------------------------------------------------------------------------

durationA=$(getDuration "$1")
durationB=$(getDuration "$2")

echo "Expected duration: $durationA"
echo "Actual duration:   $durationB"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

rm temp.mkv
rm temp.mp4
