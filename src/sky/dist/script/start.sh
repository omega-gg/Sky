#!/bin/sh
set -e

echo "Starting sky..."

PWD=$(dirname "$(readlink -f "$0")")

export LD_LIBRARY_PATH="$PWD"

export QT_PLUGIN_PATH="$PWD"

#export QT_DEBUG_PLUGINS=1

"$PWD/sky"
