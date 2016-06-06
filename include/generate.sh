#!/bin/sh

includeGenerator="../deploy/includeGenerator"

"$includeGenerator" ../../src/SkCore/src                          SkCore
"$includeGenerator" ../../src/SkCore/3rdparty/qtsingleapplication SkCore
"$includeGenerator" ../../src/SkGui/src                           SkGui
"$includeGenerator" ../../src/SkMedia/src                         SkMedia
"$includeGenerator" ../../src/SkWeb/src                           SkWeb
"$includeGenerator" ../../src/SkTorrent/src                       SkTorrent
"$includeGenerator" ../../src/SkBackend/src                       SkBackend
