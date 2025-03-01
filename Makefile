CC = clang
CFLAGS = -Wall -Wextra -O0 -Weverything -Wno-int-conversion -Wno-implicit-function-declaration
LDFLAGS = -framework IOKit -framework CoreFoundation 

TARGETS = jop jop2 jop3
SRC_jop = jop.c
SRC_jop2 = jop2.c
SRC_jop3 = jop3.c

all: $(TARGETS)

jop: $(SRC_jop)
	$(CC) $(CFLAGS) $(SRC_jop) -o jop $(LDFLAGS)

jop2: $(SRC_jop2)
	$(CC) $(CFLAGS) $(SRC_jop2) -o jop2 $(LDFLAGS)

jop3: $(SRC_jop3)
	$(CC) $(CFLAGS) $(SRC_jop3) -o jop3 $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean