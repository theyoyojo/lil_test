# Quick and dirty and relatively generic Makefile
# By Joel Savitz <jsavitz@redhat.com>

CC 	= gcc
CFLAGS  = -g -Wall -Werror -std=gnu11
OBJECTS = lil_db_test.o lil_db.o
BIN	= test_driver
SRCDIR  = src
OBJDIR  = obj

all: $(OBJDIR) $(OBJECTS)
	$(CC) $(CFLAGS) $(patsubst %.o,$(OBJDIR)/%.o, $(OBJECTS)) -o $(BIN)

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $(OBJDIR)/$@ 

.PHONEY: clean $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(BIN) $(OBJDIR)
