//the libraries that are being used by the other .h files and also by the .c file 
#include <stdio.h>
#include <stdlib.h> //Enables us to use the rand and atoi functions
#include <math.h> 
#include <string.h> 
#include <time.h> //Allow us to use the time_t type of variable

//variables that are used in both .c and .h files
//SIZES must be given in bytes
#define RAM_SIZE 1024
#define CACHE_SIZE 256
#define WORD_SIZE 4

#define Number_of_words_blocks_per_block 8 //amount of words per line/set of the structure

#define Direct_Mapped 0 //boolean 1 means that we choose this Architecture
#define N_Way_Set_Associative 1 //boolean 0 means that we don't choose this Architecture
#define Fully_Associative 0//boolean //0 means that we don't choose this Architecture
 

#define NumberOfFlushes 5 //The number of flashes that are allowed
#define temp 3 //temp * cache size = size of data file