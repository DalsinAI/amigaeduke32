/* EDuke32's allocator on the Amiga, in place of smmalloc (compat.h).
 * smmalloc reserves 8 buckets of 2 MB at start-up and a cache per thread,
 * 16 MB and more before the game has asked for anything, and leans on
 * 64-bit atomics; here every Xmalloc() comes from one exec memory pool,
 * under a semaphore, since SDL's audio task may allocate too (libnix's
 * malloc() is not to be shared between tasks). Each block has a header
 * with its start and size, for FreePooled() and for any alignment asked.
 * The AmiCradle Team. GPL 2 or later, like EDuke32. */
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <proto/exec.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct header {
	void *base;   /* what AllocPooled() gave */
	ULONG total;  /* what it was asked for */
	ULONG size;   /* what the caller asked for */
	ULONG pad;
};

static APTR pool;
static struct SignalSemaphore lock;
static int ready;

static void amiga_alloc_cleanup(void)
{
	if (pool)
		DeletePool(pool);
	pool = NULL;
}

/* Made on the first allocation, which is the game task's (SDL's audio
 * task starts long after). */
static void amiga_alloc_init(void)
{
	InitSemaphore(&lock);
	/* 64 KB puddles; anything over 16 KB is a block of its own. */
	pool = CreatePool(MEMF_ANY, 65536, 16384);
	ready = 1;
	atexit(amiga_alloc_cleanup);
}

void *amiga_malloc(size_t size, size_t alignment)
{
	struct header *h;
	char *base, *p;
	ULONG total;

	if (!ready)
		amiga_alloc_init();
	if (!pool)
		return NULL;
	if (alignment < sizeof(struct header))
		alignment = sizeof(struct header);
	total = (ULONG)(size + sizeof(struct header) + alignment - 1);

	ObtainSemaphore(&lock);
	base = (char *)AllocPooled(pool, total);
	ReleaseSemaphore(&lock);
	if (!base)
		return NULL;

	p = (char *)(((ULONG)base + sizeof(struct header) + alignment - 1) & ~(ULONG)(alignment - 1));
	h = (struct header *)p - 1;
	h->base = base;
	h->total = total;
	h->size = (ULONG)size;
	return p;
}

void amiga_free(void *ptr)
{
	struct header *h;

	if (!ptr || !pool)
		return;
	h = (struct header *)ptr - 1;
	ObtainSemaphore(&lock);
	FreePooled(pool, h->base, h->total);
	ReleaseSemaphore(&lock);
}

void *amiga_realloc(void *ptr, size_t size, size_t alignment)
{
	struct header *h;
	void *n;

	if (!ptr)
		return amiga_malloc(size, alignment);
	if (size == 0) {
		amiga_free(ptr);
		return NULL;
	}
	h = (struct header *)ptr - 1;
	/* Shrinking, or growing within the slack: the block stays. */
	if (size <= h->size || (char *)ptr + size <= (char *)h->base + h->total) {
		h->size = (ULONG)size;
		return ptr;
	}
	n = amiga_malloc(size, alignment);
	if (!n)
		return NULL;
	memcpy(n, ptr, h->size);
	amiga_free(ptr);
	return n;
}
