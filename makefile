DIST = build
TARGET = main
CC = clang

SRCS = main.c cpu.c ram.c disk.c net.c utils.c
OBJS = $(addprefix $(DIST)/, $(SRCS:.c=.o))
OUTPUT = $(DIST)/$(TARGET)

.PHONY: all clean

all: $(OUTPUT)

clean:
	rm -rf $(DIST)/$(TARGET) $(OBJS)

$(OUTPUT): $(OBJS)
	$(CC) -o $(OUTPUT) $(OBJS) $(CFLAGS)

$(DIST)/%.o: %.c | $(DIST)
	$(CC) $(CFLAGS) -c $< -o $@

$(DIST):
	mkdir -p $(DIST)
