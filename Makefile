CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror 
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

CFILES = main.c
#HFILES =

TARGET = tp2
GRUPO = G14

build: $(CFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(CFILES)

run: build
	./$(EXEC_S)

valgrind: build
	valgrind $(VFLAGS) ./$(EXEC_S)

gdb: build
	gdb $(GDBFLAGS) ./$(EXEC_S)

clean:
	rm -f *.o $(EXEC_S)

zip:
	zip $(GRUPO).zip $(CFILES) $(HFILES)

#entregar: build
#	$(MAKE) run; test $$? -eq 0
#	$(MAKE) valgrind; test $$? -eq 0
#	$(MAKE) clean
#	zip $(GRUPO).zip $(CFILES) $(HFILES)
#	@echo "Perfecto, no falla ni pierde memoria"
#	@echo "El zip esta listo para entregar"
