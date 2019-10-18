/*
    Copyright 2010, 2017, 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_INPUTS_INPUT_P_H
#define F_INC_INPUTS_INPUT_P_H

#include "general/f_system_includes.h"

#include "input/f_analog.p.h"
#include "input/f_button.p.h"
#include "math/f_fix.p.h"

extern bool f_input_readKey(AKeyId Key);
extern bool f_input_readButton(AButtonId Button);
extern AFix f_input_readAnalog(AAnalogId Axis);

#endif // F_INC_INPUTS_INPUT_P_H
