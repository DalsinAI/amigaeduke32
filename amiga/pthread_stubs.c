/*
 * What libstdc++ and libgcc's unwinder ask of POSIX threads, for a program
 * that has one: nothing to lock, keys as plain slots, once as a flag.
 * The AmiCradle Team. GPL 2 or later, like EDuke32.
 */
#define MAX_KEYS 64
static void *slots[MAX_KEYS];
static int used[MAX_KEYS];

int pthread_mutex_lock(void *m) { (void)m; return 0; }
int pthread_mutex_unlock(void *m) { (void)m; return 0; }
int pthread_cancel(unsigned long t) { (void)t; return 0; }

int pthread_key_create(unsigned *key, void (*destructor)(void *))
{
    int i;
    (void)destructor;
    for (i = 0; i < MAX_KEYS; i++)
        if (!used[i]) {
            used[i] = 1;
            slots[i] = 0;
            *key = (unsigned)i;
            return 0;
        }
    return 11; /* EAGAIN */
}

int pthread_key_delete(unsigned key) { if (key < MAX_KEYS) used[key] = 0; return 0; }
void *pthread_getspecific(unsigned key) { return key < MAX_KEYS ? slots[key] : 0; }
int pthread_setspecific(unsigned key, const void *v) { if (key >= MAX_KEYS) return 22; slots[key] = (void *)v; return 0; }

int pthread_once(int *once, void (*init)(void))
{
    if (!*once) {
        *once = 1;
        init();
    }
    return 0;
}

/* With one thread nobody waits on a condition, and nothing is cancelled. */
int pthread_cond_broadcast(void *c) { (void)c; return 0; }
int pthread_cond_wait(void *c, void *m) { (void)c; (void)m; return 0; }
void pthread_testcancel(void) {}

/* EDuke32 builds libasync (std::thread, never started: nothing spawns a
 * task) and loguru (which names the calling thread). One thread: a thread
 * cannot be made, and there is only ever this one. */
unsigned long pthread_self(void) { return 1; }
int pthread_create(unsigned long *t, const void *a, void *(*f)(void *), void *arg)
{ (void)t; (void)a; (void)f; (void)arg; return 11; /* EAGAIN */ }
int pthread_join(unsigned long t, void **v) { (void)t; (void)v; return 3; /* ESRCH */ }
int pthread_detach(unsigned long t) { (void)t; return 3; }
int pthread_cond_destroy(void *c) { (void)c; return 0; }
int pthread_cond_signal(void *c) { (void)c; return 0; }
