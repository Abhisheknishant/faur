/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_pack_pixels.p.h"

#include "a2x_pack_color.v.h"

struct APixels {
    int w, h;
    size_t bufferSize;
    APixel buffer[];
};

static inline APixel* a_pixels__getFrom(APixels* Pixels, int X, int Y)
{
    return Pixels->buffer + Y * Pixels->w + X;
}

static inline APixel a_pixels__getAt(const APixels* Pixels, int X, int Y)
{
    return *(Pixels->buffer + Y * Pixels->w + X);
}
