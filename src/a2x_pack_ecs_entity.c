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

#include "a2x_pack_ecs_entity.v.h"

static inline bool entityIsNew(const AEntity* Entity)
{
    return a_list__nodeGetList(Entity->node)
        == a__ecsCollection->newEntities;
}

static inline void* getComponent(const AComponentHeader* Header)
{
    return (void*)(Header + 1);
}

AEntity* a_entity_new(const char* Id, void* Context)
{
    AEntity* e = a_mem_malloc(sizeof(AEntity));

    e->id = Id ? a_str_dup(Id) : NULL;
    e->context = Context;
    e->parent = NULL;
    e->node = a_list_addLast(a__ecsCollection->newEntities, e);
    e->systemNodes = a_list_new();
    e->components = a_strhash_new();
    e->componentBits = a_bitfield_new(a_strhash_getSize(a__ecsComponents));
    e->handlers = a_strhash_new();
    e->lastActive = a_fps_getCounter() - 1;
    e->references = 0;
    e->muted = false;

    return e;
}

void a_entity__free(AEntity* Entity)
{
    a_list_freeEx(Entity->systemNodes, (AFree*)a_list_removeNode);

    A_STRHASH_ITERATE(Entity->components, AComponentHeader*, header) {
        if(header->component->free) {
            header->component->free(getComponent(header));
        }

        free(header);
    }

    if(Entity->parent) {
        a_entity_release(Entity->parent);
    }

    a_strhash_free(Entity->components);
    a_strhash_freeEx(Entity->handlers, free);
    a_bitfield_free(Entity->componentBits);
    free(Entity->id);
    free(Entity);
}

const char* a_entity_getId(const AEntity* Entity)
{
    return Entity->id ? Entity->id : "AEntity";
}

void* a_entity_getContext(const AEntity* Entity)
{
    return Entity->context;
}

AEntity* a_entity_getParent(const AEntity* Entity)
{
    return Entity->parent;
}

void a_entity_setParent(AEntity* Entity, AEntity* Parent)
{
    if(Entity->parent != NULL) {
        a_entity_release(Entity->parent);
    }

    Entity->parent = Parent;

    if(Parent != NULL) {
        a_entity_reference(Parent);
    }
}

void a_entity_reference(AEntity* Entity)
{
    Entity->references++;
}

void a_entity_release(AEntity* Entity)
{
    if(a__ecsCollection->deleting) {
        // Entity could have already been freed. This is the only ECS function
        // that may be called from AFree callbacks.
        return;
    }

    if(Entity->references-- == 0) {
        a_out__fatal("Release count exceeds reference count for '%s'",
                     a_entity_getId(Entity));
    }
}

void a_entity_remove(AEntity* Entity)
{
    if(!a_entity_isRemoved(Entity)) {
        a_list_removeNode(Entity->node);
        Entity->node = a_list_addLast(a__ecsCollection->removedEntities,
                                                Entity);
    }
}

bool a_entity_isRemoved(const AEntity* Entity)
{
    return a_list__nodeGetList(Entity->node)
        == a__ecsCollection->removedEntities;
}

void a_entity_markActive(AEntity* Entity)
{
    Entity->lastActive = a_fps_getCounter();
}

bool a_entity_isActive(const AEntity* Entity)
{
    return Entity->lastActive == a_fps_getCounter();
}

void* a_entity_addComponent(AEntity* Entity, const char* Component)
{
    if(!entityIsNew(Entity)) {
        a_out__fatal("Too late to add component '%s' to '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    const AComponent* c = a_strhash_get(a__ecsComponents, Component);

    if(c == NULL) {
        a_out__fatal("Unknown component '%s' for '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    if(a_bitfield_test(Entity->componentBits, c->bit)) {
        a_out__fatal("Component '%s' was already added to '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    AComponentHeader* header = a_mem_zalloc(c->size);

    header->component = c;
    header->entity = Entity;

    a_strhash_add(Entity->components, Component, header);
    a_bitfield_set(Entity->componentBits, c->bit);

    return getComponent(header);
}

bool a_entity_hasComponent(const AEntity* Entity, const char* Component)
{
    bool has = a_strhash_contains(Entity->components, Component);

    if(!has && !a_strhash_contains(a__ecsComponents, Component)) {
        a_out__fatal("Unknown component '%s' for '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    return has;
}

void* a_entity_getComponent(const AEntity* Entity, const char* Component)
{
    AComponentHeader* header = a_strhash_get(Entity->components, Component);

    if(header == NULL) {
        if(!a_strhash_contains(a__ecsComponents, Component)) {
            a_out__fatal("Unknown component '%s' for '%s'",
                         Component,
                         a_entity_getId(Entity));
        }

        return NULL;
    }

    return getComponent(header);
}

void* a_entity_reqComponent(const AEntity* Entity, const char* Component)
{
    AComponentHeader* header = a_strhash_get(Entity->components, Component);

    if(header == NULL) {
        if(!a_strhash_contains(a__ecsComponents, Component)) {
            a_out__fatal("Unknown component '%s' for '%s'",
                         Component,
                         a_entity_getId(Entity));
        }

        a_out__fatal("Missing required component '%s' in '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    return getComponent(header);
}

void a_entity_mute(AEntity* Entity)
{
    Entity->muted = true;
}

void a_entity_unmute(AEntity* Entity)
{
    Entity->muted = false;
}

bool a_entity_isMuted(const AEntity* Entity)
{
    return Entity->muted;
}
