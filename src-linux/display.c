

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XShm.h>

#include "display.h"

// xlib上下文
Display *display;
int screen_id;
Screen *screen;
int screen_w, screen_h, screen_size;
XVisualInfo vinfo;
XSetWindowAttributes window_attrs;
Window window, root_window;
XShmSegmentInfo xshmInfo;
XImage *image;

// opengl上下文
GLXContext glx_ctx;
_tex_t tex[tex_num];
uint gl_tex_to_window;
uint gl_color_to_window;

// 图像储存
img_t img[img_num];

// 还原gl状态
void gl_reset()
{
    // 启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 关闭(颜色混合的)逻辑操作
    glDisable(GL_COLOR_LOGIC_OP);
    // 启用纹理
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
    // 直接绘制到表面 (这样比较快)
    glDrawBuffer(GL_FRONT);
    //
    glClearColor(0, 0, 0, 0);
    // 重设变换矩阵
    glLoadIdentity();
    glOrtho(0, screen_w, screen_h, 0, 0, 1);
}

// 初始化显示
void display_init()
{
    // 连接 xserver
    assert(display = XOpenDisplay(NULL));
    screen_id = XDefaultScreen(display);
    assert(screen = ScreenOfDisplay(display, screen_id));
    screen_w = screen->width;
    screen_h = screen->height;
    screen_size = screen_w * screen_h;
    root_window = DefaultRootWindow(display);

    assert(XMatchVisualInfo(display, screen_id, 32, TrueColor, &vinfo), "不支持真彩色");

    // 创建共享图像缓冲区
    image = XShmCreateImage(display, vinfo.visual, 24, ZPixmap, NULL, &xshmInfo, screen_w, screen_h);
    xshmInfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777);
    xshmInfo.shmaddr = image->data = shmat(xshmInfo.shmid, 0, 0);
    xshmInfo.readOnly = 0;
    XShmAttach(display, &xshmInfo);

    window_attrs.override_redirect = 1;
    window_attrs.colormap = XCreateColormap(display, root_window, vinfo.visual, AllocNone); // 创建32位调色板

    // 创建窗口
    window = XCreateWindow(
        display, root_window,
        0, 0, screen_w, screen_h, 0,
        vinfo.depth, InputOutput, vinfo.visual,
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel,
        &window_attrs);
    assert(window, "窗口创建失败");
    XMapWindow(display, window);

    // 窗口不捕获输入
    XRectangle rect = {};
    XserverRegion region = XFixesCreateRegion(display, &rect, 1);
    XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(display, region);

    // 创建 opengl 上下文
    assert(glx_ctx = glXCreateContext(display, &vinfo, NULL, 1), "创建opengl上下文失败");
    assert(glXMakeCurrent(display, window, glx_ctx), "设置当前opengl上下文失败");

    // 创建纹理
    tex_gen(tex, tex_num);

    // 设置参数
    gl_reset();

    // 预编译显示列表
    gl_tex_to_window = glGenLists(2);
    gl_color_to_window = gl_tex_to_window + 1;
    // 从纹理绘制到窗口
    glNewList(gl_tex_to_window, GL_COMPILE);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(0, 1);
    glVertex2f(0, screen_h);
    glTexCoord2f(1, 1);
    glVertex2f(screen_w, screen_h);
    glTexCoord2f(1, 0);
    glVertex2f(screen_w, 0);
    glEnd();
    glEndList();
    // 将颜色绘制到窗口
    glNewList(gl_color_to_window, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, screen_h);
    glVertex2f(screen_w, screen_h);
    glVertex2f(screen_w, 0);
    glEnd();
    glEndList();
}
// 去初始化显示
void display_deinit()
{
    for (int i = 0; i < img_num; i++)
        if (img[i])
        {
            img_free(img[i]);
            img[i] = NULL;
        }

    // 删除显示列表
    glDeleteLists(gl_tex_to_window, 2);

    // 删除纹理
    tex_del(tex, tex_num);

    // 关闭 opengl
    glXMakeCurrent(display, 0, NULL);
    glXDestroyContext(display, glx_ctx);

    // 关闭窗口
    XDestroyWindow(display, window);

    // 关闭 xserver 连接
    XCloseDisplay(display);
}

// 图片操作
void createimage(uint di, int w, int h)
{
    di = di % img_num;
    if (img[di])
        img_free(img[di]);
    img[di] = img_new(img_format_RGBA, w, h);
}
void loadimage(uint di, char *path)
{
    di = di % img_num;
    if (img[di])
        img_free(img[di]);
    img[di] = img_load(path);
}
void saveimage(uint si, char *path) { img_save_png(img[si % img_num], path); }
void freeimage(uint di)
{
    di = di % img_num;
    img_free(img[di]);
    img[di] = NULL;
}

// 屏幕到图片
void screen2image(uint di)
{
    di = di % img_num;
    if (img[di])
        if (img[di]->w * img[di]->h * _img_format_size[img[di]->format] < screen_size * 4)
        {
            img_free(img[di]);
            img[di] = NULL;
        }
        else
        {
            img[di]->w = screen_w;
            img[di]->h = screen_h;
            img[di]->format = img_format_BGRA;
        }
    if (!img[di])
        img[di] = img_new(img_format_BGRA, screen_w, screen_h);

    XShmGetImage(display, root_window, image, 0, 0, AllPlanes);
    memcpy(img[di]->data, image->data, screen_size * 4);
}

// 屏幕到纹理
void screen2texture(uint di)
{
    // XUnmapWindow(display, window);
    XShmGetImage(display, root_window, image, 0, 0, AllPlanes);
    // XMapWindow(display, window);
    // 加载到纹理
    tex_load(tex + di % tex_num, image->data, screen_w, screen_h, GL_RGBA, GL_BGRA);
}

// 图片到纹理
void image2texture(uint si, uint di)
{
    if (img[si])
        tex_load(tex + di % tex_num, img[si]->data, img[si]->w, img[si]->h, GL_RGBA, _img_format_to_gl[img[si]->format]);
}

// 颜色到窗口
void color2window(int dx, int dy, int dw, int dh,
                  byte r, byte g, byte b, byte a,
                  uint op)
{
    if (op)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(op);
    }
    if (dw <= 0)
        dw = screen_w;
    if (dh <= 0)
        dh = screen_h;
    glColor4f(r / 255., g / 255., b / 255., a / 255.);

    glBegin(GL_QUADS);
    glVertex2d(dx, dy);
    glVertex2d(dx, dy + dh);
    glVertex2d(dx + dw, dy + dh);
    glVertex2d(dx + dw, dy);
    glEnd();

    if (op)
        glDisable(GL_COLOR_LOGIC_OP);
}
void c2wx(byte r, byte g, byte b, byte a, uint op)
{
    if (op)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(op);
    }
    glColor4f(r / 255., g / 255., b / 255., a / 255.);

    glCallList(gl_color_to_window);

    if (op)
        glDisable(GL_COLOR_LOGIC_OP);
}
void c2wa(byte r, byte g, byte b, byte a) { c2wx(r, g, b, a, 0); }
void c2w(byte r, byte g, byte b) { c2wx(r, g, b, 255, 0); }

// 透明度到窗口
void alpha2window(int dx, int dy, int dw, int dh, byte a)
{
    if (dw <= 0)
        dw = screen_w;
    if (dh <= 0)
        dh = screen_h;
    glColor4f(0, 0, 0, a / 255.);
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);

    glBegin(GL_QUADS);
    glVertex2d(dx, dy);
    glVertex2d(dx, dy + dh);
    glVertex2d(dx + dw, dy + dh);
    glVertex2d(dx + dw, dy);
    glEnd();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void a2w(byte a)
{
    glColor4f(0, 0, 0, a / 255.);
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);
    glCallList(gl_color_to_window);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// 纹理到窗口
void texture2window(uint si,
                    int sx, int sy, int sw, int sh,
                    int dx, int dy, int dw, int dh,
                    byte r, byte g, byte b, byte a,
                    uint op)
{
    if (op)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(op);
    }
    if (sw <= 0)
        sw = screen_w;
    if (sh <= 0)
        sh = screen_h;
    if (dw <= 0)
        dw = screen_w;
    if (dh <= 0)
        dh = screen_h;

    glColor4f(r / 255., g / 255., b / 255., a / 255.);
    glBindTexture(GL_TEXTURE_2D, tex[si].id);
    glBegin(GL_QUADS);
    glTexCoord2f(sx / (float)screen_w, sy / (float)screen_h);
    glVertex2f(dx, dy);
    glTexCoord2f(sx / (float)screen_w, (sy + sh) / (float)screen_h);
    glVertex2f(dx, dy + dh);
    glTexCoord2f((sx + sw) / (float)screen_w, (sy + sh) / (float)screen_h);
    glVertex2f(dx + dw, dy + dh);
    glTexCoord2f((sx + sw) / (float)screen_w, sy / (float)screen_h);
    glVertex2f(dx + dw, dy);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);

    if (op)
        glDisable(GL_COLOR_LOGIC_OP);
}
void t2wx(uint si,
          int dx, int dy, int dw, int dh,
          byte r, byte g, byte b, byte a) { texture2window(si,
                                                           0, 0, 0, 0,
                                                           dx, dy, dw, dh,
                                                           r, g, b, a,
                                                           0); }
void t2wcx(uint si, byte r, byte g, byte b, byte a, uint op)
{
    if (op)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(op);
    }

    glColor4f(r / 255., g / 255., b / 255., a / 255.);
    glBindTexture(GL_TEXTURE_2D, tex[si].id);
    glCallList(gl_tex_to_window);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);

    if (op)
        glDisable(GL_COLOR_LOGIC_OP);
}
void t2wca(uint si, byte r, byte g, byte b, byte a) { t2wcx(si, r, g, b, a, 0); }
void t2wc(uint si, byte r, byte g, byte b) { t2wcx(si, r, g, b, 255, 0); }
void t2wpx(uint si,
           int sx, int sy, int sw, int sh,
           int dx, int dy, int dw, int dh) { texture2window(si,
                                                            sx, sy, sw, sh,
                                                            dx, dy, dw, dh,
                                                            255, 255, 255, 255,
                                                            0); }
void t2wp(uint si, int dx, int dy, int dw, int dh) { t2wpx(si, 0, 0, 0, 0, dx, dy, dw, dh); }
void t2w(uint si) { t2wcx(si, 255, 255, 255, 255, 0); }

void i2w(uint si, uint ii)
{
    i2t(si, ii);
    t2w(ii);
}

// 屏幕到窗口
void screen2window(uint ii,
                   int sx, int sy, int sw, int sh,
                   int dx, int dy, int dw, int dh,
                   byte r, byte g, byte b, byte a,
                   uint op)
{
    s2t(ii);
    texture2window(ii,
                   sx, sy, sw, sh,
                   dx, dy, dw, dh,
                   r, g, b, a,
                   op);
}
void s2wx(uint ii,
          int dx, int dy, int dw, int dh,
          byte r, byte g, byte b, byte a)
{
    s2t(ii);
    t2wx(ii,
         dx, dy, dw, dh,
         r, g, b, a);
}
void s2wpx(uint ii,
           int sx, int sy, int sw, int sh,
           int dx, int dy, int dw, int dh)
{
    s2t(ii);
    t2wpx(ii,
          sx, sy, sw, sh,
          dx, dy, dw, dh);
}
void s2wp(uint ii, int dx, int dy, int dw, int dh) { s2wpx(ii, 0, 0, 0, 0, dx, dy, dw, dh); }
void s2wcx(uint ii, byte r, byte g, byte b, byte a, uint op)
{
    s2t(ii);
    t2wcx(ii, r, g, b, a, op);
}
void s2wca(uint ii, byte r, byte g, byte b, byte a) { s2wcx(ii, r, g, b, a, 0); }
void s2wc(uint ii, byte r, byte g, byte b) { s2wcx(ii, r, g, b, 255, 0); }
void s2w(uint ii) { s2wcx(ii, 255, 255, 255, 255, 0); }

void exec_effect(effect_init_t init, effect_t func, float time)
{
    s2w0();
    // XRaiseWindow(display, window);
    if (init)
        init();
    double endtime = gettimed() + time;
    for (int i = 0; gettimed() < endtime; i++)
    {
        func(i);
        glFlush();
    }
    gl_reset();
    // XLowerWindow(display, window);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    usleep(100e3);
}

void *display_thread(struct effect_data *data)
{
    // pthread_detach(pthread_self());

    display_init();

    effect_t func;
play:
    for (int i = 0; func = data[i].func; i++)
    {
        s2w0();
        if (data[i].init)
            data[i].init();
        if (!(data[i].time > 0))
            goto loop;
        float starttime = gettime() / 1e9;
        for (int i = 0; gettime() / 1e9 - starttime < data[i].time; i++)
        {
            func(i);
            glFlush();
        }
        gl_reset();
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        usleep(100e3);
    }
    goto play;
    goto end;

loop:
    for (int i = 0;; i++)
    {
        func(i);
        glFlush();
    }

end:
    display_deinit();
}
pthread_t start_display_thread(struct effect_data *data)
{
    pthread_t t;
    pthread_create(&t, NULL, (void *(*)(void *))display_thread, data);
    return t;
}
