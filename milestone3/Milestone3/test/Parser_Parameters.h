#include "Libraries_and_Defines.h"

FILE *ParametersFilePointer; //The pointer to the Parameters file

//the function that will parse the Parameters File and give values to the parameters that we are going to need
void ParserParameters(int *RAM, int *L1start, int *L1end, int *L1step, int *L2start, int *L2end, int *L2step , int *VICTIM, int *WORDsize, int *WORDSperBLOCK, int *Temp, int *DirectMapped, int *NWaySetAssociative, int *FullyAssociative, int *NumbOfFlushes, int *Fifo, int *Lru, int *Random, int *Write_Back, int *Write_Through, int *Write_Allocate, int *No_Write_Allocate, int *EnableL2, int *EnableVictim, int *Inclusive, int *Exclusive, int *Miss_Cycles_Read_L1_Cache, int *Miss_Cycles_Read_Ram, int *Miss_Cycles_Write_L1_Cache, int *Miss_Cycles_Write_Ram, int *Miss_Cycles_Read_L2_Cache,int *Miss_Cycles_Write_L2_Cache, int *Miss_Cycles_Read_Victim_Cache,int *Miss_Cycles_Write_Victim_Cache, int *Ways){
	errno_t OpenningError= fopen_s(&ParametersFilePointer,"ParametersFile.txt","r"); //Open the parameters data file. Also checks for opening errors.

	char c, d;
	int i=0, j=0;

	//All the variables that we take from the file
	int RAMSIZE[10] = {0};
	int L1START[10] = {0};
	int L1END[10] = {0};
	int L1STEP[10] = {0};
	int L2START[10] = {0};
	int L2END[10] = {0};
	int L2STEP[10] = {0};

	int VICTIM_CACHESIZE[10] = {0};
	int WORDSIZE[10] = {0};
	int WORDS_PER_BLOCK[10]= {0}; //amount of words per line/set of the structure
	int TEMP[10]= {0};
	//////
	int DirectM[10] = {0};
	int N_Way[10] = {0};
	int FullyAss[10] = {0};
	int NumbOFflushes[10] = {0};
	int AmountOfWays[10] = {0};
	//////
	int FIFO[10] = {0};
	int LRU[10] = {0};
	int RANDOM[10] = {0};
	//////
	int WBack[10] = {0};
	int WThrough[10] = {0};
	int WAllocate[10] = {0};
	int NoWAllocate[10] = {0};
	//////
	int ENABLEL2[10] = {0};
	int ENABLEVictim[10] = {0};
	int INCLUSIVE[10] = {0};
	int EXCLUSIVE[10] = {0};
	//////
	int MissCyclesReadL1Cache[10] = {0};
	int MissCyclesReadRam[10] = {0};
	int MissCyclesWriteL1Cache[10] = {0};
	int MissCyclesWriteRam[10] = {0};
	int MissCyclesReadL2Cache[10] = {0};
	int MissCyclesWriteL2Cache[10] = {0};
	int MissCyclesReadVictimCache[10] = {0};
	int MissCyclesWriteVictimCache[10] = {0};



	//Saves in a variable the RAMSIZE, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &RAMSIZE, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}
		
	//Saves in a variable the L1START, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &L1START, 1);
		if (d=fgetc(ParametersFilePointer) == ','){ break;}
		j++;
	}

	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &L1END, 1);
		if (d=fgetc(ParametersFilePointer) == ','){ break;}
		j++;
	}


	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &L1STEP, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the L1START, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &L2START, 1);
		if (d=fgetc(ParametersFilePointer) == ','){ break;}
		j++;
	}


	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &L2END, 1);
		if (d=fgetc(ParametersFilePointer) == ','){ break;}
		j++;
	}


	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &L2STEP, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}


	//Saves in a variable the WORDSIZE, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &VICTIM_CACHESIZE, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the WORDSIZE, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &WORDSIZE, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the WORDS_PER_BLOCK, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &WORDS_PER_BLOCK, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the TEMP, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &TEMP, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the DirectM, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &DirectM, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the N_Way, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &N_Way, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the FullyAss, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &FullyAss, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the NumOFFlushes, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &NumbOFflushes, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the Amount Of Ways, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &AmountOfWays, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the FIFO, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &FIFO, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the LRU, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &LRU, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the RANDOM, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &RANDOM, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the WBack, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &WBack, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the WThrough, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &WThrough, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the WAllocate, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &WAllocate, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the NoWAllocate, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &NoWAllocate, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}



	//Saves in a variable the EnableL2 cache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &ENABLEL2, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the Enable victim cache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &ENABLEVictim, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}
	
	//Saves in a variable the L2 inclusive, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &INCLUSIVE, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the L2 exclusive, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &EXCLUSIVE, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesReadCache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesReadL1Cache, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesReadRam, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesReadRam, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesWriteCache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesWriteL1Cache, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesWriteRam, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesWriteRam, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesWriteCache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesReadL2Cache, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesWriteCache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesWriteL2Cache, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesWriteCache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesReadVictimCache, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//Saves in a variable the MissCyclesWriteCache, after parsing the file
	while (c=fgetc(ParametersFilePointer) != ' '){} //Our delimiter is the space
	//We read the character after  the space and before the new line
	while (j<=10){
		fscanf_s(ParametersFilePointer, "%d", &MissCyclesWriteVictimCache, 1);
		if (d=fgetc(ParametersFilePointer) == '\n'){ break;}
		j++;
	}

	//We give values in the variables so that they can be returned to main
	*L1start = *L1START;
	*L1end = *L1END;
	*L1step = *L1STEP;

	*L2start = *L2START;
	*L2end = *L2END;
	*L2step = *L2STEP;

	*RAM = *RAMSIZE;
	*VICTIM = *VICTIM_CACHESIZE;
	*WORDsize = *WORDSIZE;
	*WORDSperBLOCK = *WORDS_PER_BLOCK;
	*Temp = *TEMP;
	////
	*DirectMapped = *DirectM;
	*NWaySetAssociative = *N_Way;
	*FullyAssociative = *FullyAss;
	*NumbOfFlushes = *NumbOFflushes;
	*Ways = *AmountOfWays;
	////
	*Fifo = *FIFO;
	*Lru = *LRU;
	*Random = *RANDOM;
	*Write_Back = *WBack;
	*Write_Through = *WThrough;
	*Write_Allocate = *WAllocate;
	*No_Write_Allocate = *NoWAllocate;
	////
	*EnableL2= *ENABLEL2;
	*EnableVictim = *ENABLEVictim;
	*Inclusive = *INCLUSIVE;
	*Exclusive = *EXCLUSIVE;
	////
	*Miss_Cycles_Read_L1_Cache = *MissCyclesReadL1Cache;
	*Miss_Cycles_Read_Ram = *MissCyclesReadRam;
	*Miss_Cycles_Write_L1_Cache = *MissCyclesWriteL1Cache;
	*Miss_Cycles_Write_Ram = *MissCyclesWriteRam;
	*Miss_Cycles_Read_L2_Cache = *MissCyclesReadL2Cache;
	*Miss_Cycles_Write_L2_Cache = *MissCyclesWriteL2Cache;
	*Miss_Cycles_Read_Victim_Cache = *MissCyclesReadVictimCache;
	*Miss_Cycles_Write_Victim_Cache = *MissCyclesWriteVictimCache;

	fclose(ParametersFilePointer); //Closes the parameters file
}