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

cp "$solid"/window-minimize.svg icons
cp "$solid"/window-maximize.svg icons
cp "$solid"/window-restore.svg  icons

cp "$solid"/xmark.svg icons/times.svg

cp "$solid"/caret-left.svg  icons
cp "$solid"/caret-right.svg icons

cp "$solid"/chevron-left.svg  icons
cp "$solid"/chevron-right.svg icons
cp "$solid"/chevron-up.svg    icons
cp "$solid"/chevron-down.svg  icons

cp "$solid"/expand.svg   icons/expand-arrows-alt.svg
cp "$solid"/compress.svg icons/compress-arrows-alt.svg

cp "$solid"/caret-down.svg icons

cp "$solid"/arrow-up-right-from-square.svg icons/external-link-alt.svg

cp "$solid"/play.svg  icons
cp "$solid"/pause.svg icons

cp "$solid"/backward-step.svg icons/backward.svg
cp "$solid"/forward-step.svg  icons/forward.svg

cp "$solid"/volume-off.svg  icons
cp "$solid"/volume-high.svg icons/volume-up.svg
