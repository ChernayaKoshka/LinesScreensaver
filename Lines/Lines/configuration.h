#pragma once
#include <Windows.h>

typedef struct tagScreensaverOptions
{
	BOOL Rectangles;
	int MaxDisplaysSupported;
	float TargetTime;
	BOOL ContinuousLines;
	BOOL DifferentScreenPerDisplay;
	BOOL ExpireDraw;
	int ExpireDrawAfter;
}Options;

int LoadConfig(Options* a);