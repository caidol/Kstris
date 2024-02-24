CFLAGS   = -g `sdl2-config --cflags`
LDFLAGS  = `sdl2-config --libs` -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_ttf
PROG = Kstris
CXX = gcc

OBJS = main.o graphics.o input.o init.o game.o util.o

# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
%.o: src/%.c src/%.h src/defs.h
	$(CXX) $(CFLAGS) -c $<

# linking the program
$(PROG): $(OBJS)
	$(CXX) $(OBJS) -o $(PROG) $(LDFLAGS)
	rm *.o

# cleaning everything that can be automatically recreated with "make"
clean:
	rm $(PROG) *.o
