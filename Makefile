CC      = gcc
# CFLAGS  = -g -fmax-errors=1 -fno-stack-protector -Wno-deprecated-declarations
CFLAGS  = -Werror -O3 -fmax-errors=1 -fno-stack-protector -Wno-deprecated-declarations

#-lfftw3f -> float precision, lfftw3 -> double precision
LDFLAGS = -lm -lglfw -lGLEW -lGL -lfftw3f -lpthread -lpulse-simple -lpulse

OBJ = globals.o glcrap.o main.o
                                                                                
all: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS)

### wtf?
# %.o: %.c %.h globals.h
# 	$(CC) $(CFLAGS) -c $<

main.o: main.c windows.c globals.c globals.h
	$(CC) $(CFLAGS) -c main.c

globals.o: globals.c globals.c globals.h
	$(CC) $(CFLAGS) -c globals.c

glcrap.o: glcrap.c glcrap.h globals.c globals.h
	$(CC) $(CFLAGS) -c glcrap.c

clean:
	rm -f *.o a.out
