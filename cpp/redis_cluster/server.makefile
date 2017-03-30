# Redis Cluster Client Makefile
#
# Copyright (C) 2016 Jian Yi <eyjian at gmail dot com>
# This file is released under the BSD license, see the COPYING file
#
# Dependencies are stored in the Makefile.dep file. To rebuild this file
# Just use 'make dep > Makefile.dep', but this is only needed by developers.

PROGRAM=server

HIREDIS?=/usr
PREFIX?=/usr/local
INCLUDE_PATH?=include/r3c
LIBRARY_PATH?=lib

OPTIMIZATION?=-O3
DEBUG?= -g -ggdb
WARNINGS=-Wall -W -Wwrite-strings
REAL_CPPFLAGS=$(CPPFLAGS) $(ARCH) -I$(HIREDIS)/include -D__STDC_FORMAT_MACROS=1 -fstrict-aliasing -fPIC $(DEBUG) $(OPTIMIZATION) $(WARNINGS)
REAL_LDFLAGS=$(LDFLAGS) $(ARCH) ./libr3c.a $(HIREDIS)/lib/libhiredis.a -lpthread

CXX:=$(shell sh -c 'type $(CXX) >/dev/null 2>/dev/null && echo $(CXX) || echo g++')

all: $(PROGRAM)

# Deps (use make dep to generate this)
server.o: server.cpp

%.o: %.cpp
	$(CXX) -c $< $(REAL_CPPFLAGS)

$(PROGRAM): server.o
	$(CXX) -o $@ $^ $(REAL_LDFLAGS)

clean:
	rm -f $(PROGRAM) *.o
