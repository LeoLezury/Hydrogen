

#pragma once

#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#include "basic.h"

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
extern uint tex[];

extern uint gl_tex_to_window;
extern uint gl_color_to_window;

extern void gl_reset();

#define c2wxx color2window
#define i2wxx image2window
#define s2wxx screen2window

void do_Payload(void (*init)(), void (*func)(int));

extern void Payload1(int idx);
extern void Payload2(int idx);
extern void Payload3(int idx);
extern void Payload4(int idx);
extern void Payload5(int idx);
extern void Payload6_init();
extern void Payload6(int idx);
extern void Payload7(int idx);
extern void Payload8_init();
extern void Payload8(int idx);
void Payload9_init();
extern void Payload9(int idx);
extern void Payload10(int idx);
