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

cp "$solid"/xmark.svg           icons/window-close.svg
cp "$solid"/window-maximize.svg icons
cp "$solid"/window-restore.svg  icons
cp "$solid"/window-minimize.svg icons

cp "$solid"/check.svg icons

cp "$solid"/chevron-right.svg icons
cp "$solid"/chevron-down.svg  icons

cp "$solid"/backward-step.svg icons/backward.svg
cp "$solid"/forward-step.svg  icons/forward.svg
cp "$solid"/play.svg          icons

cp "$solid"/rss.svg icons

cp "$solid"/house.svg icons
