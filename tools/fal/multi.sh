#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------
# https://fal.ai/models/fal-ai/flux-pro/kontext/max/multi/api

api="https://fal.run/fal-ai/flux-pro/kontext/max/multi"

fal_key="$FAL_KEY"

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
    curl --request POST --url "$api"               \
         --header "Content-Type: application/json" \
         --header "Authorization: Key $fal_key" \
         --data @data.txt
}

getData()
{
    if [ -n "$AWS_ACCESS_KEY_ID" ]; then

        local filename=$(basename -- "$1")

        aws s3 cp --quiet --acl public-read "$1" "$s3/$s3_path/$filename"

        echo "$s3_url/$filename"
    else
        local mime=$(file --brief --mime-type "$1")

        echo "data:$mime;base64,$(base64 -w 0 "$1")"
    fi
}

removeData()
{
    if [ -z "$AWS_ACCESS_KEY_ID" ]; then

        return
    fi

    local filename=$(basename -- "$1")

    aws s3 rm --quiet "$s3/$s3_path/$filename"
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 3 -o $# -gt 4 ]; then

    echo "Usage: multi <image output> <prompt> <image 1> [image 2]"

    exit 1
fi

if [ -z "$fal_key" ]; then

    echo "multi: FAL_KEY is missing in the environment."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

if [ $# = 3 ]; then

    images='[
        "'"$(getData "$3")"'"
    ]'

elif [ $# = 4 ]; then

    images='[
        "'"$(getData "$3")"'",
        "'"$(getData "$4")"'"
    ]'
fi

cat > data.txt <<EOF
{
    "image_urls": $images,
    "output_format": "png",
    "prompt": "$2"
}
EOF

cat data.txt

data=$(run)

echo "$data"

rm data.txt

url=$(echo "$data" | grep -o '"url":"[^"]*' | grep -o '[^"]*$')

curl -L -o "$1" "$url"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

if [ $# = 3 ]; then

    removeData "$3"

elif [ $# = 4 ]; then

    removeData "$3"
    removeData "$4"
fi
