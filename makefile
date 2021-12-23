CC = g++
CFLAGS = -I. -lsfml-graphics -lsfml-window -lsfml-system
DEPS = tunicate.h
OBJ = tunicate.o main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

neural-evo: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
