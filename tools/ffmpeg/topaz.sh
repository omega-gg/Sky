#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

bin="$PWD/bin"

topaz="/c/Program Files/Topaz Labs LLC/Topaz Video AI"

model="/c/ProgramData/Topaz Labs LLC/Topaz Video AI/models"

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 3 ] || [ $3 != "prob-4" -a $3 != "iris-3" ]; then

    echo "Usage: topaz <input> <output> <prob-4 | iris-3>"

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

ffmpeg="$topaz/ffmpeg"

export TVAI_MODEL_DIR="$model"

export TVAI_MODEL_DATA_DIR="$model"

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

"$ffmpeg" -hide_banner -nostdin -y -nostats -i "$1" -sws_flags \
spline+accurate_rnd+full_chroma_int -filter_complex \
tvai_up="model=$3:scale=0:w=3840:h=2160:preblur=0:noise=0:details=0:halo=0:blur=0:compression=0:estimate=8:blend=0.2:device=0:vram=1:instances=1,scale=w=3840:h=2160:flags=lanczos:threads=0" \
-c:v h264_nvenc -profile:v high -pix_fmt yuv420p -g 30 -preset p7 -tune hq -rc constqp -qp 18 \
-rc-lookahead 20 -spatial_aq 1 -aq-strength 15 -b:v 0 -an -map_metadata 0 -map_metadata:s:v 0:s:v \
-movflags frag_keyframe+empty_moov+delay_moov+use_metadata_tags+write_colr -bf 0 \
-metadata videoai="Enhanced using $3; mode: auto; revert compression at 0; recover details at 0; sharpen at 0; reduce noise at 0; dehalo at 0; anti-alias/deblur at 0; focus fix Off; and recover original detail at 20. Changed resolution to 3840x2160" \
"temp.mp4"

"$ffmpeg" -hide_banner -nostdin -y -i "temp.mp4" -i "$1" -c:v copy -map 0:v -an -map_metadata 0 \
-movflags use_metadata_tags -fps_mode passthrough "$2"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

rm temp.mp4
