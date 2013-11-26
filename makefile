CC=gcc
CFLAGS=-I.
DEPS = waveImage.h waveformGenerator.h
OBJ = waveImage.o waveformGenerator.o 
LIBS=-lm -lgd

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

wavegen: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
