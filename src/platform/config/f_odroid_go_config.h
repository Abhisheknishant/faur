/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_SYSTEM_ODROID_GO_CONFIG_H
#define F_INC_PLATFORM_SYSTEM_ODROID_GO_CONFIG_H

#if !F_CONFIG_BUILD_MAKE
    #define F_CONFIG_APP_AUTHOR "<author>"
    #define F_CONFIG_APP_NAME "Untitled"

    #define F_CONFIG_APP_VERSION_MAJOR 1
    #define F_CONFIG_APP_VERSION_MINOR 0
    #define F_CONFIG_APP_VERSION_MICRO 0

    #define F_CONFIG_BUILD_DEBUG 0
    #define F_CONFIG_BUILD_DEBUG_FATAL_SPIN 1
    #define F_CONFIG_BUILD_FAUR_GIT "<commit unknown>"
    #define F_CONFIG_BUILD_FAUR_TIME __DATE__ " " __TIME__
    #define F_CONFIG_BUILD_UID "odroid_go_arduino"

    #define F_CONFIG_COLOR_SPRITE_BORDER 0x00FF00
    #define F_CONFIG_COLOR_SPRITE_KEY 0xFF00FF

    #define F_CONFIG_FILES_PREFIX "/"

    #define F_CONFIG_FPS_RATE_DRAW 30
    #define F_CONFIG_FPS_RATE_TICK 30

    #define F_CONFIG_RENDER_SOFTWARE 1

    #define F_CONFIG_SCREEN_FORMAT F_COLOR_FORMAT_RGB_565
    #define F_CONFIG_SCREEN_HARDWARE_HEIGHT 240
    #define F_CONFIG_SCREEN_HARDWARE_WIDTH 320
    #define F_CONFIG_SCREEN_SIZE_HEIGHT 64
    #define F_CONFIG_SCREEN_SIZE_WIDTH 80
    #define F_CONFIG_SCREEN_ZOOM 3

    #define F_CONFIG_SOUND_ENABLED 0

    #define F_CONFIG_SYSTEM_ARDUINO 1
    #define F_CONFIG_SYSTEM_ODROID_GO 1
#endif // !F_CONFIG_BUILD_MAKE

#endif // F_INC_PLATFORM_SYSTEM_ODROID_GO_CONFIG_H
