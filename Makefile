CC      = g++
CFLAGS  = -O3 -fmax-errors=1 -fno-stack-protector -Wno-deprecated-declarations

#-lfftw3f float precision, lfftw3 double precision
# LDFLAGS = -lglut -lGLEW -lGL -lfftw3f -lasound -lpthread
LDFLAGS = -lglut -lGLEW -lGL -lfftw3f -lasound -lpthread -lpulse-simple -lpulse

OBJ     = globals.o things.o main.o
                                                                                
all: $(OBJ)
	$(CC) -g $(OBJ) $(LDFLAGS)
                                                                                
%.o: %.cpp
	$(CC) $(CFLAGS) -c -g $<
                                                                                
clean:
	rm -f *.o a.out
