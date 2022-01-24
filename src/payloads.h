#pragma once

#include <Windows.h>
#include <cmath>
#include "timer.h"
#include "virtual_screen.h"
#include "def.h"

#pragma  comment(lib,"WinMM.lib")
#pragma  comment(lib,"Msimg32.lib")


extern void SeedXorshift32(DWORD dwSeed);

extern void WindowsCorruptionPayload();

extern void MessageBoxPayload();

extern void ExecutePayload(TROJAN_PAYLOAD payload, int nTime);

extern void ExecuteShader(TROJAN_SHADER shader, int nTime);

extern void AudioSequence(PAUDIO_SEQUENCE_PARAMS pAudioParams);

extern void AudioSequence1(int nSamplesPerSec, int nSampleCount, PSHORT psSamples);
extern void AudioSequence2(int nSamplesPerSec, int nSampleCount, PSHORT psSamples);
extern void AudioSequence3(int nSamplesPerSec, int nSampleCount, PSHORT psSamples);
extern void AudioSequence4(int nSamplesPerSec, int nSampleCount, PSHORT psSamples);

extern void Payload1(int t, HDC hdcScreen);
extern void Payload2(int t, HDC hdcScreen);
extern void Payload3(int t, HDC hdcScreen);
extern void Payload4(int t, HDC hdcScreen);
extern void Payload5(int t, HDC hdcScreen);
extern void Payload6(int t, HDC hdcScreen);
extern void Payload7(int t, HDC hdcScreen);
extern void Payload8(int t, HDC hdcScreen);
extern void Payload9(int t, HDC hdcScreen);

extern COLORREF Shader1(int t, int num, COLORREF rgb);
extern COLORREF Shader2(int t, int num, COLORREF rgb);
extern COLORREF Shader3(int t, int num, COLORREF rgb);
extern COLORREF Shader4(int t, int num, COLORREF rgb);
extern COLORREF Shader5(int t, int num, COLORREF rgb);
extern COLORREF Shader6(int t, int num, COLORREF rgb);
extern COLORREF Shader7(int t, int num, COLORREF rgb);