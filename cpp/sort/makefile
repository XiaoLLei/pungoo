SRCS=$(wildcard *.cpp)
TARGETS=$(patsubst %cpp, %out, $(SRCS))
CC=g++
CCFLAG=-g

release : $(TARGETS)
	-@echo "build complete"

%.out : %.cpp
	$(CC) $(CCFLAG) $< -o $@

.PHONY:clean
clean:
	-@rm -rf *.o *.out