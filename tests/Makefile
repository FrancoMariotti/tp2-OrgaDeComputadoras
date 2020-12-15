CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror 
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

CFILES = test_cache.c ../cache.c 
HFILES = ../cache.h

TARGET = tests_tp2

build: $(CFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(CFILES)

run: build
	./$(EXEC_S)

valgrind: build
	valgrind $(VFLAGS) ./$(EXEC_S)

gdb: build
	gdb $(GDBFLAGS) ./$(EXEC_S)

clean:
	rm -f *.o $(TARGET)

zip:
	zip $(GRUPO).zip $(CFILES) $(HFILES)
