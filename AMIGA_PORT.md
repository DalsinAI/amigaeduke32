EDuke32 for AmigaOS 3.x
=======================

EDuke32, the Duke Nukem 3D engine, for AmigaOS 3.x on a 68040 or 68060
with an FPU, on OpenGPU's SDL 2 (LIBS:OpenGPU/SDL2.module, which OpenUp
installs). The classic software renderer, Duke's sound effects and its
MIDI music on an OPL3 emulator.

The game's data is not here: Duke Nukem 3D is a commercial game (the
shareware episode is free to give away, but is not in this drawer either).


Where things go
---------------

    EDuke32/            the game's drawer (anywhere you like)
      EDuke32           the program
      ReadMe            this file
      buildlic.txt      Ken Silverman's Build licence (the engine)
      gpl-2.0.txt       the GNU GPL 2 (the game code, audiolib, MACT)
      GameData/         put DUKE3D.GRP here (or beside the program)

Copy DUKE3D.GRP from your Duke Nukem 3D (Atomic Edition 1.4/1.5, the
1.3D registered version, the shareware DUKE3D.GRP, or the GRP of a later
release) into GameData, or into the drawer itself. EDuke32 finds it by its
contents, whatever its name's case. Add-ons (DUKEDC.GRP, VACATION.GRP,
NWINTER.GRP) go in the same place and are picked with -addon or -grp.

The first start reads every GRP through once to know it (a few seconds for
the 26 MB DUKE3D.GRP; the result is kept in grpfiles.cache). The CON
scripts are compiled at every start.

The program's drawer (PROGDIR:) is where EDuke32 looks for game data and
where it writes eduke32.cfg, settings.cfg, eduke32.log, its saved games and
its screenshots. Another drawer of game data is given with -j:

    EDuke32 -jWork:Games/Duke3D

-usecwd keeps the Shell's current drawer as the place for the
configuration and saves. From Workbench, the program's icon takes any
switch as a tool type: each tool type starting with '-' is passed as it is
(the icon has (-nm), no music, (-ns), no sound, (-jWork:Duke3D) and
(-usecwd), off in brackets; take the brackets away to use one). Other
usual switches: -nologo, -map E1L2.MAP, -cachesize <KB>, -grp FILE.GRP,
-addon N.


What it needs
-------------

- A 68040 or 68060 with an FPU (not an LC040 or EC040), AmigaOS 3.2.
- OpenGPU's runtime: opengpu.library and LIBS:OpenGPU/SDL2.module.
- 64 MB of Fast RAM is comfortable: the program is 2.9 MB with 12 MB of
  tables, and the tile cache takes 16 MB more at start-up (-cachesize or
  CacheSize in eduke32.cfg for another size).
- A 1 MB stack (the program asks for it itself).


Defaults
--------

- 320x200, full screen, 8-bit. The frame is a 32-bit texture of the game's
  size that SDL's renderer scales to the screen it opens (the nearest mode
  the Amiga has to the one asked for). 320x240, 400x300, 640x400 and
  640x480 are offered full screen too, and any size in a window; a bigger
  frame costs the 68k dearly. r_upscalefactor 2 (in the console) renders
  at half the window's size.
- Sound: 22050 Hz, 16 voices (upstream: 48 kHz, 64 voices). Music: MIDI on
  the OPL3 emulator, or the SF2 synthesizer with a SoundFont of your own
  (Options > Sound). Vorbis (.ogg) music and sounds, and MOD/S3M/XM/IT
  modules (libxmp-lite), play as well.


Building
--------

    make -f amiga/Makefile.amigaos -j2 \
         STOVE=/path/to/openamigagcc-16.2.0-open4 \
         NDK=/path/to/OpenNDK/Include_H SDL2KIT=/path/to/opengpu-sdk
    make -f amiga/Makefile.amigaos dist

STOVE is GFOpenAmigaGCC 16.2.0-open4 or later; SDL2KIT is the OpenGPU
developer kit (libSDL2.a). No other library: EDuke32 carries its Vorbis
decoder, libxmp-lite, miniz, lz4 and the OPL3 emulator. The flags are
-noixemul -m68040 -m68881 -O2; CPU="-m68060 -m68881" makes a build for the
68060 alone (see below). The output is build/amigaos/EDuke32, and 'make
dist' makes dist/EDuke32 (the drawer above) and dist/EDuke32.info. The
GlowIcons are made from EDuke32's own program icon by port_icons.py
(amiga/icons/icons.json says how). Never more than -j2 on a small machine:
gameexec.cpp, gamedef.cpp and engine.cpp each take a good deal of memory.

The build is upstream's GNUmakefile for eduke32 with RENDERTYPE=SDL
SDL_TARGET=2 USE_OPENGL=0 POLYMER=0 NETCODE=0 NOASM=1 STARTUP_WINDOW=0
USE_LIBVPX=0 HAVE_FLAC=0 USE_MIMALLOC=0, written out in
amiga/Makefile.amigaos with upstream's source lists and exclusions.


What was done
-------------

The Amiga's own parts, in amiga/:

- Makefile.amigaos: the build.
- stack.c: a 1 MB stack and the $VER string.
- amiga_screens.c: SDL2.module (0.6) leaves the screens of a full-screen
  window open, empty, after SDL_Quit(); they are closed (or sent behind the
  Workbench) when the game ends.
- amiga_dir.c: libnix's chdir() will enter a file; buildvfs_chdir() asks
  first that the name is a drawer.
- amiga_alloc.c: Xmalloc() and the rest come from an exec memory pool under
  a semaphore, in place of smmalloc, which reserves 16 MB of buckets at
  start-up and needs 64-bit atomics.
- amiga_atomic.c: GCC's __atomic_* calls (the 68k compiler will not use
  CAS on the Amiga), each done between Forbid() and Permit().
- amiga_sys.c: the machine's memory size (AvailMem), sigaction() on libnix's
  signal(), the arguments of a Workbench start (the icon's tool types), and
  an Intuition requester for EDuke32's message boxes (such as "Required game
  data was not found"), which may come after SDL_Quit().
- pthread_stubs.c: one thread (libasync is built, never started; loguru
  names the thread). net_stubs.c and include/: socket calls that fail, so
  that enet.h and the engine's mmulti.cpp build with no TCP/IP.

Changes to upstream's files, each marked __amigaos__ or B_BIG_ENDIAN:

- compat.h: the 68k is big-endian; the allocator above. compat.cpp:
  AmigaDOS paths in Bcorrectfilename() and Breaddir() (no '/' after a ':',
  "" for the current drawer, never "Vol:/x" or "a//x", which name parent
  drawers); no home drawer; the memory size. vfs.cpp: the same for search
  paths and "./". vfs.h: buildvfs_chdir().
- common.cpp (duke3d): PROGDIR: and PROGDIR:GameData are searched, and
  PROGDIR: is the current drawer unless -usecwd.
- sdlayer.cpp: the frame. OpenGPU's SDL 2 gives a 24-bit window surface
  (RGB24, three bytes a pixel), which softsurface cannot write; the 8-bit
  frame goes through the palette into a streaming XRGB8888 texture of the
  frame's size, and the renderer scales it. engine.cpp: softsurface does no
  scaling of its own on the Amiga. ImGui is left out (it draws only with
  OpenGL). SDL's audio task is asked for a 256 KB stack. The program's
  memory functions are not given to SDL (SDL2.module allocates on its own
  tasks). No backtrace on a crash (no execinfo.h).
- build.h: full-screen modes from 320x200 up (upstream lists 640x480 and
  up). config.cpp: 320x200 full screen, 22050 Hz, 16 voices. game.cpp: a
  16 MB tile cache (upstream: 96 MB).
- timer.cpp: the time from SDL's performance counter (libnix has no
  clock_gettime()). sdlayer.h: idle() on SDL_Delay() (no nanosleep()).
- loguru.cpp: sigaction with sa_handler (libnix has no sa_sigaction), and a
  crash handler libnix refuses (SIGBUS, ...) is skipped, not fatal.
  compat.cpp: a 4 KB page size (libnix's sysconf() has none).
  xxhash.h: alignment at most 8 (the hunk format's limit). libasync.h:
  EDuke32's aligned allocator (no posix_memalign()). texcache.h: mio.hpp
  (memory-mapped files) only with OpenGL.

Big-endian: the Build engine and EDuke32 ran on PPC Macs and the Wii, and
the ART, MAP, GRP, RTS, ANM, palette, VOC and WAV loaders all read through
B_LITTLE16/32. Four slips are fixed here, for every big-endian machine:

- midi.cpp: the "MThd" and "MTrk" signatures were compared in the PC's byte
  order, so no MIDI file played: all of Duke's music.
- xmp.cpp: libxmp mixes in the machine's byte order, the voice mixers read
  little-endian samples; the module's buffer is swapped. libxmp-lite itself
  is built with WORDS_BIGENDIAN, which upstream's Makefile never sets.
- sdlayer.cpp: the window icon's colour masks.
- engine.cpp: the check of the sine and arctangent tables' CRCs (made from
  little-endian bytes) warned for nothing.

Saved games and demos are written in the machine's own byte order: they
are not interchangeable with those of a PC's EDuke32 (nor were a PPC Mac's).
The CON scripts are text and compile the same.


Cut
---

- OpenGL (Polymost, Polymer), hi-res packs, models and voxels: the classic
  renderer only.
- Network games (no enet, no TCP/IP), the Steam and GOG search paths.
- Video (libvpx, .ivf cutscenes); the original .ANM cutscenes play.
- FLAC (it needs libFLAC). Vorbis and modules stay.
- The editor (Mapster32), the tools, the start-up window, LuaJIT (Lunatic is
  gone upstream anyway).


Known limits and 68k slowness
-----------------------------

- Speed. The renderer's fixed-point helpers (pragmas.h: mulscale,
  dmulscale, divscale) are 64-bit: mulscale is one MULS.L 32x32->64 on the
  68040, but the 68060 has no 64-bit MULS.L/DIVS.L and traps to
  68060.library on every one; a build with CPU="-m68060 -m68881" calls
  libgcc instead, which is faster on the 060 but still slow. divscale is a
  64-bit division (__divdi3) on either. These are in the per-column wall
  and sprite set-up, so a big window costs a lot: keep to 320x200.
- The OPL3 emulator (Nuked OPL3) is exact but heavy for a 68k; if the game
  stutters with music on, try the SF2 synth, or -nm (no music).
- timerGetTicks() divides 64-bit numbers on every call; a few calls a frame.
- No ImGui (its debug windows need OpenGL), no backtrace on a crash.
- The tile cache is taken whole at start-up (16 MB).


Licences
--------

The Build engine (source/build) is Ken Silverman's, under the BUILD
licence in buildlic.txt: free for non-commercial use, with credit. The game
code (source/duke3d), audiolib and MACT are under the GNU GPL 2
(gpl-2.0.txt), as are the Amiga parts in amiga/. Bundled libraries keep
their own licences: libxmp-lite (MIT), miniz (MIT), lz4 and xxHash
(BSD 2-clause), loguru (public domain), minivorbis (libogg and libvorbis,
BSD), TinySoundFont (MIT), Nuked OPL3 (GPL 2). Duke Nukem 3D's data is
commercial and not part of this.

Upstream: https://voidpoint.io/terminx/eduke32, commit
ec5824db81817866f70da326d3811bb0f52b3517 (r10669, 7 August 2026).
The AmiCradle Team.
