

#include <stdlib.h>
#include <pthread.h>
#include <time.h>
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

int main(int argc, char **argv)
{

    // SeedXorshift32((DWORD)__rdtsc());

    display_init();

    start_audio_thread(pcm_gen_funcs);

    do_Payload(NULL, Payload1);
    do_Payload(NULL, Payload2);
    do_Payload(NULL, Payload3);
    do_Payload(NULL, Payload4);
    do_Payload(NULL, Payload5);
    do_Payload(Payload6_init, Payload6);
    do_Payload(NULL, Payload7);
    do_Payload(Payload8_init, Payload8);
    do_Payload(Payload9_init, Payload9);
    // do_Payload(NULL,Payload10);

    // ExecuteShader(Shader1, def_play_time);
    // ExecuteShader(Shader2, def_play_time);
    // ExecuteShader(Shader3, def_play_time);
    // ExecuteShader(Shader4, def_play_time);
    // ExecuteShader(Shader5, def_play_time);
    // ExecuteShader(Shader6, def_play_time);
    // ExecuteShader(Shader7, def_play_time);
    // ExecuteShader(Shader8, def_play_time);
    // ExecuteShader(Shader9, def_play_time);
    // ExecuteShader(Shader10, def_play_time);
    // ExecuteShader(Shader11, def_play_time);
    // ExecuteShader(Shader12, def_play_time);
    // ExecuteShader(Shader13, def_play_time);
    // ExecuteShader(Shader14, def_play_time);
    // ExecuteShader(Shader15, def_play_time);
    // ExecuteShader(Shader16, def_play_time);

    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(WindowsCorruptionPayload), NULL, 0, NULL);
    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(FileMessPayload), (PVOID)SystemDirectory, 0, NULL);
    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MessageBoxPayload), NULL, 0, NULL);
    return 0;
}