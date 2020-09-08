/*
 * some misc. file/disk routines
 * Arne de Bruijn, 1998
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <string.h>
#include "d_io.h"
//added 05/17/99 Matt Mueller
#include "u_mem.h"
//end addition -MM

#if defined(_WIN32) && !defined(_WIN32_WCE)
#include <windows.h>
#define lseek(a,b,c) _lseek(a,b,c)
#endif

ulong d_getdiskfree()
{
 // FIXME:
return 999999999;
}

ulong GetDiskFree()
{
 return d_getdiskfree();
}

// remove extension from filename, doesn't work with paths.
void removeext(const char *filename, char *out) {
	char *p;
	if ((p = const_cast<char*> (strrchr (filename, '.')))) {
		strncpy(out, filename, p - filename);
		out[p - filename] = 0;
	} else
		strcpy(out, filename);
}
