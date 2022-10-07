

#pragma once

#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#include "basic.h"
#include "image.h"
#include "texture.h"

#define img_num 8
#define tex_num 8

typedef void (*effect_init_t)();
typedef void (*effect_t)(int);

struct effect_data
{
    effect_init_t init;
    effect_t func;
    float time;
};

// xlib上下文
extern Display *display;
extern int screen_id;
extern Screen *screen;
extern int screen_w, screen_h, screen_size;
extern XVisualInfo vinfo;
extern XSetWindowAttributes window_attrs;
extern Window window, root_window;
extern XShmSegmentInfo xshmInfo;
extern XImage *image;

// opengl上下文
extern GLXContext glx_ctx;
extern _tex_t tex[];
extern uint gl_tex_to_window;
extern uint gl_color_to_window;

extern img_t img[];

extern void gl_reset();

extern void display_init();
extern void display_deinit();

#define randx (rand() % screen_w)
#define randy (rand() % screen_h)
#define randxy (rand() % screen_size)

// 图片操作
extern void createimage(uint di, int w, int h);
extern void loadimage(uint di, char *path);
extern void saveimage(uint si, char *path);
extern void freeimage(uint di);
#define imagew(i) (img[i % img_num] ? img[i % img_num]->w : 0)
#define imageh(i) (img[i % img_num] ? img[i % img_num]->h : 0)

// 屏幕到图片
extern void screen2image(uint di);
#define s2ix screen2image
#define s2i screen2image

// 屏幕到纹理
extern void screen2texture(uint di);
#define s2tx screen2texture
#define s2t screen2texture

// 图片到纹理
extern void image2texture(uint si, uint di);
#define i2tx image2texture
#define i2t image2texture

extern void color2window(int dx, int dy, int dw, int dh,
                         byte r, byte g, byte b, byte a,
                         uint op);
#define c2wxx color2window
extern void c2wx(byte r, byte g, byte b, byte a, uint op);
extern void c2wa(byte r, byte g, byte b, byte a);
extern void c2w(byte r, byte g, byte b);

extern void alpha2window(int dx, int dy, int dw, int dh, byte a);
#define a2wxx alpha2window
#define a2wx alpha2window
extern void a2w(byte a);

extern void texture2window(uint si,
                           int sx, int sy, int sw, int sh,
                           int dx, int dy, int dw, int dh,
                           byte r, byte g, byte b, byte a,
                           uint op);
#define t2wxx texture2window
extern void t2wx(uint si,
                 int dx, int dy, int dw, int dh,
                 byte r, byte g, byte b, byte a);
extern void t2wcx(uint si, byte r, byte g, byte b, byte a, uint op);
extern void t2wca(uint si, byte r, byte g, byte b, byte a);
extern void t2wc(uint si, byte r, byte g, byte b);
extern void t2wpx(uint si,
                  int sx, int sy, int sw, int sh,
                  int dx, int dy, int dw, int dh);
extern void t2wp(uint si, int dx, int dy, int dw, int dh);
extern void t2w(uint si);

extern void i2w(uint si, uint ii);

extern void screen2window(uint ii,
                          int sx, int sy, int sw, int sh,
                          int dx, int dy, int dw, int dh,
                          byte r, byte g, byte b, byte a,
                          uint op);
#define s2wxx screen2window
extern void s2wx(uint ii,
                 int dx, int dy, int dw, int dh,
                 byte r, byte g, byte b, byte a);
extern void s2wpx(uint ii,
                  int sx, int sy, int sw, int sh,
                  int dx, int dy, int dw, int dh);
extern void s2wp(uint ii, int dx, int dy, int dw, int dh);
extern void s2wcx(uint ii, byte r, byte g, byte b, byte a, uint op);
extern void s2wca(uint ii, byte r, byte g, byte b, byte a);
extern void s2wc(uint ii, byte r, byte g, byte b);
extern void s2w(uint ii);
#define s2w0() s2w(0)

extern void exec_effect(effect_init_t init, effect_t func, float time);

extern void *display_thread(struct effect_data *data);

extern pthread_t start_display_thread(struct effect_data *data);
