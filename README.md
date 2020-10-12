<a href="http://omega.gg/Sky"><img src="dist/icon.png" alt="Sky kit" width="128px"></a>
---
[![azure](https://dev.azure.com/bunjee/Sky/_apis/build/status/omega-gg.Sky)](https://dev.azure.com/bunjee/Sky/_build)
[![travis](http://api.travis-ci.org/omega-gg/Sky.svg)](http://travis-ci.org/omega-gg/Sky)
[![appveyor](https://ci.appveyor.com/api/projects/status/86v4f4gv95u68w18?svg=true)](https://ci.appveyor.com/project/3unjee/sky)
[![LGPLv3](https://img.shields.io/badge/License-LGPLv3-blue.svg)](https://www.gnu.org/licenses/lgpl.html)

Sky is an Application Kit designed for Multimedia Software.<br>
With a focus on efficiency, scalability and simplicity.<br>
[omega](http://omega.gg/about) is building Sky kit to empower people.<br>

## Technology

Sky is built in C++ on the [Qt framework](http://github.com/qtproject).<br>
The GUI is powered by [QML](http://github.com/qtproject/qtdeclarative) and rendered with OpenGL.<br>
The video player is based on [VLC](http://github.com/videolan/vlc) and [libtorrent](http://en.wikipedia.org/wiki/libtorrent).<br>

## Platforms

- Windows XP and later.
- macOS 64 bit.
- Linux 32 bit and 64 bit.
- Android 32 bit and 64 bit.

## Requirements

- [Qt](http://download.qt.io/official_releases/qt) 4.8.0 / 5.5.0 or later.
- [VLC](http://download.videolan.org/pub/videolan/vlc) 2.2.0 or later.
- [libtorrent](http://github.com/arvidn/libtorrent/releases) 1.1.0 or later.
- [Boost](http://www.boost.org/users/download) 1.55.0 or later.
- [OpenSSL](http://www.openssl.org/source) / [Win32OpenSSL](http://slproweb.com/products/Win32OpenSSL.html) 1.0.1 or later.

On Windows:
- [MinGW](http://sourceforge.net/projects/mingw) or [Git for Windows](http://git-for-windows.github.io) with g++ 4.9.2 or later.

Recommended:
- [Qt Creator](http://download.qt.io/official_releases/qtcreator) 3.6.0 or later.

## 3rdparty

You can install third party libraries with:

    sh 3rdparty.sh <win32 | win64 | macOS | linux | android> [all]

## Configure

You can configure Sky with:

    sh configure.sh <win32 | win64 | macOS | linux | android> [clean]

## Build

You can build Sky with Qt Creator:
- Open [Sky.pro](Sky.pro).
- Click on "Build > Build all".

Or the build script:

    sh build.sh <win32 | win64 | macOS | linux | android> [all | deploy | tools | clean]

Or the console:

    qmake -r
    make (mingw32-make on Windows)

## Deploy

You can deploy Sky with:

    sh deploy.sh <win32 | win64 | macOS | linux | android> [tools | clean]

## License

Copyright (C) 2015 - 2020 Sky kit authors | http://omega.gg/Sky

### Authors

- Benjamin Arnaud aka [bunjee](http://bunjee.me) | <bunjee@omega.gg>

### GNU Lesser General Public License Usage

Sky kit may be used under the terms of the GNU Lesser General Public License version 3 as published
by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging of
this file. Please review the following information to ensure the GNU Lesser General Public License
requirements will be met: https://www.gnu.org/licenses/lgpl.html.

### Private License Usage

Sky kit licensees holding valid private licenses may use this file in accordance with the private
license agreement provided with the Software or, alternatively, in accordance with the terms
contained in written agreement between you and Sky kit authors. For further information contact us
at contact@omega.gg.
