

#include <stdlib.h>
#include <pthread.h>
#include <alsa/asoundlib.h>

#include "audio.h"

// 使用指定 pcm-gen 函数生成 pcm 数据并播放
void *audio_thread(struct pcm_gen_data *data)
{
    // pthread_detach(pthread_self());

    snd_pcm_t *pcm_out;
    snd_pcm_open(&pcm_out, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, def_pcm_samplerate, 0, .5e6);

    short buffer[def_buffer_len];

    pcm_gen_t func;
play:
    for (int i = 0; func = data[i].func; i++)
    {
        if (!(data[i].time > 0))
            goto loop;
        int samples = data[i].time * def_pcm_samplerate;

        for (int offset = 0; samples > 0; offset += def_buffer_len, samples -= def_buffer_len)
        {
            int len = samples > def_buffer_len ? def_buffer_len : samples;
            func(buffer, offset, len);
            for (int nwrite = 0; nwrite < len;)
                nwrite += snd_pcm_writei(pcm_out, buffer + nwrite, len - nwrite);
        }
    }
    goto play;
    goto end;

loop:
    for (int offset = 0;; offset += def_buffer_len)
    {
        func(buffer, offset, def_buffer_len);
        for (int nwrite = 0; nwrite < def_buffer_len;)
            nwrite += snd_pcm_writei(pcm_out, buffer + nwrite, def_buffer_len - nwrite);
    }

end:
    snd_pcm_close(pcm_out);
}
pthread_t start_audio_thread(struct pcm_gen_data *data)
{
    pthread_t t;
    pthread_create(&t, NULL, (void *(*)(void *))audio_thread, data);
    return t;
}
