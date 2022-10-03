

#pragma once

#include "basic.h"

#define def_pcm_samplerate 8000
#define def_buffer_len 4096

typedef void (*pcm_gen_t)(short *, int, int);
struct pcm_gen_data
{
    pcm_gen_t func;
    float time;
};

extern struct pcm_gen_data pcm_gen_funcs[];

extern void *audio_thread(struct pcm_gen_data *data) __THROW;

extern pthread_t start_audio_thread(struct pcm_gen_data *data) __THROW;
