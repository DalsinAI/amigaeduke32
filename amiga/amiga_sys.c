/* What EDuke32 asks of the system that has no POSIX answer on AmigaOS.
 * The AmiCradle Team. GPL 2 or later, like EDuke32. */
#include <exec/types.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <stddef.h>
#include <string.h>

/* Bgetsysmemsize(): the machine's memory, Fast and Chip together. The
 * engine's cache is 60% of it when the game asks for more than that
 * (tiles.cpp). */
size_t amiga_sysmemsize(void)
{
	return (size_t)AvailMem(MEMF_ANY | MEMF_TOTAL);
}

/* loguru installs its crash handlers with sigaction(), which libnix does
 * not have; its signal() does the same for a plain handler. */
#include <signal.h>
#include <errno.h>

int sigaction(int sig, const struct sigaction *act, struct sigaction *old)
{
	_sig_func_ptr prev;

	if (!act) {
		if (old) {
			prev = signal(sig, SIG_DFL);
			signal(sig, prev);
			memset(old, 0, sizeof(*old));
			old->sa_handler = prev;
		}
		return 0;
	}
	prev = signal(sig, act->sa_handler);
	if (prev == SIG_ERR) {
		errno = EINVAL;
		return -1;
	}
	if (old) {
		memset(old, 0, sizeof(*old));
		old->sa_handler = prev;
	}
	return 0;
}

/* A Workbench start has no command line (libnix gives argc 0 and the
 * WBStartup in argv), and loguru, like EDuke32, wants argv[0]. This makes
 * one: the program's name, then every tool type of its icon that starts
 * with '-', as a Shell argument would be (-jWork:Duke3D, -nomusic,
 * -cfg MyDuke.cfg is two tool types: -cfg and MyDuke.cfg is not one).
 * Then the icons given with it (shift-clicked GRPs or maps), by full name. */
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <stdlib.h>

struct Library *IconBase;

#define MAX_WB_ARGS 32

static char *copy_string(const char *s)
{
	size_t n = strlen(s) + 1;
	char *d = (char *)malloc(n);
	if (d)
		memcpy(d, s, n);
	return d;
}

void amiga_workbench_args(int *argc, char ***argv)
{
	static char *args[MAX_WB_ARGS + 1];
	struct WBStartup *wb;
	int n = 0;
	LONG i;

	if (*argc != 0)
		return;
	wb = (struct WBStartup *)*argv;
	args[n++] = (char *)"EDuke32";

	if (wb && wb->sm_NumArgs > 0) {
		struct WBArg *self = &wb->sm_ArgList[0];

		IconBase = OpenLibrary((CONST_STRPTR)"icon.library", 36);
		if (IconBase) {
			BPTR old = CurrentDir(self->wa_Lock);
			struct DiskObject *dob = GetDiskObject((STRPTR)self->wa_Name);
			if (dob) {
				STRPTR *tt = (STRPTR *)dob->do_ToolTypes;
				for (; tt && *tt && n < MAX_WB_ARGS; tt++)
					if ((*tt)[0] == '-') {
						char *a = copy_string((const char *)*tt);
						if (a)
							args[n++] = a;
					}
				FreeDiskObject(dob);
			}
			CurrentDir(old);
			CloseLibrary(IconBase);
			IconBase = NULL;
		}

		for (i = 1; i < wb->sm_NumArgs && n < MAX_WB_ARGS; i++) {
			struct WBArg *a = &wb->sm_ArgList[i];
			char buf[512];
			if (a->wa_Lock && NameFromLock(a->wa_Lock, (STRPTR)buf, sizeof(buf))) {
				char *s;
				if (a->wa_Name && *a->wa_Name)
					AddPart((STRPTR)buf, a->wa_Name, sizeof(buf));
				s = copy_string(buf);
				if (s)
					args[n++] = s;
			}
		}
	}

	args[n] = NULL;
	*argc = n;
	*argv = args;
}

/* wm_msgbox() and wm_ynbox(): an Intuition requester on the Workbench
 * screen (or the default public screen). Returns the button chosen: 1
 * for the first, ..., 0 for the last. The text goes in as an argument,
 * so a '%' in it is not taken for a format. */
#include <intuition/intuition.h>
#include <proto/intuition.h>

int amiga_requester(const char *title, const char *text, const char *buttons)
{
	struct EasyStruct es;
	APTR args[1];

	es.es_StructSize = sizeof(es);
	es.es_Flags = 0;
	es.es_Title = (UBYTE *)title;
	es.es_TextFormat = (UBYTE *)"%s";
	es.es_GadgetFormat = (UBYTE *)buttons;
	args[0] = (APTR)text;
	return (int)EasyRequestArgs(NULL, &es, NULL, args);
}
