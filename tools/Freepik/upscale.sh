#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

api="https://api.freepik.com/v1/ai/image-upscaler"

freepik_key="$FREEPIK_KEY"

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

if [ $# != 4 -a $# != 5 ]; then

    echo "Usage: upscale <image input> <image output> <scale_factor> <optimized_for> [creativity]"

    exit 1
fi

if [ -z "$freepik_key" ]; then

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

cat > data.txt <<EOF
{
    "image": "$(base64 $1)",
    "scale_factor": "$3",
    "optimized_for": "$4",
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
