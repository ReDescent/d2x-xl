/*
 *
 * SDL tKeyboard input support
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>

#include "descent.h"
#include "event.h"
#include "error.h"
#include "key.h"
#include "input.h"
#include "timer.h"
#include "hudmsgs.h"
#include "maths.h"

#define KEY_BUFFER_SIZE 16

static uint8_t bInstalled = 0;

//-------- Variable accessed by outside functions ---------

typedef struct tKeyInfo {
    uint8_t state; // state of key 1 == down, 0 == up
    uint8_t lastState; // previous state of key
    int32_t counter; // incremented each time key is down in handler
    fix timeWentDown; // simple counter incremented each time in interrupt and key is down
    fix timeHeldDown; // counter to tell how long key is down -- gets reset to 0 by key routines
    uint8_t downCount; // number of key counts key was down
    uint8_t upCount; // number of times key was released
    uint8_t flags;
} tKeyInfo;

typedef struct tKeyboard {
    uint16_t keyBuffer[KEY_BUFFER_SIZE];
    tKeyInfo keys[256];
    fix xTimePressed[KEY_BUFFER_SIZE];
    uint32_t nKeyHead, nKeyTail;
} tKeyboard;

static tKeyboard keyData;

typedef struct tKeyProps {
    const char *pszKeyText;
    wchar_t asciiValue;
    wchar_t shiftedAsciiValue;
    SDL_KeyCode sym;
} tKeyProps;

const SDL_KeyCode INVALID_KEY = SDLK_UNKNOWN;

tKeyProps keyProperties[256] = {
    {"", 255, 255, INVALID_KEY},
    {"ESC", 255, 255, SDLK_ESCAPE},
    {"1", '1', '!', SDLK_1},
    {"2", '2', '@', SDLK_2},
    {"3", '3', '#', SDLK_3},
    {"4", '4', '$', SDLK_4},
    {"5", '5', '%', SDLK_5},
    {"6", '6', '^', SDLK_6},
    {"7", '7', '&', SDLK_7},
    {"8", '8', '*', SDLK_8},
    {"9", '9', '(', SDLK_9},
    {"0", '0', ')', SDLK_0},
    {"-", '-', '_', SDLK_MINUS},
    {"=", '=', '+', SDLK_EQUALS},
    {"BSPC", 255, 255, SDLK_BACKSPACE},
    {"TAB", 255, 255, SDLK_TAB},
    {"Q", 'q', 'Q', SDLK_q},
    {"W", 'w', 'W', SDLK_w},
    {"E", 'e', 'E', SDLK_e},
    {"R", 'r', 'R', SDLK_r},
    {"T", 't', 'T', SDLK_t},
    {"Y", 'y', 'Y', SDLK_y},
    {"U", 'u', 'U', SDLK_u},
    {"I", 'i', 'I', SDLK_i},
    {"O", 'o', 'O', SDLK_o},
    {"P", 'p', 'P', SDLK_p},
    {"[", '[', '{', SDLK_LEFTBRACKET},
    {"]", ']', '}', SDLK_RIGHTBRACKET},
    {"RET", 255, 255, SDLK_RETURN},
    {"LCTRL", 255, 255, SDLK_LCTRL},
    {"A", 'a', 'A', SDLK_a},
    {"S", 's', 'S', SDLK_s},
    {"D", 'd', 'D', SDLK_d},
    {"F", 'f', 'F', SDLK_f},
    {"G", 'g', 'G', SDLK_g},
    {"H", 'h', 'H', SDLK_h},
    {"J", 'j', 'J', SDLK_j},
    {"K", 'k', 'K', SDLK_k},
    {"L", 'l', 'L', SDLK_l},
    {";", ';', ':', SDLK_SEMICOLON},
    {"'", '\'', '"', SDLK_QUOTE},
    {"`", '`', '~', SDLK_BACKQUOTE},
    {"LSHFT", 255, 255, SDLK_LSHIFT},
    {"\\", '\\', '|', SDLK_BACKSLASH},
    {"Z", 'z', 'Z', SDLK_z},
    {"X", 'x', 'X', SDLK_x},
    {"C", 'c', 'C', SDLK_c},
    {"V", 'v', 'V', SDLK_v},
    {"B", 'b', 'B', SDLK_b},
    {"N", 'n', 'N', SDLK_n},
    {"M", 'm', 'M', SDLK_m},
    {",", ',', '<', SDLK_COMMA},
    {".", '.', '>', SDLK_PERIOD},
    {"/", '/', '?', SDLK_SLASH},
    {"RSHFT", 255, 255, SDLK_RSHIFT},
    {"PAD*", '*', 255, SDLK_KP_MULTIPLY},
    {"LALT", 255, 255, SDLK_LALT},
    {"SPC", ' ', ' ', SDLK_SPACE},
    {"CPSLK", 255, 255, SDLK_CAPSLOCK},
    {"F1", 255, 255, SDLK_F1},
    {"F2", 255, 255, SDLK_F2},
    {"F3", 255, 255, SDLK_F3},
    {"F4", 255, 255, SDLK_F4},
    {"F5", 255, 255, SDLK_F5},
    {"F6", 255, 255, SDLK_F6},
    {"F7", 255, 255, SDLK_F7},
    {"F8", 255, 255, SDLK_F8},
    {"F9", 255, 255, SDLK_F9},
    {"F10", 255, 255, SDLK_F10},
    {"NMLCK", 255, 255, SDLK_NUMLOCKCLEAR},
    {"SCLK", 255, 255, SDLK_SCROLLLOCK},
    {"PAD7", 255, 255, SDLK_KP_7},
    {"PAD8", 255, 255, SDLK_KP_8},
    {"PAD9", 255, 255, SDLK_KP_9},
    {"PAD-", 255, 255, SDLK_KP_MINUS},
    {"PAD4", 255, 255, SDLK_KP_4},
    {"PAD5", 255, 255, SDLK_KP_5},
    {"PAD6", 255, 255, SDLK_KP_6},
    {"PAD+", 255, 255, SDLK_KP_PLUS},
    {"PAD1", 255, 255, SDLK_KP_1},
    {"PAD2", 255, 255, SDLK_KP_2},
    {"PAD3", 255, 255, SDLK_KP_3},
    {"PAD0", 255, 255, SDLK_KP_0},
    {"PAD.", 255, 255, SDLK_KP_PERIOD},
    {"ALTGR", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"F11", 255, 255, SDLK_F11},
    {"F12", 255, 255, SDLK_F12},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"PAUSE", 255, 255, SDLK_PAUSE},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"PAD�", 255, 255, SDLK_KP_ENTER},
    {"RCTRL", 255, 255, SDLK_RCTRL},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"PAD/", 255, 255, SDLK_KP_DIVIDE},
    {"", 255, 255, INVALID_KEY},
    {"PRSCR", 255, 255, SDLK_PRINTSCREEN},
    {"RALT", 255, 255, SDLK_RALT},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"HOME", 255, 255, SDLK_HOME},
    {"UP", 255, 255, SDLK_UP},
    {"PGUP", 255, 255, SDLK_PAGEUP},
    {"", 255, 255, INVALID_KEY},
    {"LEFT", 255, 255, SDLK_LEFT},
    {"", 255, 255, INVALID_KEY},
    {"RIGHT", 255, 255, SDLK_RIGHT},
    {"", 255, 255, INVALID_KEY},
    {"END", 255, 255, SDLK_END},
    {"DOWN", 255, 255, SDLK_DOWN},
    {"PGDN", 255, 255, SDLK_PAGEDOWN},
    {"INS", 255, 255, SDLK_INSERT},
    {"DEL", 255, 255, SDLK_DELETE},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY},
    {"RWIN", 255, 255, SDLK_RGUI},
    {"LWIN", 255, 255, SDLK_LGUI},
    {"", 255, 255, INVALID_KEY},
    {"", 255, 255, INVALID_KEY}
    /*
    { "RCMD",   255,    255,    SDLK_RMETA         },
    { "LCMD",   255,    255,    SDLK_LMETA         }
    */
};

const char *pszKeyText[256];

//------------------------------------------------------------------------------

// FIXME: this is complete BS in SDL 2
uint8_t KeyToASCII(int32_t keyCode) {
    int32_t shifted;

    shifted = keyCode & KEY_SHIFTED;
    keyCode &= 0xFF;
    return shifted ? static_cast<uint8_t>(keyProperties[keyCode].shiftedAsciiValue)
                   : static_cast<uint8_t>(keyProperties[keyCode].asciiValue);
}

uint32_t KeyGetShiftStatus(void) {
    uint32_t shift_status = 0;

    if (gameStates.input.keys.pressed[KEY_LSHIFT] || gameStates.input.keys.pressed[KEY_RSHIFT])
        shift_status |= KEY_SHIFTED;
    if (gameStates.input.keys.pressed[KEY_LALT] || gameStates.input.keys.pressed[KEY_RALT])
        shift_status |= KEY_ALTED;
    if (gameStates.input.keys.pressed[KEY_LCTRL] || gameStates.input.keys.pressed[KEY_RCTRL])
        shift_status |= KEY_CTRLED;
    return shift_status;
}


//------------------------------------------------------------------------------

void KeyHandler(SDL_KeyboardEvent *event) {
    bool is_pressed = (event->state == SDL_PRESSED);
    SDL_Keycode keySym = event->keysym.sym;

    for (int32_t i = 0; i < sizeofa(keyProperties); i++) {
        if(keyProperties[i].sym == SDLK_UNKNOWN) {
            continue;
        }
        int32_t keyCode = i;
        tKeyInfo * pKey = keyData.keys + keyCode;

        uint8_t state;

        if (keyProperties[i].sym == keySym)
            state = is_pressed;
        else
            state = pKey->lastState;

        // WTF is going on here? why are we looping over everything for every event????
        if (pKey->lastState == state) {
            if (state) {
                pKey->counter++;
                gameStates.input.keys.xLastPressTime = TimerGetFixedSeconds();
                pKey->flags = 0;
                if (gameStates.input.keys.pressed[KEY_LSHIFT] || gameStates.input.keys.pressed[KEY_RSHIFT])
                    pKey->flags |= uint8_t(KEY_SHIFTED / 256);
                if (gameStates.input.keys.pressed[KEY_LALT] || gameStates.input.keys.pressed[KEY_RALT])
                    pKey->flags |= uint8_t(KEY_ALTED / 256);
                if ((gameStates.input.keys.pressed[KEY_LCTRL]) || gameStates.input.keys.pressed[KEY_RCTRL])
                    pKey->flags |= uint8_t(KEY_CTRLED / 256);
            }
        } else {
            if (state) {
                pKey->timeWentDown = gameStates.input.keys.xLastPressTime = TimerGetFixedSeconds();
                keyData.keys[keyCode].timeHeldDown = 0;
                gameStates.input.keys.pressed[keyCode] = 1;
                pKey->downCount += state;
                pKey->counter++;
                pKey->state = 1;
                pKey->flags = 0;
                if (gameStates.input.keys.pressed[KEY_LSHIFT] || gameStates.input.keys.pressed[KEY_RSHIFT])
                    pKey->flags |= uint8_t(KEY_SHIFTED / 256);
                if (gameStates.input.keys.pressed[KEY_LALT] || gameStates.input.keys.pressed[KEY_RALT])
                    pKey->flags |= uint8_t(KEY_ALTED / 256);
                if ((gameStates.input.keys.pressed[KEY_LCTRL]) || gameStates.input.keys.pressed[KEY_RCTRL])
                    pKey->flags |= uint8_t(KEY_CTRLED / 256);
            } else {
                gameStates.input.keys.pressed[keyCode] = 0;
                pKey->upCount += pKey->state;
                pKey->state = 0;
                pKey->counter = 0;
                pKey->timeHeldDown += TimerGetFixedSeconds() - pKey->timeWentDown;
            }
        }
        if (state && (!pKey->lastState || ((pKey->counter > 30) && (pKey->counter & 1)))) {
            if (gameStates.input.keys.pressed[KEY_LSHIFT] || gameStates.input.keys.pressed[KEY_RSHIFT])
                keyCode |= KEY_SHIFTED;
            if (gameStates.input.keys.pressed[KEY_LALT] || gameStates.input.keys.pressed[KEY_RALT])
                keyCode |= KEY_ALTED;
            if ((gameStates.input.keys.pressed[KEY_LCTRL]) || gameStates.input.keys.pressed[KEY_RCTRL])
                keyCode |= KEY_CTRLED;
            if (gameStates.input.keys.pressed[KEY_LCMD] || gameStates.input.keys.pressed[KEY_RCMD])
                keyCode |= KEY_COMMAND;

            uint8_t temp = keyData.nKeyTail + 1;
            if (temp >= KEY_BUFFER_SIZE)
                temp = 0;
            if (temp != keyData.nKeyHead) {
                keyData.keyBuffer[keyData.nKeyTail] = keyCode;
                keyData.xTimePressed[keyData.nKeyTail] = gameStates.input.keys.xLastPressTime;
                keyData.nKeyTail = temp;
            }
        }
        pKey->lastState = state;
    }
}

//------------------------------------------------------------------------------

void _CDECL_ KeyClose(void) { bInstalled = 0; }

//------------------------------------------------------------------------------

void KeyInit(void) {
    int32_t i;

    if (bInstalled)
        return;
    bInstalled = 1;
    gameStates.input.keys.xLastPressTime = TimerGetFixedSeconds();
    gameStates.input.keys.nBufferType = 1;
    gameStates.input.keys.bRepeat = 1;
    for (i = 0; i < 255; i++)
        pszKeyText[i] = keyProperties[i].pszKeyText;
    // Clear the tKeyboard array
    KeyFlush();
    atexit(KeyClose);
}

//------------------------------------------------------------------------------

void KeyFlush(void) {
    int32_t i;

    if (!bInstalled)
        KeyInit();
    keyData.nKeyHead = keyData.nKeyTail = 0;
    // clear the tKeyboard buffer
    for (i = 0; i < KEY_BUFFER_SIZE; i++) {
        keyData.keyBuffer[i] = 0;
        keyData.xTimePressed[i] = 0;
    }
    int32_t curtime = TimerGetFixedSeconds();
    for (i = 0; i < 256; i++) {
        gameStates.input.keys.pressed[i] = 0;
        keyData.keys[i].state = 1;
        keyData.keys[i].lastState = 0;
        keyData.keys[i].timeWentDown = curtime;
        keyData.keys[i].downCount = 0;
        keyData.keys[i].upCount = 0;
        keyData.keys[i].timeHeldDown = 0;
        keyData.keys[i].counter = 0;
    }
}

//------------------------------------------------------------------------------

int32_t KeyAddKey(int32_t n) { return (++n < KEY_BUFFER_SIZE) ? n : 0; }

//------------------------------------------------------------------------------

int32_t KeyCheckChar(void) {
    event_poll();
    return (keyData.nKeyTail != keyData.nKeyHead);
}

//------------------------------------------------------------------------------

int32_t KeyInKeyTime(fix *time) {
    int32_t key = 0;
    int32_t bLegacy = gameOpts->legacy.bInput;

    gameOpts->legacy.bInput = 1;

    if (!bInstalled)
        KeyInit();
    event_poll();
    if (keyData.nKeyTail != keyData.nKeyHead) {
        key = keyData.keyBuffer[keyData.nKeyHead];
        if ((key == KEY_CTRLED + KEY_ALTED + KEY_ENTER) || (key == KEY_ALTED + KEY_F4))
            exit(0);
        keyData.nKeyHead = KeyAddKey(keyData.nKeyHead);
        if (time)
            *time = keyData.xTimePressed[keyData.nKeyHead];
    } else if (!time)
        G3_SLEEP(0);
    gameOpts->legacy.bInput = bLegacy;
    return key;
}

//------------------------------------------------------------------------------

int32_t KeyInKey(void) { return KeyInKeyTime(NULL); }

//------------------------------------------------------------------------------

int32_t KeyGetChar(void) {
    if (!bInstalled)
        return 0;
    while (!KeyCheckChar())
        ;
    return KeyInKey();
}

//------------------------------------------------------------------------------

// Returns the number of seconds this key has been down since last call.
fix KeyDownTime(int32_t scanCode) {
    static fix lastTime = -1;
    fix timeDown, time, slack = 0;

    if ((scanCode < 0) || (scanCode > 255))
        return 0;

    if (!gameStates.input.keys.pressed[scanCode]) {
        timeDown = keyData.keys[scanCode].timeHeldDown;
        keyData.keys[scanCode].timeHeldDown = 0;
    } else {
        int64_t s, ms;
#if DBG
        if (scanCode == 72)
            scanCode = scanCode;
#endif
        time = TimerGetFixedSeconds();
        timeDown = time - keyData.keys[scanCode].timeWentDown;
        s = timeDown / 65536;
        ms = (timeDown & 0xFFFF);
        ms *= 1000;
        ms >>= 16;
        keyData.keys[scanCode].timeHeldDown += (int32_t)(s * 1000 + ms);
        // the following code takes care of clamping in KConfig.c::control_read_all()
        if (gameStates.input.bKeepSlackTime && (timeDown > controls.PollTime())) {
            slack = (fix)(timeDown - controls.PollTime());
            time -= slack + slack / 10; // there is still some slack, so add an extra 10%
            if (time < lastTime)
                time = lastTime;
            timeDown = fix(controls.PollTime());
        }
        keyData.keys[scanCode].timeWentDown = time;
        lastTime = time;
        if (timeDown && timeDown < controls.PollTime())
            timeDown = fix(controls.PollTime());
    }
    return timeDown;
}

//------------------------------------------------------------------------------

uint32_t KeyDownCount(int32_t scanCode) {
    int32_t n;

    if ((scanCode < 0) || (scanCode > 255))
        return 0;

    n = keyData.keys[scanCode].downCount;
    keyData.keys[scanCode].downCount = 0;
    keyData.keys[scanCode].flags = 0;
    return n;
}

//------------------------------------------------------------------------------

uint8_t KeyFlags(int32_t scanCode) {
    if ((scanCode < 0) || (scanCode > 255))
        return 0;
    return keyData.keys[scanCode].flags;
}

//------------------------------------------------------------------------------

uint32_t KeyUpCount(int32_t scanCode) {
    int32_t n;

    if ((scanCode < 0) || (scanCode > 255))
        return 0;
    n = keyData.keys[scanCode].upCount;
    keyData.keys[scanCode].upCount = 0;
    return n;
}

//------------------------------------------------------------------------------

fix KeyRamp(int32_t scanCode) {
    if (!gameOpts->input.keyboard.nRamp)
        return 1;
    else {
        int32_t maxRampTime = gameOpts->input.keyboard.nRamp * 20; // / gameOpts->input.keyboard.nRamp;
        fix t = keyData.keys[scanCode].timeHeldDown;

        if (!t)
            return maxRampTime;
        if (t >= maxRampTime)
            return 1;
        t = maxRampTime / t;
        return t ? t : 1;
    }
}
