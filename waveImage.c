/*
 * waveImage.c
 *
 *  Created on: 25 May 2010
 *  Last Update: 20 June 2010
 *  Author: Matthew Page
 */
#include 	<stdlib.h>
#include 	<stdio.h>
#include	<stdbool.h>
#include	<syslog.h>
#include	<time.h>
#include	<gd.h>
#include	<gdfonts.h>

#include 	"waveformGenerator.h"
#include	"waveImage.h"

struct tm * currentTime;
int imageHeight = 175;
int imageWidth = 480;
int waveHeight = 150;
int secondsPerTick = 5;
int waveformColour;
int textColour;
int currentFile = 0;
int currentPixel = 0;
bool imageHasChanged = false;
gdImagePtr imageHandle;
gdFontPtr fontHandle;

/*
 * Create a time object to create the time line
 */
void initialiseTime(char* startTime) {
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
void incrementTime(int s) {
	currentTime->tm_sec += s;
	mktime(currentTime);
}

/*
 * draw a timeline on a newly created image
 */
void drawTimeLine() {
	int lineHeight = imageHeight - 10;
	int textHeight = imageHeight - 22;
	int pixelsPerTick = gConfig.peaksPerSecond * secondsPerTick;

	int i;
	for (i = 0; i < (imageWidth + pixelsPerTick); i += pixelsPerTick) {
		int position = i;
		gdImageLine(imageHandle, position, lineHeight, position, imageHeight,
				textColour);

		char newTime[8];
		sprintf(newTime, "%02d:%02d:%02d", currentTime->tm_hour,
				currentTime->tm_min, currentTime->tm_sec);
		gdImageString(imageHandle, fontHandle, position - 24, textHeight,
				newTime, textColour);
		incrementTime(secondsPerTick);
	}

	imageHasChanged = true;
}

/*
 * save the current image to a file
 */
void updateImageFile(bool finished) {
	char imageFileName[256];
	sprintf(imageFileName, "%s%s%d.grow.png", gConfig.outputPath,
			gConfig.baseFileName, currentFile);

	if (imageHasChanged == true) {
		debug(LOG_DEBUG, "Writing image file: %s", imageFileName);
		FILE *fpimg;
		fpimg = fopen(imageFileName, "wb");

		if (fpimg == NULL) {
			debug(LOG_ALERT, "Can't open file for output: %s", imageFileName);
			exit(1);
		}

		gdImagePng(imageHandle, fpimg);
		fclose(fpimg);

		imageHasChanged = false;
	} else {
		debug(LOG_DEBUG, "Image hasn't changed, so not writing");
	}

	if (finished == true) {
		char newFileName[256];
		sprintf(newFileName, "%s%s%d.png", gConfig.outputPath,
				gConfig.baseFileName, currentFile);
		debug(LOG_DEBUG, "Renaming image file: %s", newFileName);
		rename(imageFileName, newFileName);
	}
}

/*
 * create a new image in memory
 */
void startImageFile() {
	debug(LOG_DEBUG, "Starting a new image file");

	imageWidth = gConfig.peaksPerSecond * gConfig.secondsPerFile; // one minute per file

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
void endImageFile() {
	debug(LOG_DEBUG, "Closing image file");

	updateImageFile(true);
	currentFile++;
	incrementTime(-secondsPerTick);
}

/*
 * draw a mono peak onto current image at current pixel
 */
void drawMonoPeak(peaks_t peaks) {
	if (currentPixel == imageWidth) {
		startImageFile();
	}

	int origin = waveHeight / 2;
	int pixelMultiplier = origin * 1.5;

	int peakHigh = (abs(peaks.high) * 0.000030518) * pixelMultiplier;
	int peakLow = (abs(peaks.low) * 0.000030518) * pixelMultiplier;

	gdImageLine(imageHandle, currentPixel, origin - peakHigh, currentPixel,
			origin + peakLow, waveformColour);
	currentPixel++;

	if (currentPixel == imageWidth) {
		endImageFile();
	}

	imageHasChanged = true;
}

/*
 * draw a stereo peak onto current image at current pixel
 */
void drawStereoPeak(peaks_t left, peaks_t right) {
	if (currentPixel == imageWidth) {
		startImageFile();
	}

	int peakHigh;
	int peakLow;
	int quarter = waveHeight / 4;
	int pixelMultiplier = quarter * 1.5;

	int origin = quarter;
	peakHigh = (abs(left.high) * 0.000030518) * pixelMultiplier;
	peakLow = (abs(left.low) * 0.000030518) * pixelMultiplier;
	gdImageLine(imageHandle, currentPixel, origin - peakHigh, currentPixel,
			origin + peakLow, waveformColour);

	origin = quarter * 3;
	peakHigh = (abs(right.high) * 0.000030518) * pixelMultiplier;
	peakLow = (abs(right.low) * 0.000030518) * pixelMultiplier;
	gdImageLine(imageHandle, currentPixel, origin - peakHigh, currentPixel,
			origin + peakLow, waveformColour);

	currentPixel++;

	if (currentPixel == imageWidth) {
		endImageFile();
	}

	imageHasChanged = true;
}

