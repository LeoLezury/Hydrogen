

#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "display.h"

uint xs = 0b1011011001001011010101001010100010111101001010101;

uint Xorshift32()
{
    xs ^= xs << 13;
    xs ^= xs >> 17;
    xs ^= xs << 5;
    return xs;
}

void effect1(int idx)
{
    usleep(15e3);
    s2w(0);
    c2wx(idx % 256, (idx / 2) % 256, (idx / 2), 0, GL_XOR);
}

void effect2(int idx)
{
    usleep(15e3);
    s2wpx(0,
          (idx * 10) % screen_w, (idx * 10) % screen_h, 0, 0,
          0, 0, 0, 0);
    color2window((idx * 10) % screen_w, (idx * 10) % screen_h, 0, 0,
                 rand() % 256, rand() % 256, rand() % 256, 0, GL_XOR);
}

void effect3(int idx)
{
    usleep(100e3);
    s2t(0);
    for (int i = 0; i <= screen_w / 10; i++)
        for (int j = 0; j <= screen_h / 10; j++)
        {
            t2wpx(0,
                  i * 10, j * 10, 1, 1,
                  i * 10, j * 10, 10, 10);
        }
}

void effect4(int idx)
{
    usleep(20e3);
    s2wx(0,
         idx % 200 + 10, -idx % 25, 0, 0,
         255, 255, 255, 128);
    alpha2window(idx % 200 + 10, -idx % 25, 0, 0, 255);
}

void effect5(int idx)
{
    idx *= 3;
    int x = rand() % screen_w, y = rand() % screen_h;
    int w = idx % screen_w, h = idx % screen_h;
    screen2window(0,
                  x, y, w, h,
                  (x + idx / 2) % screen_w, y % screen_h, w, h,
                  255, 255, 255, 255, 0);
}

void effect6_init()
{
    glTranslatef(screen_w / 2., screen_h / 2., 0);
    glRotatef(1, 0, 0, 1);
    glTranslatef(-screen_w / 2., -screen_h / 2., 0);
}
void effect6(int idx)
{
    usleep(50e3);
    s2wca(0, 255, 255, 255, 128);
    a2w(255);
}

void effect7(int idx)
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

void effect8_init()
{
    float mView[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mView);
    mView[1] = mView[5] * .02;
    glLoadMatrixf(mView);
}
void effect8(int idx)
{
    usleep(50e3);
    // SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
    // SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
    // for (int i = 0; i < 5; i++)
    //     TextOut(hdcScreen, random() % screen_w, random() % screen_h, L"ӉӬլҏ ӎӬ !!!", 11);
    s2wca(0, 255, 255, 255, 128);
    a2w(255);
}

void effect9_init()
{
    float mView[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mView);
    mView[4] = mView[0] * .02;
    mView[12] += mView[4] * -(screen_h / 2);
    glLoadMatrixf(mView);
}
void effect9(int idx)
{
    usleep(50e3);
    s2wca(0, 255, 255, 255, 128);
    a2w(255);
    c2wx(rand() % 256, rand() % 256, rand() % 256, 0, GL_XOR);
}

// void effect10(int idx)
// {

//     idx *= 30;

//     Sleep(50);

//     BitBlt(hcdc, 0, 0, screen_w, screen_h, hdcScreen, 0, 0, NOTSRCCOPY);
//     SelectObject(hdcScreen, CreatePatternBrush(hBitmap));

//     Ellipse(hdcScreen, idx % screen_w + 20, idx % screen_h + 20, idx % screen_w + idx % 101 + 180, idx % screen_h + idx % 101 + 180);
//     BitBlt(hcdc, 0, 0, screen_w, screen_h, hdcScreen, 0, 0, NOTSRCCOPY);
//     SelectObject(hdcScreen, CreatePatternBrush(hBitmap));
//     Ellipse(hdcScreen, idx % screen_w + 10, idx % screen_h + 10, idx % screen_w + idx % 101 + 190, idx % screen_h + idx % 101 + 190);
//     Ellipse(hdcScreen, idx % screen_w, idx % screen_h, idx % screen_w + idx % 101 + 200, idx % screen_h + idx % 101 + 200);
//     BitBlt(hcdc, 0, 0, screen_w, screen_h, hdcScreen, 0, 0, NOTSRCCOPY);
//     SelectObject(hdcScreen, CreatePatternBrush(hBitmap));
//     Ellipse(hdcScreen, idx % screen_w, idx % screen_h, idx % screen_w + idx % 101 + 200, idx % screen_h + idx % 101 + 200);

//     SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
//     SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));

//     for (int i = 0; i < 5; i++)
//     {
//         TextOut(hdcScreen, random() % screen_w, random() % screen_h, L"     ", 5);
//     }

//     DeleteObject(hcdc);
//     DeleteObject(hBitmap);
// }

#define RGB(r, g, b) (((uint)(byte)(r)) | ((uint)(byte)(g) << 8) | ((uint)(byte)(b) << 16))
#define RGB2BGRA(v) ((((uint)(v)&0xff) << 16) | ((uint)(v)&0xff00) | (((uint)(v)&0xff0000) >> 16) | 0xff000000)
#define RGBset(v) (((uint *)data)[i] = RGB2BGRA(v))
#define BGRAfrom(n) (((uint *)data)[n])

#define r (data[i * 4 + 2])
#define g (data[i * 4 + 1])
#define b (data[i * 4])

#define _shader_func_(name)    \
    void shader##name(int idx) \
    {                          \
        s2i(0);                \
        byte *data = img[0]->data;
#define _shader_func_end_ \
    i2w(0, 0);            \
    }

_shader_func_(1);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
    RGBset(RGB(r, g, b) * 3);
_shader_func_end_;

_shader_func_(2);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
    RGBset(RGB(r + 100, (r + g + b) / 4 + idx, (r + g + b) / 4 + i) + idx * 10);
_shader_func_end_;

_shader_func_(3);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
{
    uint v = RGB(2 * r, b + idx, g + i) + (uint)sqrt(i >> idx / (r + 1)) / 10;
    RGBset(v);
}
_shader_func_end_;

_shader_func_(4);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
{
    uint v = RGB((r + g + b) / 3, (r + g + b) / 3, (r + g + b) / 3) + idx - ((int)sqrt(i) & 0xffffff);
    RGBset(v);
}
_shader_func_end_;

_shader_func_(5);
usleep(100e3);
for (int i = 0; i < screen_w * screen_h; i++)
{
    uint v = RGB(r, g, b) - Xorshift32() % ((uint)sqrt(i + 1) & 0xffffff);
    RGBset(v);
}
_shader_func_end_;

_shader_func_(6);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
{
    uint temp = ((uint *)data)[i];
    ((uint *)data)[i] = ((uint *)data)[i / 3];
    ((uint *)data)[i / 3] = temp;
}
_shader_func_end_;

_shader_func_(7);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
{
    int randPixel = Xorshift32() % screen_w;
    int tempB = b;
    r = data[randPixel * 4];
    g = data[randPixel * 4];
    b = data[randPixel * 4];
    ((uint *)data)[randPixel] = RGB2BGRA(RGB(tempB, tempB, tempB));
}
_shader_func_end_;

_shader_func_(8);
usleep(50e3);
idx *= 10;
for (int i = 0; i < screen_w * screen_h; i++)
    ((uint *)data)[(i % screen_w) * (i / screen_w)] = RGB2BGRA(RGB(i % screen_w, i / screen_w, idx));
_shader_func_end_;

_shader_func_(9);
usleep(50e3);
idx *= 50;
for (int i = 0; i < screen_w * screen_h; i++)
    RGBset(RGB(r + i / screen_w / 10, g + i % screen_w / 10, b + idx));
_shader_func_end_;

_shader_func_(10);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
{
    uint temp1 = (i / screen_w + Xorshift32() % 11 - 5);
    uint temp2 = (i % screen_w + Xorshift32() % 11 - 5);
    ((uint *)data)[i] = ((uint *)data)[(temp1 * screen_w + temp2) % (screen_w * screen_h)];
}
_shader_func_end_;

_shader_func_(11);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
{
    uint temp = ((uint *)data)[i];
    ((uint *)data)[i] = ((uint *)data)[i / 3 * 2];
    ((uint *)data)[i / 3 * 2] = temp;
}
_shader_func_end_;

_shader_func_(12);
usleep(50e3);
for (int i = 0, x = 0, y = 0; y < screen_h; i++, ++x < screen_w ? 0 : (x = 0, ++y))
{
    uint a = (uint)(i + sqrt(y * (screen_h - y))) % (screen_w * screen_h);
    BGRAfrom(i) = BGRAfrom(a);
}
_shader_func_end_;

_shader_func_(13);
usleep(100e3);
for (int i = 0, x = 0, y = 0; y < screen_h; i++, ++x < screen_w ? 0 : (x = 0, ++y))
{
    uint a = (uint)(x * screen_w + y +
                    sqrt(2 * screen_h * y - y * y)) %
             (screen_w * screen_h);
    BGRAfrom(i) = BGRAfrom(a);
}
_shader_func_end_;

_shader_func_(14);
usleep(100e3);
for (int i = 0, x = 0, y = 0; y < screen_h; i++, ++x < screen_w ? 0 : (x = 0, ++y))
{
    uint a = (uint)(x * screen_w + y +
                    sqrt(2 * screen_h * x - x * x)) %
             (screen_w * screen_h);
    BGRAfrom(i) = BGRAfrom(a);
}
_shader_func_end_;

_shader_func_(15);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
    RGBset(idx *i);
_shader_func_end_;

_shader_func_(16);
usleep(50e3);
for (int i = 0; i < screen_w * screen_h; i++)
    RGBset((Xorshift32() & 0xff) * 0x010101);
_shader_func_end_;

// #endif

#undef r
#undef g
#undef b
