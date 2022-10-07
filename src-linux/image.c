

#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>

#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>

#include "image.h"

int _img_format_size[] = {
    0,
    1,
    1,
    1,
    1,
    1,
    2,
    3,
    4,
    3,
    4,
};
GLenum _img_format_to_gl[] = {
    GL_NONE,
    GL_LUMINANCE,
    GL_RED,
    GL_GREEN,
    GL_BLUE,
    GL_ALPHA,
    GL_LUMINANCE_ALPHA,
    GL_RGB,
    GL_RGBA,
    GL_BGR,
    GL_BGRA,
};

img_t img_new(int format, int w, int h)
{
    if (format <= img_format_null || format >= img_format_count)
        return NULL;
    img_t img = calloc(1, sizeof(_img_t));
    img->w = w;
    img->h = h;
    img->format = format;
    img->data = calloc(w * h, _img_format_size[format]);
    return img;
}
void img_free(img_t img)
{
    if (!img)
        return;
    free(img->data);
}

img_t img_load(char *path)
{
    if (!path)
        return NULL;

    int w, h, channel;
    void *data = stbi_load(path, &w, &h, &channel, 0);
    if (!data)
        return NULL;

    img_t img = calloc(1, sizeof(_img_t));
    img->w = w;
    img->h = h;
    img->format = channel == 1 ? img_format_L : (channel == 3 ? img_format_RGB : img_format_RGBA);
    img->data = data;

    return img;
}
int img_save_bmp(img_t img, char *path)
{
    if (!img)
        return -1;

    return stbi_write_bmp(path, img->w, img->h, _img_format_size[img->format], img->data) ? 0 : -1;
}
int img_save_jpg(img_t img, char *path, int quality)
{
    if (!img)
        return -1;

    return stbi_write_jpg(path, img->w, img->h, _img_format_size[img->format],
                          img->data, quality)
               ? 0
               : -1;
}
int img_save_png(img_t img, char *path)
{
    if (!img)
        return -1;

    return stbi_write_png(path, img->w, img->h, _img_format_size[img->format],
                          img->data, img->w * _img_format_size[img->format])
               ? 0
               : -1;
}
int img_save_hdr(img_t img, char *path)
{
    if (!img)
        return -1;

    return stbi_write_hdr(path, img->w, img->h, _img_format_size[img->format], img->data) ? 0 : -1;
}
int img_save_tga(img_t img, char *path)
{
    if (!img)
        return -1;

    return stbi_write_tga(path, img->w, img->h, _img_format_size[img->format], img->data) ? 0 : -1;
}

img_t img_resize(img_t img, int w, int h)
{
    if (!img)
        return NULL;
    img_t new_img = img_new(img->format, w, h);
    if (!new_img)
        return NULL;
    stbir_resize_uint8(img->data, img->w, img->h, img->w * _img_format_size[img->format],
                       new_img->data, w, h, w * _img_format_size[img->format],
                       _img_format_size[img->format]);
    return new_img;
}

img_t img_copy(img_t img, int format)
{
    if (!img)
        return NULL;
    if (format <= img_format_null || format >= img_format_count)
        return NULL;
    img_t new_img = img_new(format, img->w, img->h);
    if (!new_img)
        return NULL;
    int channel = _img_format_size[img->format];
    int channel_new = _img_format_size[format];
    int len = img->w * img->h;
    if (channel == 1 && channel_new == 1)
        memcpy(new_img->data, img->data, len);
    else if (img->format == img_format_LA)
    {
        if (channel_new == 1)
            for (int i = 0; i < len; i++)
            {
                ((unsigned char *)new_img->data)[i] =
                    ((unsigned char *)img->data)[i * 2] *
                    (int)((unsigned char *)img->data)[i * 2 + 1] / 255;
            }
        else if (new_img->format == img_format_LA)
            memcpy(new_img->data, img->data, len * 2);
        else if (channel_new == 3)
            for (int i = 0; i < len; i++)
            {
                ((unsigned char *)new_img->data)[i * 3] =
                    ((unsigned char *)new_img->data)[i * 3 + 1] =
                        ((unsigned char *)new_img->data)[i * 3 + 2] =
                            ((unsigned char *)img->data)[i * 2] *
                            (int)((unsigned char *)img->data)[i * 2 + 1] / 255;
            }
        else
            for (int i = 0; i < len; i++)
            {
                ((unsigned char *)new_img->data)[i * 4] =
                    ((unsigned char *)new_img->data)[i * 4 + 1] =
                        ((unsigned char *)new_img->data)[i * 4 + 2] =
                            ((unsigned char *)img->data)[i * 2];
                ((unsigned char *)new_img->data)[i * 4 + 3] =
                    ((unsigned char *)img->data)[i * 2 + 1];
            }
    }
    else if (0)
    {
    }
    return new_img;
}
