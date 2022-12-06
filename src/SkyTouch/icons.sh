#!/bin/sh
set -e

#--------------------------------------------------------------------------------------------------
# Settings
#--------------------------------------------------------------------------------------------------

font="../../../assets/3rdparty/fontawesome/6"

solid="$font/solid"

#--------------------------------------------------------------------------------------------------
# Copy
#--------------------------------------------------------------------------------------------------

cp "$solid"/xmark.svg icons/times.svg

cp "$solid"/window-maximize.svg icons
cp "$solid"/window-restore.svg  icons
cp "$solid"/window-minimize.svg icons

cp "$solid"/check.svg icons

cp "$solid"/chevron-right.svg icons
cp "$solid"/chevron-down.svg  icons

cp "$solid"/backward.svg icons
cp "$solid"/forward.svg icons
cp "$solid"/play.svg icons

cp "$solid"/rss.svg icons
