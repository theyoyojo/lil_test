# Quick and dirty and relatively generic Makefile
# By Joel Savitz <jsavitz@redhat.com>

CC 	= gcc
CFLAGS  = -g -Wall -Werror -std=gnu11
OBJECTS = lil_db_test.o lil_db.o
BIN	= test_driver

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONEY: clean
clean:
	rm -rf $(BIN) $(OBJECTS)
