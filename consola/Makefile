# Globales
CC := gcc
CFLAGS := -Wall -Werror
LDFLAGS := -L../shared/debug -lshared
INCLUDE_PATH := ../shared/src
MODULE_NAME := ./debug/consola.out
SOURCE_FILES := $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJECT_FILES := $(SOURCE_FILES:%.c=./debug/%.o)
LINK = -lpthread -lcommons -lreadline -lm

### Reglas ###

all: makefolder $(MODULE_NAME)

$(MODULE_NAME): $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -I$(INCLUDE_PATH) $(LINK)

./debug/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_PATH) $(LINK)

makefolder:
	if [ ! -d ./debug ]; then mkdir debug; fi

makeshared:
	make -C ../shared

# Clean

clean:
	rm -rf debug
	rm -rf *.out
	rm -rf *.log
#make clean -C ../shared
	rm -f $(MODULE_NAME) $(OBJECT_FILES)