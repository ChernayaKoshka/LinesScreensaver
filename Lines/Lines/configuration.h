#pragma once
#include <Windows.h>

typedef struct tagScreensaverOptions
{
	int DrawType;
	int MaxDisplaysSupported;
	float TargetTime;
	BOOL ContinuousLines;
	BOOL DifferentScreenPerDisplay;
	BOOL ExpireDraw;
	int ExpireDrawAfter;
}Options;

typedef enum tageDrawType
{
	Lines = 0,
	Rectangles = 1,
	Dots = 2
}eDrawType;

int LoadConfig(Options* a);