TARGET  := libnemo-sims.so
INSPATH	:= /usr/lib/nemo/extensions-3.0

CC		:= gcc
RM		:= rm -rf

CFLAGS  := -g -Wall -O0
CFLAGS	+= `pkg-config libnemo-extension --cflags` `pkg-config libnemo-extension --libs`
SHARE   := -fPIC -shared

SRCS	:= $(wildcard *.c)

$(TARGET):$(SRCS)
	$(CC) $(SHARE) -o $@ $(SRCS) $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(TARGET)
install:
	cp $(TARGET) $(INSPATH)
