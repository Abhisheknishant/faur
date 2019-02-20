/*
    Copyright 2010, 2016-2017 Alex Margarit
    This file is part of a2x, a C video game framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_pack_platform_sdl_video.p.h"

#if A_CONFIG_LIB_SDL == 2
    #include <SDL.h>

    #include "a2x_pack_pixel.v.h"

    extern SDL_Renderer* a__sdlRenderer;

    #if A__PIXEL_BPP == 16
        #if A_CONFIG_LIB_RENDER_SOFTWARE
            #define A_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_RGB565
        #elif A_CONFIG_LIB_RENDER_SDL
            #define A_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_RGBA5551
        #endif
    #elif A__PIXEL_BPP == 32
        #if A_CONFIG_LIB_RENDER_SOFTWARE
            #define A_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_RGBX8888
        #elif A_CONFIG_LIB_RENDER_SDL
            #define A_SDL__PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888
        #endif
    #endif
#endif

extern void a_platform_sdl_video__init(void);
extern void a_platform_sdl_video__uninit(void);
