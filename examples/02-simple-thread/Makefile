include ../examples.mak

DEPS =  main.c ../../library/src/gcctrace.c
OBJ = main.o ../../library/src/gcctrace.o

%.o: %.c $(DEPS)
	$(CC) -g -O0 -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -g -O0 -o $@ $^ $(CFLAGS) -lpthread -ldl -Wl,--export-dynamic

.PHONY: clean
clean:
	rm -f main.o ../../library/src/gcctrace.o main gcctrace.dump

