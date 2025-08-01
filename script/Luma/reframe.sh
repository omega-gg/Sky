#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------
# https://docs.lumalabs.ai/reference/reframevideo

api="https://api.lumalabs.ai/dream-machine/v1/generations/video/reframe"

api_get="https://api.lumalabs.ai/dream-machine/v1/generations"

luma_key="$LUMA_KEY"

model="ray-2"

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

if [ $# != 3 ]; then

    echo "Usage: reframe <video input> <video output> <ratio>"

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
# Run
#--------------------------------------------------------------------------------------------------

cat > data.txt <<EOF
{
    "generation_type": "reframe_video",
    "model": "$model",
    "media": {
        "url": "$(getData "$1")"
    },
    "aspect_ratio": "$3"
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

curl -L -o "$2" "$url"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

removeData "$1"
