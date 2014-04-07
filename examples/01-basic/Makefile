include ../examples.mak

DEPS =  main.c ../../library/src/gcctrace.c
OBJ = main.o ../../library/src/gcctrace.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

.PHONY: clean
clean:
	rm -f main.o ../../library/src/gcctrace.o main

