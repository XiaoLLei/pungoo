MODULE_NAME = run

CC=gcc

BINDIR=.
OBJDIR=./obj

INC_PATH=
LIB_PATH=./lib/

$(shell mkdir -p $(OBJDIR))

CFLAGS = -g 

INC = $(INC_PATH) -I. -I./include -I./include/json-c
LIBS = -L$(LIB_PATH) -lghttp -ljson-c
OBJS = \
       $(OBJDIR)/webhdfs.o \
       $(OBJDIR)/main.o
OTHEROBJS = 

module = $(BINDIR)/$(MODULE_NAME)

release : $(module)
	-@echo "build complete"

$(module) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(OTHEROBJS) $(LIBS)

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -Wall -Wformat $(INC) -DTHREADED -DBUILD_TIME="$(BUILD_TIME)" -DPROJECT_VERSION="$(PROJECT_BUILD_VERSION)" -DMODULE_VERSION="\"$(VERSION).${BUILD_NUMBER}\"" -c $< -o $@

.PHONY:clean
clean:
	rm -rf $(OBJDIR) $(module)

.PHONY:files
files:
	rm -rf $(OBJS) $(module)

