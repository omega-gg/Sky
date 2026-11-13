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

**The engine comparison below runs x86_64 under Rosetta**: until 2026-11-13 `build.sh` pinned
`QMAKE_APPLE_DEVICE_ARCHS=x86_64`, because our libVLC was intel only. The Qt 6 builds are
universal since, and so is every library they ship, libVLC included. Running natively matters a
lot: see **Native arm64**.

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

The Qt 6 build is universal: `build.sh` passes `QMAKE_APPLE_DEVICE_ARCHS=x86_64 arm64` and every
file compiles once for both slices. A Mac runs the native slice; `arch -x86_64` forces the
other. The same bundle on the same sources (2026-11-13, FFmpeg, Metal, one run each):

| FFmpeg, same bundle | x86_64 (Rosetta) | arm64 |
|---|---|---|
| YouTube 1080p60 AV1, CPU | 154% | **80%** |
| YouTube 4K60 AV1, CPU | 476% | **228%** |
| YouTube 4K60 HDR, AV1 10 bits, CPU | 376% | **265%** |
| Playback rate, all three | 0.97 | 0.97 |
| 4K60 peak RSS | 484 MB | 512 MB |

Software AV1 decoding costs **half the CPU** natively: 4K60 goes from about 4.75 cores to 2.3 of
the 8. dav1d's hand written SIMD runs translated under Rosetta, which is where the rest goes.
The x86_64 column matches the one measured on 2026-11-11, so both tables compare.

This is not a hardware path: the arm64 slice falls back to software AV1 exactly as the x86_64
one does, it just runs its NEON code natively. For reference, 4K60 AV1 on the GPU takes 41 to
51% on the Windows laptop of `backend-ffmpeg.md`.

The HDR source gains less, a third instead of a half, and natively costs more than the SDR one
where under Rosetta it costs less. The 10 bits path may have less NEON behind it than the 8 bits
one, but that is a single run: measure it again before relying on it.

An M1 does decode VP9 in hardware, and YouTube offers VP9 beside AV1. Preferring it on Apple
hardware older than an M3 is worth a measurement: it is the only way to reach the GPU there.

**A VLC build is untested on arm64.** `CAN_COMPILE_SSE2` is still defined for every slice, and
in `WBackendVlc` it selects the SSE2 frame conversion ahead of the NEON one, so the arm64 slice
of a VLC build will most likely not compile. The FFmpeg build does not include that code, and
clang accepts the `-msse` flag in a universal compile.

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

- **Run `deploy/sky.app`.** `build.sh <platform> all` runs the whole chain, 3rdparty, Sky and
  its tools, then `build.sh <platform> deploy`, which compiles the content, shaders included,
  into the resources of a self contained `deploy/sky.app`. That is the build to run and to
  measure.
- **`bin/sky.app` is not complete.** On Windows and Linux `DESTDIR=bin` puts the executable
  beside the libraries. On macOS it lands in `bin/sky.app/Contents/MacOS`, three levels down,
  and only what `QMAKE_POST_LINK` copies is there: the engine (the `sk-` libraries, or libvlc
  with its plugins) but not `libtorrent-rasterbar.dylib`, although it rewrites it to
  `@loader_path`. Nor does a plain build fill `bin` with the content: without the shaders the
  video never draws, with `Empty shader passed to graphics pipeline` on every frame.
- **Under Rosetta, do not re-sign the bundle.** `codesign --force` on it breaks Rosetta's AOT
  cache for libvlccore and the application then fails to start with
  `rosetta error: Attachment of code signature supplement failed`.
- **Under Rosetta, the first run after a link is slow.** Rosetta translates the binary, and
  libvlc's 464 plugins, on first use: 45 s before the window appears is normal, and it is not a
  hang. Warm up a build before timing anything.
- **`environment.sh <player> all` fails.** It forwards the player to `3rdparty/environment.sh`,
  which only knows the compiler, the Qt version and the mobile target. Set the engine in each
  repository instead: `sh environment.sh vlc3` in Sky-runtime and in Sky.

## Shutdown and grabs

Two bugs that are not about the engines, found by running the benchmark over and over:

- **An abort on exit.** Sky-runtime never deleted its application object, so Qt released the
  main thread storage from the static destructors, after the font cache and the QML animation
  timer were already gone: `QThreadStorage: entry 3 destroyed before end of thread`, then
  `entry 2`, on every run. Nothing in that is specific to macOS, only measured there. On macOS that warning goes to the unified
  logger, which is torn down by then as well, and the process aborts in `free_tiny_botch` about
  one exit in four. `main` deletes the application after `exec()` now: no warning, no abort.
- **A grab of a hidden window hangs.** `takeItemShot` waits for `grabToImage` in a
  `processEvents()` loop, and a window that is covered or minimized does not render, so the
  grab never completes and the loop spins at 100% CPU forever; asking the window for 200 more
  frames does not help. It gives up after 3 s now and returns an empty picture. Testing
  `isExposed()` first is not an option: it reads false on windows that still render.

## VLC on macOS

Two things that only happen with the VLC engine, both still open:

- **It does not exit.** The application finishes its shutdown, prints `Done`, and the process
  stays in uninterruptible exit (`ps` state `UE`, 0% CPU, holding its memory). It cannot be
  killed, only rebooted away, and enough of them wedge later launches. No FFmpeg run ever
  wedged. Worth noting that `CONTROLLERMEDIA_THREAD` is not defined on macOS, so the engine
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
- keep the window in view: a covered window renders nothing, and its grabs fail;
- never `SIGKILL` the application. That is what leaves the wedged processes above, with either
  engine. `SIGTERM` is handled and exits cleanly.
