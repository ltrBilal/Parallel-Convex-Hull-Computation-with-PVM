# Cible
CIBLE = upper_par

# do not edit 

all: $(CIBLE)

clean:
	rm -f *.o $(CIBLE)

.c.o: 
	$(CC) -c $(CFLAGS) -DBPWD="\"`pwd`\"" -o $@ $< 

$(CIBLE): point.o upper_par.o
	$(CC) -o $@ $@.o $< -lpvm3