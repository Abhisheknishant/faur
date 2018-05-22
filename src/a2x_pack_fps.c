/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_system_includes.h"
#include "a2x_pack_fps.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_time.v.h"
#include "a2x_pack_timer.v.h"

#define AVERAGE_WINDOW_SEC 2
#define SKIP_ADJUST_DELAY_SEC 2
#define NO_SLEEP_RESET_SEC 20

#if A_PLATFORM_SYSTEM_EMSCRIPTEN
    #define ALLOW_SLEEP false
#else
    #define ALLOW_SLEEP true
#endif

static struct {
    unsigned tickRate;
    unsigned drawRate;
    unsigned skipFramesMax;
    bool allowSkipFrames;
    bool vsyncOn;
} g_settings;

static struct {
    unsigned head;
    unsigned len;
    unsigned* drawFrameMs;
    unsigned drawFrameMsSum;
    unsigned* drawFrameMsMin;
    unsigned drawFrameMsMinSum;
} g_history;

static struct {
    unsigned fpsThresholdFast;
    unsigned fpsThresholdSlow;
    ATimer* skipAdjustTimer;
    ATimer* noSleepTimer;
} g_skip;

static struct {
    unsigned skipFramesNum;
    unsigned targetDrawFps;
    unsigned drawFrameMs;
    unsigned tickFrameMs;
    unsigned drawFps;
    unsigned drawFpsMax;
    unsigned frameCounter;
    unsigned tickCreditMs;
    uint32_t lastFrameMs;
    bool canSleep;
} g_run;

void a_fps__init(void)
{
    g_settings.tickRate = a_settings_getUnsigned("fps.tick");
    g_settings.drawRate = a_settings_getUnsigned("fps.draw");
    g_settings.skipFramesMax = a_settings_getUnsigned("fps.draw.skip.max");
    g_settings.allowSkipFrames = a_settings_getBool("fps.draw.skip");
    g_settings.vsyncOn = a_settings_getBool("video.vsync");

    if(g_settings.tickRate < 1) {
        a_out__fatal("Invalid setting fps.tick=0");
    }

    if(g_settings.drawRate < 1) {
        a_out__fatal("Invalid setting fps.draw=0");
    } else if(g_settings.skipFramesMax >= g_settings.drawRate) {
        a_out__fatal("Invalid setting fps.draw.skip.max=%u for fps.draw=%u",
                     g_settings.skipFramesMax,
                     g_settings.drawRate);
    }

    g_history.head = 0;
    g_history.len = g_settings.drawRate * AVERAGE_WINDOW_SEC;
    g_history.drawFrameMs = a_mem_malloc(g_history.len * sizeof(unsigned));
    g_history.drawFrameMsMin = a_mem_malloc(g_history.len * sizeof(unsigned));

    g_skip.skipAdjustTimer = a_timer_new(A_TIMER_SEC, SKIP_ADJUST_DELAY_SEC);
    g_skip.noSleepTimer = a_timer_new(A_TIMER_SEC, NO_SLEEP_RESET_SEC);

    g_run.frameCounter = 0;
    g_run.canSleep = ALLOW_SLEEP;

    a_timer_start(g_skip.skipAdjustTimer);

    a_fps__reset(0);
}

void a_fps__uninit(void)
{
    a_timer_free(g_skip.skipAdjustTimer);
    a_timer_free(g_skip.noSleepTimer);

    free(g_history.drawFrameMs);
    free(g_history.drawFrameMsMin);
}

void a_fps__reset(unsigned NumFramesToSkip)
{
    g_run.skipFramesNum = NumFramesToSkip;
    g_run.targetDrawFps = g_settings.drawRate / (1 + g_run.skipFramesNum);
    g_run.drawFrameMs = 1000 / g_run.targetDrawFps;
    g_run.tickFrameMs = 1000 / g_settings.tickRate;

    g_run.drawFps = g_run.targetDrawFps;
    g_run.drawFpsMax = g_run.targetDrawFps;

    for(unsigned i = g_history.len; i--; ) {
        g_history.drawFrameMs[i] = g_run.drawFrameMs;
        g_history.drawFrameMsMin[i] = g_run.drawFrameMs;
    }

    g_history.drawFrameMsSum = g_history.len * 1000 / g_run.targetDrawFps;
    g_history.drawFrameMsMinSum = g_history.drawFrameMsSum;

    if(g_run.skipFramesNum > 0) {
        g_skip.fpsThresholdFast = g_settings.drawRate / g_run.skipFramesNum;
    }

    g_skip.fpsThresholdSlow = (g_run.targetDrawFps > 3)
                                ? (g_run.targetDrawFps - 2)
                                : 1;

    g_run.lastFrameMs = a_time_getMs();
    g_run.tickCreditMs = g_run.tickFrameMs;
}

bool a_fps__tick(void)
{
    if(g_run.tickCreditMs >= g_run.tickFrameMs) {
        g_run.tickCreditMs -= g_run.tickFrameMs;
        g_run.frameCounter++;

        return true;
    }

    return false;
}

void a_fps__frame(void)
{
    uint32_t nowMs = a_time_getMs();
    uint32_t elapsedMs = nowMs - g_run.lastFrameMs;

    if(elapsedMs > 0) {
        g_history.drawFrameMsMinSum -= g_history.drawFrameMsMin[g_history.head];
        g_history.drawFrameMsMin[g_history.head] = elapsedMs;
        g_history.drawFrameMsMinSum += elapsedMs;
        g_run.drawFpsMax = g_history.len * 1000 / g_history.drawFrameMsMinSum;
    }

    if(!g_settings.vsyncOn) {
        while(elapsedMs < g_run.drawFrameMs) {
            if(g_run.canSleep) {
                a_time_waitMs(g_run.drawFrameMs - elapsedMs);
            }

            nowMs = a_time_getMs();
            elapsedMs = nowMs - g_run.lastFrameMs;
        }
    }

    if(elapsedMs > 0) {
        g_history.drawFrameMsSum -= g_history.drawFrameMs[g_history.head];
        g_history.drawFrameMs[g_history.head] = elapsedMs;
        g_history.drawFrameMsSum += g_history.drawFrameMs[g_history.head];
        g_run.drawFps = g_history.len * 1000 / g_history.drawFrameMsSum;
    }

    g_history.head = (g_history.head + 1) % g_history.len;
    g_run.lastFrameMs = nowMs;

    if(!g_settings.vsyncOn && g_settings.allowSkipFrames
        && a_timer_isExpired(g_skip.skipAdjustTimer)) {

        unsigned newFrameSkip = UINT_MAX;

        if(g_run.drawFpsMax <= g_skip.fpsThresholdSlow
            && g_run.skipFramesNum < g_settings.skipFramesMax) {

            newFrameSkip = g_run.skipFramesNum + 1;
        } else if(g_run.drawFpsMax >= g_skip.fpsThresholdFast
            && g_run.skipFramesNum > 0) {

            newFrameSkip = g_run.skipFramesNum - 1;
        }

        if(newFrameSkip != UINT_MAX) {
            if(newFrameSkip == 0) {
                a_timer_start(g_skip.noSleepTimer);
            } else {
                a_timer_stop(g_skip.noSleepTimer);
            }

            g_run.canSleep = false;
            a_fps__reset(newFrameSkip);
        } else if(!g_run.canSleep && a_timer_isExpired(g_skip.noSleepTimer)) {
            g_run.canSleep = ALLOW_SLEEP;
        }
    }

    g_run.tickCreditMs += elapsedMs;
}

unsigned a_fps_getTickRate(void)
{
    return g_settings.tickRate;
}

unsigned a_fps_getDrawRate(void)
{
    return g_run.drawFps;
}

unsigned a_fps_getDrawRateMax(void)
{
    return g_run.drawFpsMax;
}

unsigned a_fps_getDrawSkip(void)
{
    return g_run.skipFramesNum;
}

unsigned a_fps_getCounter(void)
{
    return g_run.frameCounter;
}

bool a_fps_isNthFrame(unsigned N)
{
    return (g_run.frameCounter % N) == 0;
}

unsigned a_fps_msToFrames(unsigned Ms)
{
    return Ms * g_settings.tickRate / 1000;
}
