#include <stdio.h>
#include "configuration.h"

int LoadConfig(Options* a)
{FILE* fp;

	char* annoying = (char*)malloc(4096);

	errno_t err = fopen_s(&fp, "config.bin", "ab+");

	if (!fp) return 0;

	fseek(fp, 0L, SEEK_END);
	int fileSize = ftell(fp);
	rewind(fp);

	if (fileSize<sizeof(Options))
	{
		Options options = { 0 };
		options.ContinuousLines = TRUE;
		options.ExpireDraw = TRUE;
		options.ExpireDrawAfter = 1000;
		options.MaxDisplaysSupported = 5;
		options.TargetTime = 0.25f;
		fwrite(&options, sizeof(Options), 1, fp);
		fclose(fp);
		a = &options;
		return 0;
	}

	fgets((char*)a, sizeof(Options), fp);
	fclose(fp);
	return 0;
}