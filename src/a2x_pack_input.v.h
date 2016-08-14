/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

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

typedef struct AInputInstance AInputInstance;
typedef void (*AInputCallback)(void);

#include "a2x_pack_input.p.h"

#include "a2x_pack_collide.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_strhash.v.h"
#include "a2x_pack_list.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_screenshot.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_strtok.v.h"

typedef struct AInputCollection {
    AList* list; // inputs registered during init
    AStrHash* names; // hash table of above inputs' names
} AInputCollection;

extern void a_input__init(void);
extern void a_input__uninit(void);

extern void a_input__free(AInput* Input);

extern void a_input__addCallback(AInputCallback Callback);
extern void a_input__get(void);

extern void a_input__button_setState(AInputInstance* Button, bool Pressed);

extern void a_input__analog_setXAxis(AInputInstance* Analog, int Value);
extern void a_input__analog_setYAxis(AInputInstance* Analog, int Value);

extern void a_input__touch_addMotion(AInputInstance* Touch, int X, int Y);
extern void a_input__touch_setCoords(AInputInstance* Touch, int X, int Y, bool Tapped);

extern AInputCollection* a_input__collection_new(void);
extern void a_input__collection_free(AInputCollection* Collection);
extern void a_input__collection_add(AInputCollection* Collection, void* Instance, const char* Name);
