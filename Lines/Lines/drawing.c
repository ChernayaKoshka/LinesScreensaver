int Plot(int x, int y, int color, int* buffer, int bufferWidth)
{
	buffer[(y*bufferWidth) + x] = color;
	return 0;
}

int DrawLine(int x, int y, int x2, int y2, int color, int* buffer, int bufferWidth)
{
	int w = x2 - x;
	int h = y2 - y;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (w < 0) dx1 = -1; else if (w>0) dx1 = 1;
	if (h < 0) dy1 = -1; else if (h>0) dy1 = 1;
	if (w < 0) dx2 = -1; else if (w>0) dx2 = 1;
	int longest = Abs(w);
	int shortest = Abs(h);
	if (!(longest > shortest)) {
		longest = Abs(h);
		shortest = Abs(w);
		if (h < 0) dy2 = -1; else if (h>0) dy2 = 1;
		dx2 = 0;
	}
	int numerator = longest >> 1;
	for (int i = 0; i <= longest; i++) {
		Plot(x, y, color, buffer, bufferWidth);
		numerator += shortest;
		if (!(numerator < longest)) {
			numerator -= longest;
			x += dx1;
			y += dy1;
		}
		else {
			x += dx2;
			y += dy2;
		}
	}
	return 0;
}

int DrawRect(int x, int y, int width, int height, int* buffer, int bufferWidth)
{
	for (int i = 0; i < height; i++)
	{
		int trueHeight = (y + i);
		DrawLine(x, trueHeight, width, trueHeight, 0x00FF0000, buffer, bufferWidth);
	}
	return 0;
}