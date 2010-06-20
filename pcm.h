/*
 * pcm.h
 *
 *  Created on: 25 May 2010
 *      Author: matthew
 */

#ifndef PCM_H_
#define PCM_H_

FILE* open_pcm(char *filename);
void seek_pcm(FILE *fp, long pos);
void close_pcm(FILE *fp);

#endif /* PCM_H_ */
