

#include <stdlib.h>
#include <math.h>

#include "audio.h"

void pcm_gen1(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)((((t & t >> 8) - (t >> 13 & t)) & ((t & t >> 8) - (t >> 13))) ^ (t >> 8 & t));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen2(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)((t - (t >> 4 & t >> 8) & t >> 12) - 1);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen3(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)(((t >> 8 & t >> 4) >> (t >> 16 & t >> 8)) * t);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen4(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)((t & (t >> 7 | t >> 8 | t >> 16) ^ t) * t);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen5(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)((t * t / (1 + (t >> 9 & t >> 8))) & 128);
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen6(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)(t >> 5 | (t >> 2) * (t >> 5));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen7(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)(100 * ((t << 2 | t >> 5 | t ^ 63) & (t << 10 | t >> 11)));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen8(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)(t / 8 >> (t >> 9) * t / ((t >> 14 & 3) + 4));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen9(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)(10 * (t & 5 * t | t >> 6 | (t & 32768 ? -6 * t / 7 : (t & 65536 ? -9 * t & 100 : -9 * (t & 100)) / 11)));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen10(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples = (offset + samples) * 2;
    for (int t = offset * 2; t < samples; t++)
    {
        byte bFreq = (byte)(10 * (t >> 7 | 3 * t | t >> (t >> 15)) + (t >> 8 & 5));
        ((byte *)buffer)[t] = bFreq;
    }
}
void pcm_gen11(short *buffer, int offset, int samples)
{
    buffer -= offset;
    samples += offset;
    for (int i = offset; i < samples; i++)
    {
        buffer[i] = (short)rand() * (1 - (i * 3 & 0x7fff) / (float)(0x7fff / 2));
        buffer[i] += (short)rand() % 0x4000;
    }
}

struct pcm_gen_data pcm_gen_funcs[] = {
    {pcm_gen1, def_play_time},
    {pcm_gen2, def_play_time},
    {pcm_gen3, def_play_time},
    {pcm_gen4, def_play_time},
    {pcm_gen5, def_play_time},
    {pcm_gen6, def_play_time},
    {pcm_gen7, def_play_time},
    {pcm_gen8, def_play_time},
    {pcm_gen9, def_play_time},
    {pcm_gen10, def_play_time},
    {NULL, 0},
};
