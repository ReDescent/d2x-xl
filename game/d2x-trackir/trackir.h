#ifndef __TRACKIR_H
#define __TRACKIR_H

typedef struct fVector3D {
    float x, y, z;
} fVector3D;

typedef struct tTIRInfo {
    fVector3D fvRot;
    fVector3D fvTrans;
} tTIRInfo;

#ifdef WIN32

#include "windows.h"

int WINAPI TIRInit(HWND);
int WINAPI TIRExit(void);
int WINAPI TIRStart(void);
int WINAPI TIRStop(void);
int WINAPI TIRCenter(void);
int WINAPI TIRQuery(tTIRInfo *);

#else // WIN32

#define TIRInit(_hWnd)
#define TIRExit()
#define TIRStart()
#define TIRStop()
#define TIRCenter () 0
#define TIRQuery (_pti)0

#endif // WIN32
#endif // TRACKIR_H
