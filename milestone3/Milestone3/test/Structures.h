#include "Libraries_and_Defines.h"

//Function that decide the bits that are needed for the Index, Tag and Block Offset of the Binary Address that we have, according to the 3 different kind of architectures
void Structures(int RAM_SIZE, int L1_START, int L2_START, int Number_of_words_blocks_per_block, int WORD_SIZE, int temp, int Direct_Mapped, int N_Way_Set_Associative, int Fully_Associative, int *first, int *firstL2, int *second, int *secondL2, int *third, int *AddrBits, int *RamWords,  int *CacheEntriesL1, int *CacheWordsL1, int *CacheEntriesL2, int *CacheWordsL2 ,int AmountOfWays, int FIFO, int LRU, int RANDOM, int WRITE_BACK, int WRITE_THROUGH, int WRITE_ALLOCATE, int NO_WRITE_ALLOCATE){
	
	//We put the '#define's in regular variables, so that we can use them in the form that we want to
	double CacheSizeL1 = L1_START; //Variable that holds the cache size
	double CacheSizeL2 = L2_START; //Variable that holds the cache size
	double RamSize = RAM_SIZE;
	double WordsPerBlock = Number_of_words_blocks_per_block; //Variable that holds the amount of words per line/set
	double WordSize = WORD_SIZE;
	int Multiplier = temp;
	double RamWordCapacity = (RAM_SIZE / WordSize); //Variable that holds the ram entries
	double CacheWordCapacityL1 = (CacheSizeL1 / WordSize); //The cache entries in words
	int CacheBlocksCapacityL1 = CacheWordCapacityL1 / WordsPerBlock;
	double CacheWordCapacityL2 = (CacheSizeL2 / WordSize); //The cache entries in words
	int CacheBlocksCapacityL2 = CacheWordCapacityL2 / WordsPerBlock;
	int DR = Direct_Mapped, SA = N_Way_Set_Associative, FA = Fully_Associative;
	int F = FIFO, L = LRU, R = RANDOM, WB = WRITE_BACK, WT = WRITE_THROUGH, WA = WRITE_ALLOCATE, NWA = NO_WRITE_ALLOCATE;


	//The next variables are all initialized to 0
	int BlockOffset = 0; 
	int Index = 0;
	int IndexL2 = 0;
	int Tag = 0; 
	int TagL2 = 0; 
	double LinesL1 = 0; //Double, because we use it in the log 
	double LinesL2 = 0; //Double, because we use it in the log 
	int AddressBits = 0;
	int RamLog = 0, CacheLogL1 = 0, CacheLogL2 = 0, WordLog =0;
	int RamIsPow2 = 0, CacheIsPow2 =0, CacheIsPow2L2 =0, WordIsPow2 =0, WaysIsPow2 =0;
	//Calculate the bits needed to reference to an address
	AddressBits= log(RamWordCapacity) / log(2.0); //The way we calculate the Bits for the Addresses

	//used here temporary for the following calculations/camparisons
	RamLog = log(RamSize)/log(2.0);
	CacheLogL1 = log(CacheSizeL1)/log(2.0);
	CacheLogL2 = log(CacheSizeL2)/log(2.0);
	WordLog = log(WordSize)/log(2.0);
	//Used to check if these numbers, given by the user, are in the power of 2
	//(int)Ramsize != 0  => cast the RamSize to int then check if its >0 -> returns 1/true or 0/false
	//(Ramsize & (RamSize -1)) == 0 => bitwise AND of these numbers in binary. if the number is a power of 2 the ramainder will be 0 (0 == 0 will return true)
	//in the end, RamIsPow2 will be initialised to 1 if the number given IS a power of 2
	RamIsPow2 = (((int)(RamSize) != 0) && ( ((int)RamSize & (int)(RamSize-1)) == 0 ));
	CacheIsPow2 = (((int)(CacheSizeL1) != 0) && ( ((int)CacheSizeL1 & (int)(CacheSizeL1-1)) == 0 ));
	CacheIsPow2L2 = (((int)(CacheSizeL2) != 0) && ( ((int)CacheSizeL2 & (int)(CacheSizeL2-1)) == 0 ));
	WordIsPow2 = (((int)(WordSize) != 0) && ( ((int)WordSize & (int)(WordSize-1)) == 0 ));

	//chekc if the multiplier value is correct
	if(Multiplier <= 0){
		printf("Error! The values of the multiplier must be a possitive number (At least 1). Exiting...");
		system("pause");
		exit(0);
	}

	//check if the the numbers next to the kind of architecture were boolean
	if( ((DR < 0) || (DR > 1)) || ((SA < 0) || (SA > 1)) || ((FA < 0) || (FA > 1)) ){
		printf("Error! The values of the three different kinds of architecture must be boolean. Exiting...");
		system("pause");
		exit(0);
	}

	//Check if the numbers next to the algorithm types were boolean
	if ( ((F < 0) || (F > 1)) || ((L < 0) || (L > 1)) || ((R < 0) || (WB > 1)) ){
		printf("Error! The values of the three different algorithms must be boolean. Exiting...");
		system("pause");
		exit(0);
	}
		//Check if the numbers next to the write policies were boolean
	if ( ((WB < 0) || (WB > 1)) || ((WT < 0) || (WT > 1)) || ((WA < 0) || (WA > 1)) || ((NWA < 0) || (NWA > 1)) ){
		printf("Error! The values of the write policies must be boolean. Exiting...");
		system("pause");
		exit(0);
	}

	//check of the sizes given are possitive numbers
	if( (RamWordCapacity <= 0) || (CacheWordCapacityL1 <= 0) || (WordSize <= 0) || (CacheWordCapacityL2 <= 0)){
		printf("Error! The sizes of RAM, cache and word must be greater than 0! Exiting...");
		system("pause");
		exit(0);
	}

	//check if RAM is bigger than Cache, as it should be
	if( (RamWordCapacity < CacheWordCapacityL1) || (RamWordCapacity == CacheWordCapacityL1) || (RamWordCapacity < CacheWordCapacityL2) || (RamWordCapacity == CacheWordCapacityL2)){
		printf("Error! The size of RAM must be bigger than the cache's! Exiting...");
		system("pause");
		exit(0);
	}

	//check if the sizes of ram, cache and word size are all a number in the power of 2
	if((RamIsPow2 != 1) || (CacheIsPow2 != 1) || (CacheIsPow2L2 != 1) || (WordIsPow2 != 1)){	
		printf("Error! The sizes of cache, RAM and Word Size must be a number in power of 2! Exiting...");
		system("pause");
		exit(0);
	}

	//Check of word size is bigger or equal to ram size or cache size
	if( (WordSize >= RamSize) || (WordSize >= CacheSizeL1) || (WordSize >= CacheSizeL2) ){
			printf("Error! The size of a word cant be greater than the sizes of RAM and/or Cache! Exiting...");
			system("pause");
			exit(0);
	}

	

	
	//The case that our architecture is Direct Mapped
	if((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0)){
		BlockOffset = log(WordsPerBlock)/log(2.0); //The number of bits that are needed for the block offset
		LinesL1 = (CacheWordCapacityL1 / WordsPerBlock); //The number of lines/sets that is going to have the cache that we are creating
		Index = log(LinesL1)/log(2.0); //The bits that are needed for the Index
		Tag = (AddressBits - (BlockOffset + Index)); //The bits that are needed for the tag
		//L2
		LinesL2 = (CacheWordCapacityL2 / WordsPerBlock); //The number of lines/sets that is going to have the cache that we are creating
		IndexL2 = log(LinesL2)/log(2.0); //The bits that are needed for the Index
		TagL2 = (AddressBits - (BlockOffset + IndexL2)); //The bits that are needed for the tag
	}//end of Direct Mapped


	//The case that our architecture is N_Way_Set_Associative
	else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
		
		//check if the amount of ways is a possitive number
		if(AmountOfWays <= 0){
			printf("Error! The amount of ways must be a possitive number in the power of 2! Exiting...");
			system("pause");
			exit(0);
		}
		
		//check if the number given by the user is a power of 2
		WaysIsPow2 = (((AmountOfWays) != 0) && ( (AmountOfWays & (AmountOfWays-1)) == 0 ));		
		if(WaysIsPow2 != 1){
			printf("Error! The amount of ways must be a number in the power of 2! Exiting...");
			system("pause");
			exit(0);
		}

		//check if the amount of ways is greater than the amount of blocks => cant be created
		if(AmountOfWays >= CacheBlocksCapacityL1){
			printf("Error! The amount of ways cant be greater than the amount of blocks! Exiting...");
			system("pause");
			exit(0);
		}

		if (CacheWordCapacityL1 /(WordsPerBlock * AmountOfWays) < 1){
			printf("Error! Very Small Cached! Exiting...");
			system("pause");
			exit(0);
		}

		if(AmountOfWays >= CacheBlocksCapacityL2){
			printf("Error! The amount of ways cant be greater than the amount of blocks! Exiting...");
			system("pause");
			exit(0);
		}

		if (CacheWordCapacityL2 /(WordsPerBlock * AmountOfWays) < 1){
			printf("Error! Very Small Cached! Exiting...");
			system("pause");
			exit(0);
		}

		BlockOffset = log(WordsPerBlock)/log(2.0); //The number of bits that are needed for the block offset
		LinesL1 = ( CacheWordCapacityL1 / (WordsPerBlock * AmountOfWays)) ; //Doesn't need //The number of lines/sets that is going to have the cache that we are creating
		Index = (log(LinesL1)/log(2.0)); //The bits that are needed for the Index
		Tag = (AddressBits - (BlockOffset + Index)); //The bits that are needed for the tag
		//L2
		LinesL2 = ( CacheWordCapacityL2 / (WordsPerBlock * AmountOfWays)) ; //Doesn't need //The number of lines/sets that is going to have the cache that we are creating
		IndexL2 = (log(LinesL2)/log(2.0)); //The bits that are needed for the Index
		TagL2 = (AddressBits - (BlockOffset + IndexL2)); //The bits that are needed for the tag
	}//end of N_Way_Set_Associative


	//The case that our architecture is Fully_Associative
	else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1)){
		BlockOffset = log(WordsPerBlock)/log(2.0); //The number of bits that are needed for the block offset
		Index = 0; // Its fully associative => no index
		Tag = (AddressBits - (BlockOffset + Index)); //The bits that are needed for the tag
		//L2
		IndexL2 = 0; // Its fully associative => no index
		TagL2 = (AddressBits - (BlockOffset + IndexL2)); //The bits that are needed for the tag
	}//end of Fully_Associative

	//When the user doesn't give one of the three architectures that are allowed
	else{
		printf("You must select one of the 3 arcitecture types!  (Direct Mapped , N Way Set Associative or Fully Associative)\n");
		system("pause");
		exit(0); //We can't calculate the Index, Tag and Block Offset, so we stop the program from running
	}

	//We give values in the variables so that they can be returned to main
	*first = Index;
	*firstL2 = IndexL2;
	*second = Tag;
	*secondL2 = TagL2;
	*third = BlockOffset;
	*AddrBits = AddressBits;
	*RamWords = RamWordCapacity;
	*CacheEntriesL1 = LinesL1;
	*CacheWordsL1 = CacheWordCapacityL1;
	*CacheEntriesL2 = LinesL2;
	*CacheWordsL2 = CacheWordCapacityL2;
}//end of "Structures" function