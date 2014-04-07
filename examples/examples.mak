# Common make configuration for examples

CFLAGS=-I ../../library/include/ -finstrument-functions -finstrument-functions-exclude-function-list=_gcc_trace_

ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    CC=gcc

    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -D IA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CC=gcc
        CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CC=gcc-mp-4.8
        CCFLAGS += -D OSX
    endif

    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

