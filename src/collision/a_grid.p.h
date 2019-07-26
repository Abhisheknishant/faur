/*
    Copyright 2010-2011, 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_COLLISION_GRID_P_H
#define A_INC_COLLISION_GRID_P_H

#include "general/a_system_includes.h"

typedef struct AGrid AGrid;
typedef struct AList AGridItem;

#include "data/a_list.p.h"
#include "math/a_fix.p.h"

extern AGrid* a_grid_new(AFix Width, AFix Height, AFix MaxObjectDim);
extern void a_grid_free(AGrid* Grid);

extern AGridItem* a_griditem_new(void);
extern void a_griditem_free(AGridItem* Item);

extern void a_griditem_coordsSet(const AGrid* Grid, AGridItem* Item, void* Context, AVectorFix Coords);

extern const AList* a_grid_nearGet(const AGrid* Grid, AVectorFix Coords);

#endif // A_INC_COLLISION_GRID_P_H