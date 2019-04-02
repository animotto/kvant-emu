default: emu

emu: kvant.c emu.c
	@echo "Make Kvant emulator"
	gcc -Wall -c kvant.c
	gcc -shared -o libkvant.so kvant.o
	gcc -Wall -L. -lreadline -lkvant -o emu emu.c
	@echo "Done"

clean:
	@echo "Clean"
	@rm -f libkvant.so kvant.o emu

run: emu
	@export LD_LIBRARY_PATH=. && ./emu
