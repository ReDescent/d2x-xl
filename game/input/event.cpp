/*
 *
 * SDL Event related stuff
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define TO_EVENT_POLL 11 // ms
#include <time.h>

#include <SDL.h>

#include "descent.h"
#include "gamecntl.h"

namespace {
void WindowFocusHandler(int window, bool gained) {
    // TODO: implement game pause when losing focus
}

void WindowCloseHandler(int window) {
    // TODO: implement proper closing
}
}

extern void KeyHandler(SDL_KeyboardEvent *event);
extern void MouseButtonHandler(SDL_MouseButtonEvent *mbe);
extern void MouseMotionHandler(SDL_MouseMotionEvent *mme);
#ifndef USE_LINUX_JOY // stpohle - so we can choose at compile time..
extern void JoyButtonHandler(SDL_JoyButtonEvent *jbe);
extern void JoyHatHandler(SDL_JoyHatEvent *jhe);
extern void JoyAxisHandler(SDL_JoyAxisEvent *jae);
#endif

static int32_t initialised = 0;

void event_poll() {
    SDL_Event event;
    time_t t0 = SDL_GetTicks();
    time_t _t = t0;

    while (SDL_PollEvent(&event)) {
        if (!gameOpts->legacy.bInput)
            _t = SDL_GetTicks();

        switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            KeyHandler(reinterpret_cast<SDL_KeyboardEvent *>(&event));
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            MouseButtonHandler(reinterpret_cast<SDL_MouseButtonEvent *>(&event));
            break;
        case SDL_MOUSEMOTION:
            MouseMotionHandler(reinterpret_cast<SDL_MouseMotionEvent *>(&event));
            break;
#ifndef USE_LINUX_JOY // stpohle - so we can choose at compile time..
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            JoyButtonHandler(reinterpret_cast<SDL_JoyButtonEvent *>(&event));
            break;
        case SDL_JOYAXISMOTION:
            JoyAxisHandler(reinterpret_cast<SDL_JoyAxisEvent *>(&event));
            break;
        case SDL_JOYHATMOTION:
            JoyHatHandler(reinterpret_cast<SDL_JoyHatEvent *>(&event));
            break;
        case SDL_JOYBALLMOTION:
            break;
#endif
        case SDL_WINDOWEVENT: {
            switch(event.window.event) {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                WindowFocusHandler(event.window.windowID, true);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                WindowFocusHandler(event.window.windowID, false);
                break;
            case SDL_WINDOWEVENT_CLOSE:
                WindowCloseHandler(event.window.windowID);
                break;
            default:
                break;
            }
            break;
        }

        case SDL_QUIT:
            break;
        }

        if (!gameOpts->legacy.bInput && (_t - t0 >= TO_EVENT_POLL))
            break;
    }
}

int32_t event_init() {
    // We should now be active and responding to events.
    initialised = 1;

    return 0;
}
