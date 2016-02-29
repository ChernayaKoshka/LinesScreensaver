#include <stdio.h>
#include "configuration.h"

int LoadConfig(Options* a)
{
	FILE* fp;

	char* directory;
	size_t directoryBufSize;
	_dupenv_s(&directory, &directoryBufSize, "appdata");

	int length = strnlen_s(directory, directoryBufSize);
	char* path = malloc(length + 17); //17 = length of "\\linesconfig.bin"
	strcpy_s(path, length + 17, directory);
	strcat_s(path, length + 17, "\\linesconfig.bin");

	fopen_s(&fp, path, "ab+");
	if (!fp) return 0;
	free(path);

	fseek(fp, 0L, SEEK_END);
	int fileSize = ftell(fp);
	rewind(fp);

	if (fileSize < sizeof(Options))
	{
		Options options = { 0 };
		options.Rectangles = FALSE;
		options.ContinuousLines = TRUE;
		options.ExpireDraw = TRUE;
		options.DifferentScreenPerDisplay = TRUE;
		options.ExpireDrawAfter = 1000;
		options.MaxDisplaysSupported = 5;
		options.TargetTime = 0.25f;
		fwrite(&options, sizeof(Options), 1, fp);
		fclose(fp);
		memcpy_s(a, sizeof(Options), &options, sizeof(Options));
		return 0;
	}

	fgets((char*)a, sizeof(Options), fp);
	fclose(fp);
	return 0;
}