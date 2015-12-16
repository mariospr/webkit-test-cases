CC=gcc
DEPS=webkit2gtk-4.0
CFLAGS=`pkg-config --cflags $(DEPS)`
LIBS=`pkg-config --libs $(DEPS)`

TARGETS = testcase.bin

all: $(TARGETS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.bin: %.o
	$(CC) -o $@ $< $(LIBS)

clean:
	rm -f *.bin *.o *~
