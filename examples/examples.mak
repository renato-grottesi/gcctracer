# Common make configuration for examples

LDFLAGS=-lgcctracer -Wl,--export-dynamic

CFLAGS=-finstrument-functions -Wall -Wextra -pedantic -Werror

ifeq ($(OS),Windows_NT)
    CFLAGS += -DWIN32
    CC=gcc

    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CFLAGS += -DAMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CFLAGS += -DIA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CC=gcc
        CFLAGS += -DLINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CC=gcc-mp-4.8
        CFLAGS += -DOSX
    endif

    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CFLAGS += -DAMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CFLAGS += -DIA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CFLAGS += -DARM
    endif
endif

