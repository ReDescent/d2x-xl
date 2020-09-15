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

#ifndef _GR_H
#define _GR_H

#include "pstypes.h"
#include "fix.h"
//#include "palette.h"
#include "vecmat.h"

//-----------------------------------------------------------------------------

// these are control characters that have special meaning in the font code

#define CC_COLOR 1 // next char is new foreground color
#define CC_LSPACING 2 // next char specifies line spacing
#define CC_UNDERLINE 3 // next char is underlined

// now have string versions of these control characters (can concat inside a string)

#define CC_COLOR_S "\x1" // next char is new foreground color
#define CC_LSPACING_S "\x2" // next char specifies line spacing
#define CC_UNDERLINE_S "\x3" // next char is underlined

//-----------------------------------------------------------------------------

#define SM(w, h) ((((uint32_t)w) << 16) + (((uint32_t)h) & 0xFFFF))
#define SM_W(m) (m >> 16)
#define SM_H(m) (m & 0xFFFF)

//=========================================================================
// System functions:
// setup and set mode. this creates a grsScreen structure and sets
// grdCurScreen to point to it.  grs_curcanv points to this screen's
// canvas.  Saves the current VGA state and screen mode.

int32_t GrInit(void);

int32_t GrVideoModeOK(uint32_t mode);
int32_t GrSetMode(uint32_t mode);

//-----------------------------------------------------------------------------
// These 4 functions actuall change screen colors.

extern void gr_pal_fade_out(uint8_t *pal);
extern void gr_pal_fade_in(uint8_t *pal);
extern void gr_pal_clear(void);
extern void gr_pal_setblock(int32_t start, int32_t number, uint8_t *pal);
extern void gr_pal_getblock(int32_t start, int32_t number, uint8_t *pal);

extern uint8_t *gr_video_memory;

// shut down the 2d.  Restore the screen mode.
void _CDECL_ GrClose(void);

//=========================================================================

#include "bitmap.h"

//=========================================================================
// Color functions:

// When this function is called, the guns are set to gr_palette, and
// the palette stays the same until GrClose is called

void GrCopyPalette(uint8_t *gr_palette, uint8_t *pal, int32_t size);

//=========================================================================
// Drawing functions:

// -----------------------------------------------------------------------------
// Draw a polygon into the current canvas in the current color and drawmode.
// verts points to an ordered list of x, y pairs.  the polygon should be
// convex; a concave polygon will be handled in some reasonable manner,
// but not necessarily shaded as a concave polygon. It shouldn't hang.
// probably good solution is to shade from minx to maxx on each scan line.
// int32_t should really be fix
void DrawPixelClipped(int32_t x, int32_t y);
void DrawPixel(int32_t x, int32_t y);

void GrMergeTextures(uint8_t *lower, uint8_t *upper, uint8_t *dest, uint16_t width, uint16_t height, int32_t scale);
void GrMergeTextures1(uint8_t *lower, uint8_t *upper, uint8_t *dest, uint16_t width, uint16_t height, int32_t scale);
void GrMergeTextures2(uint8_t *lower, uint8_t *upper, uint8_t *dest, uint16_t width, uint16_t height, int32_t scale);
void GrMergeTextures3(uint8_t *lower, uint8_t *upper, uint8_t *dest, uint16_t width, uint16_t height, int32_t scale);

void SaveScreenShot(uint8_t *buf, int32_t automapFlag);
void AutoScreenshot(void);

/*
 * currently SDL and OGL are the only things that supports toggling
 * fullgameData.render.screen.  otherwise add other checks to the #if -MPM
 */
#define GR_SUPPORTS_FULLSCREEN_TOGGLE

void ResetTextures(int32_t bReload, int32_t bGame);

#pragma pack(push, 1)
typedef struct tScrSize {
    int32_t x, y, c;
} tScrSize;
#pragma pack(pop)

extern int32_t curDrawBuffer;

char *ScrSizeArg(int32_t x, int32_t y);

class CDisplayModeInfo {
    public:
    int32_t dim;
    int16_t w, h;
    int16_t renderMethod;
    int16_t flags;
    char bWideScreen;
    char bFullScreen;
    char bAvailable;

    inline bool operator<(CDisplayModeInfo &other) {
        if (bWideScreen < other.bWideScreen)
            return true;
        if (bWideScreen > other.bWideScreen)
            return false;
        if (w < other.w)
            return true;
        if (w > other.w)
            return false;
        return h < other.h;
    }

    inline bool operator>(CDisplayModeInfo &other) {
        if (bWideScreen > other.bWideScreen)
            return true;
        if (bWideScreen < other.bWideScreen)
            return false;
        if (w > other.w)
            return true;
        if (w < other.w)
            return false;
        return h > other.h;
    }
};

extern CArray<CDisplayModeInfo> displayModeInfo;

#define NUM_DISPLAY_MODES int32_t(displayModeInfo.Length())
#define CUSTOM_DISPLAY_MODE (NUM_DISPLAY_MODES - 1)
#define MAX_DISPLAY_MODE (NUM_DISPLAY_MODES - 2)

int32_t FindDisplayMode(int32_t nScrSize);
int32_t SetCustomDisplayMode(int32_t w, int32_t h, int32_t bFullScreen);

#endif /* def _GR_H */
