#include <stdlib.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define def_time 10
#define def_pcm_samplerate 8000
#define def_buffer_len 4096
#define _pcm_gen_(name, expr)                                    \
    void pcm_gen##name(short *buffer, int offset, int samples) { \
        for (int i = offset; i < samples + offset; i++)          \
            buffer[i - offset] = (unsigned char)(expr) << 8;     \
    }

_pcm_gen_(1, i + (i & i ^ i >> 6) - i * ((i >> 9) & (i % 16 ? 2 : 6) & i >> 9));
_pcm_gen_(2, (i - (i >> 4 & i >> 8) & i >> 12) - 1);
_pcm_gen_(3, i & 598 ? i >> 4 : i >> 10);
_pcm_gen_(4, 100 * ((i << 2 | i >> 5 | i ^ 63) & (i << 10 | i >> 11)));
_pcm_gen_(5, i *(i >> 8 | i >> 9) & 46 & i >> 8 ^ (i & i >> 13 | i >> 6));
_pcm_gen_(6, 2 * (i >> 5 & i) - (i >> 5) + i * (i >> 14 & 14));
_pcm_gen_(7, i *(3 + (1 ^ i >> 10 & 5)) * (5 + (3 & i >> 14)) >> (i >> 8 & 3));
_pcm_gen_(8, 3 * ((i >> 1) + 20) * i >> 14 * i >> 18);
_pcm_gen_(9, 5 * i & i >> 7 | 3 * i & 4 * i >> 10);
_pcm_gen_(10, i >> 5 | (i >> 2) * (i >> 5));
_pcm_gen_(11, (i >> 4) * (i >> 3) | i >> (i & 4096 ? 3 : 2));
_pcm_gen_(12, i *(i & 16384 ? 6 : 5) * (3 + (1 & i >> 8)) >> (3 & i >> 8) | i >> 4);
_pcm_gen_(13, i >> 5 | (i << 3) + 12 * i * (i >> 13 | (i >> 1 | i >> 10 | i >> 2) & i >> 8));
_pcm_gen_(14, i *(42 & i >> 10));
_pcm_gen_(15, (i & 64 | i >> 5) ^ (i & 33 | i >> 8) ^ (i & 14 | i >> 9 | i & 76) ^ (i | 187) ^ i * (i >> 8 & 838 + i >> 13) & 644);

struct pcm_gen_data { void (*func)(short *, int, int); float time; } data[] = {
    pcm_gen1, def_time, pcm_gen2, def_time, pcm_gen3, def_time, pcm_gen4, def_time, pcm_gen5, def_time, pcm_gen6, def_time,
    pcm_gen7, def_time, pcm_gen8, def_time, pcm_gen9, def_time, pcm_gen10, def_time, pcm_gen11, def_time, pcm_gen12, def_time,
    pcm_gen13, def_time, pcm_gen14, def_time, pcm_gen15, def_time};
int main() {
    snd_pcm_t *pcm_out;
    snd_pcm_open(&pcm_out, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, def_pcm_samplerate, 0, .5e6);
    short buffer[def_buffer_len];
    for (int i = 0, len;; ++i >= sizeof(data) / sizeof(struct pcm_gen_data) ? i = 0 : 0)
    {
        if (data[i].func == pcm_gen3)
            snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, 32000, 0, .5e6);
        for (int offset = 0, samples = data[i].time * def_pcm_samplerate; samples > 0; offset += def_buffer_len, samples -= def_buffer_len)
        {
            data[i].func(buffer, offset, len = samples > def_buffer_len ? def_buffer_len : samples);
            for (int nwrite = 0; nwrite < len;)
                nwrite += snd_pcm_writei(pcm_out, buffer + nwrite, len - nwrite);
        }
    }
    snd_pcm_close(pcm_out);
}