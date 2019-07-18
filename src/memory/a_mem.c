/*
    Copyright 2010, 2016-2017 Alex Margarit <alex@alxm.org>
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

#include "a_mem.v.h"
#include <a2x.v.h>

void* a_mem_malloc(size_t Size)
{
    void* ptr = malloc(Size);

    if(ptr == NULL) {
        A__FATAL("malloc(%u) failed", Size);
    }

    return ptr;
}

void* a_mem_zalloc(size_t Size)
{
    void* ptr = calloc(1, Size);

    if(ptr == NULL) {
        A__FATAL("calloc(1, %u) failed", Size);
    }

    return ptr;
}

void* a_mem_dup(const void* Buffer, size_t Size)
{
    void* copy = malloc(Size);

    if(copy == NULL) {
        A__FATAL("malloc(%u) failed", Size);
    }

    memcpy(copy, Buffer, Size);

    return copy;
}
