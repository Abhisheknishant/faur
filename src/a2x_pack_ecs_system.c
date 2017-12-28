/*
    Copyright 2016, 2017 Alex Margarit

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

#include "a2x_pack_ecs_system.v.h"

static AStrHash* g_systems; // table of declared ASystem

void a_ecs_system__init(void)
{
    g_systems = a_strhash_new();
}

void a_ecs_system__uninit(void)
{
    A_STRHASH_ITERATE(g_systems, ASystem*, system) {
        a_list_free(system->entities);
        a_bitfield_free(system->componentBits);
        free(system);
    }

    a_strhash_free(g_systems);
}

void a_system_declare(const char* Name, const char* Components, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities)
{
    if(a_strhash_contains(g_systems, Name)) {
        a_out__fatal("System '%s' already declared", Name);
    }

    ASystem* s = a_mem_malloc(sizeof(ASystem));

    s->handler = Handler;
    s->compare = Compare;
    s->entities = a_list_new();
    s->componentBits = a_bitfield_new(a_strhash_getSize(a__ecsComponents));
    s->onlyActiveEntities = OnlyActiveEntities;
    s->muted = false;
    s->runsInCurrentState = false;

    a_strhash_add(g_systems, Name, s);

    AList* tok = a_str_split(Components, " ");

    A_LIST_ITERATE(tok, char*, name) {
        AComponent* c = a_strhash_get(a__ecsComponents, name);

        if(c == NULL) {
            a_out__fatal("Unknown component '%s' for system '%s'",
                         name, Name);
        }

        a_bitfield_set(s->componentBits, c->bit);
    }

    a_list_freeEx(tok, free);
}

void a_ecs_system__run(const ASystem* System)
{
    if(System->muted) {
        return;
    }

    if(System->compare) {
        a_list_sort(System->entities, (AListCompare*)System->compare);
    }

    if(System->onlyActiveEntities) {
        A_LIST_FILTER(System->entities, AEntity*, entity, !entity->muted) {
            if(a_entity_isActive(entity)) {
                System->handler(entity);
            }
        }
    } else {
        A_LIST_FILTER(System->entities, AEntity*, entity, !entity->muted) {
            System->handler(entity);
        }
    }
}

void a_system_execute(const char* Systems)
{
    AList* tok = a_str_split(Systems, " ");

    A_LIST_ITERATE(tok, char*, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("a_system_execute: unknown system '%s'", name);
        }

        if(!system->runsInCurrentState) {
            a_out__fatal("a_system_execute: '%s' does not run in state", name);
        }

        a_ecs_system__run(system);
    }

    a_list_freeEx(tok, free);
}

static void muteSystems(const char* Systems, bool Muted)
{
    AList* tok = a_str_split(Systems, " ");

    A_LIST_ITERATE(tok, char*, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("%s: unknown system '%s'",
                         Muted ? "a_system_mute" : "a_system_unmute",
                         name);
        }

        if(!system->runsInCurrentState) {
            a_out__fatal("%s: '%s' does not run in state",
                         Muted ? "a_system_mute" : "a_system_unmute",
                         name);
        }

        system->muted = Muted;
    }

    a_list_freeEx(tok, free);
}

void a_system_mute(const char* Systems)
{
    muteSystems(Systems, true);
}

void a_system_unmute(const char* Systems)
{
    muteSystems(Systems, false);
}

AList* a_ecs_system__parseIds(const char* Systems)
{
    AList* systems = a_list_new();
    AList* tok = a_str_split(Systems, " ");

    A_LIST_ITERATE(tok, char*, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("Unknown system '%s'", name);
        }

        a_list_addLast(systems, system);
    }

    a_list_freeEx(tok, free);

    return systems;
}
