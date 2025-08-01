#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------
# https://docs.lumalabs.ai/reference/creategeneration

api="https://api.lumalabs.ai/dream-machine/v1/generations/video"

api_get="https://api.lumalabs.ai/dream-machine/v1/generations"

luma_key="$LUMA_KEY"

model="ray-2"

ratio="16:9"

resolution="1080p"

#--------------------------------------------------------------------------------------------------
# Amazon S3

# NOTE: This script depends on:
#       AWS_ACCESS_KEY_ID
#       AWS_SECRET_ACCESS_KEY
#       AWS_DEFAULT_REGION

s3="s3://omega.gg"

s3_path="upload"

s3_url="https://s3-eu-west-1.amazonaws.com/omega.gg/$s3_path"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

run()
{
    curl --request POST --url "$api" --ssl-no-revoke \
         --header "Content-Type: application/json" \
         --header "Authorization: Bearer $luma_key" \
         --data @data.txt
}

get()
{
    curl --request GET --url "$api_get/$1" --ssl-no-revoke \
         --header "Authorization: Bearer $luma_key"
}

getData()
{
    local filename=$(basename -- "$1")

    aws s3 cp --quiet --acl public-read "$1" "$s3/$s3_path/$filename"

    echo "$s3_url/$filename"
}

removeData()
{
    local filename=$(basename -- "$1")

    aws s3 rm --quiet "$s3/$s3_path/$filename"
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 3 -o $# -gt 5 ]; then

    echo "Usage: startEnd <frame start> <frame end> <output>"
    echo "                [ratio = 16:9]"
    echo "                [resolution = 1080p]"

    exit 1
fi

if [ -z "$luma_key" ]; then

    echo "reframe: LUMA_KEY is missing in the environment."

    exit 1
fi

if [ -z "$AWS_ACCESS_KEY_ID" ]; then

    echo "reframe: AWS_ACCESS_KEY_ID is missing in the environment."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------

if [ $# -gt 3 ]; then

    ratio="$4"
fi

if [ $# -gt 4 ]; then

    resolution="$5"
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

cat > data.txt <<EOF
{
    "model": "ray-2",
    "keyframes": {
            "frame0": {
            "type": "image",
            "url": "$(getData "$1")"
        },
            "frame1": {
            "type": "image",
            "url": "$(getData "$2")"
        }
    },
    "aspect_ratio": "$ratio",
    "resolution": "$resolution"
}
EOF

cat data.txt

data=$(run)

echo "$data"

rm data.txt

id=$(echo "$data" | grep -o '"id":"[^"]*' | grep -o '[^"]*$')

while :
do
    sleep 10

    data=$(get "$id")

    echo "$data"

    status=$(echo "$data" | grep -o '"state":"[^"]*' | grep -o '[^"]*$')

    if [ "$status" != "completed" ]; then

        continue
    fi

    url=$(echo "$data" | grep -o '"video":"[^"]*' | grep -o '[^"]*$')

    break
done

curl -L -o "$3" "$url"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

removeData "$1"
removeData "$2"
