

#include <stdlib.h>
#include <pthread.h>
#include <time.h>
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
#include "audio.h"
#include "display.c"

#if 0

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
GLXContext glx_ctx;
uint tex;

uint gl_tex_to_window;
uint gl_color_to_window;

void screen2image()
{
    XUnmapWindow(display, window);
    XShmGetImage(display, root_window, image, 0, 0, AllPlanes);
    XMapWindow(display, window);
}

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
}

void do_Payload(void (*func)(int))
{
    time_t starttime = time(NULL);
    for (int i = 0; time(NULL) - starttime < PAYLOAD_TIME; i++)
    {
        func(i);
        glFlush(); // 刷新
    }
    usleep(100e3);

    // 还原gl状态
    gl_reset();
}

void Payload1(int i)
{
    usleep(15e3);
    // 桌面截图
    screen2image();
    // 加载到纹理
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_w, screen_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->data);
    // 绘制
    glDisable(GL_COLOR_LOGIC_OP);
    glCallList(gl_tex_to_window);

    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glColor4f((i % 256) / 255., ((i / 2) % 256) / 255., ((i / 2) % 256) / 255., 0);
    glCallList(gl_color_to_window);
}

void init()
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
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    gl_reset();

    // 预编译显示列表
    gl_tex_to_window = glGenLists(2);
    gl_color_to_window = gl_tex_to_window + 1;

    glNewList(gl_tex_to_window, GL_COMPILE);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(-1, -1);
    glTexCoord2f(0, 0);
    glVertex2f(-1, 1);
    glTexCoord2f(1, 0);
    glVertex2f(1, 1);
    glTexCoord2f(1, 1);
    glVertex2f(1, -1);
    glEnd();
    glEndList();

    glNewList(gl_color_to_window, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
    glBegin(GL_QUADS);
    glVertex2d(-1, -1);
    glVertex2d(-1, 1);
    glVertex2d(1, 1);
    glVertex2d(1, -1);
    glEnd();
    glEndList();
}

#endif

int main(int argc, char **argv)
{

    // SeedXorshift32((DWORD)__rdtsc());

    init();

    pthread_t t;
    // pthread_create(&t, NULL, audio_thread, pcm_gen_funcs);

    // do_Payload(Payload1_init, Payload1);
    // do_Payload(NULL, Payload2);
    // do_Payload(NULL, Payload3);
    do_Payload(NULL, Payload5);

    // ExecuteShader(Shader1, PAYLOAD_TIME);
    // ExecuteShader(Shader2, PAYLOAD_TIME);
    // ExecuteShader(Shader3, PAYLOAD_TIME);
    // ExecuteShader(Shader4, PAYLOAD_TIME);
    // ExecuteShader(Shader5, PAYLOAD_TIME);
    // ExecuteShader(Shader6, PAYLOAD_TIME);
    // ExecuteShader(Shader7, PAYLOAD_TIME);
    // ExecuteShader(Shader8, PAYLOAD_TIME);
    // ExecuteShader(Shader9, PAYLOAD_TIME);
    // ExecuteShader(Shader10, PAYLOAD_TIME);
    // ExecuteShader(Shader11, PAYLOAD_TIME);
    // ExecuteShader(Shader12, PAYLOAD_TIME);
    // ExecuteShader(Shader13, PAYLOAD_TIME);
    // ExecuteShader(Shader14, PAYLOAD_TIME);
    // ExecuteShader(Shader15, PAYLOAD_TIME);
    // ExecuteShader(Shader16, PAYLOAD_TIME);

    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(WindowsCorruptionPayload), NULL, 0, NULL);
    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(FileMessPayload), (PVOID)SystemDirectory, 0, NULL);
    // CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MessageBoxPayload), NULL, 0, NULL);
    return 0;
}