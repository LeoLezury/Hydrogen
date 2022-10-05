

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

    int buffer_len = def_pcm_buffer_len;
    short buffer[buffer_len];

    pcm_gen_t func;
    int rate, channel;
play:
    for (int i = 0; func = data[i].func; i++)
    {
        rate = data[i].rate > 0 ? data[i].rate : def_pcm_samplerate;
        channel = data[i].channel > 0 ? data[i].channel : def_pcm_channels;
        snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, channel, rate, 0, .5e6);

        if (*(uint *)&data[i].time == 0x7f800000) // 如果是正无穷
            goto loop;

        int samples = (data[i].time > 0 ? data[i].time : def_play_time) * rate;

        for (int offset = 0; samples > 0; offset += buffer_len, samples -= buffer_len)
        {
            int len = samples > buffer_len ? buffer_len : samples;
            func(buffer, offset, len);
            for (int nwrite = 0; nwrite < len;)
                nwrite += snd_pcm_writei(pcm_out, buffer + nwrite, len - nwrite);
        }
    }
    goto play;
    goto end;

loop:
    for (int offset = 0;; offset += buffer_len)
    {
        func(buffer, offset, buffer_len);
        for (int nwrite = 0; nwrite < buffer_len;)
            nwrite += snd_pcm_writei(pcm_out, buffer + nwrite, buffer_len - nwrite);
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
