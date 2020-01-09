CC      = g++
# CFLAGS  = -g -fmax-errors=1 -fno-stack-protector -Wno-deprecated-declarations
CFLAGS  = -Werror -O3 -fmax-errors=1 -fno-stack-protector -Wno-deprecated-declarations

#-lfftw3f -> float precision, lfftw3 -> double precision
LDFLAGS = -lglut -lGLEW -lGL -lfftw3f -lpthread -lpulse-simple -lpulse

OBJ = globals.o things.o main.o
                                                                                
all: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS)

### wtf?
# %.o: %.cpp %.h globals.h
# 	$(CC) $(CFLAGS) -c $<

main.o: main.cpp globals.h
	$(CC) $(CFLAGS) -c main.cpp

globals.o: globals.cpp globals.h
	$(CC) $(CFLAGS) -c globals.cpp

things.o: things.cpp things.h globals.h
	$(CC) $(CFLAGS) -c things.cpp

clean:
	rm -f *.o a.out
