/*
 * waveformGenerator.h
 *
 *  Created on: 22 May 2010
 *  Last modified: 20 June 2010
 *  Author: Matthew Page
 */

#ifndef WAVEFORMGENERATOR_H_
#define WAVEFORMGENERATOR_H_

#define VERSION 	1.02

enum
{
	MSG_ERROR=-2,
	MSG_WARN,
	MSG_INFO,
	MSG_DETAIL,
	MSG_DEBUG
};

struct wg_config
{
	char* inputFile;
	char* outputPath;
	char* baseFileName;
	char* startTime;

	int sampleRate;
	int channels;
	int waitTime;

	long fileStartOffset;

	int peaksPerSecond;
	int secondsPerFile;
}
typedef wg_config;
wg_config gConfig;

/*
 * structure to hold peak data
 */
struct peaks_t
{
	short high;
	short low;
}
typedef peaks_t;


void log_message( int , const char *, ... );

#endif /* WAVEFORMGENERATOR_H_ */
