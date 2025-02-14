CC = gcc -Wall -g

# executable directory
BDIR = $(PWD)

# Programmes maitre et esclave
PROG = upperMaitre upperEsclave


EXEC = $(PROG:%=$(BDIR)/%)

PVM_LIB = -lpvm3
CFLAGS =  -DEPATH=\"$(BDIR)\"

#### generation rules #####

all: $(EXEC)
	
$(BDIR)/upperMaitre: point.o upperMaitre.o pb.o
	$(CC) -o $@ upperMaitre.o point.o pb.o $(PVM_LIB)

$(BDIR)/upperEsclave: point.o upperEsclave.o pb.o
	$(CC) -o $@ upperEsclave.o point.o pb.o $(PVM_LIB)

.c.o: point.h
	$(CC) $(CFLAGS) $(ARCHCFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)
