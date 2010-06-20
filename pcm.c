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
		log_message(MSG_ERROR, "Can't open input file %s\n", filename);
		exit(1);
	}

	return fp;
}

void seek_pcm(FILE *fp, long pos)
{
	fseek(fp, pos, 0);
	if (ferror(fp))
	{
		log_message(MSG_ERROR, "Error seeking within file\n");
	}
}

void close_pcm(FILE *fp)
{
	fclose(fp);
}
