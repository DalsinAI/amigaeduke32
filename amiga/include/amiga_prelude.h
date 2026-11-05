/* Included before every upstream C++ file (-include).
 * libstdc++'s thread layer includes the toolchain's <pthread.h>, which,
 * if <unistd.h> came first, turns read(), write(), close() ... into
 * macros (cancellation points). Taken first, <pthread.h> leaves them
 * alone.
 * The AmiCradle Team. GPL 2 or later, like EDuke32. */
#ifdef __cplusplus
#include <pthread.h>
#endif
