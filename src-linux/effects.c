

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "display.h"

uint xs = 2840689237;

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

void effect7_init()
{
    loadimage(1, "./Hydrogen-linux.png");
    i2t(1, 1);
}
void effect7(int idx)
{
    usleep(50e3);
    glPushMatrix();
    glTranslatef(screen_w / 2., screen_h / 2., 0);
    glRotatef((4 - idx * 131 % 9) / 5., 0, 0, 1);
    glTranslatef(-screen_w / 2., -screen_h / 2., 0);
    int x = randx, y = randy, w = imagew(1), h = imageh(1);
    c2wxx(x, y, w, h, rand(), rand(), rand(), 255, 0);
    t2wx(1, x, y, w, h, rand(), rand(), rand(), 255);
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
    loadimage(1, "./effect8-text.png");
    i2t(1, 1);
}
void effect8(int idx)
{
    usleep(50e3);
    for (int i = 0; i < 5; i++)
    {
        int x = randx, y = randy, w = imagew(1), h = imageh(1);
        c2wxx(x, y, w, h, rand(), rand(), rand(), 255, 0);
        t2wx(1, x, y, w, h, rand(), rand(), rand(), 255);
    }
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
#define Pixelat(idx) (((uint *)data)[idx])
#define exchPixel(i1, i2)          \
    do                             \
    {                              \
        uint t = Pixelat(i1);      \
        Pixelat(i1) = Pixelat(i2); \
        Pixelat(i2) = t;           \
    } while (0)

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
for (int i = 0; i < screen_size; i++)
    RGBset(RGB(r, g, b) * 3);
_shader_func_end_;

_shader_func_(2);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
    RGBset(RGB(r + 100, (r + g + b) / 4 + idx, (r + g + b) / 4 + i) + idx * 10);
_shader_func_end_;

_shader_func_(3);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
{
    uint v = RGB(2 * r, b + idx, g + i) + (uint)sqrt(i >> idx / (r + 1)) / 10;
    RGBset(v);
}
_shader_func_end_;

_shader_func_(4);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
{
    uint v = RGB((r + g + b) / 3, (r + g + b) / 3, (r + g + b) / 3) + idx - ((int)sqrt(i) & 0xffffff);
    RGBset(v);
}
_shader_func_end_;

_shader_func_(5);
usleep(100e3);
for (int i = 0; i < screen_size; i++)
{
    uint v = RGB(r, g, b) - Xorshift32() % ((uint)sqrt(i + 1) & 0xffffff);
    RGBset(v);
}
_shader_func_end_;

_shader_func_(6);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
    exchPixel(i, i / 3);
_shader_func_end_;

_shader_func_(7);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
{
    int randPixel = Xorshift32() % screen_w;
    int tempB = b;
    r = data[randPixel * 4];
    g = data[randPixel * 4];
    b = data[randPixel * 4];
    Pixelat(randPixel) = RGB2BGRA(RGB(tempB, tempB, tempB));
}
_shader_func_end_;

_shader_func_(8);
usleep(50e3);
for (int i = 0, x = 0, y = 0; y < screen_h; i++, ++x < screen_w ? 0 : (x = 0, ++y))
    Pixelat(x *y) = RGB2BGRA(RGB(i % screen_w, i / screen_w, idx * 10));
_shader_func_end_;

_shader_func_(9);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
    RGBset(RGB(r + i / screen_w / 10, g + i % screen_w / 10, b + idx * 50));
_shader_func_end_;

_shader_func_(10);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
{
    uint temp1 = (i / screen_w + Xorshift32() % 11 - 5);
    uint temp2 = (i % screen_w + Xorshift32() % 11 - 5);
    ((uint *)data)[i] = ((uint *)data)[(temp1 * screen_w + temp2) % (screen_size)];
}
_shader_func_end_;

_shader_func_(11);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
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
    uint a = (uint)(i + sqrt(y * (screen_h - y))) % (screen_size);
    Pixelat(i) = Pixelat(a);
}
_shader_func_end_;

_shader_func_(13);
usleep(100e3);
for (int i = 0, x = 0, y = 0; y < screen_h; i++, ++x < screen_w ? 0 : (x = 0, ++y))
{
    uint a = (uint)(x * screen_w + y +
                    sqrt(2 * screen_h * y - y * y)) %
             (screen_size);
    Pixelat(i) = Pixelat(a);
}
_shader_func_end_;

_shader_func_(14);
usleep(100e3);
for (int i = 0, x = 0, y = 0; y < screen_h; i++, ++x < screen_w ? 0 : (x = 0, ++y))
{
    uint a = (uint)(x * screen_w + y +
                    sqrt(2 * screen_h * x - x * x)) %
             (screen_size);
    Pixelat(i) = Pixelat(a);
}
_shader_func_end_;

_shader_func_(15);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
    RGBset(idx *i);
_shader_func_end_;

_shader_func_(16);
usleep(50e3);
for (int i = 0; i < screen_size; i++)
    RGBset((Xorshift32() & 0xff) * 0x010101);
_shader_func_end_;

void Sleep(int t) { usleep(t * 1e3); }

void effect_2_1(int idx)
{
    usleep(100e3);
    for (int i = 0; i <= 10; i++)
    {
        c2wxx(i * (screen_w / (2 * (10 + 1))), i * (screen_h / (2 * (10 + 1))),
              screen_w - 2 * (i * (screen_w / (2 * (10 + 1)))), screen_h - 2 * (i * (screen_h / (2 * (10 + 1)))),
              rand(), rand(), rand(), 0, GL_XOR);
    }
}

void effect_2_2(int idx)
{
    usleep(20e3);
    c2wx(rand(), rand(), rand(), 0, GL_XOR);
    s2wp(0, -20, -20, 0, 0);
}

void effect_2_3(int idx)
{
    usleep(20e3);
    s2wxx(0, 0, 0, 0, 0,
          -20, 25 - (5 * (rand() % 10)), screen_w - 20, screen_h,
          255, 255, 255, 255, GL_COPY_INVERTED);
    a2w(255);
}

void effect_2_4(int idx)
{
    for (int y = 0; y <= screen_h; y++)
        t2wpx(0, 0, y, 0, 1,
              10 * ((rand() % 5) - 3), y, screen_w, 1);
    Sleep(100);
}

#if 0


void effect_2_5(int idx)
{
    POINT pos[3];
    pos[0].x = 0, pos[0].y = 0;
    pos[1].x = cos(PI / 36) * screen_w, pos[1].y = sin(PI / 36) * screen_w;
    pos[2].x = (-1) * (sin(PI / 36) * screen_h), pos[2].y = cos(PI / 36) * screen_h;
    PlgBlt(hcdc, pos, hcdc, 0, 0, screen_w, screen_h, 0, 0, 0);
    SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)));
    PatBlt(hcdc, 0, 0, screen_w, screen_h, PATINVERT);
    BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
    Sleep(100);
}
void effect_2_6(int idx)
{
    POINT pos[3];
    pos[0].x = screen_h / 40, pos[0].y = screen_h / 40;
    pos[1].x = screen_w - (screen_h / 40), pos[1].y = 0;
    pos[2].x = screen_h / 40, pos[2].y = screen_h;
    PlgBlt(hcdc, pos, hcdc, 0, 0, screen_w, screen_h, 0, 0, 0);
    SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)));
    PatBlt(hcdc, 0, 0, screen_w, screen_h, PATINVERT);
    BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
    Sleep(100);
}
void effect_2_7(int idx)
{
    POINT pos[3];
    pos[0].x = screen_w - (cos(PI / 36) * screen_w), pos[0].y = (-1) * (sin(PI / 36) * screen_w);
    pos[1].x = screen_w, pos[1].y = 0;
    pos[2].x = screen_w - ((cos(PI / 36) * screen_w) + (sin(PI / 36) * screen_h)), pos[2].y = (cos(PI / 36) * screen_h) - (sin(PI / 36) * screen_w);
    PlgBlt(hcdc, pos, hcdc, 0, 0, screen_w, screen_h, 0, 0, 0);
    SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)));
    PatBlt(hcdc, 0, 0, screen_w, screen_h, PATINVERT);
    BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
    Sleep(100);
}
void effect_2_8(int idx)
{
    POINT pos[3];
    pos[0].x = rand() % screen_w, pos[0].y = rand() % screen_h;
    pos[1].x = rand() % screen_w, pos[1].y = rand() % screen_h;
    pos[2].x = rand() % screen_w, pos[2].y = rand() % screen_h;
    PlgBlt(hcdc, pos, hcdc, 0, 0, screen_w, screen_h, 0, 0, 0);
    BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
    Sleep(100);
}
void effect_2_9(int idx)
{
    POINT pos[3];
    pos[0].x = 0, pos[0].y = 0;
    pos[1].x = screen_w, pos[1].y = 0;
    pos[2].x = screen_w / 20, pos[2].y = screen_h;
    PlgBlt(hcdc, pos, hcdc, 0, 0, screen_w, screen_h, 0, 0, 0);
    SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)));
    PatBlt(hcdc, 0, 0, screen_w, screen_h, PATINVERT);
    BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
    Sleep(100);
}
void effect_2_10(int idx)
{
    switch (rand() % 3)
    {
    case 0:
        SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255)));
        Pie(hcdc, rand() % screen_w, rand() % screen_h, rand() % screen_w, rand() % screen_h, rand() % screen_w, rand() % screen_h, rand() % screen_w, rand() % screen_h);
        break;
    case 1:
        SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255)));
        Ellipse(hcdc, rand() % screen_w, rand() % screen_h, rand() % screen_w, rand() % screen_h);
        break;
    case 2:
        SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255)));
        Rectangle(hcdc, rand() % screen_w, rand() % screen_h, rand() % screen_w, rand() % screen_h);
        break;
    }
    BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
    Sleep(20);
}
void effect_2_11(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            prgbScreen[i].r = prgbScreen[i].r >= 255 ? prgbScreen[i].r += 10 : prgbScreen[i].r;
            prgbScreen[i].g = prgbScreen[i].g - (prgbScreen[i].g / 10);
            prgbScreen[i].b = prgbScreen[i].b - (prgbScreen[i].b / 10);
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_12(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_h; i++)
        {
            int rgbTemp = rand() % 255;
            for (int l = 0; l < screen_w; l++)
            {
                prgbScreen[i * screen_w + l].rgb = RGB(rgbTemp, rgbTemp, rgbTemp);
            }
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_13(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            prgbScreen[i].r = rand() % 255;
            prgbScreen[i].g = rand() % 255;
            prgbScreen[i].b = rand() % 255;
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_14(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            prgbScreen[i].rgb = (prgbScreen[i].rgb * 2) % RGB(255, 255, 255);
            ;
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_15(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            prgbScreen[i].rgb = RGB((prgbScreen[i].r << 1) % 255, (prgbScreen[i].g << 1) % 255, (prgbScreen[i].b << 1) % 255);
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_16(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_h; i++)
        {
            for (int j = 0; j < screen_w; j++)
            {
                int y = (i + rand() % 11 - 5);
                if (y < 0)
                {
                    y = -y;
                }
                int x = (j + rand() % 11 - 5);
                if (x < 0)
                {
                    x = -x;
                }
                prgbScreen[i * screen_w + j].rgb = prgbScreen[(y * screen_w + x) % (screen_w * screen_h)].rgb;
            }
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_17(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            prgbScreen[i].rgb = ((10 * t + i) / 10) % RGB(255, 255, 255);
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_18(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            prgbScreen[i].rgb = (t << i) % RGB(255, 255, 255);
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_19(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            int x = i % screen_w, y = i / screen_h;
            prgbScreen[i].r += GetRValue(x * y);
            prgbScreen[i].g += GetGValue(x * y);
            prgbScreen[i].b += GetBValue(x * y);
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_20(int idx)
{
    int dwStartTime = time(NULL);
    HDC hdc = GetDC(NULL);
    int screen_w = GetSystemMetrics(SM_CXSCREEN), screen_h = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = screen_w;
    bmi.bmiHeader.biHeight = screen_h;
    PRGBQUAD prgbScreen;
    HDC hcdc = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, 0, (void **)&prgbScreen, NULL, 0);
    SelectObject(hcdc, hBitmap);
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        hdc = GetDC(NULL);
        BitBlt(hcdc, 0, 0, screen_w, screen_h, hdc, 0, 0, SRCCOPY);
        for (int i = 0; i < screen_w * screen_h; i++)
        {
            int x = i % screen_w, y = i / screen_h, tmp = y ^ y | x;
            prgbScreen[i].rgb = x & y & tmp;
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        DeleteObject(hdc);
        Sleep(10);
    }
}

void effect_2_21(int idx)
{
    int dwStartTime = time(NULL);
    double angle = 0;
    for (int t = 0; time(NULL) < (dwStartTime + nTime); t++)
    {
        for (float i = 0; i < screen_w + screen_h; i += 0.99f)
        {
            int a = sin(angle) * 20;
            BitBlt(hcdc, 0, i, screen_w, 1, hdc, a, i, SRCCOPY);
            angle += PI / 40;
        }
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        Sleep(5);
    }
    void effect_2_22(int idx)
    {
        BitBlt(hcdc, screen_w / 20, 0, screen_w - (screen_w / 20), screen_h - (screen_h / 20), hdc, 0, screen_h / 20, SRCCOPY);
        BitBlt(hcdc, 0, screen_h - (screen_h / 20), screen_w, screen_h / 20, hdc, 0, 0, SRCCOPY);
        BitBlt(hcdc, 0, screen_h / 20, screen_w / 20, screen_h - (screen_h / 20), hdc, screen_w - (screen_w / 20), screen_h / 20, SRCCOPY);
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        Sleep(5);
    }
    void effect_2_23(int idx)
    {
        SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)));
        PatBlt(hcdc, 0, (t % 40) * (screen_h / 40), screen_w, screen_h / 40, PATINVERT);
        SelectObject(hcdc, CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)));
        PatBlt(hcdc, (t % 40) * (screen_w / 40), 0, screen_w / 40, screen_h, PATINVERT);
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        Sleep(5);
    }
    void effect_2_24(int idx)
    {
        StretchBlt(hcdc, 20 * ((t % 3) - 1), 20 * ((t % 3) - 1), screen_w - 40 * ((t % 3) - 1), screen_h - 40 * ((t % 3) - 1), hcdc, 0, 0, screen_w, screen_h, SRCCOPY);
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        Sleep(5);
    }
    void effect_2_25(int idx)
    {
        SetBkMode(hcdc, 1);
        LPCSTR text[5] = {"Hopejieshuo", "Temp.exe", "GDI Malware", "Fuck You", "You are a Idiot"};
        int tmp = rand() % 5;
        SetTextColor(hcdc, RGB(rand() % 255, rand() % 255, rand() % 255));
        HFONT font = CreateFont(20 + (rand() % 30), 20 + (rand() % 30), 0, 0, FW_THIN, 1, 1, 0, ANSI_CHARSET, 0, 0, 0, 0, "Baby Kruffy");
        SelectObject(hcdc, font);
        TextOut(hcdc, rand() % screen_w, rand() % screen_h, text[tmp], strlen(text[tmp]));
        BitBlt(hdc, 0, 0, screen_w, screen_h, hcdc, 0, 0, SRCCOPY);
        ReleaseDC(NULL, hdc);
        ReleaseDC(NULL, hcdc);
        DeleteObject(font);
        DeleteObject(hdc);
        DeleteObject(hcdc);
        DeleteObject(hBitmap);
        Sleep(5);
    }

#endif

#undef r
#undef g
#undef b

#undef RGB
#undef RGB2BGRA
#undef RGBset
#undef Pixelat
#undef exchPixel
