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

/*
 *
 * Stuff for rendering the HUD
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "descent.h"
#include "error.h"
#include "rendermine.h"
#include "screens.h"
#include "console.h"
#include "cockpit.h"
#include "gamefont.h"
#include "newdemo.h"
#include "text.h"
#include "textdata.h"
#include "gr.h"
#include "ogl_render.h"
#include "endlevel.h"
#include "playerprofile.h"
#include "automap.h"
#include "gamepal.h"
#include "visibility.h"
#include "lightning.h"
#include "rendershadows.h"
#include "transprender.h"
#include "radar.h"
#include "menubackground.h"
#include "addon_bitmaps.h"
#include "createmesh.h"
#include "renderthreads.h"
#include "glow.h"
#include "postprocessing.h"

#define bSavingMovieFrames 0

void StartLightingFrame(CObject *viewer);
bool GuidedMissileActive(void);

uint32_t nClearWindowColor = 0;

//------------------------------------------------------------------------------

int32_t RenderMissileView(void) {
    ENTER(0, 0);
    CObject *pObj = NULL;

    if (GuidedMslView(&pObj)) {
        if (gameOpts->render.cockpit.bGuidedInMainView) {
            gameStates.render.nRenderingType = 6 + (1 << 4);
            cockpit->RenderWindow(1, gameData.objData.pViewer, 0, WBUMSL, "SHIP");
        } else {
            gameStates.render.nRenderingType = 1 + (1 << 4);
            cockpit->RenderWindow(1, pObj, 0, WBU_GUIDED, "GUIDED");
        }
        RETVAL(1)
    } else {
        if (pObj) { // used to be active
            if (!gameOpts->render.cockpit.bGuidedInMainView)
                cockpit->RenderWindow(1, NULL, 0, WBU_STATIC, NULL);
            gameData.objData.SetGuidedMissile(N_LOCALPLAYER, NULL);
        }
        if (gameData.objData.pMissileViewer && !gameStates.render.bChaseCam) { // do missile view
            static int32_t mslViewerSig = -1;
            if (mslViewerSig == -1)
                mslViewerSig = gameData.objData.pMissileViewer->info.nSignature;
            if (gameOpts->render.cockpit.bMissileView && (gameData.objData.pMissileViewer->info.nType != OBJ_NONE) &&
                (gameData.objData.pMissileViewer->info.nSignature == mslViewerSig)) {
                // HUDMessage (0, "missile view");
                gameStates.render.nRenderingType = 2 + (1 << 4);
                cockpit->RenderWindow(1, gameData.objData.pMissileViewer, 0, WBUMSL, "MISSILE");
                RETVAL(1)
            } else {
                gameData.objData.pMissileViewer = NULL;
                mslViewerSig = -1;
                gameStates.render.nRenderingType = 255;
                cockpit->RenderWindow(1, NULL, 0, WBU_STATIC, NULL);
            }
        }
    }
    RETVAL(0)
}

//------------------------------------------------------------------------------

void FadeMine(float color) {
    ogl.SetBlendMode(OGL_BLEND_MULTIPLY);
    glColor4f(color, color, color, 1.0f);
    ogl.ResetClientStates(1);
    ogl.SetTexturing(false);
    ogl.SetDepthTest(false);
    ogl.RenderScreenQuad();
    ogl.SetDepthTest(true);
    ogl.SetBlendMode(OGL_BLEND_ALPHA);
}

//------------------------------------------------------------------------------

void FlashMine(void) {
    if (gameOpts->app.bEpilepticFriendly || !gameStates.render.nFlashScale)
        return;
    FadeMine(1.0f - X2F(gameStates.render.nFlashScale) * 0.8f);
}

//------------------------------------------------------------------------------

int32_t FadeInMine(void) {
    if (LOCALOBJECT->AppearanceStage() > -1)
        return 0;
    int32_t t = LOCALOBJECT->AppearanceTimer();
    int32_t d = I2X(1);
    int32_t a = APPEARANCE_DELAY - d;
    if (t > -a)
        return 0;
    t += a;
    FadeMine(X2F(d + t) / X2F(d));
    return 1;
}

//------------------------------------------------------------------------------

void Draw2DFrameElements(void) {
    ENTER(0, 0);
    fix xStereoSeparation = ogl.StereoSeparation();

    ogl.ColorMask(1, 1, 1, 1, 0);
    if (ogl.StereoDevice() < 0)
        ogl.ChooseDrawBuffer();
    else {
        ogl.SetDrawBuffer(GL_BACK, 0);
        ogl.SetStereoSeparation(0);
    }

    if (gameStates.app.bGameRunning) {
        if (automap.Active())
            automap.RenderInfo();
        else {
            PROF_START
            cockpit->Render(!(gameOpts->render.cockpit.bGuidedInMainView && GuidedMissileActive()), 0);
            PROF_END(ptCockpit)
        }
    }
    if (xStereoSeparation >= 0) {
        paletteManager.RenderEffect();
        if (!FadeInMine())
            FlashMine();
    }

    console.Draw();
    if (gameStates.app.bShowVersionInfo || gameStates.app.bSaveScreenShot ||
        (gameData.demoData.nState == ND_STATE_PLAYBACK))
        PrintVersionInfo();
    if (CMenu::Active()) {
#if DBG
        if (!gameStates.menus.nInMenu)
            PrintLog(0, "invalid menu handle in renderer\n");
        else
#endif
            CMenu::Active()->Render();
    }
    ogl.SetStereoSeparation(xStereoSeparation);
    RETURN
}

//------------------------------------------------------------------------------

void FlushFrame(fix xStereoSeparation) {
    ENTER(0, 0);
    int32_t i = ogl.StereoDevice();

    if (!(i && xStereoSeparation)) { // no stereo or shutter glasses or Oculus Rift
        // if (gameStates.render.bRenderIndirect <= 0)
        // Draw2DFrameElements ();
        ogl.SwapBuffers(0, 0);
    } else {
        if ((i < 0) || (xStereoSeparation > 0)) {
            gameData.renderData.scene.SetViewport(&gameData.renderData.screen);
            if (i < 0)
                Draw2DFrameElements();
            if (xStereoSeparation > 0) {
                gameData.renderData.screen.SetViewport();
                ogl.SwapBuffers(0, 0);
            }
        } else {
            ogl.ColorMask(1, 1, 1, 1, 0);
        }
    }
    RETURN
}

//------------------------------------------------------------------------------

#if MAX_SHADOWMAPS

static int32_t RenderShadowMap(CDynLight *pLight, int32_t nLight, fix xStereoSeparation) {
    if (!pLight)
        return 0;

    CCamera *pCamera = cameraManager.ShadowMap(nLight);

    if (!(pCamera || (pCamera = cameraManager.AddShadowMap(nLight, pLight))))
        return 0;

    if (pCamera->HaveBuffer(0))
        pCamera->Setup(
            pCamera->Id(),
            pLight->info.nSegment,
            pLight->info.nSide,
            -1,
            -1,
            (pLight->info.nObject < 0) ? NULL : OBJECT(pLight->info.nObject),
            0);
    else if (!pCamera->Create(
                 cameraManager.Count() - 1,
                 pLight->info.nSegment,
                 pLight->info.nSide,
                 -1,
                 -1,
                 (pLight->info.nObject < 0) ? NULL : OBJECT(pLight->info.nObject),
                 1,
                 0)) {
        cameraManager.DestroyShadowMap(nLight);
        return 0;
    }
    CCamera *current = cameraManager[cameraManager.Current()];
    pCamera = cameraManager.ShadowMap(nLight);
    cameraManager.SetCurrent(pCamera);
    pCamera->Render();
    cameraManager.SetCurrent(current);
    return 1;
}

//------------------------------------------------------------------------------

static void RenderShadowMaps(fix xStereoSeparation) {
    if (EGI_FLAG(bShadows, 0, 1, 0)) {
        lightManager.ResetActive(1, 0);
        int16_t nSegment = OBJSEG(gameData.objData.pViewer);
        lightManager.ResetNearestStatic(nSegment, 1);
        lightManager.SetNearestStatic(nSegment, 1, 1);
        CDynLightIndex *pLightIndex = &lightManager.Index(0, 1);
        CActiveDynLight *pActiveLights = lightManager.Active(1) + pLightIndex->nFirst;
        int32_t nLights = 0,
                h = (pLightIndex->nActive < abs(MAX_SHADOWMAPS)) ? pLightIndex->nActive : abs(MAX_SHADOWMAPS);
        for (gameStates.render.nShadowMap = 1; gameStates.render.nShadowMap <= h; gameStates.render.nShadowMap++)
            nLights += RenderShadowMap(lightManager.GetActive(pActiveLights, 1), nLights, xStereoSeparation);
        lightManager.SetLightCount(nLights, 2);
        gameStates.render.nShadowMap = 0;
    }
}

#endif

//------------------------------------------------------------------------------

extern CBitmap bmBackground;

void RenderFrame(fix xStereoSeparation, int32_t nWindow) {
    ENTER(0, 0);
    int16_t nStartSeg;
    fix nEyeOffsetSave = gameStates.render.xStereoSeparation[0];

    gameStates.render.nType = -1;
    gameStates.render.SetRenderWindow(nWindow);
    gameStates.render.SetStereoSeparation(xStereoSeparation);
    if (gameStates.app.bEndLevelSequence) {
        RenderEndLevelFrame(xStereoSeparation, nWindow);
        gameData.appData.nFrameCount++;
        gameStates.render.xStereoSeparation[0] = nEyeOffsetSave;
        RETURN
    }
    if ((gameData.demoData.nState == ND_STATE_RECORDING) && (xStereoSeparation >= 0)) {
        if (!gameStates.render.nRenderingType)
            NDRecordStartFrame(gameData.appData.nFrameCount, gameData.timeData.xFrame);
        if (gameStates.render.nRenderingType != 255)
            NDRecordViewerObject(gameData.objData.pViewer);
    }

    StartLightingFrame(gameData.objData.pViewer); // this is for ugly light-smoothing hack
    // ogl.m_states.bEnableScissor = !gameStates.render.cameras.bActive && nWindow;

    if (!nWindow)
        cockpit->Setup(true);

    {
        PROF_START
        G3StartFrame(
            transformation,
            0,
            (nWindow || gameStates.render.cameras.bActive) ? 0 : 1,
            nWindow ? nEyeOffsetSave : xStereoSeparation);
        ogl.SetStereoSeparation(xStereoSeparation);
        if (!nWindow) {
            // gameData.renderData.scene.Activate ();
            fontManager.SetCurrent(GAME_FONT);
            gameData.renderData.dAspect = (double)CCanvas::Current()->Width() / (double)CCanvas::Current()->Height();
        }
        SetupRenderView(xStereoSeparation, &nStartSeg, 1);
        transformation.ComputeFrustum();
#if MAX_SHADOWMAPS
        if (!(nWindow || gameStates.render.cameras.bActive)) {
            ogl.SetupTransform(1);
            transformation.SystemMatrix(-1).Get(GL_MODELVIEW_MATRIX, true); // inverse
            transformation.SystemMatrix(-2).Get(GL_PROJECTION_MATRIX, true);
            transformation.SystemMatrix(-3).Get(GL_PROJECTION_MATRIX, false);
            ogl.ResetTransform(1);
            glPushMatrix();
            transformation.SystemMatrix(-1).Set();
            transformation.SystemMatrix(-2).Mul();
            transformation.SystemMatrix(-3).Get(GL_MODELVIEW_MATRIX, false); // inverse (modelview * projection)
            glPopMatrix();
        }
#endif
        PROF_END(ptAux)
    }

    if (0 > (gameStates.render.nStartSeg = nStartSeg)) {
        G3EndFrame(transformation, nWindow);
        gameStates.render.xStereoSeparation[0] = nEyeOffsetSave;
        RETURN
    }

#if MAX_SHADOWMAPS
    RenderMine(nStartSeg, xStereoSeparation, nWindow);
#else
    if (!ogl.m_features.bStencilBuffer.Available())
        extraGameInfo[0].bShadows = extraGameInfo[1].bShadows = 0;

    if (SHOW_SHADOWS && !(nWindow || gameStates.render.cameras.bActive || automap.Active())) {
        if (!gameStates.render.nShadowMap) {
            gameStates.render.nShadowPass = 1;
#if SOFT_SHADOWS
            if (gameOpts->render.shadows.bSoft = 1)
                gameStates.render.nShadowBlurPass = 1;
#endif
            ogl.StartFrame(0, 0, xStereoSeparation);
#if SOFT_SHADOWS
            ogl.SetViewport(CCanvas::Current()->props.x, CCanvas::Current()->props.y, 128, 128);
#endif
            RenderMine(nStartSeg, xStereoSeparation, nWindow);
            PROF_START
            if (FAST_SHADOWS)
                RenderFastShadows(xStereoSeparation, nWindow, nStartSeg);
            else
                RenderNeatShadows(xStereoSeparation, nWindow, nStartSeg);
#if SOFT_SHADOWS
            if (gameOpts->render.shadows.bSoft) {
                PROF_START
                CreateShadowTexture();
                PROF_END(ptEffects)
                gameStates.render.nShadowBlurPass = 2;
                gameStates.render.nShadowPass = 0;
                ogl.StartFrame(0, 1, xStereoSeparation);
                SetupRenderView(xStereoSeparation, &nStartSeg, 1);
                RenderMine(nStartSeg, xStereoSeparation, nWindow);
                RenderShadowTexture();
            }
#endif
            PROF_END(ptEffects)
            nWindow = 0;
        }
    } else {
        if (gameStates.render.nRenderPass < 0)
            RenderMine(nStartSeg, xStereoSeparation, nWindow);
        else {
            for (gameStates.render.nRenderPass = 0; gameStates.render.nRenderPass < 2;
                 gameStates.render.nRenderPass++) {
                ogl.StartFrame(0, 1, xStereoSeparation);
                RenderMine(nStartSeg, xStereoSeparation, nWindow);
            }
        }
        // The following code is a hack resetting the blur buffers, because otherwise certain stuff rendered to it
        // doesn't get properly blended and colored. I have not been able to determine why this is so.
        if (glowRenderer.Available(0xFFFFFFFF)) {
            if (!(ogl.BlurBuffer(0)->Handle() && ogl.BlurBuffer(0)->ColorBuffer()))
                ogl.SelectBlurBuffer(0);
            if (!(ogl.BlurBuffer(1)->Handle() && ogl.BlurBuffer(1)->ColorBuffer()))
                ogl.SelectBlurBuffer(1);
            ogl.ChooseDrawBuffer();
        }
    }
    ogl.StencilOff();
#endif
    RenderSkyBox(nWindow);
    RenderEffects(nWindow);

    if (!(nWindow || gameStates.render.cameras.bActive || gameStates.app.bEndLevelSequence || GuidedInMainView())) {
        radar.Render();
    }
    gameStates.render.nShadowPass = 0;

    {
        PROF_START
        G3EndFrame(transformation, nWindow);
        if (nWindow) // set the auxiliary window viewport for fog rendering
            gameData.renderData.window.Activate("HUD Window (window)", &gameData.renderData.frame);
#if DBG
        if (nWindow)
            BRP;
#endif
        RenderFog();
        if (nWindow)
            gameData.renderData.window.Deactivate();
        // cockpit->Canvas ()->Deactivate ();
        if (nWindow)
            ogl.SetStereoSeparation(gameStates.render.xStereoSeparation[0] = nEyeOffsetSave);
        if (!ShowGameMessage(gameData.messages, -1, -1))
            ShowGameMessage(gameData.messages + 1, -1, -1);
        PROF_END(ptAux)
    }
    RETURN
}

//------------------------------------------------------------------------------

void RenderMonoFrame(fix xStereoSeparation = 0) {
    ENTER(0, 0);
    gameData.renderData.screen.Activate("RenderMonoFrame", NULL, true);
#if MAX_SHADOWMAPS
    RenderShadowMaps(xStereoSeparation);
#endif
    ogl.SetStereoSeparation(xStereoSeparation);
    SetupCanvasses();
    if (xStereoSeparation <= 0) {
        PROF_START
        SEM_ENTER(SEM_LIGHTNING)
        bool bRetry;
        do {
            bRetry = false;
            try {
                lightningManager.SetLights();
            } catch (...) {
                bRetry = true;
            }
        } while (bRetry);
        SEM_LEAVE(SEM_LIGHTNING)
        PROF_END(ptLighting)
    }

    if (gameOpts->render.cockpit.bGuidedInMainView && gameData.objData.GetGuidedMissile(N_LOCALPLAYER)) {
        int32_t w, h, aw;
        const char *msg = "Guided Missile View";

        if (gameStates.render.cockpit.nType == CM_FULL_COCKPIT) {
            gameStates.render.cockpit.bBigWindowSwitch = 1;
            gameStates.render.cockpit.bRedraw = 1;
            cockpit->Activate(CM_STATUS_BAR);
            RETURN
        }
        CObject *pViewerSave = gameData.SetViewer(gameData.objData.GetGuidedMissile(N_LOCALPLAYER));
        UpdateRenderedData(0, gameData.objData.pViewer, 0, 0);
        if ((xStereoSeparation <= 0) && cameraManager.Render())
            CCanvas::Current()->SetViewport();
        RenderFrame(xStereoSeparation, 0);
        if (xStereoSeparation <= 0)
            WakeupRenderedObjects(gameData.objData.pViewer, 0);
        gameData.SetViewer(pViewerSave);
        fontManager.SetCurrent(GAME_FONT); // GAME_FONT);
        fontManager.SetColorRGBi(RED_RGBA, 1, 0, 0);
        fontManager.Current()->StringSize(msg, w, h, aw);
        GrPrintF(NULL, (CCanvas::Current()->Width() - w) / 2, 3, msg);
        // DrawGuidedCrosshairs (xStereoSeparation);
        HUDRenderMessageFrame();
    } else {
        if (gameStates.render.cockpit.bBigWindowSwitch) {
            gameStates.render.cockpit.bRedraw = 1;
            cockpit->Activate(CM_FULL_COCKPIT);
            gameStates.render.cockpit.bBigWindowSwitch = 0;
            RETURN
        }
        UpdateRenderedData(0, gameData.objData.pViewer, gameStates.render.bRearView, 0);
        if ((xStereoSeparation <= 0) && cameraManager.Render())
            CCanvas::Current()->SetViewport();
        RenderFrame(xStereoSeparation, 0);
        cockpit->RenderWindows();
    }
    FlushFrame(xStereoSeparation);
    RETURN
}

//------------------------------------------------------------------------------

#define WINDOW_W_DELTA ((gameData.renderData.frame.Width() / 16) & ~1) // 24	//20
#define WINDOW_H_DELTA ((gameData.renderData.frame.Height() / 16) & ~1) // 12	//10

#define WINDOW_MIN_W ((gameData.renderData.frame.Width() * 10) / 22) // 160
#define WINDOW_MIN_H ((gameData.renderData.frame.Height() * 10) / 22)

//------------------------------------------------------------------------------

extern CBitmap bmBackground;

void CopyBackgroundRect(int32_t left, int32_t top, int32_t right, int32_t bot) {
    if (right < left || bot < top)
        return;

    int32_t x, y;
    int32_t tileLeft, tileRight, tileTop, tileBot;
    int32_t xOffs, yOffs;
    int32_t xDest, yDest;

    tileLeft = left / bmBackground.Width();
    tileRight = right / bmBackground.Width();
    tileTop = top / bmBackground.Height();
    tileBot = bot / bmBackground.Height();

    yOffs = top % bmBackground.Height();
    yDest = top;

    for (y = tileTop; y <= tileBot; y++) {
        xOffs = left % bmBackground.Width();
        xDest = left;
        int32_t h = Min(bot - yDest + 1, bmBackground.Height() - yOffs);
        for (x = tileLeft; x <= tileRight; x++) {
            int32_t w = Min(right - xDest + 1, bmBackground.Width() - xOffs);
            bmBackground.Blit(CCanvas::Current(), xDest, yDest, w, h, xOffs, yOffs, 1);
            xOffs = 0;
            xDest += w;
        }
        yOffs = 0;
        yDest += h;
    }
}

//------------------------------------------------------------------------------

void UpdateSlidingFaces(void) {
    ENTER(0, 0);
    CSegFace *pFace;
    int16_t nOffset;
    tTexCoord2f *pTexCoord, *ovlTexCoordP;
    tUVL *pUVL;

    for (pFace = FACES.slidingFaces; pFace; pFace = pFace->nextSlidingFace) {
#if DBG
        if ((pFace->m_info.nSegment == nDbgSeg) && ((nDbgSide < 0) || (pFace->m_info.nSide == nDbgSide)))
            BRP;
#endif
        pTexCoord = FACES.texCoord + pFace->m_info.nIndex;
        ovlTexCoordP = FACES.ovlTexCoord + pFace->m_info.nIndex;
        pUVL = SEGMENT(pFace->m_info.nSegment)->m_sides[pFace->m_info.nSide].m_uvls;
        nOffset = pFace->m_info.nType == SIDE_IS_TRI_13;
        if (gameStates.render.bTriangleMesh) {
            static int16_t nTriVerts[2][6] = {{0, 1, 2, 0, 2, 3}, {0, 1, 3, 1, 2, 3}};
            int32_t j = pFace->m_info.nTriangles * 3;
            for (int32_t i = 0; i < j; i++) {
                int16_t k = nTriVerts[nOffset][i];
                pTexCoord[i].v.u = X2F(pUVL[k].u);
                pTexCoord[i].v.v = X2F(pUVL[k].v);
                RotateTexCoord2f(ovlTexCoordP[i], pTexCoord[i], pFace->m_info.nOvlOrient);
            }
        } else {
            int32_t j = 2 * pFace->m_info.nTriangles;
            for (int32_t i = 0; i < j; i++) {
                pTexCoord[i].v.u = X2F(pUVL[(i + nOffset) % 4].u);
                pTexCoord[i].v.v = X2F(pUVL[(i + nOffset) % 4].v);
                RotateTexCoord2f(ovlTexCoordP[i], pTexCoord[i], pFace->m_info.nOvlOrient);
            }
        }
    }
    RETURN
}

//------------------------------------------------------------------------------

void GameRenderFrame(void) {
    ENTER(0, 0);
    if (gameData.segData.nFaceKeys < 0)
        meshBuilder.ComputeFaceKeys();
    PROF_START
    UpdateSlidingFaces();
    PROF_END(ptAux);
    SetScreenMode(SCREEN_GAME);
    if (!ogl.StereoDevice() || !(gameData.appData.nFrameCount & 1)) {
        cockpit->PlayHomingWarning();
        transparencyRenderer.Reset();
    }
    if (!ogl.StereoDevice())
        RenderMonoFrame();
    else {
        if (gameOpts->render.stereo.xSeparation[ogl.IsOculusRift()] == 0)
            gameOpts->render.stereo.xSeparation[ogl.IsOculusRift()] = I2X(1);
        fix xStereoSeparation = (automap.Active() && !ogl.IsSideBySideDevice())
                                    ? 2 * gameOpts->render.stereo.xSeparation[0]
                                    : gameOpts->render.stereo.xSeparation[ogl.IsOculusRift()];
        SetupCanvasses(-1.0f);
        RenderMonoFrame(-xStereoSeparation);
        RenderMonoFrame(xStereoSeparation);
        SetupCanvasses();
    }
    // StopTime ();
    // if (!gameStates.menus.nInMenu)
    // paletteManager.EnableEffect ();
    // StartTime (0);
    gameData.appData.nFrameCount++;
    PROF_END(ptRenderFrame)
    RETURN
}

//------------------------------------------------------------------------------
// eof
