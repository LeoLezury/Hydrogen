

#pragma once

#include <stdlib.h>
#include <GL/gl.h>

#include "basic.h"

typedef struct img _img_t, *img_t;
struct img
{
    int format;
    int w, h;
    void *data;
};

enum
{
    img_format_null,
    img_format_L,
    img_format_R,
    img_format_G,
    img_format_B,
    img_format_A,
    img_format_LA,
    img_format_RGB,
    img_format_RGBA,
    img_format_BGR,
    img_format_BGRA,
    img_format_count,
};

extern int _img_format_size[];
extern GLenum _img_format_to_gl[];

extern img_t img_new(int format, int w, int h)
    __THROW __attribute_malloc__ __wur;
extern void img_free(img_t img)
    __THROW;

extern img_t img_load(char *path)
    __THROW __attribute_malloc__ __wur;
extern int img_save_bmp(img_t img, char *path)
    __THROW;
extern int img_save_jpg(img_t img, char *path, int quality)
    __THROW;
extern int img_save_png(img_t img, char *path)
    __THROW;
extern int img_save_hdr(img_t img, char *path)
    __THROW;
extern int img_save_tga(img_t img, char *path)
    __THROW;

extern img_t img_resize(img_t img, int w, int h)
    __THROW __attribute_malloc__ __wur;

// img_t img_copy(img_t img, int format);
