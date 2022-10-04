

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XShm.h>

#include "basic.h"
#include "audio.h"
#include "display.h"

#include "pcm-gen.h"
#include "effects.h"

int main(int argc, char **argv)
{

    // SeedXorshift32((DWORD)__rdtsc());

    display_init();

    start_audio_thread(pcm_gen_funcs);

    exec_effect(NULL, effect1, def_play_time);
    exec_effect(NULL, effect2, def_play_time);
    exec_effect(NULL, effect3, def_play_time);
    exec_effect(NULL, effect4, def_play_time);
    exec_effect(NULL, effect5, def_play_time);
    exec_effect(effect6_init, effect6, def_play_time);
    exec_effect(NULL, effect7, def_play_time);
    exec_effect(effect8_init, effect8, def_play_time);
    exec_effect(effect9_init, effect9, def_play_time);
    // exec_effect(NULL,effect10);

    exec_effect(NULL, shader1, def_play_time);
    exec_effect(NULL, shader2, def_play_time);
    exec_effect(NULL, shader3, def_play_time);
    exec_effect(NULL, shader4, def_play_time);
    exec_effect(NULL, shader5, def_play_time);
    exec_effect(NULL, shader6, def_play_time);
    exec_effect(NULL, shader7, def_play_time);
    exec_effect(NULL, shader8, def_play_time);
    exec_effect(NULL, shader9, def_play_time);
    exec_effect(NULL, shader10, def_play_time);
    exec_effect(NULL, shader11, def_play_time);
    exec_effect(NULL, shader12, def_play_time);
    exec_effect(NULL, shader13, def_play_time);
    exec_effect(NULL, shader14, def_play_time);
    exec_effect(NULL, shader15, def_play_time);
    exec_effect(NULL, shader16, def_play_time);

    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(WindowsCorruptioneffect), NULL, 0, NULL);
    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(FileMesseffect), (PVOID)SystemDirectory, 0, NULL);
    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MessageBoxeffect), NULL, 0, NULL);
    return 0;
}