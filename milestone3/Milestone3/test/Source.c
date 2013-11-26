/* PROJECT ARXITEKTONIKIS
MICHALIS PRODROMOU - MARIALENA SHIAMMA
*/

#include "Libraries_and_Defines.h" //It contains all the defines and the '#include's
#include "Converter.h" //The header file that we created in which there are some functions and the definitions of variables
#include "Parser.h" //The header file that parses the Data File
#include "Structures.h" //The header file that create the Cache memory depending on the Architecture of the memory
#include "Parser_Parameters.h" //The header file that parses the Parameters File

int main(){

int RandomAddress; //holds the value of the random generated RAM address
int RandomAction; //holds the value of the random generated Action 
int i, k=0, FlushesCounter=0,  FlushesFrequency = 0; // usd for counters later on
int IndexBitsAmount=0, TagBitsAmount=0, BOBitsAmount=0; //used for structures function
int IndexBitsAmountL2=0, TagBitsAmountL2=0;
int AddressBitsAmount = 0; //Calculates the bits needed for each RAM reference
int RamWordCapacity = 0; //The size of RAM -entries
int CacheWordCapacity = 0; //The size of Cache -entries
int CacheLines = 0; //The sime of Cache -entries-
int CacheWordCapacityL2 = 0; //The size of Cache -entries
int CacheLinesL2 = 0; //The sime of Cache -entries-

int CacheIsPow2;
int CacheIsPow2L2;
int AddressBits;

//All the variables that we learn and can use, after the parsing of the Parameters File
int RAM_SIZE;
int L1_START, L1_END, L1_STEP;
int L2_START, L2_END, L2_STEP;
int VICTIM_SIZE;
int WORD_SIZE;
int Number_of_words_blocks_per_block;
int temp;
int Direct_Mapped, N_Way_Set_Associative, Fully_Associative;
int NumberOfFlushes;
int Ways=0; //Way of the N-Way set associative architecture

int FIFO, LRU, RANDOM;
int WRITE_BACK, WRITE_THROUGH;
int WRITE_ALLOCATE, NO_WRITE_ALLOCATE;
int EnableL2, EnableVictim, Inclusive, Exclusive;

int MISS_CYCLES_READ_L1_CACHE, MISS_CYCLES_READ_RAM;
int MISS_CYCLES_WRITE_L1_CACHE, MISS_CYCLES_WRITE_RAM;
int MISS_CYCLES_READ_L2_CACHE, MISS_CYCLES_READ_VICIM;
int MISS_CYCLES_WRITE_L2_CACHE, MISS_CYCLES_WRITE_VICTIM;

int temp2;
errno_t OpenningError= fopen_s(&DataFilePointer,"DataFile.txt","w+"); //Open (or create) the data file. also checks for opening errors. w+ == write and read permission

//Parse the Parameters File so that we can use the info in it
ParserParameters(&RAM_SIZE, &L1_START, &L1_END, &L1_STEP, &L2_START, &L2_END, &L2_STEP, &VICTIM_SIZE, &WORD_SIZE, &Number_of_words_blocks_per_block, &temp, &Direct_Mapped, &N_Way_Set_Associative, &Fully_Associative, &NumberOfFlushes, &FIFO, &LRU, &RANDOM, &WRITE_BACK, &WRITE_THROUGH, &WRITE_ALLOCATE, &NO_WRITE_ALLOCATE, &EnableL2, &EnableVictim, &Inclusive, &Exclusive, &MISS_CYCLES_READ_L1_CACHE, &MISS_CYCLES_READ_RAM, &MISS_CYCLES_WRITE_L1_CACHE, &MISS_CYCLES_WRITE_RAM,  &MISS_CYCLES_READ_L2_CACHE, &MISS_CYCLES_WRITE_L2_CACHE, &MISS_CYCLES_READ_VICIM, &MISS_CYCLES_WRITE_VICTIM, &Ways);
//printf("RAM %d L1S %d L1E %d L1ST %d \n", RAM_SIZE, L1_START, L1_END, L1_STEP);
//printf("L2S %d L2E %d L2ST %d VICTIM %d \n", L2_START, L2_END, L2_STEP, VICTIM_SIZE);
//printf("word %d wperbl %d temp %d DM %d NW %d FA %d NOFL %d F %d LRU %d R %d WB %d WT %d WA %d NWA %d L2 %d VIC %d INCL %d EXCL %d ", WORD_SIZE, Number_of_words_blocks_per_block, temp, Direct_Mapped, N_Way_Set_Associative, Fully_Associative, NumberOfFlushes, FIFO, LRU, RANDOM, WRITE_BACK, WRITE_THROUGH, WRITE_ALLOCATE, NO_WRITE_ALLOCATE, EnableL2, EnableVictim, Inclusive, Exclusive, MISS_CYCLES_READ_L1_CACHE, MISS_CYCLES_READ_RAM, MISS_CYCLES_WRITE_L1_CACHE, MISS_CYCLES_WRITE_RAM,  MISS_CYCLES_READ_L2_CACHE,MISS_CYCLES_WRITE_L2_CACHE, MISS_CYCLES_READ_VICIM, MISS_CYCLES_WRITE_VICTIM, Ways);


//used to randomize the generated addresses and actions
srand((unsigned) time(NULL)); //It is reseeding with a different value each second

RamWordCapacity = (int)(RAM_SIZE / WORD_SIZE); //Variable that holds the ram entries
//Calculate the bits needed to reference to an address
AddressBits= log((double)RamWordCapacity) / log(2.0); //The way we calculate the Bits for the Addresses

//Check if the Number of flush actions given is greater than 0
if (NumberOfFlushes > 0){
  FlushesFrequency = RamWordCapacity*temp/NumberOfFlushes; //Calculates the frequency in which a Flush can appeared in the output
 if ( NumberOfFlushes >= (temp * RamWordCapacity)){
	printf("Error! The amount of flush actions cant be greater than the total amount of actions! Programm terminated! \n");
	system("pause");
	exit(0);
 }
 else{
 	//runs for temp * RamWordCapacity so that we can have at least some misses/hits
	for (i=0; i<(temp*RamWordCapacity); i++){ 
		FlushesCounter++; //Increases the counter 

		if(FlushesCounter>=(FlushesFrequency)){//we are allowed to do a flush
				RandomAction= rand() %4; //Choose a random number between 0 and 3 to the variable RandomAction
				//It is a 'Flush' Action
				if (RandomAction == 3)
				{
				//printf("%c \n", Actions[RandomAction]);//prints the content of the Actions table in position RandomAction (=int value)
				fprintf(DataFilePointer, "%c \n", Actions[RandomAction]);//writes the content of the Actions table in position RandomAction (=int value) in the data file
				FlushesCounter=0;
				}
				//The random action wasn't a flush
				else {
					RandomAction= rand() %3;  //Choose a random number between 0 and 2 to the variable RandomAction
					RandomAddress = rand() %((int)(RamWordCapacity)-1); //sets a random number between 0 and ramsize (given) -1 to the variable RandomAddress
		
					//printf("%c ", Actions[RandomAction]);//prints the content of the Actions table in position RandomAction (=int value)
					fprintf(DataFilePointer, "%c ", Actions[RandomAction]);//writes the content of the Actions table in position RandomAction (=int value) in the data file
		
					DecimalToBinary(RandomAddress); //call the function named DecimalToBinary so that it will convert the Random address which is Decimal to Binary
			
					//In each for loop, it returns one bit of the binary address
					for (k=AddressBits-1; k>=0; k--){
						//printf("%i",binary[k]); //It prints the binary table that contain the binary address that is returned from the function DecimalToBinary
						fprintf(DataFilePointer,"%i",binary[k]); //It writes the binary table that contain the binary address that is returned from the function DecimalToBinary in the data file
					}

					//printf("  | %d \n", RandomAddress); //prints the value of the RandomAddress variable
					fprintf(DataFilePointer,"  | %d \n", RandomAddress); //writes the value of the RandomAddress variable in the data file
				}
			}
    //FlushesCounter < FlushesFrequency. That means that it is not the right time to print a 'Flush' Action in the output. So you prin only one of the 3 other actions.
	else {
			RandomAction= rand() %3; 
			RandomAddress = rand() %((int)(RamWordCapacity)-1); //sets a random number between 0 and ramsize (given) -1 to the variable RandomAddress
		
			//printf("%c ", Actions[RandomAction]);//prints the content of the Actions table in position RandomAction (=int value)
			fprintf(DataFilePointer, "%c ", Actions[RandomAction]);//writes the content of the Actions table in position RandomAction (=int value) in the data file
		
			DecimalToBinary(RandomAddress); //call the function named DecimalToBinary so that it will convert the Random address which is Decimal to Binary
			
			//In each for loop, it returns one bit of the binary address
			for (k=AddressBits-1; k>=0; k--){
				//printf("%i",binary[k]); //It prints the binary table that contain the binary address that is returned from the function DecimalToBinary
				fprintf(DataFilePointer,"%i",binary[k]); //It writes the binary table that contain the binary address that is returned from the function DecimalToBinary in the data file
			}

			//printf("  | %d \n", RandomAddress); //prints the value of the RandomAddress variable
			fprintf(DataFilePointer,"  | %d \n", RandomAddress); //writes the value of the RandomAddress variable in the data file
		}//end of "action wasnt a flush"
	}//end of for(temp*RamWordCapacity)
}//end of else
}//end of numberflushes>0
else if (NumberOfFlushes == 0){//User chose to have 0 flushes in the program
		for (i=0; i<(temp*RamWordCapacity); i++){ 
			
			RandomAction= rand() %3; 
			RandomAddress = rand() %((int)(RamWordCapacity)-1); //sets a random number between 0 and ramsize (given) -1 to the variable RandomAddress
		
			//printf("%c ", Actions[RandomAction]);//prints the content of the Actions table in position RandomAction (=int value)
			fprintf(DataFilePointer, "%c ", Actions[RandomAction]);//writes the content of the Actions table in position RandomAction (=int value) in the data file
		
			DecimalToBinary(RandomAddress); //call the function named DecimalToBinary so that it will convert the Random address which is Decimal to Binary
			
			//In each for loop, it returns one bit of the binary address
			for (k=AddressBits-1; k>=0; k--){
				//printf("%i",binary[k]); //It prints the binary table that contain the binary address that is returned from the function DecimalToBinary
				fprintf(DataFilePointer,"%i",binary[k]); //It writes the binary table that contain the binary address that is returned from the function DecimalToBinary in the data file
			}

			//printf("  | %d \n", RandomAddress); //prints the value of the RandomAddress variable
			fprintf(DataFilePointer,"  | %d \n", RandomAddress); //writes the value of the RandomAddress variable in the data file
		}
}//end of if Flashes=0
else{
	printf("Error! The amount of flush actions cant be a negative number. Programm terminated! \n");
	system("pause");
	exit(0);
}

fclose(DataFilePointer); //Closes the data file


//Print useful info about the program, in terminal
printf("Given Data: \n");
printf("-----------\n");
printf("RAM Entries = %d \n", (RAM_SIZE / WORD_SIZE));
printf("Word Size = %d Bytes \n", WORD_SIZE);

//The case that our architecture is Direct Mapped
if((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0))
		printf("Kind of architecture = Direct Mapped \n");
//The case that our architecture is N Way Set Associative
else if ((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0))
		printf("Kind of architecture = N Way Set Associative \n");
//The case that our architecture is Fully Associative
else if ((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1))
		printf("Kind of architecture = Fully Associative \n");

printf("Words per Block = %d \n", Number_of_words_blocks_per_block);
printf("\n\n---------------------------------------------------------------------------\n");
printf("  For a better view of the data, we created the 'Data File'.\n");
printf("  After parsing and processing the Data File, the output file has been created! \n");
printf("---------------------------------------------------------------------------\n");


//L2 is Disable
if (EnableL2 == 0){
	while (L1_START <= L1_END){ 
		CacheIsPow2 = (((int)(L1_START) != 0) && ( ((int)L1_START & (int)(L1_START-1)) == 0 ));
		//if the current Cache isn't a power of 2 ignore this size
		if (CacheIsPow2 != 1){
			L1_START = L1_START + L1_STEP;
			continue;
		}
		if (L1_START > RAM_SIZE)
			break;
		//Call the structures function to do the calculations
		Structures(RAM_SIZE, L1_START, L2_START, Number_of_words_blocks_per_block, WORD_SIZE, temp, Direct_Mapped, N_Way_Set_Associative, Fully_Associative, &IndexBitsAmount,&IndexBitsAmountL2, &TagBitsAmount, &TagBitsAmountL2, &BOBitsAmount, &AddressBitsAmount, &RamWordCapacity, &CacheLines, &CacheWordCapacity, &CacheLinesL2, &CacheWordCapacityL2, Ways, FIFO, LRU, RANDOM, WRITE_BACK, WRITE_THROUGH, WRITE_ALLOCATE, NO_WRITE_ALLOCATE);

		Parser(temp, Direct_Mapped, N_Way_Set_Associative, Fully_Associative, AddressBitsAmount, IndexBitsAmount, IndexBitsAmountL2, TagBitsAmount, TagBitsAmountL2, BOBitsAmount, Number_of_words_blocks_per_block, RamWordCapacity, CacheLines, CacheWordCapacity, CacheLinesL2, CacheWordCapacityL2, VICTIM_SIZE, EnableL2, EnableVictim, Inclusive, Exclusive, Ways, WRITE_BACK,WRITE_THROUGH,WRITE_ALLOCATE,NO_WRITE_ALLOCATE,MISS_CYCLES_READ_L1_CACHE,MISS_CYCLES_READ_RAM,MISS_CYCLES_WRITE_L1_CACHE,MISS_CYCLES_WRITE_RAM, &MISS_CYCLES_READ_L2_CACHE, MISS_CYCLES_WRITE_L2_CACHE, MISS_CYCLES_READ_VICIM, MISS_CYCLES_WRITE_VICTIM, FIFO, LRU, RANDOM); //call the Parser function

		printf("\n L1: %d \n", L1_START);
		L1_START = L1_START + L1_STEP;
	}
}
//L2 is Enable
else if (EnableL2 == 1){
	temp2 = L2_START;
	while (L1_START <= L1_END){ 
		CacheIsPow2 = (((int)(L1_START) != 0) && ( ((int)L1_START & (int)(L1_START-1)) == 0 ));
		//if the current Cache isn't a power of 2 ignore this size
		if (CacheIsPow2 != 1){
			//printf("L1: %d \n" , L1_START);
			L1_START = L1_START + L1_STEP;
			continue;
		}
		if (L1_START > RAM_SIZE)
			break;
		while (L2_START <= L2_END){
			CacheIsPow2L2 = (((int)(L2_START) != 0) && ( ((int)L2_START & (int)(L2_START-1)) == 0 ));
			//if the current Cache isn't a power of 2 and L2 is smaller than L1 ignore this size
			if ((CacheIsPow2L2 != 1)){
				//printf("L2_START %d \n" , L2_START);
				L2_START = L2_START + L2_STEP;
				continue;
			}
			if (L2_START < L1_START){
				L2_START = L2_START + L2_STEP;
				continue;
			}

			if (L2_START > RAM_SIZE)
				continue;
			//Call the structures function to do the calculations
			Structures(RAM_SIZE, L1_START, L2_START, Number_of_words_blocks_per_block, WORD_SIZE, temp, Direct_Mapped, N_Way_Set_Associative, Fully_Associative, &IndexBitsAmount,&IndexBitsAmountL2, &TagBitsAmount, &TagBitsAmountL2, &BOBitsAmount, &AddressBitsAmount, &RamWordCapacity, &CacheLines, &CacheWordCapacity, &CacheLinesL2, &CacheWordCapacityL2, Ways, FIFO, LRU, RANDOM, WRITE_BACK, WRITE_THROUGH, WRITE_ALLOCATE, NO_WRITE_ALLOCATE);

			Parser(temp, Direct_Mapped, N_Way_Set_Associative, Fully_Associative, AddressBitsAmount, IndexBitsAmount, IndexBitsAmountL2, TagBitsAmount, TagBitsAmountL2, BOBitsAmount, Number_of_words_blocks_per_block, RamWordCapacity, CacheLines, CacheWordCapacity, CacheLinesL2, CacheWordCapacityL2, VICTIM_SIZE, EnableL2, EnableVictim, Inclusive, Exclusive, Ways, WRITE_BACK,WRITE_THROUGH,WRITE_ALLOCATE,NO_WRITE_ALLOCATE,MISS_CYCLES_READ_L1_CACHE,MISS_CYCLES_READ_RAM,MISS_CYCLES_WRITE_L1_CACHE,MISS_CYCLES_WRITE_RAM, &MISS_CYCLES_READ_L2_CACHE, MISS_CYCLES_WRITE_L2_CACHE, MISS_CYCLES_READ_VICIM, MISS_CYCLES_WRITE_VICTIM, FIFO, LRU, RANDOM); //call the Parser function

			printf("\n L1: %d L2: %d \n", L1_START , L2_START); 
			L2_START = L2_START + L2_STEP;
		}
		L2_START = temp2;
		L1_START = L1_START + L1_STEP;
	}
}

system("pause");   
return 0;
}//end of main