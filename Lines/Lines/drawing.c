#include "math_custom.h"
#include "drawing.h"

void Plot(int x, int y, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	if (x > bufferWidth) x = x - bufferWidth;
	if (x < 0) return;

	if (y > bufferHeight) y = y - bufferHeight;
	if (y < 0) return;

	if (((y*bufferWidth) + x)>(bufferWidth*bufferHeight)) return;

	buffer[(y*bufferWidth) + x] = color;
}

//code credit of http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
void DrawLine(int x, int y, int x2, int y2, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	int w = x2 - x;
	int h = y2 - y;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (w < 0) dx1 = -1; else if (w>0) dx1 = 1;
	if (h < 0) dy1 = -1; else if (h>0) dy1 = 1;
	if (w < 0) dx2 = -1; else if (w>0) dx2 = 1;
	int longest = Abs(w);
	int shortest = Abs(h);
	if (!(longest > shortest)) 
	{
		longest = Abs(h);
		shortest = Abs(w);
		if (h < 0) dy2 = -1; else if (h>0) dy2 = 1;
		dx2 = 0;
	}
	int numerator = longest >> 1;
	for (int i = 0; i <= longest; i++) 
	{
		Plot(x, y, color, buffer, bufferWidth, bufferHeight);
		numerator += shortest;
		if (!(numerator < longest)) 
		{
			numerator -= longest;
			x += dx1;
			y += dy1;
		}
		else 
		{
			x += dx2;
			y += dy2;
		}
	}
}

void DrawRect(int x, int y, int width, int height, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	for (int i = 0; i < height; i++)
	{
		DrawLine(x, (y + i), width, (y + i), color, buffer, bufferWidth, bufferHeight);
	}
}

void DrawTriangle(TRIANGLE tri, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	/*
		1
	   / \
	  /	  \
	 /	   \
	2-------3

	1 = tri.top
	2 = tri.left
	3 = tri.right
	*/

	//2->1
	DrawLine(tri.left.x, tri.left.y, tri.top.x, tri.top.y, color, buffer, bufferWidth, bufferHeight);
	//2->3
	DrawLine(tri.left.x, tri.left.y, tri.right.x, tri.right.y, color, buffer, bufferWidth, bufferHeight);
	//3->1
	DrawLine(tri.right.x, tri.right.y, tri.top.x, tri.top.y, color, buffer, bufferWidth, bufferHeight);
}

void DrawCircle(int x, int y, int radius, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	int x1 = radius;
	int y1 = 0;
	int decisionOver2 = 1 - x1;

	while (y1 <= x1)
	{
		Plot(x1 + x, y1 + y,color, buffer, bufferWidth, bufferHeight); // Octant 1
		Plot(y1 + x, x1 + y ,color, buffer, bufferWidth, bufferHeight); // Octant 2
		Plot(-x1 + x, y1 + y,color, buffer, bufferWidth, bufferHeight); // Octant 4
		Plot(-y1 + x, x1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 3
		Plot(-x1 + x, -y1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 5
		Plot(-y1 + x, -x1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 6
		Plot(x1 + x, -y1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 7
		Plot(y1 + x, -x1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 8
		y1++;
		if (decisionOver2 <= 0)
		{
			decisionOver2 += 2 * y1 + 1;
		}
		else
		{
			x1--;
			decisionOver2 += 2 * (y1 - x1) + 1;
		}
	}
}

//Funny story about this one, I accidentally wrote it by fucking up the Circle algo
void DrawChevron(int x, int y, int radius, int color, int* buffer, int bufferWidth, int bufferHeight)
{
	int x1 = radius;
	int y1 = 0;
	int decisionOver2 = 1 - x1;

	while (y1 <= x1)
	{
		Plot(x1 + x, y1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 1
		Plot(y1 + x, x1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 2
		Plot(-x1 + x, y1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 4
		Plot(-y1 + x, x1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 3
		Plot(-x1 + x, -y1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 5
		Plot(-y1 + x, -x1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 6
		Plot(x1 + x, -y1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 7
		Plot(y1 + x, -x1 + y, color, buffer, bufferWidth, bufferHeight); // Octant 8

		y1++;

		if (decisionOver2 <= 0)
		{
			decisionOver2 += 2 * y + 1;
		}
		else 
		{
			x--;
			decisionOver2 += 2 * (y - x) + 1;
		}
	}
}