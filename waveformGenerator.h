/*
 * waveformGenerator.h
 *
 *  Created on: 22 May 2010
 *      Author: matthew
 */

#ifndef WAVEFORMGENERATOR_H_
#define WAVEFORMGENERATOR_H_

#define VERSION 	1.01

enum
{
	MSG_ERROR=-2,
	MSG_WARN,
	MSG_INFO,
	MSG_DETAIL,
	MSG_DEBUG
};

/*
 * structure to hold peak data
 */
typedef struct peaks_t
{
	short high;
	short low;
} peaks_t;


void log_message( int , const char *, ... );

#endif /* WAVEFORMGENERATOR_H_ */
