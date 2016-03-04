#include <Windows.h>

typedef struct tagTriangle
{
	POINT left;
	POINT top;
	POINT right;
}TRIANGLE;

void Plot(int x, int y, int color, int* buffer, int bufferWidth, int bufferHeight);

void DrawLine(int x, int y, int x2, int y2, int color, int* buffer, int bufferWidth, int bufferHeight);

void DrawRect(int x, int y, int width, int height, int color, int* buffer, int bufferWidth, int bufferHeight);

void DrawTriangle(TRIANGLE tri, int color, int* buffer, int bufferWidth, int bufferHeight);

void DrawCircle(int x, int y, int radius, int color, int* buffer, int bufferWidth, int bufferHeight);

void DrawChevron(int x, int y, int radius, int color, int* buffer, int bufferWidth, int bufferHeight);