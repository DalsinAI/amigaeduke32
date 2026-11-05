/* AmigaDOS locks files as readily as drawers, and libnix's chdir() makes
 * whatever it locked the current directory: chdir("DUKE3D.GRP") succeeds
 * and every relative open after it fails. buildvfs_chdir() (vfs.h) comes
 * here, which refuses anything that is not a drawer, as chdir() does on
 * the systems EDuke32 was written for.
 * The AmiCradle Team. GPL 2 or later, like EDuke32. */
#include <exec/types.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <errno.h>
#include <unistd.h>

int amiga_is_drawer(const char *path)
{
	struct FileInfoBlock *fib;
	BPTR lock;
	int drawer = 0;

	lock = Lock((CONST_STRPTR)path, ACCESS_READ);
	if (!lock)
		return 0;
	fib = (struct FileInfoBlock *)AllocDosObject(DOS_FIB, NULL);
	if (fib) {
		if (Examine(lock, fib))
			drawer = fib->fib_DirEntryType > 0;
		FreeDosObject(DOS_FIB, fib);
	}
	UnLock(lock);
	return drawer;
}

int amiga_chdir(const char *path)
{
	if (!amiga_is_drawer(path)) {
		errno = ENOTDIR;
		return -1;
	}
	return chdir(path);
}
