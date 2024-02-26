CC=gcc
override INCLUDE_DIRS += -I. -I./include
override CCFLAGS += -std=gnu11 -O2 -Wall -Wextra $(INCLUDE_DIRS)
override LDFLAGS +=
OBJDIR := build
BINDIR := bin
TARGET := ${BINDIR}/main


SRC_FILES:=$(shell find . -iname "*.c")
OBJECTS:=$(patsubst %.c, $(OBJDIR)/%.o, $(SRC_FILES))
DEPENDENCIES:=$(patsubst %.c, $(OBJDIR)/%.d, $(SRC_FILES))

all: build

build: $(OBJECTS)
	@mkdir -p $(dir $(TARGET))
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@


.PHONY: clean

clean:
	if [ -d "$(OBJDIR)" ]; then rm -rf $(OBJDIR); fi
	if [ -d "$(BINDIR)" ]; then rm -rf $(BINDIR); fi

-include $(DEPENDENCIES)
