/*
    Copyright 2011, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_FILES_DIR_P_H
#define A_INC_FILES_DIR_P_H

#include "general/a_system_includes.h"

typedef struct ADir ADir;

#include "data/a_list.p.h"
#include "files/a_path.p.h"

extern ADir* a_dir_new(const char* Path);
extern void a_dir_free(ADir* Dir);

extern const APath* a_dir_pathGet(const ADir* Dir);
extern const AList* a_dir_entriesGet(const ADir* Dir);

#endif // A_INC_FILES_DIR_P_H
