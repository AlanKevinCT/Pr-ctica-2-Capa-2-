CC     = gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lpcap

all: emisor receptor

emisor: src/emisor.c
	$(CC) $(CFLAGS) -o emisor src/emisor.c $(LDLIBS)

receptor: src/receptor.c
	$(CC) $(CFLAGS) -o receptor src/receptor.c $(LDLIBS)

clean:
	rm -f emisor receptor *.o