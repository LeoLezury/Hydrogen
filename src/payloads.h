#pragma once
#include <Windows.h>
#include <cmath>
#include "timer.h"
#include "virtual_screen.h"
#pragma  comment(lib,"WinMM.lib")
#pragma  comment(lib,"Msimg32.lib")
#define PAYLOAD_TIME 10
typedef void(TROJAN_PAYLOAD)(int t, HDC hdcScreen);
extern LPCWSTR GenRandomUnicodeString(int len);
extern BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam);
extern void WindowsCorruptionPayload();
extern void MessageBoxThread();
extern void MessageBoxPayload();
extern void ExecutePayload(TROJAN_PAYLOAD payload, int nTime);
extern void Payload1(int t, HDC hdcScreen);
extern void Payload2(int t, HDC hdcScreen);
extern void Payload3(int t, HDC hdcScreen);
extern void Payload4(int t, HDC hdcScreen);
extern void Payload5(int t, HDC hdcScreen);
extern void Payload6(int t, HDC hdcScreen);
extern void Payload7(int t, HDC hdcScreen);
extern void Payload8(int t, HDC hdcScreen);
extern void Payload9(int t, HDC hdcScreen);
extern void AudioPayload();
extern int RotateDC(HDC hdc, int Angle, POINT ptCenter);
extern int random();