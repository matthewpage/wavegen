/*
 * waveImage.c
 *
 *  Created on: 25 May 2010
 *      Author: matthew
 */
#include 	<stdlib.h>
#include 	<stdio.h>
#include	<stdbool.h>
#include	<time.h>
#include	<gd.h>
#include	<gdfonts.h>

#include 	"waveformGenerator.h"
#include	"waveImage.h"

char* outputPath;
char* baseFileName = "waveform_";
char* startTime = "00:00:00";
int peaksPerSecond = 16;
int secondsPerFile = 300;

struct tm * currentTime;
int imageHeight = 175;
int imageWidth = 480;
int waveHeight = 150;
int secondsPerTick = 5;
int waveformColour;
int textColour;
int currentFile = 0;
int currentPixel = 0;
gdImagePtr imageHandle;
gdFontPtr fontHandle;

/*
 * Create a time object to create the time line
 */
void initialiseTime(char* startTime)
{
	int hour;
	int min;
	int sec;
	sscanf(startTime, "%d:%d:%d", &hour, &min, &sec);

	time_t tmpTime;
	time(&tmpTime);
	currentTime = localtime(&tmpTime);
	currentTime->tm_hour = hour;
	currentTime->tm_min = min;
	currentTime->tm_sec = sec;
}

/*
 * add s seconds to the time object
 */
void incrementTime(int s)
{
	currentTime->tm_sec += s;
	mktime(currentTime);
}

/*
 * draw a timeline on a newly created image
 */
void drawTimeLine()
{
	int lineHeight = imageHeight - 10;
	int textHeight = imageHeight - 22;
	int pixelsPerTick = peaksPerSecond * secondsPerTick;

	int i;
	for (i = 0; i < (imageWidth + pixelsPerTick); i += pixelsPerTick)
	{
		int position = i;
		gdImageLine(imageHandle, position, lineHeight, position, imageHeight, textColour);

		char newTime[8];
		sprintf(newTime, "%02d:%02d:%02d", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
		gdImageString(imageHandle, fontHandle, position - 24, textHeight, newTime, textColour);
		incrementTime(secondsPerTick);
	}
}

/*
 * save the current image to a file
 */
void updateImageFile(bool finished)
{
	char imageFileName[256];
	sprintf(imageFileName, "%s%s%d.grow.png", outputPath, baseFileName, currentFile);

	log_message(MSG_DEBUG, "updateImageFile | Writing image file: %s\n", imageFileName);

	FILE *fpimg;
	fpimg = fopen(imageFileName, "wb");

	if (fpimg == NULL)
	{
		log_message(MSG_ERROR, "updateImageFile | Can't open file for output: %s\n", imageFileName);
		exit(1);
	}

	gdImagePng(imageHandle, fpimg);
	fclose(fpimg);

	if (finished == true)
	{
		char newFileName[256];
		sprintf(newFileName, "%s%s%d.png", outputPath, baseFileName, currentFile);
		log_message(MSG_DEBUG, "updateImageFile | Renaming image file: %s\n", newFileName);
		rename(imageFileName, newFileName);
	}
}

/*
 * create a new image in memory
 */
void startImageFile()
{
	log_message(MSG_DEBUG, "startImageFile | Starting a new image file\n");

	imageWidth = peaksPerSecond * secondsPerFile;	// one minute per file

	imageHandle = gdImageCreateTrueColor(imageWidth, imageHeight);
	fontHandle = gdFontGetSmall();
	waveformColour = gdImageColorAllocate(imageHandle, 0, 255, 0);
	textColour = gdImageColorAllocate(imageHandle, 255, 255, 255);
	currentPixel = 0;

	drawTimeLine();
}

/*
 * save the current image and increment the file count
 */
void endImageFile()
{
	log_message(MSG_DEBUG, "endImageFile | Closing image file\n");

	updateImageFile(true);
	currentFile ++;
	incrementTime(-secondsPerTick);
}

/*
 * draw a mono peak onto current image at current pixel
 */
void drawMonoPeak(peaks_t peaks)
{
	if (currentPixel == imageWidth)
	{
		startImageFile();
	}

	int origin = waveHeight / 2;
	int pixelMultiplier = origin * 1.5;

	int peakHigh = (abs(peaks.high) * 0.000030518) * pixelMultiplier;
	int peakLow = (abs(peaks.low) * 0.000030518) * pixelMultiplier;

	gdImageLine(imageHandle, currentPixel, origin - peakHigh, currentPixel, origin + peakLow, waveformColour);
	currentPixel++;

	if (currentPixel == imageWidth)
	{
		endImageFile();
	}
}

/*
 * draw a stereo peak onto current image at current pixel
 */
void drawStereoPeak(peaks_t left, peaks_t right)
{
	if (currentPixel == imageWidth)
	{
		startImageFile();
	}

	int peakHigh;
	int peakLow;
	int quarter = waveHeight / 4;
	int pixelMultiplier = quarter * 1.5;

	int origin = quarter;
	peakHigh = (abs(left.high) * 0.000030518) * pixelMultiplier;
	peakLow = (abs(left.low) * 0.000030518) * pixelMultiplier;
	gdImageLine(imageHandle, currentPixel, origin - peakHigh, currentPixel, origin + peakLow, waveformColour);

	origin = quarter * 3;
	peakHigh = (abs(right.high) * 0.000030518) * pixelMultiplier;
	peakLow = (abs(right.low) * 0.000030518) * pixelMultiplier;
	gdImageLine(imageHandle, currentPixel, origin - peakHigh, currentPixel, origin + peakLow, waveformColour);

	currentPixel++;

	if (currentPixel == imageWidth)
	{
		endImageFile();
	}
}

