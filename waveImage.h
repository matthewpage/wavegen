/*
 * waveImage.h
 *
 *  Created on: 25 May 2010
 *  Last Update: 20 June 2010
 *  Author: Matthew Page
 */

#ifndef WAVEIMAGE_H_
#define WAVEIMAGE_H_

void initialiseTime(char* startTime);
void updateImageFile(bool finished);
void startImageFile();
void endImageFile();
void drawMonoPeak(peaks_t peaks);
void drawStereoPeak(peaks_t left, peaks_t right);

#endif /* WAVEIMAGE_H_ */
