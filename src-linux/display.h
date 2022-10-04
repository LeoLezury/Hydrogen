

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
extern int screen_w, screen_h;
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

void screen2image(uint di);
#define s2ix screen2image
#define s2i screen2image

void screen2texture(uint di);
#define s2tx screen2texture
#define s2t screen2texture

void image2texture(uint si, uint di);
#define i2tx image2texture
#define i2t image2texture

void color2window(int dx, int dy, int dw, int dh,
                  byte r, byte g, byte b, byte a,
                  uint op);
#define c2wxx color2window
void c2wx(byte r, byte g, byte b, byte a, uint op);
void c2wa(byte r, byte g, byte b, byte a);
void c2w(byte r, byte g, byte b);

void alpha2window(int dx, int dy, int dw, int dh, byte a);
#define a2wxx alpha2window
#define a2wx alpha2window
void a2w(byte a);

void texture2window(uint si,
                    int sx, int sy, int sw, int sh,
                    int dx, int dy, int dw, int dh,
                    byte r, byte g, byte b, byte a,
                    uint op);
#define t2wxx texture2window
void t2wx(uint si,
          int dx, int dy, int dw, int dh,
          byte r, byte g, byte b, byte a);
void t2wcx(uint si, byte r, byte g, byte b, byte a, uint op);
void t2wca(uint si, byte r, byte g, byte b, byte a);
void t2wc(uint si, byte r, byte g, byte b);
void t2wpx(uint si,
           int sx, int sy, int sw, int sh,
           int dx, int dy, int dw, int dh);
void t2wp(uint si, int dx, int dy, int dw, int dh);
void t2w(uint si);

void i2w(uint si, uint ii);

void screen2window(uint ii,
                   int sx, int sy, int sw, int sh,
                   int dx, int dy, int dw, int dh,
                   byte r, byte g, byte b, byte a,
                   uint op);
#define s2wxx screen2window
void s2wx(uint ii,
          int dx, int dy, int dw, int dh,
          byte r, byte g, byte b, byte a);
void s2wpx(uint ii,
           int sx, int sy, int sw, int sh,
           int dx, int dy, int dw, int dh);
void s2wp(uint ii, int dx, int dy, int dw, int dh);
void s2wcx(uint ii, byte r, byte g, byte b, byte a, uint op);
void s2wca(uint ii, byte r, byte g, byte b, byte a);
void s2wc(uint ii, byte r, byte g, byte b);
void s2w(uint ii);
#define s2w0() s2w(0)

extern void exec_effect(effect_init_t init, effect_t func, float time);

extern void *display_thread(struct effect_data *data);

extern pthread_t start_display_thread(struct effect_data *data);
