

#include <stdlib.h>
#include <unistd.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "display.h"

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
