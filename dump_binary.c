/* Dumps files to screen as binary, either in bits or bytes. */
/* William D. Wu, 1/19/2011 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		// getopt()
#include <sys/stat.h>	

#define byte unsigned char
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )

void bin_prnt_byte(int x, int nibble_spacing);
int get_file_size(const char *fpath);
void usage(void);


unsigned long int Endian_DWord_Conversion(unsigned long int dword)
{
   return ((dword>>24)&0x000000FF) | ((dword>>8)&0x0000FF00) | ((dword<<8)&0x00FF0000) | ((dword<<24)&0xFF000000);
}

int 
main(int argc, char** argv) 
{
	
	/* COMMAND-LINE PARAMETERS 
	-i [filename]		input file to dump bits from
	-n [integer]		number of bits to read out
	-t [integer]		byte offset to start read out from
	-x 					print hex values
	-s 					print as one long string (use with caution)
	-h 					help
	*/
	int g;
	int iflag=0, nflag=0, bflag=0, xflag=0, dflag=0, sflag=0, tflag=0, vflag=0;
	int nibble_spacing=1;
	int size;
	int n_bytes;
	int init_offset=0;
	FILE *sfp;
	char *input_filename = NULL;
	while ((g = getopt(argc,argv,"i:n:t:bxdshv")) != -1) {
		switch(g) {
			case 'i': {
				iflag=1;
				input_filename = optarg;
				if ((sfp = fopen(input_filename,"rb")) == NULL)  {	
					printf("Error: File %s could not be opened.\n",input_filename);
					return -1;
				}		
				break;
			}
			case 'n': {
				nflag=1;
				n_bytes = atoi(optarg);
				break;				
			}
			case 't': {
				tflag=1;
				init_offset = atoi(optarg);
				break;				
			}
			case 'b': {
				bflag=1;				
				break;
			}
			case 'x': {
				xflag=1;				
				break;
			}
			case 'd': {
				dflag=1;				
				break;
			}
			case 's': {
				sflag=1;				
				nibble_spacing=0;
				break;
			}
			case 'v': {
				vflag=1;
				break;
			}
			case 'h': {
				usage();
				return 1;
				break;
			}
			case '?': {
				usage();
				return 1;
				break;
			}
			default: {
				usage();
				return 1;
			}			
		}
	}
	// argument checking
	if (0==iflag) {
		printf("Please specify an input file with the -i option.\n");
		return 1;
	}
	if (0==bflag && 0==xflag && 0==dflag) {
		bflag = 1;
	}
	if (bflag + xflag + dflag > 1) {
		printf("Please specify only one of the following three options: -b (binary), -x (hex), and -d (decimal). The default output is binary.\n");
		return 1;
	}
	size = get_file_size(input_filename); // input file's size in bytes
	if (nflag) {
		if (n_bytes > size) 
			printf("Number of bytes requested is greater than the input file's size of %d bytes. Defaulting to %d.\n",size,size);
		if (n_bytes < 0)
			printf("Number of bytes requested is negative. Defaulting to input file's size of %d bytes.\n",size);
	}	

	/* MOVE OFFSET */
	if (0 != fseek(sfp,init_offset,SEEK_SET)) {
		printf("ERROR: Could not seek to position %d.",init_offset);
		exit(1);
	}

	/* BINARY DUMP */
	int byte_index_of_last_line = 16*(((nflag)?min(size-init_offset,n_bytes):(size-init_offset))/16);
	printf("byte index of last line: %d\n",byte_index_of_last_line);
	byte c;
	int byte_count = 0;
	if (sflag) {	// print as one long string
	    while(!feof(sfp)) {	// begin main loop
	        if (fread(&c, 1, 1, sfp) == 1) {
				byte_count++;
				if (dflag) 
					printf("%d",c);
				else if (xflag)
					printf("%02x",c); 				
				else 
					bin_prnt_byte(c,nibble_spacing);
			}
			if (nflag && byte_count == n_bytes) break;		// quit if we reach the byte quota
	    } // end main loop
	} else { // print with hexedit style formatting: one space between nibbles, tab between bytes, and newlines
		int i = 0;
		while(!feof(sfp)) {	// begin main loop
			putchar(' ');
			if (i % 16 == 0) {
				
				if (vflag) {
					if (i < byte_index_of_last_line) {
						printf("\nbytes %5d (%5d) to %5d (%5d):\t",i+init_offset,i,i+15+init_offset,i+15);
					} else if ( (nflag?n_bytes:size) % 16) {
						printf("\nbytes %5d (%5d) to %5d (%5d):\t",i+init_offset,i,
							(nflag)?( min(n_bytes-1+init_offset,size-1) ):(size-1),
							(nflag)?( min(n_bytes-1,size-1-init_offset) ):(size-1-init_offset)
						);
					}					
				} else {
					if (i < byte_index_of_last_line) {
						printf("\nbytes %5d to %5d:\t",i+init_offset,i+15+init_offset);
					} else if ( (nflag?n_bytes:size) % 16) {
						printf("\nbytes %5d to %5d:\t",i+init_offset,(nflag)?( min(n_bytes-1+init_offset,size-1) ):(size-1));
					}
				}
				
			}
			if (i % 2 == 0)
				putchar('\t');
			if (i % 1 == 0)
				printf(" ");
	        if (fread(&c, 1, 1, sfp) == 1) {
				byte_count++;
				if (dflag) 
					printf("%d",c);
				else if (xflag)
					printf("%02x",c);
				else 
					bin_prnt_byte(c,nibble_spacing);
			}
			if (nflag && byte_count == n_bytes) break;		// quit if we reach the byte quota
			i++;			
	    }					// end main loop
	}
	putchar('\n');		
    fclose(sfp);		
	return 0;
}
        
void bin_prnt_byte(int x, int nibble_spacing)
{
   	int n;
	for(n=0; n<8; n++) {
      	if((x & 0x80) !=0)
         	printf("1");
      	else
         	printf("0");
		if (nibble_spacing && n==3) 
       		printf(" "); /* insert a space between nibbles */     
      	x = x<<1;
   }
}

int 
get_file_size(const char *fpath)
{
    int ret = -1;
    struct stat* buf = (struct stat*)malloc(sizeof(struct stat));
    if(stat(fpath, buf) == 0) {
        ret = buf->st_size;
    }
    free(buf);
    return ret;
}

void 
usage(void) {
    printf("Usage:\n");
	printf("Dumps bits from a file to the screen (stdout).\n");
    printf("\t./dump_bits -i [input_filename] -n [number_of_bytes]\n");
	printf("Optional:\n");
	printf("\t-t [byte_offset]	starts read out from a specific byte offset\n");
    printf("\t-b	prints binary values (default)\n");
    printf("\t-x	prints hex values\n");
    printf("\t-d	prints decimal values\n");
    printf("\t-s	prints as one long string\n");
    printf("\t-h	prints as one long string\n");
	printf("\t\tIf number of bits is not specified, dumps whole file.\n");	
}