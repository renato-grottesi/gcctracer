DEPS=src/gcctracer.c src/memwrapper.c
OBJ=src/gcctracer.o src/memwrapper.c

CFLAGS=-Iinclude 
CFLAGS+=-finstrument-functions 
CFLAGS+=-finstrument-functions-exclude-function-list=_gcc_trace_ 
CFLAGS+=-fPIC
CFLAGS+=-Wall -Wextra -pedantic -Werror
CFLAGS+=-g -O0

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

libgcctracer.so: $(OBJ)
	$(CC) -shared -o $@ $^ $(CFLAGS) -lpthread -ldl -Wl,--export-dynamic

.PHONY: all
all: libgcctracer.so

.PHONY: install
install:
	cp libgcctracer.so /usr/lib/
	cp include/gcctracer.h /usr/include/

.PHONY: clean
clean:
	rm -f src/*.o libgcctracer.so

