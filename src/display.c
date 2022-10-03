

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XShm.h>

#include "basic.h"

// xlib上下文
Display *display;
int screen_id;
Screen *screen;
int screen_w, screen_h;
XVisualInfo vinfo;
XSetWindowAttributes window_attrs;
Window window, root_window;
XShmSegmentInfo xshmInfo;
XImage *image;

// opengl上下文
#define tex_num 8
GLXContext glx_ctx;
uint tex[tex_num];

// GLXContext glx_root_ctx;

uint gl_tex_to_window;
uint gl_color_to_window;

// 还原gl状态
void gl_reset()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_COLOR_LOGIC_OP);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
    glDrawBuffer(GL_FRONT);
    glClearColor(0, 0, 0, 0);
    glLoadIdentity();
    glOrtho(0, screen_w, screen_h, 0, 0, 1);
}

// 初始化显示
void display_init()
{
    // 连接 x 服务器
    assert(display = XOpenDisplay(NULL));
    screen_id = XDefaultScreen(display);
    assert(screen = ScreenOfDisplay(display, screen_id));
    screen_w = screen->width;
    screen_h = screen->height;
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
    float color[4] = {0, 0, 0, 0};
    glEnable(GL_TEXTURE_2D);
    glGenTextures(tex_num, tex);
    for (int i = 0; i < tex_num; i++)
    {
        glBindTexture(GL_TEXTURE_2D, tex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }
    //
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

// 复制屏幕到图片
void screen2image(uint di)
{
    // XUnmapWindow(display, window);
    XShmGetImage(display, root_window, image, 0, 0, AllPlanes);
    // XMapWindow(display, window);
    // 加载到纹理
    glBindTexture(GL_TEXTURE_2D, tex[di % tex_num]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_w, screen_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->data);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
}
void s2ix(uint di) { screen2image(di); }
void s2i(uint di) { screen2image(di); }

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

// 图片到窗口
void image2window(uint si,
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
    glBindTexture(GL_TEXTURE_2D, tex[si]);
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
void i2wx(uint si,
          int dx, int dy, int dw, int dh,
          byte r, byte g, byte b, byte a) { image2window(si,
                                                         0, 0, 0, 0,
                                                         dx, dy, dw, dh,
                                                         r, g, b, a,
                                                         0); }
void i2wcx(uint si, byte r, byte g, byte b, byte a, uint op)
{
    if (op)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(op);
    }

    glColor4f(r / 255., g / 255., b / 255., a / 255.);
    glBindTexture(GL_TEXTURE_2D, tex[si]);
    glCallList(gl_tex_to_window);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);

    if (op)
        glDisable(GL_COLOR_LOGIC_OP);
}
void i2wca(uint si, byte r, byte g, byte b, byte a) { i2wcx(si, r, g, b, a, 0); }
void i2wc(uint si, byte r, byte g, byte b) { i2wcx(si, r, g, b, 255, 0); }
void i2wpx(uint si,
           int sx, int sy, int sw, int sh,
           int dx, int dy, int dw, int dh) { image2window(si,
                                                          sx, sy, sw, sh,
                                                          dx, dy, dw, dh,
                                                          255, 255, 255, 255,
                                                          0); }
void i2wp(uint si, int dx, int dy, int dw, int dh) { i2wpx(si, 0, 0, 0, 0, dx, dy, dw, dh); }
void i2w(uint si) { i2wcx(si, 255, 255, 255, 255, 0); }

// 屏幕到窗口
void screen2window(uint ii,
                   int sx, int sy, int sw, int sh,
                   int dx, int dy, int dw, int dh,
                   byte r, byte g, byte b, byte a,
                   uint op)
{
    s2i(ii);
    image2window(ii,
                 sx, sy, sw, sh,
                 dx, dy, dw, dh,
                 r, g, b, a,
                 op);
}
void s2wx(uint ii,
          int dx, int dy, int dw, int dh,
          byte r, byte g, byte b, byte a)
{
    s2i(ii);
    i2wx(ii,
         dx, dy, dw, dh,
         r, g, b, a);
}
void s2wpx(uint ii,
           int sx, int sy, int sw, int sh,
           int dx, int dy, int dw, int dh)
{
    s2i(ii);
    i2wpx(ii,
          sx, sy, sw, sh,
          dx, dy, dw, dh);
}
void s2wp(uint ii, int dx, int dy, int dw, int dh) { s2wpx(ii, 0, 0, 0, 0, dx, dy, dw, dh); }
void s2wcx(uint ii, byte r, byte g, byte b, byte a, uint op)
{
    s2i(ii);
    i2wcx(ii, r, g, b, a, op);
}
void s2wca(uint ii, byte r, byte g, byte b, byte a) { s2wcx(ii, r, g, b, a, 0); }
void s2wc(uint ii, byte r, byte g, byte b) { s2wcx(ii, r, g, b, 255, 0); }
void s2w(uint ii) { s2wcx(ii, 255, 255, 255, 255, 0); }
void s2w0() { s2w(0); }

// 特效部分

void do_Payload(void (*init)(), void (*func)(int))
{
    s2w0();

    if (init)
        init();

    time_t starttime = time(NULL);
    for (int i = 0; time(NULL) - starttime < def_play_time; i++)
    {
        func(i);

        glFlush(); // 刷新
    }
    // 还原gl状态
    gl_reset();
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    usleep(100e3);
}

void Payload1(int idx)
{
    usleep(15e3);
    s2w(0);
    c2wx(idx % 256, (idx / 2) % 256, (idx / 2), 0, GL_XOR);
}

void Payload2(int idx)
{
    usleep(15e3);
    s2wpx(0,
          (idx * 10) % screen_w, (idx * 10) % screen_h, 0, 0,
          0, 0, 0, 0);
    color2window((idx * 10) % screen_w, (idx * 10) % screen_h, 0, 0,
                 rand() % 256, rand() % 256, rand() % 256, 0, GL_XOR);
}

void Payload3(int idx)
{
    usleep(100e3);
    s2i(0);
    for (int i = 0; i <= screen_w / 10; i++)
        for (int j = 0; j <= screen_h / 10; j++)
        {
            i2wpx(0,
                  i * 10, j * 10, 1, 1,
                  i * 10, j * 10, 10, 10);
        }
}

void Payload4(int idx)
{
    usleep(20e3);
    s2wx(0,
         idx % 200 + 10, -idx % 25, 0, 0,
         255, 255, 255, 128);
    alpha2window(idx % 200 + 10, -idx % 25, 0, 0, 255);
}

void Payload5(int idx)
{
    idx *= 3;
    int x = rand() % screen_w, y = rand() % screen_h;
    int w = idx % screen_w, h = idx % screen_h;
    screen2window(0,
                  x, y, w, h,
                  (x + idx / 2) % screen_w, y % screen_h, w, h,
                  255, 255, 255, 255, 0);
}

void Payload6_init()
{
    glTranslatef(screen_w / 2., screen_h / 2., 0);
    glRotatef(1, 0, 0, 1);
    glTranslatef(-screen_w / 2., -screen_h / 2., 0);
}
void Payload6(int idx)
{
    usleep(50e3);
    s2wca(0, 255, 255, 255, 128);
    a2w(255);
}

void Payload7(int idx)
{
    usleep(50e3);
    glPushMatrix();
    glTranslatef(screen_w / 2., screen_h / 2., 0);
    glRotatef((4 - idx * 131 % 9) / 5., 0, 0, 1);
    glTranslatef(-screen_w / 2., -screen_h / 2., 0);
    // 文本输出有点麻烦, 暂时不搞
    // SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
    // SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
    // TextOut(hdcScreen, random() % screen_w, random() % screen_h, L"HYDROGEN", 8);
    s2wca(0, 255, 255, 255, 128);
    glPopMatrix();
    a2w(255);
}

void Payload8_init()
{
    float mView[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mView);
    mView[1] = mView[5] * .02;
    glLoadMatrixf(mView);
}
void Payload8(int idx)
{
    usleep(50e3);
    // SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
    // SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
    // for (int i = 0; i < 5; i++)
    //     TextOut(hdcScreen, random() % screen_w, random() % screen_h, L"ӉӬլҏ ӎӬ !!!", 11);
    s2wca(0, 255, 255, 255, 128);
    a2w(255);
}

void Payload9_init()
{
    float mView[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mView);
    mView[4] = mView[0] * .02;
    mView[12] += mView[4] * -(screen_h / 2);
    glLoadMatrixf(mView);
}
void Payload9(int idx)
{
    usleep(50e3);
    s2wca(0, 255, 255, 255, 128);
    a2w(255);
    c2wx(rand() % 256, rand() % 256, rand() % 256, 0, GL_XOR);
}

#if 0

void Payload10(int idx)
{
    POINT ptScreen = GetVirtualScreenPos();
    SIZE szScreen = GetVirtualScreenSize();

    idx *= 30;

    RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);

    HDC hcdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screen_w, screen_h);
    SelectObject(hcdc, hBitmap);
    Sleep(50);
    BitBlt(hcdc, 0, 0, screen_w, screen_h, hdcScreen, 0, 0, NOTSRCCOPY);
    SelectObject(hdcScreen, CreatePatternBrush(hBitmap));

    Ellipse(hdcScreen, idx % screen_w + 20, idx % screen_h + 20, idx % screen_w + idx % 101 + 180, idx % screen_h + idx % 101 + 180);
    BitBlt(hcdc, 0, 0, screen_w, screen_h, hdcScreen, 0, 0, NOTSRCCOPY);
    SelectObject(hdcScreen, CreatePatternBrush(hBitmap));
    Ellipse(hdcScreen, idx % screen_w + 10, idx % screen_h + 10, idx % screen_w + idx % 101 + 190, idx % screen_h + idx % 101 + 190);
    Ellipse(hdcScreen, idx % screen_w, idx % screen_h, idx % screen_w + idx % 101 + 200, idx % screen_h + idx % 101 + 200);
    BitBlt(hcdc, 0, 0, screen_w, screen_h, hdcScreen, 0, 0, NOTSRCCOPY);
    SelectObject(hdcScreen, CreatePatternBrush(hBitmap));
    Ellipse(hdcScreen, idx % screen_w, idx % screen_h, idx % screen_w + idx % 101 + 200, idx % screen_h + idx % 101 + 200);

    SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
    SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));

    for (int i = 0; i < 5; i++)
    {
        TextOut(hdcScreen, random() % screen_w, random() % screen_h, L"     ", 5);
    }

    DeleteObject(hcdc);
    DeleteObject(hBitmap);
}
#endif
