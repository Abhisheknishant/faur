#
# App
#
A_CONFIG_APP_AUTHOR ?= Unknown
A_CONFIG_APP_NAME ?= Untitled
A_CONFIG_APP_VERSION_MAJOR ?= 1
A_CONFIG_APP_VERSION_MINOR ?= 0
A_CONFIG_APP_VERSION_MICRO ?= 0

#
# Build and compilation options
#
A_CONFIG_BUILD_AR_FLAGS ?=
A_CONFIG_BUILD_CFLAGS ?=
A_CONFIG_BUILD_DEBUG ?= 0
A_CONFIG_BUILD_DEBUG_WAIT ?= 0
A_CONFIG_BUILD_ID ?= default
A_CONFIG_BUILD_LIBS ?=
A_CONFIG_BUILD_OPT ?= -O0
A_CONFIG_BUILD_PLATFORM ?= unknown
A_CONFIG_BUILD_UID := $(A_CONFIG_BUILD_PLATFORM)_$(A_CONFIG_BUILD_ID)

#
# Colors
#
A_CONFIG_COLOR_SCREEN_BORDER ?= 0x1f0f0f
A_CONFIG_COLOR_SPRITE_BORDER ?= 0x00FF00
A_CONFIG_COLOR_SPRITE_KEY ?= 0xFF00FF
A_CONFIG_COLOR_VOLBAR_BACKGROUND ?= 0x1f0f0f
A_CONFIG_COLOR_VOLBAR_BORDER ?= 0x3f8fdf
A_CONFIG_COLOR_VOLBAR_FILL ?= 0x9fcf3f

#
# Project dirs
#
#   A_CONFIG_DIR_ASSETS - Project's media assets
#   A_CONFIG_DIR_BUILD - Root build dir
#   A_CONFIG_DIR_SCREENSHOTS - Executable-relative path to screenshots dir
#   A_CONFIG_DIR_SRC - Project's source tree
#
A_CONFIG_DIR_ASSETS ?= assets
A_CONFIG_DIR_BUILD ?= build
A_CONFIG_DIR_SCREENSHOTS ?= screenshots
A_CONFIG_DIR_SRC ?= src

#
# ECS
#
#   A_CONFIG_ECS_COM_NUM - Source dir-relative file and enum value to read
#   A_CONFIG_ECS_SYS_NUM - Source dir-relative file and enum value to read
#
ifdef A_CONFIG_ECS_COM_NUM
    A_CONFIG_ECS_COM_NUM := $(shell $(A2X_PATH)/bin/a2x_enum -q $(A_DIR_ROOT)/$(A_CONFIG_DIR_SRC)/$(A_CONFIG_ECS_COM_NUM))
endif

ifdef A_CONFIG_ECS_SYS_NUM
    A_CONFIG_ECS_SYS_NUM := $(shell $(A2X_PATH)/bin/a2x_enum -q $(A_DIR_ROOT)/$(A_CONFIG_DIR_SRC)/$(A_CONFIG_ECS_SYS_NUM))
endif

A_CONFIG_ECS_COM_NUM ?= 1
A_CONFIG_ECS_SYS_NUM ?= 1

#
# FPS
#
A_CONFIG_FPS_CAP_LAG ?= 0
A_CONFIG_FPS_RATE_TICK ?= 30
A_CONFIG_FPS_RATE_DRAW ?= $(A_CONFIG_FPS_RATE_TICK)

ifeq ($(shell expr $(A_CONFIG_FPS_RATE_TICK) \< 1), 1)
    $(error A_CONFIG_FPS_RATE_TICK ($(A_CONFIG_FPS_RATE_TICK)) < 1)
endif

ifeq ($(shell expr $(A_CONFIG_FPS_RATE_DRAW) \< 1), 1)
    $(error A_CONFIG_FPS_RATE_DRAW ($(A_CONFIG_FPS_RATE_DRAW)) < 1)
endif

ifeq ($(shell expr $(A_CONFIG_FPS_RATE_TICK) \< $(A_CONFIG_FPS_RATE_DRAW)), 1)
    $(error A_CONFIG_FPS_RATE_TICK ($(A_CONFIG_FPS_RATE_TICK)) < A_CONFIG_FPS_RATE_DRAW ($(A_CONFIG_FPS_RATE_DRAW)))
endif

#
# Inputs
#
A_CONFIG_INPUT_ANALOG_AXES_INVERT ?= 0
A_CONFIG_INPUT_ANALOG_AXES_SWITCH ?= 0
A_CONFIG_INPUT_MOUSE_CURSOR ?= 1
A_CONFIG_INPUT_MOUSE_TRACK ?= 0

#
# Make
#
#   A_CONFIG_MAKE_CLEAN - Targets to run before the default clean target
#

#
# Debug Output
#
A_CONFIG_OUTPUT_CONSOLE_SHOW ?= 0
A_CONFIG_OUTPUT_ON ?= 1

#
# File paths
#
#   A_CONFIG_PATH_EMBED - Root-relative space-separated file and dir paths
#   A_CONFIG_PATH_EMBED_EMSCRIPTEN - Same as above, but using Emscripten
#   A_CONFIG_PATH_GFX - Root-relative image and dir paths to convert to bin data
#   A_CONFIG_PATH_STATIC_COPY - dirs in build/static to expand and copy to bin
#   A_CONFIG_PATH_STORAGE_PREFIX - IDBFS mount point on Emscripten targets
#
A_CONFIG_PATH_EMBED ?=
A_CONFIG_PATH_EMBED_EMSCRIPTEN ?=
A_CONFIG_PATH_GFX ?=
A_CONFIG_PATH_STATIC_COPY ?=
A_CONFIG_PATH_STORAGE_PREFIX ?= ./

#
# Screen properties
#
#   A_CONFIG_SCREEN_ALLOCATE - Allocate a buffer or try to use the platform's
#   A_CONFIG_SCREEN_BPP - Color depth, bits per pixel
#   A_CONFIG_SCREEN_FORMAT - Colors order: RGBA, ABGR
#   A_CONFIG_SCREEN_FULLSCREEN - Whether to start in fullscreen mode
#   A_CONFIG_SCREEN_MAXIMIZED - Whether the window should start maximized
#   A_CONFIG_SCREEN_HARDWARE_HEIGHT, WIDTH - Device-specific resolution
#   A_CONFIG_SCREEN_HEIGHT, WIDTH - Logical screen resolution
#   A_CONFIG_SCREEN_VSYNC - Try to use V-sync
#   A_CONFIG_SCREEN_WIZ_FIX - Fix screen tearing on GP2X Wiz
#   A_CONFIG_SCREEN_ZOOM - Integer zoom when showing the logical screen
#
A_CONFIG_SCREEN_HARDWARE_WIDTH ?= 0
A_CONFIG_SCREEN_HARDWARE_HEIGHT ?= 0

ifneq ($(A_CONFIG_SCREEN_HARDWARE_WIDTH), 0)
    A_CONFIG_SCREEN_WIDTH ?= $(A_CONFIG_SCREEN_HARDWARE_WIDTH)
endif

ifneq ($(A_CONFIG_SCREEN_HARDWARE_HEIGHT), 0)
    A_CONFIG_SCREEN_HEIGHT ?= $(A_CONFIG_SCREEN_HARDWARE_HEIGHT)
endif

A_CONFIG_SCREEN_WIDTH ?= 320
A_CONFIG_SCREEN_HEIGHT ?= 240

ifdef A_CONFIG_SCREEN_ZOOM
    ifneq ($(A_CONFIG_SCREEN_ZOOM), 1)
        A_CONFIG_SCREEN_ALLOCATE := 1
    endif
endif

ifeq ($(A_CONFIG_LIB_SDL), 2)
    A_CONFIG_SCREEN_ALLOCATE := 1
    A_CONFIG_SCREEN_VSYNC ?= 1
endif

ifeq ($(A_CONFIG_SYSTEM_WIZ), 1)
    A_CONFIG_SCREEN_WIZ_FIX ?= 1
endif

ifeq ($(A_CONFIG_SCREEN_WIZ_FIX), 1)
    A_CONFIG_SCREEN_ALLOCATE := 1
endif

ifeq ($(A_CONFIG_TRAIT_DESKTOP), 1)
    A_CONFIG_SCREEN_BPP ?= 32
endif

A_CONFIG_SCREEN_ALLOCATE ?= 0
A_CONFIG_SCREEN_BPP ?= 16
A_CONFIG_SCREEN_FORMAT ?= RGBA
A_CONFIG_SCREEN_FULLSCREEN ?= 0
A_CONFIG_SCREEN_MAXIMIZED ?= 1
A_CONFIG_SCREEN_VSYNC ?= 0
A_CONFIG_SCREEN_WIZ_FIX ?= 0
A_CONFIG_SCREEN_ZOOM ?= 1

#
# Sound
#
A_CONFIG_SOUND_ENABLED ?= 1
A_CONFIG_SOUND_MUTE ?= 0
A_CONFIG_SOUND_SAMPLE_CHANNELS_TOTAL ?= 64
A_CONFIG_SOUND_SAMPLE_CHANNELS_RESERVED ?= 32
A_CONFIG_SOUND_VOLUME_SCALE_MUSIC ?= 100
A_CONFIG_SOUND_VOLUME_SCALE_SAMPLE ?= 100

#
# Systems and platforms
#
A_CONFIG_SYSTEM_CAANOO ?= 0
A_CONFIG_SYSTEM_EMSCRIPTEN ?= 0
A_CONFIG_SYSTEM_GP2X ?= 0
A_CONFIG_SYSTEM_LINUX ?= 0
A_CONFIG_SYSTEM_MINGW ?= 0
A_CONFIG_SYSTEM_PANDORA ?= 0
A_CONFIG_SYSTEM_WIZ ?= 0

A_CONFIG_SYSTEM_GP2X_MENU ?= 1
A_CONFIG_SYSTEM_GP2X_MHZ ?= 0

A_CONFIG_SYSTEM_EMSCRIPTEN_TOTAL_MEMORY ?=

#
# System traits
#
A_CONFIG_TRAIT_DESKTOP ?= 0
A_CONFIG_TRAIT_KEYBOARD ?= 0
A_CONFIG_TRAIT_NOSLEEP ?= 0

#
# Libraries
#
#   A_CONFIG_LIB_RENDER - Possible values: SOFTWARE, SDL
#   A_CONFIG_LIB_SDL - 1 or 2 if using SDL
#   A_CONFIG_LIB_SDL_CONFIG - Path to sdl-config host binary
#   A_CONFIG_LIB_SDL_GAMEPADMAP - Bin-relative path to SDL2 gamepad mappings
#   A_CONFIG_LIB_SDL_TIME - Whether to use the SDL timer
#
A_CONFIG_LIB_SDL ?= 0
A_CONFIG_LIB_SDL_GAMEPADMAP ?= gamecontrollerdb.txt
A_CONFIG_LIB_SDL_TIME ?= 0

ifneq ($(A_CONFIG_PATH_GFX), )
    A_CONFIG_LIB_RENDER ?= SOFTWARE

    ifneq ($(A_CONFIG_LIB_RENDER), SOFTWARE)
        $(error A_CONFIG_PATH_GFX requires A_CONFIG_LIB_RENDER = SOFTWARE)
    endif
endif

ifeq ($(A_CONFIG_LIB_SDL), 2)
    A_CONFIG_LIB_RENDER ?= SDL
else
    A_CONFIG_LIB_RENDER ?= SOFTWARE
endif

ifdef A_CONFIG_LIB_SDL_CONFIG
    A_CONFIG_BUILD_CFLAGS += $(shell $(A_CONFIG_LIB_SDL_CONFIG) --cflags)

    ifeq ($(A_CONFIG_LIB_SDL), 1)
        A_CONFIG_BUILD_LIBS += $(shell $(A_CONFIG_LIB_SDL_CONFIG) --libs)

        ifeq ($(A_CONFIG_SOUND_ENABLED), 1)
            A_CONFIG_BUILD_LIBS += -lSDL_mixer
        endif
    else ifeq ($(A_CONFIG_LIB_SDL), 2)
        A_CONFIG_BUILD_LIBS += $(shell $(A_CONFIG_LIB_SDL_CONFIG) --libs)

        ifeq ($(A_CONFIG_SOUND_ENABLED), 1)
            A_CONFIG_BUILD_LIBS += -lSDL2_mixer
        endif
    endif
endif

#
# C defines
#
A_CONFIG_BUILD_CFLAGS += \
    -DA_CONFIG_APP_AUTHOR=\"$(call A_MAKE_SPACE_ESCAPE,$(A_CONFIG_APP_AUTHOR))\" \
    -DA_CONFIG_APP_NAME=\"$(call A_MAKE_SPACE_ESCAPE,$(A_CONFIG_APP_NAME))\" \
    -DA_CONFIG_APP_VERSION_MAJOR=$(A_CONFIG_APP_VERSION_MAJOR) \
    -DA_CONFIG_APP_VERSION_MICRO=$(A_CONFIG_APP_VERSION_MICRO) \
    -DA_CONFIG_APP_VERSION_MINOR=$(A_CONFIG_APP_VERSION_MINOR) \
    -DA_CONFIG_APP_VERSION_STRING=\"$(A_CONFIG_APP_VERSION_MAJOR).$(A_CONFIG_APP_VERSION_MINOR).$(A_CONFIG_APP_VERSION_MICRO)\" \
    -DA_CONFIG_BUILD_DEBUG=$(A_CONFIG_BUILD_DEBUG) \
    -DA_CONFIG_BUILD_DEBUG_WAIT=$(A_CONFIG_BUILD_DEBUG_WAIT) \
    -DA_CONFIG_BUILD_UID=\"$(A_CONFIG_BUILD_UID)\" \
    -DA_CONFIG_COLOR_SPRITE_BORDER=$(A_CONFIG_COLOR_SPRITE_BORDER) \
    -DA_CONFIG_COLOR_SPRITE_KEY=$(A_CONFIG_COLOR_SPRITE_KEY) \
    -DA_CONFIG_COLOR_SCREEN_BORDER=$(A_CONFIG_COLOR_SCREEN_BORDER) \
    -DA_CONFIG_COLOR_VOLBAR_BACKGROUND=$(A_CONFIG_COLOR_VOLBAR_BACKGROUND) \
    -DA_CONFIG_COLOR_VOLBAR_BORDER=$(A_CONFIG_COLOR_VOLBAR_BORDER) \
    -DA_CONFIG_COLOR_VOLBAR_FILL=$(A_CONFIG_COLOR_VOLBAR_FILL) \
    -DA_CONFIG_DIR_SCREENSHOTS=\"$(A_CONFIG_DIR_SCREENSHOTS)\" \
    -DA_CONFIG_ECS_COM_NUM=$(A_CONFIG_ECS_COM_NUM) \
    -DA_CONFIG_ECS_SYS_NUM=$(A_CONFIG_ECS_SYS_NUM) \
    -DA_CONFIG_FPS_CAP_LAG=$(A_CONFIG_FPS_CAP_LAG) \
    -DA_CONFIG_FPS_RATE_DRAW=$(A_CONFIG_FPS_RATE_DRAW) \
    -DA_CONFIG_FPS_RATE_TICK=$(A_CONFIG_FPS_RATE_TICK) \
    -DA_CONFIG_INPUT_ANALOG_AXES_INVERT=$(A_CONFIG_INPUT_ANALOG_AXES_INVERT) \
    -DA_CONFIG_INPUT_ANALOG_AXES_SWITCH=$(A_CONFIG_INPUT_ANALOG_AXES_SWITCH) \
    -DA_CONFIG_INPUT_MOUSE_CURSOR=$(A_CONFIG_INPUT_MOUSE_CURSOR) \
    -DA_CONFIG_INPUT_MOUSE_TRACK=$(A_CONFIG_INPUT_MOUSE_TRACK) \
    -DA_CONFIG_LIB_RENDER_$(A_CONFIG_LIB_RENDER)=1 \
    -DA_CONFIG_LIB_SDL=$(A_CONFIG_LIB_SDL) \
    -DA_CONFIG_LIB_SDL_GAMEPADMAP=\"$(A_CONFIG_LIB_SDL_GAMEPADMAP)\" \
    -DA_CONFIG_LIB_SDL_TIME=$(A_CONFIG_LIB_SDL_TIME) \
    -DA_CONFIG_OUTPUT_CONSOLE_SHOW=$(A_CONFIG_OUTPUT_CONSOLE_SHOW) \
    -DA_CONFIG_OUTPUT_ON=$(A_CONFIG_OUTPUT_ON) \
    -DA_CONFIG_PATH_STORAGE_PREFIX=\"$(A_CONFIG_PATH_STORAGE_PREFIX)\" \
    -DA_CONFIG_SCREEN_ALLOCATE=$(A_CONFIG_SCREEN_ALLOCATE) \
    -DA_CONFIG_SCREEN_BPP=$(A_CONFIG_SCREEN_BPP) \
    -DA_CONFIG_SCREEN_FORMAT_$(A_CONFIG_SCREEN_FORMAT)=1 \
    -DA_CONFIG_SCREEN_FULLSCREEN=$(A_CONFIG_SCREEN_FULLSCREEN) \
    -DA_CONFIG_SCREEN_HARDWARE_HEIGHT=$(A_CONFIG_SCREEN_HARDWARE_HEIGHT) \
    -DA_CONFIG_SCREEN_HARDWARE_WIDTH=$(A_CONFIG_SCREEN_HARDWARE_WIDTH) \
    -DA_CONFIG_SCREEN_HEIGHT=$(A_CONFIG_SCREEN_HEIGHT) \
    -DA_CONFIG_SCREEN_MAXIMIZED=$(A_CONFIG_SCREEN_MAXIMIZED) \
    -DA_CONFIG_SCREEN_VSYNC=$(A_CONFIG_SCREEN_VSYNC) \
    -DA_CONFIG_SCREEN_WIDTH=$(A_CONFIG_SCREEN_WIDTH) \
    -DA_CONFIG_SCREEN_WIZ_FIX=$(A_CONFIG_SCREEN_WIZ_FIX) \
    -DA_CONFIG_SCREEN_ZOOM=$(A_CONFIG_SCREEN_ZOOM) \
    -DA_CONFIG_SOUND_ENABLED=$(A_CONFIG_SOUND_ENABLED) \
    -DA_CONFIG_SOUND_MUTE=$(A_CONFIG_SOUND_MUTE) \
    -DA_CONFIG_SOUND_SAMPLE_CHANNELS_RESERVED=$(A_CONFIG_SOUND_SAMPLE_CHANNELS_RESERVED) \
    -DA_CONFIG_SOUND_SAMPLE_CHANNELS_TOTAL=$(A_CONFIG_SOUND_SAMPLE_CHANNELS_TOTAL) \
    -DA_CONFIG_SOUND_VOLUME_SCALE_MUSIC=$(A_CONFIG_SOUND_VOLUME_SCALE_MUSIC) \
    -DA_CONFIG_SOUND_VOLUME_SCALE_SAMPLE=$(A_CONFIG_SOUND_VOLUME_SCALE_SAMPLE) \
    -DA_CONFIG_SYSTEM_CAANOO=$(A_CONFIG_SYSTEM_CAANOO) \
    -DA_CONFIG_SYSTEM_EMSCRIPTEN=$(A_CONFIG_SYSTEM_EMSCRIPTEN) \
    -DA_CONFIG_SYSTEM_GP2X=$(A_CONFIG_SYSTEM_GP2X) \
    -DA_CONFIG_SYSTEM_GP2X_MENU=$(A_CONFIG_SYSTEM_GP2X_MENU) \
    -DA_CONFIG_SYSTEM_GP2X_MHZ=$(A_CONFIG_SYSTEM_GP2X_MHZ) \
    -DA_CONFIG_SYSTEM_LINUX=$(A_CONFIG_SYSTEM_LINUX) \
    -DA_CONFIG_SYSTEM_MINGW=$(A_CONFIG_SYSTEM_MINGW) \
    -DA_CONFIG_SYSTEM_PANDORA=$(A_CONFIG_SYSTEM_PANDORA) \
    -DA_CONFIG_SYSTEM_WIZ=$(A_CONFIG_SYSTEM_WIZ) \
    -DA_CONFIG_TRAIT_DESKTOP=$(A_CONFIG_TRAIT_DESKTOP) \
    -DA_CONFIG_TRAIT_KEYBOARD=$(A_CONFIG_TRAIT_KEYBOARD) \
    -DA_CONFIG_TRAIT_NOSLEEP=$(A_CONFIG_TRAIT_NOSLEEP) \