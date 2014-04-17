include ../examples.mak

DEPS =  main.c
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -g -O0 -c -o $@ $< $(CFLAGS)

basics: $(OBJ)
	$(CC) -g -O0 -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f main.o basics gcctrace.dump

