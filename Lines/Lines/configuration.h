#pragma once
#include <Windows.h>

typedef struct tagScreensaverOptions
{
	int MaxDisplaysSupported;
	float TargetTime;
	BOOL ContinuousLines;
	BOOL ExpireDraw;
	int ExpireDrawAfter;
}Options;

int LoadConfig(Options* a);