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
	BOOL Starburst;
	POINT StartburstFrom;
	int ExpireDrawAfter;
}Options;

typedef enum tageDrawType
{
	Lines = 0,
	Rectangles = 1,
	Dots = 2,
	Triangles = 3,
	Circles = 4,
	Chevrons = 5
}eDrawType;

BOOL LoadConfig(Options* opt);

BOOL WriteConfig(Options* opt);