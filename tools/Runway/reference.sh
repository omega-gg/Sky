#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------
# https://docs.dev.runwayml.com/api/#tag/Start-generating/paths/~1v1~1text_to_image/post

api="https://api.dev.runwayml.com/v1/text_to_image"

api_status="https://api.dev.runwayml.com/v1/text_to_image"

runway_key="$RUNWAY_KEY"

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
    curl --request POST --url "$api"                  \
         --header "Content-Type: application/json"    \
         --header "Authorization: Bearer $runway_key" \
         --header "X-Runway-Version: 2024-11-06"      \
         --data @data.txt
}

get()
{
    curl --request GET --url "https://api.dev.runwayml.com/v1/tasks/$1" \
         --header "Authorization: Bearer $runway_key"                   \
         --header "X-Runway-Version: 2024-11-06"
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

if [ $# -lt 2 -o $# -gt 5 ]; then

    echo "Usage: generate <image output> <prompt> [image 1] [image 2] [image 3]"

    exit 1
fi

if [ -z "$runway_key" ]; then

    echo "upscale: RUNWAY_KEY is missing in the environment."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

if [ $# = 3 ]; then

    references='"referenceImages": [
        { "tag": "ref1", "uri": "'"$(getData "$3")"'" }
    ]'

elif [ $# = 4 ]; then

    references='"referenceImages": [
        { "tag": "ref1", "uri": "'"$(getData "$3")"'" },
        { "tag": "ref2", "uri": "'"$(getData "$4")"'" }
    ]'

elif [ $# = 5 ]; then

    references='"referenceImages": [
        { "tag": "ref1", "uri": "'"$(getData "$3")"'" },
        { "tag": "ref2", "uri": "'"$(getData "$4")"'" },
        { "tag": "ref3", "uri": "'"$(getData "$5")"'" }
    ]'
fi

echo "{ \"model\": \"gen4_image\", \"promptText\": \"$2\", \"ratio\": \"1920:1080\", $references }" > data.txt

cat data.txt

data=$(run)

echo "$data"

rm data.txt

id=$(echo "$data" | grep -o '"id":"[^"]*' | grep -o '[^"]*$')

while :
do
    sleep 5

    data=$(get "$id")

    echo "$data"

    status=$(echo "$data" | grep -o '"status":"[^"]*' | grep -o '[^"]*$')

    if [ "$status" != "SUCCEEDED" ]; then

        continue
    fi

    url=$(echo "$data" | grep -o '"output":\["[^"]*' | grep -o '[^"]*$')

    break
done

curl -L -o "$1" "$url"

#--------------------------------------------------------------------------------------------------
# Clean
#--------------------------------------------------------------------------------------------------

if [ $# = 3 ]; then

    removeData "$3"

elif [ $# = 4 ]; then

    removeData "$3"
    removeData "$4"

elif [ $# = 5 ]; then

    removeData "$3"
    removeData "$4"
    removeData "$5"
fi
