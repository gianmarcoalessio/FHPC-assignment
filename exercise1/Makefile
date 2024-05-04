CC=mpicc -O3 -fopenmp -march=native -g -Iinclude
location ?= .  # default location is current directory if not specified
OBJDIR=obj
INCDIR=include

RW=$(OBJDIR)/read_write_pgm.o
STATIC= $(OBJDIR)/static_evolution.o
ORDERED= $(OBJDIR)/ordered_evolution.o
OBJECTS=main.o $(RW) $(STATIC) $(ORDERED)

par: $(location)/main.x

$(location)/main.x: $(OBJECTS)
	$(CC) -lm $(OBJECTS) -o $@

main.o: main.c
	$(CC) -c main.c

$(OBJDIR)/%.o: src/%.c $(INCDIR)/%.h
	mkdir -p $(OBJDIR)
	$(CC) -c $< -o $@

clean: x_clean clean_img

x_clean:
	rm -rf $(location)/*.x $(OBJDIR)/*.o $(OBJDIR)/*.a

clean_img:
	rm $(location)/*.pgm
