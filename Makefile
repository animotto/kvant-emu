CC=gcc
CFLAGS=-Wall

default: emu

emu: kvant.c emu.c
	@echo "Make Kvant emulator"
	$(CC) $(CFLAGS) -c kvant.c
	$(CC) $(CFLAGS) -shared -o libkvant.so kvant.o
	$(CC) $(CFLAGS) -o emu emu.c -L. -lkvant -lreadline
	@echo "Done"

clean:
	@echo "Clean"
	@rm -f libkvant.so kvant.o emu

run: emu
	@export LD_LIBRARY_PATH=. && ./emu
