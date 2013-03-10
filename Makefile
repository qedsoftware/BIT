# William Wu, 2011-06-01

# choice of compiler 
CC = gcc

# debugging hook, all warnings, treat warnings as errors
CFLAGS = -g -Wall -Werror

# name of final program
OUTPUT = bit

# all -- build program and test it
all: $(OUTPUT) 

# linking -- put object files together into machine-code
$(OUTPUT): bit.o 
	$(CC) $(CFLAGS) bit.o -o $(OUTPUT)

# compilation -- create individual assembly-language object files
bit.o: bit.c 
	$(CC) $(CFLAGS) -c bit.c
	
# clean -- remove object files and program	
clean:
	-rm -rf *.o $(OUTPUT) 