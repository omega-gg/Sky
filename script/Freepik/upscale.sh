#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------
# https://docs.freepik.com/api-reference/image-upscaler/post-image-upscaler

api="https://api.freepik.com/v1/ai/image-upscaler"

freepik_key="$FREEPIK_KEY"

scale_factor="2x"

optimized_for="standard"
# Available options:
# standard,
# soft_portraits,
# hard_portraits,
# art_n_illustration,
# videogame_assets,
# nature_n_landscapes,
# films_n_photography,
# 3d_renders,
# science_fiction_n_horror

creativity="0"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

run()
{
    curl --request POST --url "$api" --ssl-no-revoke \
         --header "Content-Type: application/json"   \
         --header "x-freepik-api-key: $freepik_key"  \
         --data @data.txt
}

get()
{
    curl --request GET --url "$api/$1" --ssl-no-revoke \
         --header "x-freepik-api-key: $freepik_key"
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 2 -o $# -gt 5 ]; then

    echo "Usage: upscale <image input> <image output>"
    echo "               [scale_factor = 2x] [optimized_for = standard] [creativity = 0]"
    echo ""
    echo "optimized_for: standard"
    echo "               soft_portraits"
    echo "               hard_portraits"
    echo "               art_n_illustration"
    echo "               videogame_assets"
    echo "               nature_n_landscapes"
    echo "               films_n_photography"
    echo "               3d_renders"
    echo "               science_fiction_n_horror"

    exit 1
fi

if [ -z "$freepik_key" ]; then

    echo "upscale: FREEPIK_KEY is missing in the environment."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

if [ $# -gt 2 ]; then

    scale_factor="$3"
fi

if [ $# -gt 3 ]; then

    optimized_for="$4"
fi

if [ $# -gt 4 ]; then

    creativity="$5"
fi

cat > data.txt <<EOF
{
    "image": "$(base64 "$1")",
    "scale_factor": "$scale_factor",
    "optimized_for": "$optimized_for",
    "creativity": "$creativity"
}
EOF

data=$(run)

echo "$data"

rm data.txt

id=$(echo "$data" | grep -o '"task_id":"[^"]*' | grep -o '[^"]*$')

while :
do
    sleep 10

    data=$(get "$id")

    echo "$data"

    status=$(echo "$data" | grep -o '"status":"[^"]*' | grep -o '[^"]*$')

    if [ "$status" != "COMPLETED" ]; then

        continue
    fi

    url=$(echo "$data" | grep -o '"generated":\["[^"]*' | grep -o '[^"]*$')

    break
done

curl -L -o "$2" "$url"
