# Sky: the FFmpeg player backend, instead of libVLC (Qt 6)

## Context

Seeking an online video with VLC 3 is not exact:

- with VLC's FFmpeg demuxer, a paused seek can leave a stale picture or land a few frames off;
- with VLC's own MP4 demuxer, any HTTP source ends about 3.6 s early. This was measured through
  a local proxy and a local HTTP server: it comes from VLC's HTTP path, not from the network.

VLC's input queue also overflowed under seek spam, and its stop can deadlock. None of this can
be fixed from Sky.

`WBackendFfmpeg` plays on our own FFmpeg, on Qt 6. It replaces libVLC, chosen at build time:

- a build has one engine, FFmpeg or VLC, never both;
- FFmpeg is the default on Qt 6, VLC is the engine before Qt 6;
- Qt keeps its own FFmpeg for Qt Multimedia, untouched.

The priorities are exact seeks, decoding and rendering performance, a fast start, portability,
and clean simple code shaped like `WBackendVlc`. Everything VLC does is supported except
Chromecast.

## Choosing the engine

- **The environment flag:** `player="ffmpeg|vlc3|vlc4"` in the scripts, set with
  `sh environment.sh ffmpeg` (or `vlc3`, `vlc4`) in Sky, Sky-runtime and MotionBox.
- **What configure installs:** `configure.sh` copies one engine into Sky: `include/ffmpeg` and
  the `sk-*` libraries, or `include/vlc` and libvlc. It deletes the other every time, and
  `deploy.sh` ships the same one.
- **The define:** `src/Sk.pri` defines `SK_VLC` when Qt is older than 6 or `include/vlc` is
  there, so a build made from Qt Creator gets the same engine. `Sk.h` turns it into the class
  guards: with `SK_VLC`, `SK_NO_BACKENDFFMPEG` and `SK_NO_FFMPEGPLAYER`; without it,
  `SK_NO_VLCENGINE`, `SK_NO_VLCPLAYER`, `SK_NO_VLCAUDIO` and `SK_NO_BACKENDVLC`.
- **The sources:** each engine's files are in its module (`vlc.pri`, `ffmpeg.pri`), included by
  the define in `SkMedia.pro` and in the applications that build Sky's sources (MotionBox,
  Sky-runtime). The shared ones (`WBackendManager`, `WBackendYuv`) are in `media.pri`.
- **QML:** the applications' `content/generate.sh` set a `VLC` deployer define with VLC, so QML
  can pick `BackendVlc` or `BackendFfmpeg` (`SkyPresentation/SlidePlayer.qml`).
- **`WControllerMedia`:** `CONTROLLERMEDIA_VLC` or `CONTROLLERMEDIA_FFMPEG`. The VLC engine, its
  thread, `createVlcPlayer` and `versionVlc` are VLC only; FFmpeg builds have `versionFfmpeg`.
  `startLog` sends FFmpeg's errors to the log as `[ffmpeg]` lines, as VLC's go as `[vlc]`.

## FFmpeg

- **The build:** our own, from the FFmpeg build repository (`omega-gg/FFmpeg`): FFmpeg 9.0.2,
  LGPL 3, for win64, macOS, iOS, linux64 and Android. Its `implementation.md` lists what it
  contains and why.
  - `3rdparty/generate.sh` sets `FFmpeg_version` and downloads the release into
    `3rdparty/<platform>/FFmpeg/<version>`.
  - `Sky/configure.sh` and `deploy.sh` set the same `FFmpeg_version`.
- **The libraries are named `sk-avcodec` and so on** (`sk-avcodec-63.dll`, `libsk-avcodec.so.63`,
  `sk-avcodec.framework`), so they never clash with Qt Multimedia's FFmpeg, which ships under
  FFmpeg's names. Sky links `-lsk-avformat -lsk-avcodec ...`.
- **Configure and deploy, per platform:**
  - win64: the `sk-*.lib` (MSVC) and `libsk-*.dll.a` (MinGW) import libraries in `lib`, the
    DLLs in the deploy folder;
  - macOS and Linux: the versioned libraries and their links;
  - iOS: the frameworks of the slice `$mobile` picks;
  - Android: the libraries per ABI.
- **TLS:** SChannel on Windows, SecureTransport on Apple, with the system's certificates;
  mbedTLS on Linux and Android, where the player passes the system's certificates as a CA file
  (`getCertificates`: the distribution's bundle on Linux, the system folder on Android gathered
  once in the cache).

## The classes

### WFfmpegPlayer (`SkMedia/src/ffmpeg`)

It mirrors `WVlcPlayer`:

- the same interface (`setBackend` with setup, lock and unlock frame callbacks, `setSource`,
  `play(at)`, `pause`, `stop`, `seek`, `nextFrame`, `setSpeed`, `setVolume`, `deletePlayer`);
- the same events posted to the backend.

It owns the decoding, so it could also serve without a QML player.

- **Inputs:** one read thread per input, opened in parallel, each feeding a video queue, an
  audio queue or both. Queues are bounded to 4 s, and an input waits only once all its queues
  are full.
  - **`OutputMedia`:** the video, plus the audio from the separate audio URL (the input slave of
    VLC) when there is one, otherwise from the file itself. The audio URL can be any audio file
    FFmpeg reads, local or over https. An audio URL that fails to open is closed, and the video
    plays alone.
  - **`OutputVideo`:** the video alone, and the audio is never read.
  - **`OutputAudio`:** the audio alone.
- **Decode thread:** decodes up to 3 frames ahead and copies them from the GPU, so a slow
  frame never comes late. A copy from the GPU takes 11 to 13 ms for a 4K frame, with peaks
  past 30 ms.
- **Video thread:** converts the next frame ahead into the backend's buffer, then only publishes
  it when it's due.
- **Late frames:** behind the clock, a frame late by two frames is dropped before its copy from
  the GPU, never more than five in a row, and a frame whose next one is due already is skipped,
  so the video stays on the audio. A seek's frame and a step are never dropped.
- **Audio thread:** owns a `QAudioSink` in push mode, in its own event loop. Every 10 ms it
  decodes what the device can take, converts it with `swresample` to the device format (float
  when supported), and writes it into a 100 ms device buffer.
- **Speed:** at another speed than 1x the samples go through libavfilter's `atempo`, which
  keeps the pitch (chained below 0.5x). At 1x there's no filter, the samples go straight to the
  device. A speed change rebuilds the filter and goes on from where the audio decodes, without a
  seek.
- **Shared state:** a mutex and a wait condition guard what the threads share.
- **Events:**
  - events go through the player, which drops the ones of a previous source and forwards the
    rest;
  - the time of a frame is posted before the frame itself, so the frame comes to a backend
    already done loading;
  - during playback the time goes every 250 ms, as VLC's (about 4 a second), rather than with
    each frame: every update runs the QML bound to `currentTime`. The first frame, a seek, a
    step and a pause send theirs at once, so a paused frame always says its exact time.
- **Network:** FFmpeg's HTTP with reconnects, keep-alive for seeks, and a 20 s read timeout.
  Requests are of 10 MB at most (`request_size`): YouTube serves one long request at about twice
  the playback speed and 10 MB ones at full speed. A seek on a YouTube video at 60 s went from
  5 to 7.5 s to its first frame down to 1.5 s, with its audio.
  There's no reconnect for a source without byte ranges: it can't resume, and at the end of a
  stream sent without length FFmpeg would take the end for a cut and retry.
- **Sources that can't seek:** a server without byte ranges is read through `WFfmpegCache`, which
  keeps in memory what was read:
  - seeks back and forth land in it;
  - a file with its index at its end (an m4a with its `moov` last) plays too;
  - a seek to the end of a source without length is refused rather than read to the end, as a
    radio has no end.
- **Stop:** an interrupt callback on an abort flag makes any blocking read return at once, so a
  stop joins both threads right away.
- **Seek:**
  - the latest target wins, with no queue;
  - `av_seek_frame` goes back to the keyframe, then the player flushes and decodes forward;
  - it skips non-reference frames until 0.5 s before the target;
  - it shows the frame the target time falls in.

  Times are whole milliseconds and frames are 16 or 17 ms apart at 60 fps, so a frame that
  starts within 1 ms after the target counts as well. That way, a step of one frame duration
  lands on the next frame in either direction.
- **Decoding:** hardware first (d3d11va, VideoToolbox, vaapi), on the graphics API Qt renders
  with where it can (`QQuickWindow::graphicsApi`): with the Vulkan renderer, on Windows and
  Linux, the decoder is Vulkan's. With two APIs on the same GPU, a 4K frame's copy to memory
  took 16 to 18 ms instead of 4 to 7. A renderer without video decoding falls back to the
  platform's device. AV1 goes to FFmpeg's own AV1
  decoder on the GPU, since dav1d is software only. When the GPU can't decode a stream, its
  first frame fails, and the player decodes it in software from the last seek.
- **Frames:** hardware frames come down as NV12 or P010, and `sws_scale` writes I420 straight
  into the backend's planes. Full range frames are brought to the video range of the shader.
- **10-bit frames:** they go to 8 bits on the GPU before their copy to memory (`applyDepth`):
  `scale_d3d11` on Windows (our FFmpeg patches it, see its implementation.md), `scale_vaapi` on
  Linux (untested yet). A 4K 10-bit frame is 25 MB, and its copy alone took longer than a frame
  lasts at 60 fps; at 8 bits it's half. The shader shows 8 bits anyway, so the picture is the
  same. Without the filter, or when it fails, the frame is copied as it is. Vulkan frames are
  copied as they are too: their 10-bit copy is fast enough (10 ms for 4K). Apple (VideoToolbox
  can output 8 bits itself) is still to do.
- **Playback:** frames are paced on their timestamps against a clock.
  - **With audio:** the audio leads. What we hear is the end of the decoded audio, minus what
    waits in the device and in the player, each millisecond of it being `speed` milliseconds of
    media. The audio thread moves the clock to it only when the two drift apart by more than
    20 ms, so the video stays smooth.
  - **Without audio:** a steady clock times the speed, restarted from the first frame shown
    after a play or a seek.
  - **Seeks:** they start the audio on their target, to the sample, once the video shows its
    frame, so both start together.
  - **End:** when the audio ends first, the clock goes on alone.
  - **On the display:** a frame goes in a slot halfway between two swaps of the window, rather
    than at its time. `WDeclarativePlayer` follows the window's swaps on its render thread
    (`frameSwapped`, the last regular swap and the period) and hands them over with each frame
    (`WBackendFrame`). A frame takes the slot after the previous frame's while it's within 3/4
    of a period of its time, then moves to the nearest slot.

    Without it, 59.94 fps on a 59.97 Hz display drifts across a swap every 33 s or so, and
    around it the 1 ms jitter of the frames gives a swap two frames and the next none, 14 to 91
    times in a few seconds, with both engines. With it, each crossing repeats one frame. A
    window that does not swap for 100 ms brings the frames back to their time.
- **Volume:** VLC's curve.
- **Output:** as VLC, another output plays the source again from the current time, here with
  its last frame on screen meanwhile. turbopixel's `LayerPlayer` reads the audio once its sound
  goes on, until the next video, so turning the sound off is the volume alone. The audio output
  loads the audio alone (`getMode` gives `SourceAudio`), so switching to or from it loads the
  sources again, as VLC does; otherwise a source started in audio stayed black in video.
- **Logs:** FFmpeg logs its fatal errors only. The player reports its own, FFmpeg's are mostly
  the ones of a source we cut, as a probe interrupted (dav1d complaining of a cut packet) or a
  TLS connection we close.

### WBackendFfmpeg (`SkMedia/src/media`)

It follows the structure of `WBackendVlc`, trimmed:

- the same reply and quality handling;
- the same double buffered I420 planes and setup event, with planes aligned to 32 bytes for
  swscale;
- the same state events.

Where it differs:

- **Pause:** it pauses the player at once, even while loading. A paused player shows the frame
  at its time and stays there.
- **Seek:** no coalescing in the backend, since the player keeps only the last seek.
- **`getFrame`:** `backendUpdateFrame` converts the current planes to RGB with swscale (BT.709,
  video range), so a frame can be taken at any time, paused or playing. `SK_SOFTWARE` converts
  on each frame and draws with `backendDrawFrame`, as VLC does.
- **Tracks:** every video and audio stream is a track, its id the stream index, its name the
  stream's title, otherwise its language. Picking one plays the source again from the current
  time with that stream (as an output change does), the last frame on screen meanwhile; the
  choice stays while the source is the same. Each opening reports its tracks, and
  `WAbstractBackend` applies the picked ones again: the backend restarts only for a stream the
  player does not play, and once for a pick the source does not have, so a quality or source
  change never loops.
- **Picture adjust:** `backendSetAdjust` leaves the frames as they are and the node adjusts
  them (see WBackendYuv). The current frame is sent again so a paused video shows it.
- **Proxy:** `setProxy` and `clearProxy`, as VLC's, through FFmpeg's `http_proxy` option.
  `WBackendManager` forwards them to its backend, for both engines. A password goes as the
  proxy's Basic authentication, after its challenge. The player opens a source with
  `avio_open2`, which takes the options it uses, and goes through the demuxer only when FFmpeg
  has no protocol for it (`AVERROR_PROTOCOL_NOT_FOUND`, as RTSP), its options untouched. Any
  other failure fails the source at once: opened again, it would go without the options taken,
  and a source the proxy refuses played without the proxy.
- **Live:** a source without a length is live from its first time; `playAt` then plays from
  the live point instead of seeking.
- **Variant lists:** an HLS master playlist has a program per variant. Its video is the one
  closest to the quality, as VLC's `preferred-resolution`: the highest under it, otherwise the
  lowest, the highest for the default quality. The audio comes from the same variant. A quality
  change clears the tracks picked on such a source, so the new variant is not overridden.
- **Hardware decoding:** d3d11va on Windows, VideoToolbox on Apple, VA-API on Linux, the
  MediaCodec decoders on Android (`h264_mediacodec` and so on, after `av_jni_set_java_vm`).
  Any of them falls back to software on its first frame. The decoding device stays from a source
  to the next one: a switch between two YouTube videos went from about 60 ms of device release,
  creation and first frame to 15 ms on D3D11, and the Vulkan device takes 300 ms to create. A
  fallback to software releases it, so a device broken by a GPU reset is not kept.
- **VLC's source options** (`demux=avformat` from the VBML backends) are ignored: FFmpeg always
  demuxes with avformat.

### WBackendYuv

The I420 node, material and shader moved out of `WBackendVlc` (`WBackendYuvNode`). Both
backends render with them. The shader is compiled for every Qt RHI backend (SPIR-V, GLSL, HLSL,
MSL), so OpenGL, Vulkan, D3D11, D3D12 and Metal all render it.

- **Adjust:** `WBackendNode::setAdjust`, from the `adjust` of the frame (`WBackendFrame`).
  Contrast, brightness, hue and saturation are one matrix applied before the color matrix, and
  gamma one uniform, in the order of VLC's adjust filter. A VLC build leaves the frame's adjust
  neutral, VLC adjusting its frames itself.
- **Software rendering:** Qt's software renderer can't run the shader, so when the window's
  graphics API is Software, `WDeclarativePlayer` shows the backend's RGB frame (`getFrame`)
  through an image node, for both engines.
- **Textures:** each new source gets a new node, and `WTextureVideo` releases its `QRhiTexture`
  with it. It used to keep them, about 4 MB per 1080p source with either engine on Qt 6: 90
  switches between two videos took the process from 330 MB to 815 MB.

### WBackendManager

- **The backend:** `init` creates `WBackendFfmpeg`, or `WBackendVlc` with `SK_VLC`.
- **Render node:** `backendCreateNode` asks the inner backend for its node.
- **Proxy:** `setProxy` and `clearProxy`, forwarded to the inner backend.

## Qt Multimedia

The player uses Qt Multimedia for the audio output only (`QMediaDevices`, `QAudioSink`).

- **Qt's FFmpeg is loaded too:** Qt Multimedia loads its media backend on first use, and on
  Windows its default backend is its FFmpeg plugin (`ffmpegmediaplugin`, with Qt's own FFmpeg
  7.1.2, the "Using Qt multimedia with FFmpeg" line). Both FFmpeg builds sit side by side: ours
  has `sk-` names and other versions. The audio output itself is Qt's WASAPI code, the same
  with any backend: with `QT_MEDIA_BACKEND=windows`, Qt loads its Media Foundation plugin and
  no FFmpeg of its own. Not set for now: the camera and barcode scanner would run on that
  backend, which is still to check.
- **Stopping a sink (Qt 6):** `QAudioSink` starts a new stream with its own worker thread at
  each `start`. `stop`, which the sink's destructor calls, lets the stream drain in the
  background, and the stream is never freed: it keeps about 0.2 MB in Qt 6.10 (0.09 MB in
  6.11.0), its worker thread and that thread's MMCSS registration (QTBUG-151655). Windows takes
  32 registrations per process: past 32 seeks or source changes, the log showed
  `AvSetMmThreadCharacteristics failed ... too many threads` and the audio lost its thread
  priority. The player calls `reset` instead, which drops what the sink holds and frees the
  stream at once. Over 200 source changes with audio, memory stays flat (11 MB more before)
  and no warning shows (68 before). The previous source's audio no longer plays on after a
  change. A seek is silent until the new position is ready, 20 to 130 ms depending on the
  keyframes, where the previous position's audio used to fill that time.

## Torrent

`WHookTorrent` serves the torrent over a local HTTP server, which FFmpeg reads as any HTTP
source. VLC needed fake data for its early probes past the downloaded part; FFmpeg reads the
real index at the end of a file (MP4, MKV), so FFmpeg builds never skip (`skip = -1`).

## Measured

All on 1080p60 AV1 (`itag=399`, 990.6 s), on a laptop RTX A1000 with d3d11va.

| Test | Result |
|---|---|
| Seek to 6 targets, a step back from each (from the time the player says, as the bar does), then the targets in reverse, against the frames the ffmpeg CLI decodes at those times | 18 of 18 identical (difference 0.00), on disk and on YouTube |
| Same time sought twice | same picture every time |
| Paused seek on YouTube | about 0.4 s to the frame |
| Playback, 16 s at the end of the YouTube video | 60 frames a second, to the last frame and `ended` (VLC's MP4 demuxer stopped at 987 s) |
| CPU while playing those 16 s | FFmpeg 2.5 to 2.7 s, VLC 7.9 s |
| Previous frame held on YouTube for 40 s, then quit | no freeze, exit in 0.4 s |
| One player switching VLC, FFmpeg, VLC while playing | each plays from its time, exit in 0.4 s |
| YouTube with its separate audio URL, from 975 s | sound from the start to `ended` (Windows audio meter of the process), the time on the wall clock |
| A file with its own audio: pause, seek paused, play, 2x, back to 1x | silent paused, sound at play and at 2x and back at 1x, `ended` on time |
| Speeds 2x, 0.5x, 0.25x and back to 1x, on that file and on YouTube | media at 2.00, 0.50, 0.25 and 1.00 s a second, sound at each, no stall at the changes |
| YouTube start at 960 s | 0.7 s to resolve the page, 0.6 s to open both inputs, 0.15 s to seek, 0.8 s to fetch and decode up to the exact frame, then audio and video together |

4K60 streaming from YouTube against VLC, the frame times as they reach the player over 20 s of
playback, in alternate runs (the laptop heats up):

| 4K60 | VLC 3.0.23 | FFmpeg engine |
|---|---|---|
| SDR, AV1 (`itag=401`): spacing jitter (sd) | 2.2 to 2.5 ms | 1.0 to 1.2 ms |
| SDR: late frames / dropped | 0 to 0.1% / 0% | 0% / 0% |
| SDR: CPU | 218 to 249% (dav1d, software) | 41 to 51% (d3d11va) |
| HDR, AV1 10 bits (`itag=701`): jitter (sd) | 4.5 to 5.2 ms | 1.1 ms, one run 5.0 ms |
| HDR: late frames / dropped | 2.5 to 3.6% / 0%, 6.1% dropped when hot | 0% / 0%, one run 4.1% / 1.9% |
| HDR: CPU | 479 to 711% | 93 to 110% |

Neither engine maps HDR to SDR yet: the 10 bits frame is shown through the BT.709 shader, so
its colors are flat.

| Test | Result |
|---|---|
| `OutputVideo` playback (turbopixel's players) | no sound, the audio is never read |
| A local video with an https audio file as its input slave: ogg (byte ranges), then mp3, m4a with its index last, opus, flac and aac from a server without byte ranges, a seek back mid play | each plays with sound from its start, the seek back lands and the sound goes on |

Backward seeks on YouTube take 0.2 to 1.5 s each, since each one fetches again from its
keyframe.

## Later phases

- **2. Rendering:** NV12 and P010 shader variants (2 planes, no conversion), then zero copy on
  D3D11. FFmpeg would decode on the device Qt's RHI renders with, and the texture would go to
  the scene graph as is, which removes the copy from the GPU, the main cost at 4K. HDR to SDR
  tone mapping in that shader. VideoToolbox to Metal would work the same way on Apple.
- **Chromecast:** VLC's renderer discovery and casting have no FFmpeg counterpart yet.
- **Parity:** embedded subtitle tracks, HLS/DASH variant switching on the bandwidth, a packet
  cache for backward seeks on a stream.
