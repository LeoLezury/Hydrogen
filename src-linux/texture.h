

#pragma once

#include <stdlib.h>

#include <GL/gl.h>

#include "basic.h"

typedef struct tex _tex_t, *tex_t;
struct tex
{
    uint id;
    GLenum format;
    int w, h;
};

extern void tex_gen(tex_t tex, int num);
extern void tex_del(tex_t tex, int num);

extern void tex_load(tex_t tex, void *data, int w, int h, GLenum iformat, GLenum format);
extern void tex_loadf(tex_t tex, void *data, int w, int h, GLenum iformat, GLenum format);

#define tex_bind(tex) glBindTexture(GL_TEXTURE_2D, (tex).id)
#define tex_unbind() glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0)
