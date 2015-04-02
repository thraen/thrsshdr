CC      = g++
#CFLAGS  = 
#LDFLAGS = -lglut -lGLEW -lGL -lfftw3 -lasound 

#  fno-stack-protector schaelt stack smash protection aus, die gcc standardmaessig an hat. das kostet kleines bisschen
LDFLAGS = -fpermissive -lglut -lGLEW -lGL -lfftw3 -lasound -lpthread -fno-stack-protector
OBJ     = initalsa.o globals.o  things.o main.o
                                                                                
                                                                                
all: $(OBJ)
	$(CC) -g $(OBJ) $(LDFLAGS)
                                                                                
%.o: %.cpp
	$(CC) -c -g $<
                                                                                
clean:
	rm -f *.o
