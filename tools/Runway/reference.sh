#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

api="https://api.dev.runwayml.com/v1/text_to_image"

api_status="https://api.dev.runwayml.com/v1/text_to_image"

runway_key="$RUNWAY_KEY"

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
    echo "data:image/jpg;base64,$(base64 -w 0 "$1")"
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# -lt 2 -o $# -gt 5 ]; then

    echo "Usage: generate <prompt> <image output> [image 1] [image 2] [image 3]"

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

echo "{ \"model\": \"gen4_image\", \"promptText\": \"$1\", \"ratio\": \"1920:1080\", $references }" > data.txt

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

curl -L -o "$2" "$url"
