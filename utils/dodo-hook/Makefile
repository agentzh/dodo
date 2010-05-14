NGX_HOME := ${HOME}/src/nginx-0.8.35
TARGET := dodo-hook.so
SRC := dodo-hook.c
INCS := \
	-I$(NGX_HOME)/objs \
	-I$(NGX_HOME)/src/core \
	-I$(NGX_HOME)/src/event \
	-I$(NGX_HOME)/src/http \
	-I$(NGX_HOME)/src/mail \
	-I$(NGX_HOME)/src/misc \
	-I$(NGX_HOME)/src/os/unix
CFLAGS := -g3 -O0 $(INCS)

all: $(TARGET)

$(TARGET): $(SRC)
	gcc -shared -fPIC $(CFLAGS) $^ -o $@

clean:
	-rm -f $(TARGET)

.PHONY: all clean

