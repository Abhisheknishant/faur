/*
    Copyright 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_ENTITY_V_H
#define F_INC_ECS_ENTITY_V_H

#include "ecs/f_entity.p.h"

#include "data/f_list.v.h"
#include "ecs/f_system.v.h"
#include "ecs/f_template.v.h"

typedef enum {
    F_ENTITY__ACTIVE_REMOVED = F_FLAGS_BIT(0), // kicked by active-only system
    F_ENTITY__ACTIVE_PERMANENT = F_FLAGS_BIT(1), // always report as active
    F_ENTITY__DEBUG = F_FLAGS_BIT(2), // print debug messages for this entity
    F_ENTITY__REMOVED = F_FLAGS_BIT(3), // marked for removal, may have refs
    F_ENTITY__REMOVE_INACTIVE = F_FLAGS_BIT(4), // mark for removal if kicked
} AEntityFlags;

extern void f_entity__free(AEntity* Entity);
extern void f_entity__freeEx(AEntity* Entity);

extern const ATemplate* f_entity__templateGet(const AEntity* Entity);

extern bool f_entity__canDelete(const AEntity* Entity);
extern void f_entity__ecsListAdd(AEntity* Entity, AList* List);

extern void f_entity__systemsMatch(AEntity* Entity, ASystem* System);
extern void f_entity__systemsAddTo(AEntity* Entity);
extern void f_entity__systemsRemoveFromAll(AEntity* Entity);
extern void f_entity__systemsRemoveFromActive(AEntity* Entity);

#endif // F_INC_ECS_ENTITY_V_H
