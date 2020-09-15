/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "u_mem.h"
#include "error.h"

//------------------------------------------------------------------------------

char *strcompress(char *str) {
    for (char *ps = str; *ps; ps++)
        if (*ps == 'a')
            *ps = '4';
        else if (*ps == 'e')
            *ps = '3';
        else if (*ps == 'i')
            *ps = '1';
        else if (*ps == 'o')
            *ps = '0';
        else if (*ps == 'u')
            *ps = 'v';
    return str;
}

//------------------------------------------------------------------------------

char *StrDup(const char *source) {
    char *newstr;
    int32_t l = (int32_t)strlen(source) + 1;

    if ((newstr = new char[l]))
        memcpy(newstr, source, l);
    return newstr;
}

//------------------------------------------------------------------------------

#ifndef _WIN32
char *strlwr(char *s1) {
    char *p = s1;
    while (*s1) {
        *s1 = tolower(*s1);
        s1++;
    }
    return p;
}

char *strupr(char *s1) {
    char *p = s1;
    while (*s1) {
        *s1 = toupper(*s1);
        s1++;
    }
    return p;
}

char *strrev(char *s1) {
    char *h, *t;
    h = s1;
    t = s1 + strlen(s1) - 1;
    while (h < t) {
        char c;
        c = *h;
        *h++ = *t;
        *t-- = c;
    }
    return s1;
}
#endif

#if !(defined(_WIN32) && !defined(_WIN32_WCE))
void _splitpath(char *name, char *drive, char *path, char *base, char *ext) {
    char *s, *p;

    p = name;
    s = strchr(p, ':');
    if (s != NULL) {
        if (drive) {
            *s = '\0';
            strcpy(drive, p);
            *s = ':';
        }
        p = s + 1;
        if (!p)
            return;
    } else if (drive)
        *drive = '\0';

    s = strrchr(p, '\\');
    if (s != NULL) {
        if (path) {
            char c;

            c = *(s + 1);
            *(s + 1) = '\0';
            strcpy(path, p);
            *(s + 1) = c;
        }
        p = s + 1;
        if (!p)
            return;
    } else if (path)
        *path = '\0';

    s = strchr(p, '.');
    if (s != NULL) {
        if (base) {
            *s = '\0';
            strcpy(base, p);
            *s = '.';
        }
        p = s + 1;
        if (!p)
            return;
    } else if (base)
        *base = '\0';

    if (ext)
        strcpy(ext, p);
}
#endif
