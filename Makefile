CC      = g++
CFLAGS  = -O3 -Wfatal-errors -fno-stack-protector 

#-lfftw3f float precision, lfftw3 double precision
LDFLAGS = -lglut -lGLEW -lGL -lfftw3f -lasound -lpthread

OBJ     = globals.o things.o main.o
                                                                                
all: $(OBJ)
	$(CC) -g $(OBJ) $(CFLAGS) $(LDFLAGS)
                                                                                
%.o: %.cpp
	$(CC) -c -g $<
                                                                                
clean:
	rm -f *.o a.out
