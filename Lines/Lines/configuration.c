#include <stdio.h>
#include "configuration.h"

static char* path = 0;

void SetPath()
{
	char* directory;
	size_t directoryBufSize;
	_dupenv_s(&directory, &directoryBufSize, "appdata");

	int length = strnlen_s(directory, directoryBufSize);
	path = malloc(length + 17); //17 = length of "\\linesconfig.bin"
	strcpy_s(path, length + 17, directory);
	strcat_s(path, length + 17, "\\linesconfig.bin");
}

BOOL WriteConfig(Options* opt)
{
	FILE* fp;
	if (path == 0) SetPath();
	fopen_s(&fp, path, "w");
	if (!fp) return FALSE;
	fwrite(opt, sizeof(Options), 1, fp);
	fclose(fp);
	return TRUE;
}

BOOL LoadConfig(Options* opt)
{
	FILE* fp;

	if (path == NULL) SetPath();

	fopen_s(&fp, path, "ab+");
	if (!fp) return FALSE;

	fseek(fp, 0L, SEEK_END);
	int fileSize = ftell(fp);
	rewind(fp);

	if (fileSize != sizeof(Options))
	{
		Options options = { 0 };
		options.DrawType = Lines;
		options.ContinuousLines = TRUE;
		options.DifferentScreenPerDisplay = TRUE;
		options.ExpireDraw = TRUE;
		options.ExpireDrawAfter = 500;
		options.MaxDisplaysSupported = 5;
		options.TargetTime = 0.10f;

		fclose(fp);
		WriteConfig(&options);

		memcpy_s(opt, sizeof(Options), &options, sizeof(Options));
		return TRUE;
	}

	fgets((char*)opt, sizeof(Options), fp);
	fclose(fp);
	return TRUE;
}