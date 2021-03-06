/*
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_ECS_V_H
#define F_INC_ECS_ECS_V_H

#include "f_ecs.p.h"

#include "../ecs/f_component.v.h"
#include "../ecs/f_system.v.h"
#include "../general/f_init.v.h"

extern const FPack f_pack__ecs;

extern void f_ecs__populate(void);

extern void f_ecs__set(FComponent* const* Components, size_t ComponentsNum, FSystem* const* Systems, size_t SystemsNum);

#endif // F_INC_ECS_ECS_V_H
