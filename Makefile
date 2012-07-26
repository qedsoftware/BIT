# William Wu, 2011-06-01

# choice of compiler 
CC = gcc

# debugging hook, all warnings, treat warnings as errors
CFLAGS = -g -Wall -Werror

# name of final program
OUTPUT = dump_binary

# all -- build program and test it
all: $(OUTPUT) 

# linking -- put object files together into machine-code
$(OUTPUT): dump_binary.o 
	$(CC) $(CFLAGS) dump_binary.o -o $(OUTPUT)

# compilation -- create individual assembly-language object files
dump_binary.o: dump_binary.c 
	$(CC) $(CFLAGS) -c dump_binary.c
	
# clean -- remove object files and program	
clean:
	-rm -rf *.o $(OUTPUT) 