include ../examples.mak

DEPS =  main.c
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -g -O0 -c -o $@ $< $(CFLAGS)

threads: $(OBJ)
	$(CC) -g -O0 -o $@ $^ $(CFLAGS) -lpthread $(LDFLAGS)

.PHONY: clean
clean:
	rm -f main.o threads gcctrace.dump

