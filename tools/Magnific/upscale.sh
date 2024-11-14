#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

api="https://api.freepik.com/v1/ai/image-upscaler"

#--------------------------------------------------------------------------------------------------
# Functions
#--------------------------------------------------------------------------------------------------

run()
{
    echo $(curl --request POST --url "$api" --ssl-no-revoke \
                --header "Content-Type: application/json"   \
                --header "x-freepik-api-key: $FREEPIK_KEY"  \
                --data @data.txt)
}

get()
{
    echo $(curl --request GET --url "$api/$1" --ssl-no-revoke \
                --header "x-freepik-api-key: $FREEPIK_KEY")
}

#--------------------------------------------------------------------------------------------------
# Syntax
#--------------------------------------------------------------------------------------------------

if [ $# != 4 -a $# != 5 ]; then

    echo "Usage: upscale <image input> <image output> <scale_factor> <optimized_for> [creativity]"

    exit 1
fi

if [ -z "$FREEPIK_KEY" ]; then

    echo "upscale: FREEPIK_KEY is missing in the environment."

    exit 1
fi

#--------------------------------------------------------------------------------------------------
# Run
#--------------------------------------------------------------------------------------------------

if [ $# = 5 ]; then

    creativity="$5"
else
    creativity="0"
fi

echo "{ \"image\": \"$(base64 $1)\", \"scale_factor\": \"$3\", \"optimized_for\": \"$4\", \"creativity\": \"$creativity\" }" >> data.txt

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

    if [ $status != "COMPLETED" ]; then

        continue
    fi

    url=$(echo "$data" | grep -o '"generated":\["[^"]*' | grep -o '[^"]*$')

    break
done

curl -L -o "$2" "$url"
