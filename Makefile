CC = gcc
CFLAGS = -O2 -Wall

spiloop: spiloop.c
	$(CC) $(CFLAGS) spiloop.c -o spiloop

clean:
	rm -rf spiloop
 
