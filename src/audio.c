

#include <stdlib.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define PAYLOAD_TIME 10

typedef void (*pcm_gen_t)(short *, int, int);

#define def_pcm_samplerate 8000
#define def_pcm_samples (def_pcm_samplerate * PAYLOAD_TIME)

typedef unsigned char byte;

// 播放音频
void *audio_thread(pcm_gen_t *funcs)
{
    snd_pcm_t *pcm_out;
    snd_pcm_open(&pcm_out, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, def_pcm_samplerate, 1, .5e6);

    short *buffer = malloc(def_pcm_samples * 2);

    int i = 0;
    pcm_gen_t func;
    while (1)
    {
        if (!(func = funcs[i++]))
        {
            i = 0;
            continue;
        }

        func(buffer, def_pcm_samplerate, def_pcm_samples);
        int nwrite = 0;
        while (nwrite < def_pcm_samples)
            nwrite += snd_pcm_writei(pcm_out, buffer + nwrite, def_pcm_samples - nwrite);
    }

    free(buffer);

    snd_pcm_close(pcm_out);
}

void pcm_gen1(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)((((t & t >> 8) - (t >> 13 & t)) & ((t & t >> 8) - (t >> 13))) ^ (t >> 8 & t));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen2(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)((t - (t >> 4 & t >> 8) & t >> 12) - 1);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen3(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)(((t >> 8 & t >> 4) >> (t >> 16 & t >> 8)) * t);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen4(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)((t & (t >> 7 | t >> 8 | t >> 16) ^ t) * t);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen5(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)((t * t / (1 + (t >> 9 & t >> 8))) & 128);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen6(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)(t >> 5 | (t >> 2) * (t >> 5));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen7(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)(100 * ((t << 2 | t >> 5 | t ^ 63) & (t << 10 | t >> 11)));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen8(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)(t / 8 >> (t >> 9) * t / ((t >> 14 & 3) + 4));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen9(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)(10 * (t & 5 * t | t >> 6 | (t & 32768 ? -6 * t / 7 : (t & 65536 ? -9 * t & 100 : -9 * (t & 100)) / 11)));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen10(short *buffer, int samplerate, int samples)
{
    for (int t = 0; t < samples * 2; t++)
    {
        byte bFreq = (byte)(10 * (t >> 7 | 3 * t | t >> (t >> 15)) + (t >> 8 & 5));
        ((byte *)buffer)[t] = bFreq;
    }
}

pcm_gen_t pcm_gen_funcs[] = {
    pcm_gen1,
    pcm_gen2,
    pcm_gen3,
    pcm_gen4,
    pcm_gen5,
    pcm_gen6,
    pcm_gen7,
    pcm_gen8,
    pcm_gen9,
    pcm_gen10,
    NULL,
};

int main()
{
    audio_thread(pcm_gen_funcs);
}
