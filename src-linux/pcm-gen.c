

#include <stdlib.h>
#include <math.h>

#include "audio.h"

#define _pcm_gen_funcs_(name)                                  \
    void pcm_gen##name(short *buffer, int offset, int samples) \
    {                                                          \
        buffer -= offset;                                      \
        samples += offset;
#define _pcm_gen_funcs_end_ }

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

// struct pcm_gen_data pcm_gen_funcs[] = {
//     {pcm_gen1, def_play_time},
//     {pcm_gen2, def_play_time},
//     {pcm_gen3, def_play_time},
//     {pcm_gen4, def_play_time},
//     {pcm_gen5, def_play_time},
//     {pcm_gen6, def_play_time},
//     {pcm_gen7, def_play_time},
//     {pcm_gen8, def_play_time},
//     {pcm_gen9, def_play_time},
//     {pcm_gen10, def_play_time},
//     {NULL, 0},
// };

_pcm_gen_funcs_(_2_1);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i + (i & i ^ i >> 6) - i * ((i >> 9) & (i % 16 ? 2 : 6) & i >> 9));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_2);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)((i - (i >> 4 & i >> 8) & i >> 12) - 1);
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_3);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i & 598 ? i >> 4 : i >> 10);
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_4);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(100 * ((i << 2 | i >> 5 | i ^ 63) & (i << 10 | i >> 11)));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_5);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i * (i >> 8 | i >> 9) & 46 & i >> 8 ^ (i & i >> 13 | i >> 6));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_6);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(2 * (i >> 5 & i) - (i >> 5) + i * (i >> 14 & 14));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_7);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i * (3 + (1 ^ i >> 10 & 5)) * (5 + (3 & i >> 14)) >> (i >> 8 & 3));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_8);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(3 * ((i >> 1) + 20) * i >> 14 * i >> 18);
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_9);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(5 * i & i >> 7 | 3 * i & 4 * i >> 10);
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_10);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i >> 5 | (i >> 2) * (i >> 5));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_11);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)((i >> 4) * (i >> 3) | i >> (i & 4096 ? 3 : 2));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_12);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i * (i & 16384 ? 6 : 5) * (3 + (1 & i >> 8)) >> (3 & i >> 8) | i >> 4);
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_13);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i >> 5 | (i << 3) + 12 * i * (i >> 13 | (i >> 1 | i >> 10 | i >> 2) & i >> 8));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_14);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)(i * (42 & i >> 10));
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

_pcm_gen_funcs_(_2_15);
for (int i = offset; i < samples; i++)
{
    byte bFreq = (byte)((i & 64 | i >> 5) ^ (i & 33 | i >> 8) ^ (i & 14 | i >> 9 | i & 76) ^ (i | 187) ^ i * (i >> 8 & 838 + i >> 13) & 644);
    buffer[i] = bFreq << 8;
}
_pcm_gen_funcs_end_;

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
    {pcm_gen_2_1, def_play_time},
    {pcm_gen_2_2, def_play_time},
    {pcm_gen_2_3, def_play_time},
    {pcm_gen_2_4, def_play_time},
    {pcm_gen_2_5, def_play_time},
    {pcm_gen_2_6, def_play_time},
    {pcm_gen_2_7, def_play_time},
    {pcm_gen_2_8, def_play_time},
    {pcm_gen_2_9, def_play_time},
    {pcm_gen_2_10, def_play_time},
    {pcm_gen_2_11, def_play_time},
    {pcm_gen_2_12, def_play_time},
    {pcm_gen_2_13, def_play_time},
    {pcm_gen_2_14, def_play_time},
    {pcm_gen_2_15, def_play_time},
    {NULL, 0},
};
