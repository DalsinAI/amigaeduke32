/* A Shell gives 4 KB of stack; libnix's startup swaps to this much instead.
 * The Build engine's renderer and EDuke32's CON interpreter recurse
 * (scansector(), the mirror and camera views, nested CON states), and
 * audiolib's decoders keep their frames on the stack of whoever calls
 * them.
 * The AmiCradle Team. GPL 2 or later, like EDuke32. */
unsigned long __stack = 1024 * 1024;
static const char version[] __attribute__((used)) = "$VER: EDuke32 r10669 Amiga (05.11.2026)";
