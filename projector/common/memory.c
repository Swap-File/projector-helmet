#include "memory.h"
#include "effects.h"
#include <string.h> // memcpy
#include <stdlib.h> //exit
#include <stdio.h> //perror
#include <sched.h> //piHiPri
#include <time.h> //millis
#include "opengl.h" //max and min


uint32_t millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    uint64_t now  = (uint64_t)ts.tv_sec * (uint64_t)1000 + (uint64_t)(ts.tv_nsec / 1000000L);
    return (uint32_t)now ;
}

uint32_t micros(void)
{
    uint64_t now ;
    struct  timespec ts ;
    clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
    now  = (uint64_t)ts.tv_sec * (uint64_t)1000000 + (uint64_t)(ts.tv_nsec / 1000) ;
    return (uint32_t)now;
}

int piHiPri(const int pri)
{
    struct sched_param sched;
    memset (&sched, 0, sizeof(sched));
    if (pri > sched_get_priority_max (SCHED_RR))
        sched.sched_priority = sched_get_priority_max (SCHED_RR);
    else
        sched.sched_priority = pri;
    return sched_setscheduler (0, SCHED_RR, &sched);
}
void fps_counter(char * title,uint32_t start_time,int skip)
{
    static const uint32_t interval = 1000; //in milliseconds
    static uint32_t render_time = 0;
    static uint32_t time_fps = 0;
    static uint32_t frame_time_min = interval * 1000;
    static uint32_t frame_time_max = 0;
    static int fps = 0;
    static int frameskip = 0;
    frameskip += skip;
    uint32_t frame_time = micros() - start_time;   //in microseconds
    frame_time_max = MAX2(frame_time_max,frame_time);
    frame_time_min = MIN2(frame_time_min,frame_time);
    render_time += frame_time;
    fps++;
    if (time_fps < millis()) {
        float ms_per_frame = (float)render_time/(fps * interval);
        int load_percent = (float)render_time/(interval * 10.0);
        float jitter = (float)(frame_time_max-frame_time_min)/100.0;
        printf("%s fps:%d\t%.2fms frame\t%.2fms jitter\t%d%% load %d skipped\n",title,fps, ms_per_frame,jitter,load_percent,frameskip);
        fps = 0;
        render_time = 0;
        frameskip = 0;
        time_fps += interval;
        frame_time_min = interval * 1000;
        frame_time_max = 0;
        /* readjust counter if we missed a cycle */
        if (time_fps < millis()) time_fps = millis() + interval;
    }
}

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}