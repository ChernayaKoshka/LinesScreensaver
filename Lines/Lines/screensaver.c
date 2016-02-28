#include <Windows.h>
#include <stdio.h>
#include <ShlObj.h>
#include "math_custom.h"
#include "drawing.h"

//completely arbitrary, but I don't wanna deal with lists in C
#define MAX_DISPLAYS_SUPPORTED 5

#define TARGET_TIME 0.25f

BOOL Running = TRUE;

int monCount = 0;

typedef struct tagEnumeratedDisplay
{
	HDC DrawingContext;
	int ScreenWidth;
	int ScreenHeight;
	void* BackBuffer;
	BITMAPINFO BitMapInfo;
}EnumeratedDisplay;

//Either I'm an idiot or Microsoft is an idiot, one of the two
typedef struct tagMONITORINFOEX {
	DWORD cbSize;
	RECT  rcMonitor;
	RECT  rcWork;
	DWORD dwFlags;
	TCHAR szDevice[CCHDEVICENAME];
} MONITORINFOEX2, *LPMONITORINFOEX2;

EnumeratedDisplay Displays[MAX_DISPLAYS_SUPPORTED];

static double GTimePassed = 0;
static float SecondsPerTick = 0;
static __int64 GTimeCount = 0;

float Sys_InitFloatTime()
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	SecondsPerTick = 1.0f / (float)Frequency.QuadPart;

	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);
	GTimeCount = Counter.QuadPart;
	return SecondsPerTick;
}

float Sys_FloatTime()
{
	LARGE_INTEGER Counter;
	QueryPerformanceCounter(&Counter);

	__int64 Interval = Counter.QuadPart - GTimeCount;

	GTimeCount = Counter.QuadPart;
	double SecondsGoneBy = (double)Interval*SecondsPerTick;
	GTimePassed += SecondsGoneBy;

	return (float)GTimePassed;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	if (monCount > MAX_DISPLAYS_SUPPORTED) return TRUE;

	MONITORINFOEX2 info = { 0 };
	info.cbSize = sizeof(MONITORINFOEX2);

	#pragma warning (suppress : 4133)
	GetMonitorInfoW(hMonitor, &info);
	HDC dc = CreateDC(TEXT("DISPLAY"), info.szDevice, NULL, NULL);

	EnumeratedDisplay display = { 0 };

	display.DrawingContext = dc;

	int screenHeight = Difference(info.rcMonitor.bottom, info.rcMonitor.top);
	int screenWidth = Difference(info.rcMonitor.right, info.rcMonitor.left);

	display.ScreenHeight = screenHeight;
	display.ScreenWidth = screenWidth;

	int bufferSize = screenHeight*screenWidth * 4;
	display.BackBuffer = malloc(bufferSize); //4 = bytes to display RGB
	memset(display.BackBuffer, 0, bufferSize);

	display.BitMapInfo.bmiHeader.biSize = sizeof(display.BitMapInfo.bmiHeader);
	display.BitMapInfo.bmiHeader.biWidth = screenWidth;
	display.BitMapInfo.bmiHeader.biHeight = -screenHeight;
	display.BitMapInfo.bmiHeader.biPlanes = 1;
	display.BitMapInfo.bmiHeader.biBitCount = 32;
	display.BitMapInfo.bmiHeader.biCompression = BI_RGB;

	Displays[monCount] = display;
	monCount++;
	return TRUE;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	Sys_InitFloatTime();

	float PrevTime = Sys_InitFloatTime();
	float TimeAccumulated = 0;

	POINT oldPos;
	GetCursorPos(&oldPos);

	while (Running)
	{
		float NewTime = Sys_FloatTime();
		TimeAccumulated += NewTime - PrevTime;
		PrevTime = NewTime;

		if (TimeAccumulated > TARGET_TIME)
		{
			TimeAccumulated = 0.0f;

			for (int i = 0; i < monCount; i++)
			{
				EnumeratedDisplay display = Displays[i];

				unsigned char red = rand() % 256;
				unsigned char green = rand() % 256;
				unsigned char blue = rand() % 256;
				unsigned int color = (red << 16) | (green << 8) | blue;

				DrawLine(rand() % display.ScreenWidth, 
						 rand() % display.ScreenHeight, 
						 rand() % display.ScreenWidth,
						 rand() % display.ScreenHeight,
						 color, (int*)display.BackBuffer,
						 display.ScreenWidth);

				StretchDIBits(display.DrawingContext,
					0, 0, display.ScreenWidth, display.ScreenHeight,
					0, 0, display.ScreenWidth, display.ScreenHeight,
					display.BackBuffer, &display.BitMapInfo,
					DIB_RGB_COLORS, SRCCOPY);
			}
		}

		for (int i = 0; i < monCount; i++)
		{
			StretchDIBits(Displays[i].DrawingContext,
				0, 0, Displays[i].ScreenWidth, Displays[i].ScreenHeight,
				0, 0, Displays[i].ScreenWidth, Displays[i].ScreenHeight,
				Displays[i].BackBuffer, &Displays[i].BitMapInfo,
				DIB_RGB_COLORS, SRCCOPY);
		}

		POINT newPos;
		GetCursorPos(&newPos);
		if (Abs(newPos.x - oldPos.x) > 5 || Abs(newPos.y - oldPos.y) > 5)
			Running = FALSE;
		else
			oldPos = newPos;

	}
	for (int i = 0; i < monCount; i++)
		ReleaseDC(NULL, Displays[i].DrawingContext);

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

	return EXIT_SUCCESS;
}

