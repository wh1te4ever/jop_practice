CC = clang
CFLAGS = -Wall -Wextra -O2 -Weverything
LDFLAGS = -framework IOKit -framework CoreFoundation 

TARGET = main
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all sign clean