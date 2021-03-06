/*
    Copyright 2016-2018 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_STRINGS_STRBUILDER_P_H
#define F_INC_STRINGS_STRBUILDER_P_H

#include "../general/f_system_includes.h"

typedef struct FStrBuilder FStrBuilder;

extern FStrBuilder* f_strbuilder_new(size_t Bytes);
extern void f_strbuilder_free(FStrBuilder* Builder);

extern const char* f_strbuilder_get(FStrBuilder* Builder);

extern bool f_strbuilder_add(FStrBuilder* Builder, const char* String);
extern bool f_strbuilder_addf(FStrBuilder* Builder, const char* Format, ...) F__ATTRIBUTE_FORMAT(2);

#endif // F_INC_STRINGS_STRBUILDER_P_H
