CFLAGS   = -g `sdl2-config --cflags`
LDFLAGS  = `sdl2-config --libs` -lSDL2 -lSDL2_image -lSDL2_gfx
PROG = kstris
CXX = gcc

OBJS = main.o graphics.o input.o init.o

# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
%.o: src/%.c src/%.h src/defs.h
	echo "Hello"
	$(CXX) $(CFLAGS) -c $<

# linking the program
$(PROG): $(OBJS)
	echo "Hi"
	$(CXX) $(OBJS) -o $(PROG) $(LDFLAGS)

# cleaning everything that can be automatically recreated with "make"
clean:
	echo "Howdy"
	rm $(PROG) *.o
