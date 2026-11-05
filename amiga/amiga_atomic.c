/* GCC's __atomic library calls, which libgcc does not have for the 68k on
 * the Amiga (the compiler will not use CAS there: the Amiga's chip bus has
 * no read-modify-write cycle). EDuke32 uses std::atomic in its console,
 * its lock-free lists and the parts of libasync it builds; the other task
 * that can touch them is SDL's audio task. One CPU, so an operation done
 * between Forbid() and Permit() is atomic to every other task.
 * The memory order is ignored: every call is a full barrier.
 * The AmiCradle Team. GPL 2 or later, like EDuke32. */
#include <exec/types.h>
#include <proto/exec.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"

#define BARRIER() __asm__ __volatile__("" ::: "memory")
#define LOCK()    do { Forbid(); BARRIER(); } while (0)
#define UNLOCK()  do { BARRIER(); Permit(); } while (0)

#define ATOMIC_FUNCS(N, T)                                                              \
T __atomic_load_##N(const volatile void *p, int m)                                      \
{ T v; (void)m; LOCK(); v = *(const volatile T *)p; UNLOCK(); return v; }               \
void __atomic_store_##N(volatile void *p, T v, int m)                                   \
{ (void)m; LOCK(); *(volatile T *)p = v; UNLOCK(); }                                    \
T __atomic_exchange_##N(volatile void *p, T v, int m)                                   \
{ T o; (void)m; LOCK(); o = *(volatile T *)p; *(volatile T *)p = v; UNLOCK(); return o; } \
bool __atomic_compare_exchange_##N(volatile void *p, void *e, T d, bool w, int s, int f) \
{                                                                                       \
    bool ok; T o; (void)w; (void)s; (void)f;                                            \
    LOCK();                                                                             \
    o = *(volatile T *)p;                                                               \
    ok = (o == *(T *)e);                                                                \
    if (ok) *(volatile T *)p = d; else *(T *)e = o;                                     \
    UNLOCK();                                                                           \
    return ok;                                                                          \
}                                                                                       \
ATOMIC_RMW(N, T, add, o + v)                                                            \
ATOMIC_RMW(N, T, sub, o - v)                                                            \
ATOMIC_RMW(N, T, and, o & v)                                                            \
ATOMIC_RMW(N, T, or, o | v)                                                             \
ATOMIC_RMW(N, T, xor, o ^ v)                                                            \
ATOMIC_RMW(N, T, nand, ~(o & v))

#define ATOMIC_RMW(N, T, OP, EXPR)                                                      \
T __atomic_fetch_##OP##_##N(volatile void *p, T v, int m)                               \
{ T o; (void)m; LOCK(); o = *(volatile T *)p; *(volatile T *)p = (T)(EXPR); UNLOCK(); return o; } \
T __atomic_##OP##_fetch_##N(volatile void *p, T v, int m)                               \
{ T o, n; (void)m; LOCK(); o = *(volatile T *)p; n = (T)(EXPR); *(volatile T *)p = n; UNLOCK(); return n; }

ATOMIC_FUNCS(1, uint8_t)
ATOMIC_FUNCS(2, uint16_t)
ATOMIC_FUNCS(4, uint32_t)
ATOMIC_FUNCS(8, uint64_t)

/* The generic forms, for objects of any other size. */
void __atomic_load(size_t n, const volatile void *p, void *r, int m)
{ (void)m; LOCK(); memcpy(r, (const void *)p, n); UNLOCK(); }
void __atomic_store(size_t n, volatile void *p, void *v, int m)
{ (void)m; LOCK(); memcpy((void *)p, v, n); UNLOCK(); }
void __atomic_exchange(size_t n, volatile void *p, void *v, void *r, int m)
{ (void)m; LOCK(); memcpy(r, (const void *)p, n); memcpy((void *)p, v, n); UNLOCK(); }
bool __atomic_compare_exchange(size_t n, volatile void *p, void *e, void *d, int s, int f)
{
    bool ok;
    (void)s; (void)f;
    LOCK();
    ok = memcmp((const void *)p, e, n) == 0;
    if (ok) memcpy((void *)p, d, n); else memcpy(e, (const void *)p, n);
    UNLOCK();
    return ok;
}
bool __atomic_is_lock_free(size_t n, const volatile void *p) { (void)n; (void)p; return false; }
