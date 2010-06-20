/*
 * waveImage.h
 *
 *  Created on: 25 May 2010
 *      Author: matthew
 */

#ifndef WAVEIMAGE_H_
#define WAVEIMAGE_H_

/*
typedef struct wf_t
{
	char* outputPath;
	char* baseFilename;
	int peaksPerSecond;
	int secondsPerFile;
	int imageHeight;
	int imageWidth;
	int waveHeight;
	int secondsPerTick;
	char* startTime;
} wf_t;
*/

void initialiseTime(char* startTime);
void updateImageFile(bool finished);
void startImageFile();
void endImageFile();
void drawMonoPeak(peaks_t peaks);
void drawStereoPeak(peaks_t left, peaks_t right);

#endif /* WAVEIMAGE_H_ */
