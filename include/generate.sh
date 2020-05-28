#!/bin/sh
set -e

includeGenerator="../deploy/includeGenerator"

"$includeGenerator" ../../src/SkCore/src                              SkCore
"$includeGenerator" ../../src/SkCore/src/3rdparty/qtsingleapplication SkCore
"$includeGenerator" ../../src/SkGui/src                               SkGui
"$includeGenerator" ../../src/SkGui/src/3rdparty/qtsingleapplication  SkGui
"$includeGenerator" ../../src/SkMedia/src                             SkMedia
"$includeGenerator" ../../src/SkWeb/src                               SkWeb
"$includeGenerator" ../../src/SkTorrent/src                           SkTorrent
"$includeGenerator" ../../src/SkBackend/src                           SkBackend
