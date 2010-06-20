/*
 * waveformGenerator.c
 *
 *  Created on: 13 May 2010
 *  Author: Matthew Page
 */
#include	<unistd.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<stdbool.h>
#include 	<stdarg.h>
#include	<time.h>
#include	<gd.h>
#include	<gdfonts.h>

#include	"waveformGenerator.h"
#include	"pcm.h"
#include	"waveImage.h"

int verbosity = MSG_DEBUG+1;

extern char* outputPath;
extern char* baseFileName;
extern char* startTime;
extern int peaksPerSecond;
extern int secondsPerFile;

char* inputFile;
int waitTime = 10;
int fileOffset = 0;
int sampleRate = 44100;
int channels = 1;
float peakCoEfficient;


static int readArguments(int argc, char** argv)
{
	int opts;
	for (opts = 1; opts < argc; opts++) {

		if (argv[opts][0] == '-') {
			switch (argv[opts][1]) {
			case 'i':
				inputFile = argv[++opts];
				break;
			case 'o':
				outputPath = argv[++opts];
				break;
			case 'f':
				baseFileName = argv[++opts];
				break;
			case 'r':
				sampleRate = atoi(argv[++opts]);
				break;
			case 'p':
				peaksPerSecond = atoi(argv[++opts]);
				break;
			case 's':
				fileOffset = atoi(argv[++opts]);
				break;
			case 'c':
				channels = atoi(argv[++opts]);
				break;
			case 'w':
				waitTime = atoi(argv[++opts]);
				break;
			case 't':
				startTime = argv[++opts];
				break;
			case 'l':
				secondsPerFile = atoi(argv[++opts]);
				break;
			}
		}
	}

	if (inputFile == NULL)
	{
		log_message(MSG_WARN, "You must pass an input file. Use -i <input file>\n");
		return 1;
	}

	if (outputPath == NULL)
	{
		log_message(MSG_WARN, "You must pass an output file. Use -o <output file>\n");
		return 1;
	}

	switch (channels)
	{
		case 2:
			channels = 2;
			break;
		default:
			channels = 1;
			break;
	}

	switch (sampleRate)
	{
		case 48000:
			sampleRate = 48000;
			break;
		default:
			sampleRate = 44100;
			break;
	}

	peakCoEfficient = (float) peaksPerSecond / (float) sampleRate;

	log_message(MSG_INFO, "output path: %s\n", outputPath);
	log_message(MSG_INFO, "input file: %s\n", inputFile);
	log_message(MSG_INFO, "base file name: %s\n", baseFileName);
	log_message(MSG_INFO, "file offset: %d\n", fileOffset);
	log_message(MSG_INFO, "sample rate: %d\n", sampleRate);
	log_message(MSG_INFO, "peaks per second: %d\n", peaksPerSecond);
	log_message(MSG_INFO, "channels: %d\n", channels);
	log_message(MSG_INFO, "wait time: %d\n", waitTime);
	log_message(MSG_INFO, "start time: %s\n", startTime);
	log_message(MSG_INFO, "seconds per file: %d\n", secondsPerFile);

	return 0;
}

/*
 * Log
 */
void log_message(int type, const char *format, ...)
{
	va_list args;
	va_start( args, format );
	if (type < verbosity)
	{
		vfprintf(stderr, format, args );
	}
	va_end( args );
}

/*
 * Main Function
 */
int main(int argc, char** argv) {
	log_message(MSG_INFO, "waveformGenerator v%02.02f (c)Matthew Page, 2010\n", VERSION);

	int result = readArguments(argc, argv);
	if (result != 0)
	{
		// didn't get all the details we need so exiting
		log_message(MSG_INFO, "Exiting due to lack of information.");
		return 1;
	}

	initialiseTime(startTime);

	/*
	 * Initialise sample variables
	 */
	short sampleBuffer[16384];
	int samplesRead = 0;
	int sampleCount = 0;
	short currentSample = 0;
	short currentChannel = 0;
	peaks_t peaks[2];

	/*
	 * Initialise file access
	 */
	bool continueReading = true;
	bool reachedEndOfFile = false;
	long filePosition = 0;
	FILE *fpi;

	fpi = open_pcm(inputFile);
	seek_pcm(fpi, fileOffset);

	startImageFile();

	/*
	 * Read input file and generate peaks
	 */
	while (continueReading == true)
	{
		samplesRead = fread(sampleBuffer, 2, sizeof(sampleBuffer) / 2, fpi);
		if (ferror(fpi))
		{
			log_message(MSG_ERROR, "Error reading from file\n");
		}
		//printf("bytes read = %d\n", samplesRead);

		if (samplesRead > 0)
		{
			int i;
			for (i = 0; i < samplesRead; i++) {
				currentSample = sampleBuffer[i];
				if (peaks[currentChannel].high > currentSample)
					peaks[currentChannel].high = currentSample;
				if (peaks[currentChannel].low < currentSample)
					peaks[currentChannel].low = currentSample;

				if ((sampleCount * peakCoEfficient) >= 1) {

					if (channels == 2)
					{
						drawStereoPeak(peaks[0], peaks[1]);
					}
					else
					{
						drawMonoPeak(peaks[0]);
					}

					peaks[0].low = 0;
					peaks[0].high = 0;
					peaks[1].low = 0;
					peaks[1].high = 0;

					sampleCount = 0;
				}

				if (channels == 2)
				{
					if (currentChannel == 0)
					{
						currentChannel = 1;
					}
					else
					{
						sampleCount ++;
						currentChannel = 0;
					}
				}
				else
				{
					sampleCount ++;
				}
			}

			// reset reachedEndOfFile flag as we obviously haven't because we've just read some data
			if (reachedEndOfFile == true)
			{
				log_message(MSG_INFO, "File is still growing...\n");
				reachedEndOfFile = false;
			}
		}
		else
		{
			if (reachedEndOfFile == true)
			{
				// we've been here before so the file has stopped growing, so give up and finish
				continueReading = false;
				log_message(MSG_INFO, "File has stopped growing\n");
			}
			else
			{
				// wait for a while to see if the file is growing
				updateImageFile(false);

				log_message(MSG_INFO, "Waiting to see if file is still growing\n");
				reachedEndOfFile = true;
				filePosition = ftell(fpi);
				sleep(waitTime);

				close_pcm(fpi);
				fpi = open_pcm(inputFile);
				seek_pcm(fpi, filePosition);
			}
		}
	}


	endImageFile();
	close_pcm(fpi);

	log_message(MSG_INFO, "Complete");

	return 0;
}
