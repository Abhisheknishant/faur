/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#include "a_collection.v.h"
#include <a2x.v.h>

struct ACollection {
    AList* entities; // list of AEntity
};

ACollection* a_collection_new(void)
{
    ACollection* c = a_mem_malloc(sizeof(ACollection));

    c->entities = a_list_new();

    return c;
}

void a_collection_free(ACollection* Collection)
{
    a_ecs__refDecIgnoreSet(true);
    a_list_freeEx(Collection->entities, (AFree*)a_entity__freeEx);
    a_ecs__refDecIgnoreSet(false);

    a_mem_free(Collection);
}

AList* a_collection__listGet(const ACollection* Collection)
{
    return Collection->entities;
}

void a_collection_clear(ACollection* Collection)
{
    a_list_clearEx(Collection->entities, (AFree*)a_entity_removeSet);
}

void a_collection_muteInc(ACollection* Collection)
{
    A_LIST_ITERATE(Collection->entities, AEntity*, e) {
        a_entity_muteInc(e);
    }
}

void a_collection_muteDec(ACollection* Collection)
{
    A_LIST_ITERATE(Collection->entities, AEntity*, e) {
        a_entity_muteDec(e);
    }
}