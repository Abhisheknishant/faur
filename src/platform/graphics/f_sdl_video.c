/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_sdl_video.v.h"
#include <faur.v.h>

#if F_CONFIG_LIB_SDL
#if F_CONFIG_LIB_SDL == 1
    #include <SDL/SDL.h>
#elif F_CONFIG_LIB_SDL == 2
    #include <SDL2/SDL.h>
#endif

#if F_CONFIG_LIB_SDL == 1
    static SDL_Surface* g_sdlScreen;
#elif F_CONFIG_LIB_SDL == 2
    SDL_Renderer* f__sdlRenderer;
    static SDL_Window* g_sdlWindow;
    static SDL_Texture* g_sdlTexture;
    static ARgb g_clearRgb;
#endif

#if F_CONFIG_LIB_SDL == 2 || F_CONFIG_SYSTEM_EMSCRIPTEN
    static bool g_vsync = F_CONFIG_SCREEN_VSYNC;
#else
    static const bool g_vsync = false;
#endif

#if F_CONFIG_SCREEN_SIZE_WIDTH < 0 || F_CONFIG_SCREEN_SIZE_HEIGHT < 0
    #define F__SIZE_DYNAMIC 1
#endif

#if F__SIZE_DYNAMIC
static AVectorInt g_size = {
#else
static const AVectorInt g_size = {
#endif
    F_CONFIG_SCREEN_SIZE_WIDTH,
    F_CONFIG_SCREEN_SIZE_HEIGHT
};
static bool g_fullscreen = F_CONFIG_SCREEN_FULLSCREEN;
static int g_zoom = F_CONFIG_SCREEN_ZOOM;
static APixels g_pixels;

void f_platform_sdl_video__init(void)
{
    #if F_CONFIG_SYSTEM_PANDORA
        putenv("SDL_VIDEODRIVER=omapdss");
        putenv("SDL_OMAP_LAYER_SIZE=pixelperfect");
    #endif

    if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        F__FATAL("SDL_InitSubSystem: %s", SDL_GetError());
    }
}

void f_platform_sdl_video__uninit(void)
{
    #if F_CONFIG_LIB_SDL == 1
        #if !F_CONFIG_SCREEN_ALLOCATE
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }
        #endif
    #elif F_CONFIG_LIB_SDL == 2
        SDL_DestroyTexture(g_sdlTexture);
        SDL_DestroyRenderer(f__sdlRenderer);
        SDL_DestroyWindow(g_sdlWindow);
    #endif

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

#if F_CONFIG_LIB_SDL == 1
static bool sdl1ScreenSet(int Width, int Height, uint32_t Flags)
{
    if(SDL_VideoModeOK(Width, Height, F_CONFIG_SCREEN_BPP, Flags) == 0) {
        f_out__error("SDL_VideoModeOK(%d, %d, %d): Mode not available",
                     Width,
                     Height,
                     F_CONFIG_SCREEN_BPP);

        return false;
    }

    g_sdlScreen = SDL_SetVideoMode(Width, Height, F_CONFIG_SCREEN_BPP, Flags);

    if(g_sdlScreen == NULL) {
        F__FATAL("SDL_SetVideoMode(%d, %d, %d): %s",
                 Width,
                 Height,
                 F_CONFIG_SCREEN_BPP,
                 SDL_GetError());
    }

    SDL_SetClipRect(g_sdlScreen, NULL);

    return true;
}
#endif

#if F__SIZE_DYNAMIC
#if F_CONFIG_SCREEN_HARDWARE_WIDTH > 0 && F_CONFIG_SCREEN_HARDWARE_HEIGHT > 0
static AVectorInt sdlScreenSizeGetNative(void)
{
    return (AVectorInt){F_CONFIG_SCREEN_HARDWARE_WIDTH,
                        F_CONFIG_SCREEN_HARDWARE_HEIGHT};
}
#elif F_CONFIG_LIB_SDL == 1
static AVectorInt sdlScreenSizeGetNative(void)
{
    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    return (AVectorInt){info->current_w, info->current_h};
}
#elif F_CONFIG_LIB_SDL == 2
static AVectorInt sdlScreenSizeGetNative(void)
{
    SDL_DisplayMode mode;

    if(SDL_GetCurrentDisplayMode(0, &mode) < 0) {
        f_out__error("SDL_GetCurrentDisplayMode: %s", SDL_GetError());

        return (AVectorInt){0, 0};
    }

    f_out__info("Display info: %dx%d %dbpp",
                mode.w,
                mode.h,
                SDL_BITSPERPIXEL(mode.format));

    return (AVectorInt){mode.w, mode.h};
}
#endif
#endif // F__SIZE_DYNAMIC

void f_platform_api__screenInit(void)
{
    #if F__SIZE_DYNAMIC
        AVectorInt res = sdlScreenSizeGetNative();

        if(res.x > 0 && res.y > 0) {
            if(g_size.x < 0) {
                g_size.x = res.x / -g_size.x;
            }

            if(g_size.y < 0) {
                g_size.y = res.y / -g_size.y;
            }
        }
    #endif

    if(g_size.x <= 0 || g_size.y <= 0) {
        F__FATAL("Invalid screen resolution %dx%d", g_size.x, g_size.y);
    } else {
        f_out__info("Screen resolution %dx%d, zoom x%d",
                    g_size.x,
                    g_size.y,
                    F_CONFIG_SCREEN_ZOOM);
    }

    APixelsFlags pFlags = 0;

    #if F_CONFIG_SCREEN_ALLOCATE
        pFlags |= F_PIXELS__ALLOC;
    #endif

    f_pixels__init(&g_pixels, g_size.x, g_size.y, 1, pFlags);

    #if F_CONFIG_LIB_SDL == 1
        uint32_t videoFlags = SDL_SWSURFACE;

        #if F_CONFIG_SCREEN_FULLSCREEN
            videoFlags |= SDL_FULLSCREEN;
        #endif

        if(!sdl1ScreenSet(g_size.x * F_CONFIG_SCREEN_ZOOM,
                          g_size.y * F_CONFIG_SCREEN_ZOOM,
                          videoFlags)) {

            F__FATAL("Could not create SDL screen surface");
        }

        #if !F_CONFIG_SCREEN_ALLOCATE
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    F__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            f_pixels__bufferSet(&g_pixels,
                                g_sdlScreen->pixels,
                                g_sdlScreen->pitch / sizeof(APixel),
                                g_sdlScreen->h);
        #endif
    #elif F_CONFIG_LIB_SDL == 2
        int ret;
        uint32_t windowFlags = 0;

        #if !F_CONFIG_SYSTEM_EMSCRIPTEN
            windowFlags |= SDL_WINDOW_RESIZABLE;
        #endif

        #if F_CONFIG_SCREEN_MAXIMIZED
            windowFlags |= SDL_WINDOW_MAXIMIZED;
        #endif

        #if F_CONFIG_SCREEN_FULLSCREEN
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        #endif

        g_sdlWindow = SDL_CreateWindow(NULL,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       g_size.x * F_CONFIG_SCREEN_ZOOM,
                                       g_size.y * F_CONFIG_SCREEN_ZOOM,
                                       windowFlags);
        if(g_sdlWindow == NULL) {
            F__FATAL("SDL_CreateWindow: %s", SDL_GetError());
        }

        #if F_CONFIG_SCREEN_FULLSCREEN
            if(SDL_SetWindowFullscreen(
                g_sdlWindow, SDL_WINDOW_FULLSCREEN) < 0) {

                f_out__error("SDL_SetWindowFullscreen: %s", SDL_GetError());
            }
        #endif

        uint32_t rendererFlags = SDL_RENDERER_ACCELERATED
                               | SDL_RENDERER_TARGETTEXTURE;

        #if F_CONFIG_SCREEN_VSYNC
            rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
        #endif

        f__sdlRenderer = SDL_CreateRenderer(g_sdlWindow, -1, rendererFlags);

        if(f__sdlRenderer == NULL) {
            F__FATAL("SDL_CreateRenderer: %s", SDL_GetError());
        }

        SDL_RendererInfo info;
        SDL_GetRendererInfo(f__sdlRenderer, &info);

        if(!(info.flags & SDL_RENDERER_TARGETTEXTURE)) {
            F__FATAL("SDL_CreateRenderer: "
                     "System does not support SDL_RENDERER_TARGETTEXTURE");
        }

        if(info.flags & SDL_RENDERER_ACCELERATED) {
            f_out__info("Using SDL2 accelerated renderer");
        } else {
            f_out__info("Using SDL2 software renderer");
        }

        g_vsync = info.flags & SDL_RENDERER_PRESENTVSYNC;

        ret = SDL_RenderSetLogicalSize(f__sdlRenderer, g_size.x, g_size.y);

        if(ret < 0) {
            F__FATAL("SDL_RenderSetLogicalSize: %s", SDL_GetError());
        }

        #if F_CONFIG_LIB_RENDER_SOFTWARE
            int access = SDL_TEXTUREACCESS_STREAMING;
        #else
            int access = SDL_TEXTUREACCESS_TARGET;
        #endif

        g_sdlTexture = SDL_CreateTexture(f__sdlRenderer,
                                         F_SDL__PIXEL_FORMAT,
                                         access,
                                         g_size.x,
                                         g_size.y);

        if(g_sdlTexture == NULL) {
            F__FATAL("SDL_CreateTexture: %s", SDL_GetError());
        }

        #if F_CONFIG_LIB_RENDER_SDL
            f_platform_api__screenTextureSet(
                f_platform_api__screenTextureGet());
        #endif

        SDL_SetHintWithPriority(
            SDL_HINT_RENDER_SCALE_QUALITY, "nearest", SDL_HINT_OVERRIDE);

        g_clearRgb = f_pixel_toRgb(
                        f_pixel_fromHex(F_CONFIG_COLOR_SCREEN_BORDER));
    #endif

    f_out__info("V-sync is %s", g_vsync ? "on" : "off");

    #if F_CONFIG_TRAIT_DESKTOP
        const char* caption = f_str__fmt512("%s %s",
                                            F_CONFIG_APP_NAME,
                                            F_CONFIG_APP_VERSION_STRING);

        #if F_CONFIG_LIB_SDL == 1
            SDL_WM_SetCaption(caption, NULL);
        #elif F_CONFIG_LIB_SDL == 2
            SDL_SetWindowTitle(g_sdlWindow, caption);
        #endif
    #endif

    f_platform_api__screenMouseCursorSet(F_CONFIG_INPUT_MOUSE_CURSOR);

    #if F_CONFIG_SCREEN_WIZ_FIX
        f_platform_wiz__portraitModeSet();
    #endif
}

void f_platform_api__screenUninit(void)
{
    f_pixels__free(&g_pixels);
}

#if F_CONFIG_LIB_SDL == 2
void f_platform_api__screenClear(void)
{
    if(SDL_RenderClear(f__sdlRenderer) < 0) {
        f_out__error("SDL_RenderClear: %s", SDL_GetError());
    }
}

APlatformTexture* f_platform_api__screenTextureGet(void)
{
    return (APlatformTexture*)&g_sdlTexture;
}

void f_platform_api__screenTextureSet(APlatformTexture* Texture)
{
    SDL_Texture* t = Texture ? *(SDL_Texture**)Texture : NULL;

    if(SDL_SetRenderTarget(f__sdlRenderer, t) < 0) {
        F__FATAL("SDL_SetRenderTarget: %s", SDL_GetError());
    }
}

void f_platform_api__screenTextureRead(APixels* Pixels, unsigned Frame)
{
    // Unreliable on texture targets
    if(SDL_RenderReadPixels(f__sdlRenderer,
                            NULL,
                            F_SDL__PIXEL_FORMAT,
                            f_pixels__bufferGetFrom(Pixels, Frame, 0, 0),
                            Pixels->w * (int)sizeof(APixel)) < 0) {

        F__FATAL("SDL_RenderReadPixels: %s", SDL_GetError());
    }
}

void f_platform_api__screenClipSet(int X, int Y, int Width, int Height)
{
    SDL_Rect area = {X, Y, Width, Height};

    if(SDL_RenderSetClipRect(f__sdlRenderer, Width > 0 ? &area : NULL) < 0) {
        f_out__error("SDL_RenderSetClipRect: %s", SDL_GetError());
    }
}

void f_platform_api__screenDraw(void)
{
    if(SDL_RenderCopy(f__sdlRenderer, g_sdlTexture, NULL, NULL) < 0) {
        F__FATAL("SDL_RenderCopy: %s", SDL_GetError());
    }
}
#endif

void f_platform_api__screenShow(void)
{
    #if F_CONFIG_LIB_SDL == 1
        #if F_CONFIG_SCREEN_WIZ_FIX
            // The Wiz screen has diagonal tearing in landscape mode. As a slow
            // but simple workaround, the screen is set to portrait mode where
            // 320,0 is top-left and 0,240 is bottom-right, and the game's
            // landscape pixel buffer is rotated to this format every frame.

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    F__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            #define F__SCREEN_TOTAL (F_CONFIG_SCREEN_HARDWARE_WIDTH \
                                        * F_CONFIG_SCREEN_HARDWARE_HEIGHT)

            APixel* dst = (APixel*)g_sdlScreen->pixels + F__SCREEN_TOTAL;
            const APixel* src = g_pixels.buffer;

            for(int i = F_CONFIG_SCREEN_HARDWARE_HEIGHT;
                i--;
                dst += F__SCREEN_TOTAL + 1) {

                for(int j = F_CONFIG_SCREEN_HARDWARE_WIDTH; j--; ) {
                    dst -= F_CONFIG_SCREEN_HARDWARE_HEIGHT;
                    *dst = *src++;
                }
            }

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);
        #elif F_CONFIG_SCREEN_ALLOCATE
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    F__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            if(g_zoom <= 1) {
                if(g_pixels.w * (int)sizeof(APixel) == g_sdlScreen->pitch) {
                    memcpy(g_sdlScreen->pixels,
                           g_pixels.buffer,
                           g_pixels.bufferSize);
                } else {
                    uint8_t* dst = g_sdlScreen->pixels;
                    const APixel* src = g_pixels.buffer;
                    size_t rowSize = (size_t)g_sdlScreen->w * sizeof(APixel);

                    for(int y = g_sdlScreen->h; y--; ) {
                        memcpy(dst, src, rowSize);

                        dst += g_sdlScreen->pitch;
                        src += g_sdlScreen->w;
                    }
                }
            } else {
                APixel* dst = g_sdlScreen->pixels;
                const APixel* src = g_pixels.buffer;
                int realH = g_sdlScreen->h / g_zoom;
                int realW = g_sdlScreen->w / g_zoom;
                size_t rowLen = g_sdlScreen->pitch / sizeof(APixel);
                ptrdiff_t rowRemainder = (int)rowLen - g_sdlScreen->w;

                for(int y = realH; y--; ) {
                    const APixel* firstLine = dst;

                    for(int x = realW; x--; ) {
                        for(int z = g_zoom; z--; ) {
                            *dst++ = *src;
                        }

                        src++;
                    }

                    dst += rowRemainder;

                    for(int z = g_zoom - 1; z--; ) {
                        memcpy(dst, firstLine, g_sdlScreen->pitch);
                        dst += rowLen;
                    }
                }
            }

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);
        #else
            if(SDL_MUSTLOCK(g_sdlScreen)) {
                SDL_UnlockSurface(g_sdlScreen);
            }

            SDL_Flip(g_sdlScreen);

            if(SDL_MUSTLOCK(g_sdlScreen)) {
                if(SDL_LockSurface(g_sdlScreen) < 0) {
                    F__FATAL("SDL_LockSurface: %s", SDL_GetError());
                }
            }

            f_pixels__bufferSet(&g_pixels,
                                g_sdlScreen->pixels,
                                g_sdlScreen->pitch / sizeof(APixel),
                                g_sdlScreen->h);
        #endif
    #elif F_CONFIG_LIB_SDL == 2
        #if F_CONFIG_LIB_RENDER_SDL
            f_platform_api__screenTextureSet(NULL);
            f_platform_api__screenClipSet(0, 0, 0, 0);
        #endif

        if(SDL_SetRenderDrawColor(f__sdlRenderer,
                                  (uint8_t)g_clearRgb.r,
                                  (uint8_t)g_clearRgb.g,
                                  (uint8_t)g_clearRgb.b,
                                  SDL_ALPHA_OPAQUE) < 0) {

            f_out__error("SDL_SetRenderDrawColor: %s", SDL_GetError());
        }

        f_platform_api__screenClear();

        #if F_CONFIG_LIB_RENDER_SOFTWARE
            if(SDL_UpdateTexture(g_sdlTexture,
                                 NULL,
                                 g_pixels.buffer,
                                 g_pixels.w * (int)sizeof(APixel)) < 0) {

                F__FATAL("SDL_UpdateTexture: %s", SDL_GetError());
            }

            f_platform_api__screenDraw();
        #else
            f_color_push();
            f_color_blendSet(F_COLOR_BLEND_PLAIN);
            f_color_fillBlitSet(false);

            f_platform_api__screenDraw();

            f_platform_api__screenTextureSet(
                f_platform_api__screenTextureGet());
            f_platform_api__screenClipSet(f__screen.clipX,
                                          f__screen.clipY,
                                          f__screen.clipWidth,
                                          f__screen.clipHeight);

            f_color_pop();
        #endif

        SDL_RenderPresent(f__sdlRenderer);
    #endif
}

APixels* f_platform_api__screenPixelsGet(void)
{
    return &g_pixels;
}

AVectorInt f_platform_api__screenSizeGet(void)
{
    return g_size;
}

bool f_platform_api__screenVsyncGet(void)
{
    return g_vsync;
}

int f_platform_api__screenZoomGet(void)
{
    return g_zoom;
}

void f_platform_api__screenZoomSet(int Zoom)
{
    #if F_CONFIG_LIB_SDL == 1
        #if F_CONFIG_SCREEN_ALLOCATE
            int newWidth = g_pixels.w * Zoom;
            int newHeight = g_pixels.h * Zoom;

            if(sdl1ScreenSet(newWidth, newHeight, g_sdlScreen->flags)) {
                g_zoom = Zoom;
            }
        #else
            F_UNUSED(Zoom);

            f_out__warning(
                "SDL 1.2 screen zoom needs F_CONFIG_SCREEN_ALLOCATE=1");
        #endif
    #elif F_CONFIG_LIB_SDL == 2
        SDL_SetWindowSize(g_sdlWindow, g_pixels.w * Zoom, g_pixels.h * Zoom);

        g_zoom = Zoom;
    #endif
}

bool f_platform_api__screenFullscreenGet(void)
{
    return g_fullscreen;
}

void f_platform_api__screenFullscreenFlip(void)
{
    g_fullscreen = !g_fullscreen;

    #if F_CONFIG_LIB_SDL == 1
        #if F_CONFIG_SCREEN_ALLOCATE
            uint32_t videoFlags = g_sdlScreen->flags;

            if(g_fullscreen) {
                videoFlags |= SDL_FULLSCREEN;
            } else {
                videoFlags &= ~(uint32_t)SDL_FULLSCREEN;
            }

            sdl1ScreenSet(g_sdlScreen->w, g_sdlScreen->h, videoFlags);
        #else
            f_out__warning(
                "SDL 1.2 fullscreen needs F_CONFIG_SCREEN_ALLOCATE=1");
        #endif
    #elif F_CONFIG_LIB_SDL == 2
        if(SDL_SetWindowFullscreen(
            g_sdlWindow, g_fullscreen ? SDL_WINDOW_FULLSCREEN : 0) < 0) {

            f_out__error("SDL_SetWindowFullscreen: %s", SDL_GetError());
        }
    #endif

    f_platform_api__screenMouseCursorSet(!g_fullscreen);
}

void f_platform_api__screenMouseCursorSet(bool Show)
{
    #if F_CONFIG_INPUT_MOUSE_CURSOR
        int setting = Show ? SDL_ENABLE : SDL_DISABLE;
    #else
        F_UNUSED(Show);
        int setting = SDL_DISABLE;
    #endif

    if(SDL_ShowCursor(setting) < 0) {
        f_out__error("SDL_ShowCursor: %s", SDL_GetError());
    }
}

#if F_CONFIG_LIB_RENDER_SDL
int f_platform_sdl_video__pixelBlendToSdlBlend(void)
{
    switch(f__color.blend) {
        case F_COLOR_BLEND_MOD:
            return SDL_BLENDMODE_MOD;

        case F_COLOR_BLEND_ADD:
            return SDL_BLENDMODE_ADD;

        default:
            return SDL_BLENDMODE_BLEND;
    }
}

uint8_t f_platform_sdl_video__pixelAlphaToSdlAlpha(void)
{
    switch(f__color.blend) {
        case F_COLOR_BLEND_RGBA:
            return (uint8_t)f__color.alpha;

        case F_COLOR_BLEND_RGB25:
            return SDL_ALPHA_OPAQUE / 4;

        case F_COLOR_BLEND_RGB50:
            return SDL_ALPHA_OPAQUE / 2;

        case F_COLOR_BLEND_RGB75:
            return SDL_ALPHA_OPAQUE * 3 / 4;

        default:
            return SDL_ALPHA_OPAQUE;
    }
}

void f_platform_api__renderSetDrawColor(void)
{
    if(SDL_SetRenderDrawColor(
        f__sdlRenderer,
        (uint8_t)f__color.rgb.r,
        (uint8_t)f__color.rgb.g,
        (uint8_t)f__color.rgb.b,
        f_platform_sdl_video__pixelAlphaToSdlAlpha()) < 0) {

        f_out__error("SDL_SetRenderDrawColor: %s", SDL_GetError());
    }
}

void f_platform_api__renderSetBlendMode(void)
{
    SDL_BlendMode blend =
        (SDL_BlendMode)f_platform_sdl_video__pixelBlendToSdlBlend();

    if(SDL_SetRenderDrawBlendMode(f__sdlRenderer, blend) < 0) {
        f_out__error("SDL_SetRenderDrawBlendMode: %s", SDL_GetError());
    }
}
#endif // F_CONFIG_LIB_RENDER_SDL
#endif // F_CONFIG_LIB_SDL
