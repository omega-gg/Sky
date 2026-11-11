# Sky: the player engines on macOS, FFmpeg against libVLC

## Context

`backend-ffmpeg.md` measures the FFmpeg engine on Windows, where AV1 decodes on the GPU. This
is the same comparison on Apple Silicon, where it does not: an M1 has no AV1 decoder, and
YouTube serves AV1, so both engines decode it in software.

AV1 in hardware is a matter of the chip, not of the architecture. The media engine of an M1 or
an M2 decodes H.264, HEVC and VP9, and an M3 or later adds AV1. The player asks for
`videotoolbox_vld` first and falls back on the first frame either way, so the same build picks
the hardware up on an M3 with nothing to change. On this M1, every run below, x86_64 and arm64
alike, logs `Your platform doesn't support hardware accelerated AV1 decoding`.

Measured on 2026-11-11, on a Mac mini M1 (8 cores, 8 GB), macOS 15.5, Qt 6.10.1, FFmpeg 9.0.2
against VLC 3.0.24, both built from the same commit through `build.sh macOS`.

**Every desktop build is x86_64 under Rosetta**, because `build.sh` pins
`QMAKE_APPLE_DEVICE_ARCHS=x86_64` for libVLC, whose macOS build is intel only. Our FFmpeg
libraries are universal, so an FFmpeg build does not need that pin. It matters a lot: see
**Native arm64**.

## Measured

1280x720 window, paused seeks, then a timed playback. The rate is the media time the player
reports over the wall time it took, so 1.000 is real time. The CPU is the process CPU over the
playback window, sampled from `ps` every 250 ms.

| YouTube 1080p60 AV1 | FFmpeg | VLC 3.0.24 |
|---|---|---|
| Source to first frame | 673 ms | 1045 ms |
| Playback rate | 0.979 | 0.996 |
| CPU | 156% | 162% |
| Peak RSS | 227 MB | 311 MB |

| YouTube 4K60 AV1 | FFmpeg | VLC 3.0.24 |
|---|---|---|
| Source to first frame | 698 ms | 949 ms |
| Playback rate | 0.986 | 0.992 |
| CPU | 452% | 473% |
| Peak RSS | 473 MB | 633 MB |

| YouTube 4K60 HDR, AV1 10 bits | FFmpeg | VLC 3.0.24 |
|---|---|---|
| Source to first frame | 1210 ms | 1568 ms |
| Playback rate | 0.986 | 0.996 |
| CPU | 376% | 388% |
| Peak RSS | 706 MB | 837 MB |

| A local 1080p24 file | FFmpeg | VLC 3.0.24 |
|---|---|---|
| Source to first frame | 406 ms | 1059 ms |
| Playback rate | 0.969 | 0.988 |
| CPU | 13.1% | 17.0% |
| Peak RSS | 234 MB | 204 MB |

The two engines play equally smoothly: the rates sit within the run to run spread, VLC a hair
ahead. FFmpeg opens a source in about two thirds of the time, holds a sixth to a quarter less
memory on the heavy sources, and takes a few percent less CPU for each second of media it
decodes. Neither engine quite holds 4K AV1 under Rosetta; both fall a little behind the clock.

The HDR source costs less than the SDR one because it is the shorter, calmer clip, not because
10 bits are cheaper: the two 4K rows are different videos and only compare down their column.
Neither engine maps HDR to SDR, so its picture comes out flat, as on Windows.

Other Qt render backends cost what they always do, with no difference between the engines
(FFmpeg, local file): Metal 13.1%, OpenGL 17.5%, the software renderer 15.9%, each with the
same picture.

## Native arm64

An FFmpeg build has no reason to be x86_64. Built for arm64 (`QMAKE_APPLE_DEVICE_ARCHS=arm64`,
`SK_NO_TORRENT`, since libtorrent and Boost ship intel only), the same engine on the same
sources:

| FFmpeg | x86_64 (Rosetta) | arm64 |
|---|---|---|
| YouTube 1080p60, CPU | 156% | **69%** |
| YouTube 4K60, CPU | 452% | **222%** |
| 4K60 playback rate | 0.986 | 0.977 |
| 4K60 peak RSS | 473 MB | 527 MB |

Software AV1 decoding costs **half the CPU** natively: 4K60 goes from about 4.5 cores to 2.2 of
the 8. dav1d's hand written SIMD runs translated under Rosetta, which is where the rest goes.
This is not a hardware path: the arm64 build falls back to software AV1 exactly as the x86_64
one does, it just runs its NEON code natively. For reference, 4K60 AV1 on the GPU takes 41 to
51% on the Windows laptop of `backend-ffmpeg.md`.

An M1 does decode VP9 in hardware, and YouTube offers VP9 beside AV1. Preferring it on Apple
hardware older than an M3 is worth a measurement: it is the only way to reach the GPU there.

Nothing of this is in the tree: the measurement was taken with local changes, reverted since.
What an arm64 build needs, whenever we get to it:

- **`build.sh` drops the x86_64 pin** for an FFmpeg build, or takes the architecture as an
  argument. It is there for libVLC alone.
- **`-msse` stops applying to arm64**, which clang refuses outright. `CAN_COMPILE_SSE2` only
  guards code in `WBackendVlc`, so both the define and the flag belong to the VLC engine rather
  than to the platform, which removes the need to test the architecture at all. Careful with the
  order: `SK_VLC` only exists after `Sk.pri` is included, and today both lines sit above it.
- **The torrent libraries stop being linked** with `SK_NO_TORRENT`, the way the sources already
  are. They would follow the `ENGINE` idiom well, a `TORRENT` variable emptied by the define.
- **libtorrent and Boost get an arm64 build** in `3rdparty`, or a native build stays without
  torrent support. They are the only intel only libraries left: Qt and our FFmpeg are universal
  already.

## Pictures

Checked by saving the backend's frame (`core.saveFrame`) and an item grab through the scene
graph (`core.saveItemShot`), then comparing them:

- every source decodes at its real size: 1920x1080, 3840x2160, and 3840x2160 for the HDR one;
- the two engines land on the same frame for the same time, within the rounding of the YUV to
  RGB conversion (PSNR 62 to 67 dB on 1080p, max difference 1 of 255);
- a seek to the same time twice gives the same frame, to the pixel (PSNR 99);
- Metal, OpenGL and the software renderer draw the same picture (mean luma 155.60, 155.55,
  155.55 on the same frame);
- an HDR frame comes through flat and dark, as expected with no tone mapping, and not broken.

**A seek reports its time before its picture exists.** On a network source the player answers
with the target time in under 10 ms while the frame is still being fetched: a frame read 150 ms
after the time was already correct showed the *previous* picture, on both engines. Reading it
1.2 s later gave the right one every time. Anything that samples a frame right after a seek, a
test or a thumbnail, has to wait for the picture rather than trust the time.

## macOS specifics

- **The application bundle.** On Windows and Linux `DESTDIR=bin` puts the executable beside the
  libraries. On macOS it lands in `bin/sky.app/Contents/MacOS`, three levels down, and only
  what `QMAKE_POST_LINK` copies is there. It copies the engine (the `sk-` libraries, or libvlc
  with its plugins) but not `libtorrent-rasterbar.dylib` nor `libboost_system.dylib`, although
  it rewrites both to `@loader_path`: a build that was not deployed does not start until they
  are copied in by hand.
- **The shaders come with `all`.** `configure.sh` empties `bin`, and only
  `generate.sh <platform> all` (or `deploy`) fills it back with the content, the shaders
  included. A plain `build.sh <platform>` after a configure leaves `bin/shaders` missing and
  the video never draws, with `Empty shader passed to graphics pipeline` on every frame. Run
  `build.sh <platform> all` once after a configure, as it is meant to be.
- **Do not re-sign the bundle.** `codesign --force` on it breaks Rosetta's AOT cache for
  libvlccore and the application then fails to start with
  `rosetta error: Attachment of code signature supplement failed`.
- **The first run after a link is slow.** Rosetta translates the binary, and libvlc's 464
  plugins, on first use: 45 s before the window appears is normal, and it is not a hang.
  Warm up a build before timing anything.
- **`environment.sh <player> all` fails.** It forwards the player to `3rdparty/environment.sh`,
  which only knows the compiler, the Qt version and the mobile target. Set the engine in each
  repository instead: `sh environment.sh vlc3` in Sky-runtime and in Sky.

## VLC on macOS

Two things that only happen with the VLC engine, both still open:

- **It does not exit.** The application finishes its shutdown, prints `Done`, and the process
  stays in uninterruptible exit (`ps` state `UE`, 0% CPU, holding its memory). It cannot be
  killed, only rebooted away, and enough of them wedge later launches. Every FFmpeg run exited
  cleanly. Worth noting that `CONTROLLERMEDIA_THREAD` is not defined on macOS, so the engine
  and its players live on the main thread there and nowhere else.
- **A bad URL hangs instead of failing.** `libvlc_media_new_location` wants a URL, and
  `sky-player` wraps a bare path as `file:///...` before handing it over. Given a path that was
  not wrapped, VLC parses `/Users/x/v.mp4` as the demuxer `Users/x/v.mp4`, falls through to its
  RIST access module, which binds `0.0.0.0:0` and retries forever: no error, no state change,
  a log growing by megabytes. The FFmpeg engine opens the bare path.

## Running it again

The benchmark is a self contained `.sky` script driving its own `Player`: it loads a source,
seeks a few times saving a frame and a scene graph grab at each, plays for a few seconds while
sampling `currentTime` against the wall clock, and quits through `window.close()`. Two notes
for whoever writes the next one:

- an item grab pumps the event loop, so a polling timer re-enters itself unless it is guarded;
- never `SIGKILL` the application. That is what leaves the wedged processes above, with either
  engine. `SIGTERM` is handled and exits cleanly.
