

#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>

#include "texture.h"

void tex_gen(tex_t tex, int num)
{
    if (!tex || num < 0)
        return;
    memset(tex, 0, num * sizeof(_tex_t));
    uint texs[num];
    glEnable(GL_TEXTURE_2D);
    glGenTextures(num, texs);
    float color[4] = {0, 0, 0, 0};
    for (int i = 0; i < num; i++)
    {
        glBindTexture(GL_TEXTURE_2D, tex[i].id = texs[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }
    tex_unbind();
}
void tex_del(tex_t tex, int num)
{
    uint texs[num];
    for (int i = 0; i < num; i++)
        texs[i] = tex[i].id;
    glDeleteTextures(num, texs);
}

void tex_load(tex_t tex, void *data, int w, int h, GLenum iformat, GLenum format)
{
    if (!tex)
        return;
    tex_bind(*tex);
    glTexImage2D(GL_TEXTURE_2D, 0, tex->format = iformat, tex->w = w, tex->h = h, 0, format, GL_UNSIGNED_BYTE, data);
    tex_unbind();
}
void tex_loadf(tex_t tex, void *data, int w, int h, GLenum iformat, GLenum format)
{
    if (!tex)
        return;
    glEnable(GL_TEXTURE_2D);
    tex_bind(*tex);
    glTexImage2D(GL_TEXTURE_2D, 0, tex->format = iformat, tex->w = w, tex->h = h, 0, format, GL_FLOAT, data);
    tex_unbind();
}
