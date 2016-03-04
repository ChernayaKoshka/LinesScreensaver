#include <Windows.h>
#include <stdio.h>
#include <ShlObj.h>
#include "math_custom.h"
#include "drawing.h"
#include "configuration.h"

BOOL Running = TRUE;

int monCount = 0;

int ExpireCount = 0;

Options options = { 0 };

typedef struct tagEnumeratedDisplay
{
	HDC DrawingContext;
	int ScreenWidth;
	int ScreenHeight;
	POINT Previous;
	void* BackBuffer;
	BITMAPINFO BitMapInfo;
}EnumeratedDisplay;

/* 
   Either I'm an idiot or Microsoft is an idiot, one of the two.
   I had to define this struct myself because MONITORINFOEX only exposed
   the szDevice field for some reason 
*/
typedef struct tagMONITORINFOEX {
	DWORD cbSize;
	RECT  rcMonitor;
	RECT  rcWork;
	DWORD dwFlags;
	TCHAR szDevice[CCHDEVICENAME];
} MONITORINFOEX2, *LPMONITORINFOEX2;

EnumeratedDisplay* Displays;

static double GTimePassed = 0;
static float SecondsPerTick = 0;
static __int64 GTimeCount = 0;

HWND SettingsWindow = 0;

HWND DrawTypeComBox = 0;
HWND MaxDisplaysSupportedNUD = 0;
HWND TargetTimeNUD = 0; //IntegerUpDown or FloatingUpDown? iunno, TODO: look up later
HWND ContinuousLinesCB = 0;
HWND DifferenScreenPerDisplayCB = 0;
HWND ExpireDrawCB = 0;
HWND StarburstCB = 0;
HWND StarburstXNUD = 0;
HWND StarburstYNUD = 0;
HWND ExpireDrawAfterNUD = 0;

HWND SaveSettingsBtn = 0;
HINSTANCE hThisInstance = 0;
int nThisShowCmd = 0;
BOOL bSettingsWindowVisible = FALSE;

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
	if (monCount > options.MaxDisplaysSupported) return TRUE;

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

	display.Previous.x = -1;
	display.Previous.x = -1;

	if (options.DifferentScreenPerDisplay || monCount == 0) 
	{
		int bufferSize = screenHeight*screenWidth * 4;
		display.BackBuffer = malloc(bufferSize); //4 = bytes to display RGB
		ZeroMemory(display.BackBuffer, bufferSize);

		display.BitMapInfo.bmiHeader.biSize = sizeof(display.BitMapInfo.bmiHeader);
		display.BitMapInfo.bmiHeader.biWidth = screenWidth;
		display.BitMapInfo.bmiHeader.biHeight = -screenHeight;
		display.BitMapInfo.bmiHeader.biPlanes = 1;
		display.BitMapInfo.bmiHeader.biBitCount = 32;
		display.BitMapInfo.bmiHeader.biCompression = BI_RGB;
	}
	else
	{
		display.BackBuffer = Displays[0].BackBuffer;
		display.BitMapInfo = Displays[0].BitMapInfo;
	}


	Displays[monCount] = display;
	monCount++;
	return TRUE;
}

int CalculateScreen()
{
	if (options.ExpireDraw)
	{
		if (ExpireCount >= options.ExpireDrawAfter) {
			for (int i = 0; i < monCount; i++)
				ZeroMemory(Displays[i].BackBuffer, Displays[i].ScreenHeight*Displays[i].ScreenWidth * 4);
			ExpireCount = 0;
			return 0;
		}
		++ExpireCount;
	}

	for (int i = 0; i < monCount; i++)
	{
		EnumeratedDisplay display = Displays[i];

		unsigned char red = rand() % 256;
		unsigned char green = rand() % 256;
		unsigned char blue = rand() % 256;
		unsigned int color = (red << 16) | (green << 8) | blue;

		POINT StartingPoint = { 0 };

		if (options.ContinuousLines)
		{
			StartingPoint = display.Previous;
		}

		if (display.Previous.x <= -1 || display.Previous.y <= -1)
		{
			StartingPoint.x = rand() % display.ScreenWidth;
			StartingPoint.y = rand() % display.ScreenHeight;
		}
		
		if (options.Starburst)
		{
			StartingPoint.x = options.StartburstFrom.x;
			StartingPoint.y = options.StartburstFrom.y;
		}

		POINT EndingPoint = { 0 };
		EndingPoint.x = rand() % display.ScreenWidth;
		EndingPoint.y = rand() % display.ScreenHeight;

		switch (options.DrawType)
		{
		case Lines:
			DrawLine(StartingPoint.x,
				StartingPoint.y,
				EndingPoint.x,
				EndingPoint.y,
				color, (int*)display.BackBuffer,
				display.ScreenWidth, display.ScreenHeight);
			break;
		case Rectangles:
			DrawRect(StartingPoint.x,
				StartingPoint.y,
				EndingPoint.x,
				EndingPoint.y,
				color,
				(int*)display.BackBuffer,
				display.ScreenWidth,
				display.ScreenHeight);
			break;
		case Dots:
			Plot(StartingPoint.x,
				StartingPoint.y,
				color,
				(int*)display.BackBuffer,
				display.ScreenWidth, display.ScreenHeight);
			Plot(EndingPoint.x,
				EndingPoint.y,
				color,
				(int*)display.BackBuffer,
				display.ScreenWidth, display.ScreenHeight);
			break;
		case Triangles:
		{
			TRIANGLE tri = { 0 };
			tri.top = StartingPoint;
			tri.right = EndingPoint;
			tri.left.x = rand() % Displays[i].ScreenWidth;
			tri.left.y = rand() % Displays[i].ScreenHeight;

			DrawTriangle(tri, color, Displays[i].BackBuffer, Displays[i].ScreenWidth, Displays[i].ScreenHeight);
			break; 
		}
		case Circles:
			DrawCircle(StartingPoint.x, 
					   StartingPoint.y,
					   EndingPoint.x, color, Displays[i].BackBuffer, Displays[i].ScreenWidth, Displays[i].ScreenHeight);
			break;
		case Chevrons:
			DrawChevron(StartingPoint.x,
					    StartingPoint.y,
					    EndingPoint.x, color, Displays[i].BackBuffer, Displays[i].ScreenWidth, Displays[i].ScreenHeight);
		default:
			break;
		}

		if (options.ContinuousLines)
		{
			Displays[i].Previous = EndingPoint;
		}

		if (options.DifferentScreenPerDisplay == FALSE) return 0;
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, WPARAM lParam)
{
	LRESULT Result = 0;

	switch (uMsg)
	{
	case WM_KEYUP:
		return Result;
	case WM_CLOSE:
		Running = FALSE;
		return Result;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BN_CLICKED:
			if ((HWND)lParam == ContinuousLinesCB)
			{
				if (options.ContinuousLines)
					options.ContinuousLines = FALSE;
				else
					options.ContinuousLines = TRUE;
			}
			else if ((HWND)lParam == SaveSettingsBtn)
			{
				WriteConfig(&options);
			}
			break;
		}
		return Result;
	case WM_PAINT:
	default:
		Result = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return Result;
}

void RegisterClasses()
{
	//define window
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hThisInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"screenoptions";

	if (!RegisterClassEx(&wc))
		return;

	RECT r = { 0 };
	r.right = 200;
	r.bottom = 200;
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

	SettingsWindow = CreateWindowExW(
		0,
		L"screenoptions",
		L"Screensaver Settings",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		NULL,
		NULL,
		hThisInstance,
		0);

	ContinuousLinesCB = CreateWindowExW(0, L"BUTTON", L"Continuous Lines", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 0, 0, 200, 20, SettingsWindow, NULL, hThisInstance, NULL);
	SaveSettingsBtn = CreateWindowExW(0, L"BUTTON", L"Save", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 40, 80, 80, SettingsWindow, NULL, hThisInstance, NULL);
	ShowWindow(SettingsWindow, nThisShowCmd);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hThisInstance = hInstance;
	nThisShowCmd = nShowCmd;
	//RegisterClasses();


	LoadConfig(&options);

	Displays = (EnumeratedDisplay*)malloc(sizeof(EnumeratedDisplay)*options.MaxDisplaysSupported);
	ZeroMemory(Displays, sizeof(EnumeratedDisplay)*options.MaxDisplaysSupported);
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	Sys_InitFloatTime();

	float PrevTime = Sys_InitFloatTime();
	float TimeAccumulated = 0;

	POINT oldPos;
	GetCursorPos(&oldPos);
	MSG msg;
	bSettingsWindowVisible = TRUE;
	int x = 0;
	int y = 0;
	while (Running)
	{
		if (bSettingsWindowVisible)
		{
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		float NewTime = Sys_FloatTime();
		TimeAccumulated += NewTime - PrevTime;
		PrevTime = NewTime;

		if (TimeAccumulated > options.TargetTime)
		{
			TimeAccumulated = 0.0f;
			CalculateScreen();
		}

		for (int i = 0; i < monCount; i++)
		{
			StretchDIBits(Displays[i].DrawingContext,
				0, 0, Displays[i].ScreenWidth, Displays[i].ScreenHeight,
				0, 0, Displays[i].BitMapInfo.bmiHeader.biWidth, Abs(Displays[i].BitMapInfo.bmiHeader.biHeight),
				Displays[i].BackBuffer, &Displays[i].BitMapInfo,
				DIB_RGB_COLORS, SRCCOPY);
		}

		POINT newPos;
		GetCursorPos(&newPos);
		if (Abs(newPos.x - oldPos.x) > 5 || Abs(newPos.y - oldPos.y) > 5)
			Running = FALSE;

	}
	for (int i = 0; i < monCount; i++) 
	{
		ReleaseDC(NULL, Displays[i].DrawingContext);
		if (Displays[i].BackBuffer == Displays[0].BackBuffer && i != 0)
			continue;
		free(Displays[i].BackBuffer);
	}
	free(Displays);

	//refresh explorer to remove remanents
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

	return EXIT_SUCCESS;
}

