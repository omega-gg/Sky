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

if [ $# -lt 3 -o $# -gt 6 ] \
   || \
   [ $3 != "prob-4" -a $3 != "iris-3" -a $3 != "rhea-1" ] \
   || \
   [ $# -gt 3 -a "$4" != "default" -a "$4" != "letterbox" -a "$4" != "crop" -a "$4" != "wide" ]; then

    echo "Usage: topaz <input> <output> <prob-4 | iris-3 | rhea-1> [default | letterbox | crop] [width = $width] [height = $height]"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $# = 5 ]; then

    width="$5"

elif [ $# = 6 ]; then

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

filter="tvai_up=model=$3:scale=0:w=$width:h=$height:preblur=0:noise=0:details=0:halo=0:blur=0:compression=0:estimate=8:blend=0.2:device=0:vram=1:instances=1,scale=w=$width:h=$height:flags=lanczos:threads=0"

if [ "$4" = "letterbox" ]; then

    filter="$filter:force_original_aspect_ratio=decrease,pad=$width:$height:-1:-1:color=black"

elif [ "$4" = "crop" ]; then

    filter="$filter:force_original_aspect_ratio=increase,crop=$width:$height"
fi

movflags="frag_keyframe+empty_moov+delay_moov+use_metadata_tags+write_colr"

metadata="videoai=Enhanced using $3; mode: auto; revert compression at 0; recover details at 0; sharpen at 0; reduce noise at 0; dehalo at 0; anti-alias/deblur at 0; focus fix Off; and recover original detail at 20. Changed resolution to ${width}x${height}"

duration=$(getDuration "$1")

# FIXME Topaz: Sometimes the output duration differs from the source so we enforce it manually.
#              This does not seem to happen with the UI.
# NOTE: This line comes from Topaz Video AI (right click > FFmpeg command).
"$ffmpeg" -y "-hide_banner" "-nostdin" "-nostats" "-i" "$1" \
"-sws_flags" "spline+accurate_rnd+full_chroma_int" "-filter_complex" "$filter" \
"-c:v" "hevc_nvenc" "-profile:v" "main" "-pix_fmt" "yuv420p" "-b_ref_mode" "disabled" \
"-tag:v" "hvc1" "-g" "30" "-preset" "p7" "-tune" "hq" "-rc" "constqp" "-qp" "17" \
"-rc-lookahead" "20" "-spatial_aq" "1" "-aq-strength" "15" "-b:v" "0" "-an" "-map_metadata" "0" \
"-map_metadata:s:v" "0:s:v" "-fps_mode:v" "passthrough" "-movflags" "$movflags" "-bf" "0" \
"-metadata" "$metadata" "-t" "$duration" "temp.mp4"

#--------------------------------------------------------------------------------------------------
# FIXME Topaz: Sometimes the output duration differs from the source. This does not seem to happen
#              with the UI.

#durationA=$(getDuration "$1")
#durationB=$(getDuration "temp.mp4")

#if [ $durationA != $durationB ]; then

#    echo "topaz: Video output differs from source ($durationB). Restarting..."

#    rm temp.mp4

#    if [ $# = 4 ]; then

#        sh topaz.sh "$1" "$2" "$3" "$4"
#    else
#        sh topaz.sh "$1" "$2" "$3"
#    fi

#    exit 0
#fi

#--------------------------------------------------------------------------------------------------

# NOTE: This line comes from Topaz Video AI logs (Help > logging > Open current log).
"$ffmpeg" -y "-hide_banner" "-nostdin" "-i" "temp.mp4" "-i" "$1" "-c:v" "copy" "-map" "0:v" "-an" \
"-map_metadata" "0" "-movflags" "use_metadata_tags" "-fps_mode" "passthrough" "$2"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

rm temp.mp4
