/*
 * pcm.c
 *
 *  Created on: 25 May 2010
 *      Author: matthew
 */
#include <stdlib.h>
#include <stdio.h>

#include "waveformGenerator.h"

FILE* open_pcm(char *filename)
{
	FILE *fp;
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		debug(LOG_ALERT, "Can't open input file %s", filename);
		exit(1);
	}

	return fp;
}

void seek_pcm(FILE *fp, long pos)
{
	fseek(fp, pos, 0);
	if (ferror(fp))
	{
		debug(LOG_ALERT, "Error seeking within file");
	}
}

void close_pcm(FILE *fp)
{
	fclose(fp);
}
