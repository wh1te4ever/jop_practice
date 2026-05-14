CC = clang
CFLAGS = -Wall -Wextra -O0 -Weverything -Wno-int-conversion -Wno-implicit-function-declaration
LDFLAGS = -framework IOKit -framework CoreFoundation 

TARGETS = jop jop2 jop3 jop4 jop_claude
SRC_jop = jop.c
SRC_jop2 = jop2.c
SRC_jop3 = jop3.c
SRC_jop4 = jop4.c
SRC_jop_claude = jop_claude.c

all: $(TARGETS)

jop: $(SRC_jop)
	$(CC) $(CFLAGS) $(SRC_jop) -o jop $(LDFLAGS)

jop2: $(SRC_jop2)
	$(CC) $(CFLAGS) $(SRC_jop2) -o jop2 $(LDFLAGS)

jop3: $(SRC_jop3)
	$(CC) $(CFLAGS) $(SRC_jop3) -o jop3 $(LDFLAGS)

jop4: $(SRC_jop4)
	$(CC) $(CFLAGS) $(SRC_jop4) -o jop4 $(LDFLAGS)

SRC_jop_claude: $(SRC_jop_claude)
	$(CC) $(CFLAGS) $(SRC_jop_claude) -o jop4 $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean