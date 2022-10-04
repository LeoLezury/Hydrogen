

#pragma once

#include <stdlib.h>

#include "basic.h"

extern void pcm_gen1(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen2(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen3(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen4(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen5(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen6(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen7(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen8(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen9(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen10(short *buffer, int offset, int samples) __THROW __nonnull((1));
extern void pcm_gen11(short *buffer, int offset, int samples) __THROW __nonnull((1));

extern struct pcm_gen_data pcm_gen_funcs[];
