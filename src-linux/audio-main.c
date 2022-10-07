

#include <stdio.h>
#include <stdlib.h>

#include "audio.h"

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        struct pcm_gen_data data[2] = {};
        data[0] = pcm_gen_funcs[atoi(argv[1])];
        audio_thread(data);
        return 0;
    }

    audio_thread(pcm_gen_funcs);
}
