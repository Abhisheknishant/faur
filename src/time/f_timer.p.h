/*
    Copyright 2011, 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_TIME_TIMER_P_H
#define F_INC_TIME_TIMER_P_H

#include "general/f_system_includes.h"

typedef struct ATimer ATimer;

typedef enum {
    F_TIMER_INVALID = -1,
    F_TIMER_MS,
    F_TIMER_SEC,
    F_TIMER_TICKS,
    F_TIMER_NUM
} ATimerType;

extern ATimer* f_timer_new(ATimerType Type, unsigned Period, bool Repeat);
extern ATimer* f_timer_dup(const ATimer* Timer);
extern void f_timer_free(ATimer* Timer);

extern unsigned f_timer_elapsedGet(const ATimer* Timer);

extern unsigned f_timer_periodGet(const ATimer* Timer);
extern void f_timer_periodSet(ATimer* Timer, unsigned Period);

extern void f_timer_start(ATimer* Timer);
extern void f_timer_stop(ATimer* Timer);
extern bool f_timer_isRunning(const ATimer* Timer);

extern bool f_timer_expiredGet(const ATimer* Timer);
extern unsigned f_timer_expiredGetCount(const ATimer* Timer);
extern void f_timer_expiredClear(ATimer* Timer);

#endif // F_INC_TIME_TIMER_P_H
