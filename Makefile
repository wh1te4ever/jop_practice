CC = clang
CFLAGS = -Wall -Wextra -O0 -Weverything -Wno-int-conversion -Wno-implicit-function-declaration
LDFLAGS = -framework IOKit -framework CoreFoundation 

TARGET = main
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all sign clean