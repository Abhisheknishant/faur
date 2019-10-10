/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_GRAPHICS_FONT_P_H
#define A_INC_GRAPHICS_FONT_P_H

#include "general/a_system_includes.h"

typedef struct ASprite AFont;

typedef enum {
    A_FONT_ALIGN_INVALID = -1,
    A_FONT_ALIGN_LEFT,
    A_FONT_ALIGN_MIDDLE,
    A_FONT_ALIGN_RIGHT,
} AFontAlign;

#include "graphics/a_sprite.p.h"

extern AFont* a_font_newFromPng(const char* Path, int X, int Y, int CharWidth, int CharHeight);
extern AFont* a_font_newFromSprite(const ASprite* Sheet, int X, int Y, int CharWidth, int CharHeight);
extern AFont* a_font_dup(const AFont* Font, APixel Color);
extern void a_font_free(AFont* Font);

extern void a_font_push(void);
extern void a_font_pop(void);

extern void a_font_reset(void);

extern void a_font_fontSet(const AFont* Font);
extern void a_font_alignSet(AFontAlign Align);

extern void a_font_coordsSet(int X, int Y);
extern int a_font_coordsGetX(void);
extern int a_font_coordsGetY(void);

extern int a_font_lineHeightGet(void);
extern void a_font_lineHeightSet(int Height);
extern void a_font_lineWrapSet(int Width);
extern void a_font_lineNew(void);

extern int a_font_widthGet(const char* Text);
extern int a_font_widthGetf(const char* Format, ...);
extern int a_font_widthGetv(const char* Format, va_list Args);

extern void a_font_print(const char* Text);
extern void a_font_printf(const char* Format, ...);
extern void a_font_printv(const char* Format, va_list Args);

#endif // A_INC_GRAPHICS_FONT_P_H
