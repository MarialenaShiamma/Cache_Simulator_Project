#include "Libraries_and_Defines.h"

FILE *DataFilePointer; //The pointer to the data file
FILE *OutputFilePointer; //The pointer to the output file

//A struct that represents the content of the nodes of the list for the Direct Mapped Architecture
struct List_node_DM {
  int Tag1;
  int Index1;
  int BlockOffset1[10];
  int Valid1;
  int Dirty1; //We need it when our cache does Write Back
  struct List_node_DM *Next1;
};

//A struct that represents the content of the nodes of the list for the Fully Associative Architecture
struct List_node_FA {
  int Tag2;
  int BlockOffset2[10];
  int Valid2;
  int Dirty2; //We need it when our cache does Write Back
  int Age;//used for LRU and FIFO algorithms
  struct List_node_FA *Next2;
};

//A struct that represents the content of the nodes of the list for the N-Way Set Associative Architecture
struct List_node_NW {
  int Tag3;
  int Index3;
  int BlockOffset3[10];
  int Valid3;
  int Dirty3; //We need it when our cache does Write Back
  int Age3;
  int Way;
  int Number;
  struct List_node_NW *Next3;
};



//Function that converts Binary Numbers to Decimal
int BinaryToDecimal(int binaryNumber[], int k){
	int decimal=0, l=0, m=0;

	for (l=k; l>=0; l--){
		decimal = decimal + (binaryNumber[l] * pow (2.0, m));
		m= m+1;
	}
	return decimal;
}



//Function that is parsing the Data File and creates the Output Files with the info that we need in order to build the memories
void Parser(int temp,  int Direct_Mapped, int N_Way_Set_Associative, int Fully_Associative, int AddBits, int index, int indexL2, int tag, int tagL2, int offset, int WordsPerBlock, int RamWords, int CacheEntriesL1, int CacheWordsL1, int CacheEntriesL2, int CacheWordsL2, int Victim, int EnableL2, int EnableVictim, int Inclusive, int Exclusive, int Way, int WB, int WT, int WA, int NWA, int MCRL1C, int MCRR, int MCWL1C, int MCWR, int MCRL2C, int MCWL2C, int MCRV, int MCWV, int FIFO, int LRU, int RANDOM){

	errno_t OpenningError= fopen_s(&DataFilePointer,"DataFile.txt","r+"); //Open (or create) the data file. Also checks for opening errors.
	errno_t OpenningError2= fopen_s(&OutputFilePointer,"OutputFile.txt","w+"); //Open (or create) the output file. Also checks for opening errors. w+ == write and read permission

	struct List_node_DM *FirstNode1, *Pointer1, *tmp1, *FirstNodeV, *NodeV, *VTemp, *tmp4;  //Our first node of the list
	struct List_node_FA *FirstNode2, *Pointer2, *Pointer2b, *PointerFifo, *PointerLru,*MostUsed,*PointerRandom, *tmp2;  //Our first node of the list
	struct List_node_NW *FirstNode3, *Pointer3, *Pointer3b, *PointerFifo3, *Oldest3, *PointerLru3,*MostUsed3,*PointerRandom3, *tmp3;  //Our first node of the list

	int i=0, j=0, k=0, l=0, m=0, n=0, p=0, q=0, u=0, s=0, w=0, y=0, r=0, Bit=0, Which_Way =0;//used for counters
	int total_penalty_cycles = 0;//used to hold the total amount of penalty cycles
	int penalty_cycles =0;
	char c; //A character that is useful only to find the new line character
	char Action=' '; // R, W, M, F
	char Space=' ';	
	int Binary_Parser[128]={0}; //An array that will take in each cell of it, one byte of a binary Address.The rest cells will have the value '0'. We assume that we don't have addresses that need more than 127 bits
	int TagBits[30]={0};// We assume that we don't have addresses that need more than 30 bits for the Tag
	int IndexBits[30]={0};//We assume that we don't have addresses that need more than 30 bits for the Index
	int TagBitsL2[30]={0};// We assume that we don't have addresses that need more than 30 bits for the Tag
	int IndexBitsL2[30]={0};//We assume that we don't have addresses that need more than 30 bits for the Index
	
	int BlockOffsetBits[30]={0};//We assume that we don't have addresses that need more than 30 bits for the Block Offset
	int Address;
	int TagDec =0, IndexDec = 0, BlockOffsetDec = 0;
	float hits=0 , misses=0, VictimHits=0, VictimMiss=0;
	float total_hits, total_misses, total_VictimHits, total_VictimMiss;
	int inCache=0; //if it is =1 it means that the address is in the Cache -list's nodes (existed or been written after the Reference) because Cache had free space 
				  // =0 means that we couldn't find the address in the list's nodes and there was not space to write the Address
	int MaxUsed=0; //for LRU policy
	int RandomNumber=0; //for RANDOM policy
	int NW_flag=0;
	int NodeNumber=0;
	int WayInNode = -1;
	int ReadModify = 0;
	int MissRM = 0;

	int temp3, temp4, temp5;

	int old;
	int inVictim =1;
	//////-----------------------Creating the correct number of the List's Nodes-------------------------------------------/////////

	//Create the list if the Architecture is Direct Mapped
	if ((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0)){
		FirstNode1 = (struct List_node_DM *) malloc( sizeof(struct List_node_DM) ); //Our fist node pointes to a List_node struct
		FirstNode1->Next1 = 0;  //Our first node is the last too so the Next node is the NULL 
		FirstNode1->Index1 = 0; //We initialize The Index field in the node of the list
		Pointer1 = FirstNode1; //The Pointer that will move into the list
		Pointer1->Valid1 = 0; //Initialize the Valid field of the struct
		Pointer1->Tag1 = -1; //Initialize the Tag field of the struct
		Pointer1->Dirty1 = 0; //Initialize the Dirty field of the struct

		for(s=1; s <= CacheEntriesL1; s++){
			Pointer1->Next1 = (struct List_node_DM *) malloc( sizeof(struct List_node_DM) ); //Create all the nodes in the for loop according to how many index we will have
			Pointer1 = Pointer1->Next1; //Move the Pointer to the next node
			Pointer1->Index1 = s; 
			Pointer1->Valid1 = 0; //Initialize the Valid field of the struct
			Pointer1->Tag1 = -1; //Initialize the Tag field of the struct
			Pointer1->Dirty1 = 0; //Initialize the Dirtyfield of the struct
		}
		Pointer1->Next1 = 0; //the next node is the NULL
		Pointer1->Index1 = s; 
		Pointer1->Dirty1 = 0; //Initialize the Dirty field of the struct of the last node
		Pointer1 = FirstNode1; //Reinitialize the Pointer in the first node (root)
		
		///VICTIM CACHE
		FirstNodeV = (struct List_node_DM *) malloc( sizeof(struct List_node_DM) ); //Our fist node pointes to a List_node struct
		FirstNodeV->Next1 = 0;  //Our first node is the last too so the Next node is the NULL 
		FirstNodeV->Index1 = 0; //We initialize The Index field in the node of the list
		NodeV = FirstNodeV; //The Pointer that will move into the list
		NodeV->Valid1 = 0; //Initialize the Valid field of the struct
		NodeV->Tag1 = -1; //Initialize the Tag field of the struct
		NodeV->Dirty1 = 0; //Initialize the Dirty field of the struct

		for(s=1; s <= Victim; s++){
			NodeV->Next1 = (struct List_node_DM *) malloc( sizeof(struct List_node_DM) ); //Create all the nodes in the for loop according to how many index we will have
			NodeV = NodeV->Next1; //Move the Pointer to the next node
			NodeV->Index1 = 0; 
			NodeV->Valid1 = 0; //Initialize the Valid field of the struct
			NodeV->Tag1 = -1; //Initialize the Tag field of the struct
			NodeV->Dirty1 = 0; //Initialize the Dirtyfield of the struct
		}
		NodeV->Next1 = 0; //the next node is the NULL
		NodeV->Index1 = 0; 
		NodeV->Dirty1 = 0; //Initialize the Dirty field of the struct of the last node
		NodeV = FirstNodeV; //Reinitialize the Pointer in the first node (root)
	}

	//Create the list if the Architecture is Fully Associative
	else if ((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1)){
		FirstNode2 = (struct List_node_FA *) malloc( sizeof(struct List_node_FA) ); //Our fist node pointes to a List_node struct
		FirstNode2->Next2 = 0;  //Our first node is the last too so the Next node is the NULL 
		Pointer2 = FirstNode2; //The Pointer that will move into the list
		Pointer2->Valid2 = 0; //Initialize the Valid field of the struct
		Pointer2->Age =0; //Initialize age to 0
		Pointer2->Tag2 = -1; //We initialize The Index field in the node of the list
		Pointer2->Dirty2 = 0; //Initialize the Dirtyfield of the struct
		
		for(s=1; s <= CacheWordsL1; s++){
			Pointer2->Next2 = (struct List_node_FA *) malloc( sizeof(struct List_node_FA) ); //Create all the nodes in the for loop according to how many index we will have
			Pointer2 = Pointer2->Next2; //Move the Pointer to the next node
			Pointer2->Valid2 = 0; //Initialize the Valid field of the struct
			Pointer2->Age = 0; //Initialize the Age field of the struct
			FirstNode2->Tag2 = -1;  //Initialize the Tag field of the struct
			Pointer2->Dirty2 = 0; //Initialize the Dirtyfield of the struct
		}

		Pointer2->Next2 = 0; //the next node is the NULL
		Pointer2->Tag2 = -1;  //Initialize the Tag field of the struct for the last node
		Pointer2->Age = 0; //Initialize the Age field of the struct for the last node
		Pointer2->Valid2 = 0; //Initialize the Valid field of the struct for the last node
		Pointer2->Dirty2 = 0; //Initialize the Dirtyfield of the struct
		Pointer2 = FirstNode2; //Reinitialize the Pointer in the first node (root)
		PointerFifo = FirstNode2;
		
	}

	//Create the list if the Architecture is N-Way set Associative
	else if ((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
		FirstNode3 = (struct List_node_NW *) malloc( sizeof(struct List_node_NW) ); //Our fist node pointes to a List_node struct
		FirstNode3->Next3 = 0;  //Our first node is the last too so the Next node is the NULL 
		Pointer3 = FirstNode3; //The Pointer that will move into the list
		Pointer3->Valid3 = 0; //Initialize the Valid field of the struct
		Pointer3->Tag3 = -1;
		Pointer3->Age3 = 0;
		Pointer3->Way = 0;
		Pointer3->Number = 0;
		Pointer3->Dirty3 = 0; //Initialize the Dirtyfield of the struct
		Pointer3->Index3 = 0; //We initialize The Index field in the node of the list
		
		//We create the nodes
		for(s=1; s <= CacheWordsL1 /(WordsPerBlock * Way); s++){
			if (NW_flag == 0){
				//We do ti for every way
				for (i=1; i<Way; i++){
					Pointer3->Next3 = (struct List_node_NW *) malloc( sizeof(struct List_node_NW) ); //Create all the nodes in the for loop according to how many index we will have
					Pointer3 = Pointer3->Next3; //Move the Pointer to the next node
					Pointer3->Index3 = s; 
					Pointer3->Age3 = 0;
					Pointer3->Tag3 = -1;
					Pointer3->Valid3 = 0; //Initialize the Valid field of the struct
					j++;
					Pointer3->Number = j;
					Pointer3->Dirty3 = 0; //Initialize the Dirtyfield of the struct
					Pointer3->Way = i;
					
					NW_flag = 1;
				}
			}
			for (i=0; i<Way; i++){
				Pointer3->Next3 = (struct List_node_NW *) malloc( sizeof(struct List_node_NW) ); //Create all the nodes in the for loop according to how many index we will have
				Pointer3 = Pointer3->Next3; //Move the Pointer to the next node
				Pointer3->Index3 = s; 
				Pointer3->Age3 = 0;
				Pointer3->Tag3 = -1;
				Pointer3->Valid3 = 0; //Initialize the Valid field of the struct
				j++;
				Pointer3->Number = j;
				Pointer3->Dirty3 = 0; //Initialize the Dirtyfield of the struct
				Pointer3->Way = i;
				}
		}
		//Pointer3->Next3 = (struct List_node_NW *) malloc(sizeof(struct List_node_NW) );; //the next node is the NULL
		//Pointer3 = Pointer3->Next3;
		//Pointer3->Index3 = s; 
		//Pointer3->Age3 = 0;
		//Pointer3->Valid3 = 0;
		//Pointer3->Tag3 = -1;
		//Pointer3->Dirty3 = 0; //Initialize the Dirtyfield of the struct
		//Pointer3->Number = j;
		//Pointer3 = FirstNode3; //Reinitialize the Pointer in the first node (root)	

		Pointer3->Next3= 0; //last node
		Pointer3->Index3 = s; 
		Pointer3->Valid3 = 0;
		Pointer3->Tag3 = -1;
		Pointer3->Dirty3 = 0; //Initialize the Dirtyfield of the struct
		Pointer3->Age3 = 0;
		Pointer3->Number = j+1;

		Pointer3 = FirstNode3; //Reinitialize the Pointer as the first node

		while (Pointer3->Next3 != 0){
			for (r=0; r<10; r++)
				Pointer3->BlockOffset3[BlockOffsetDec] == -1;
			Pointer3 = Pointer3->Next3;
		}

		Pointer3 = FirstNode3; //Reinitialize the Pointer as the first node
    }//end of N-Way
    

	else{
		printf("Error! You must select one of the 3 architectures! Exiting...");
		exit(0);
	}

	//////-----------------------Finished the creating of the correct List's Nodes-------------------------------------------/////////


	//runs for temp *Ramsize to read all the lines from the DataFile
	for (j=0; j<(temp*RamWords); j++){ 
		fscanf_s(DataFilePointer, "%c", &Action, 1); //Reads from the Data File the first character.This character represent the Action - R,W,M,F
		
		penalty_cycles = 0;	//The total penalty cycles ara being initializes with 0 for every address
		inCache = 0; //There is no such address in the Cache because Cache had not free space in it
		inVictim = 0;
		MaxUsed=0;	
		WayInNode=-1;
		ReadModify = 0;
		MissRM = 0;

		old = -1 ; //in order to understand when it is replaced

		if (EnableL2 == 0){
			//Check the value of the Action that was read before
			switch (Action){
//---------------------------------------------------------------------------------------------------------------------------------------------------------------//
			//If it is a Flush action
			case 'F': {
					fprintf(OutputFilePointer, " FLUSH");	
					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.
					
					//"Deletes" the list if the architecture is Direct Mapped
					if ((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0)){
						Pointer1 = FirstNode1;
						
						if (WT ==1)
							fprintf(OutputFilePointer,"\n");

						//Write Back & Write Allocate
						if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
							//We use do- while so that we cover the case of the last node
							do{
								//Every node that has Dirty =1, it means that it has to write the old data of the block in the RAM before it'll be deleted in Cache
								if (Pointer1->Dirty1 == 1){
									penalty_cycles = MCWR; //Cycles needed to Write in RAM
									total_penalty_cycles = total_penalty_cycles + MCWR; //Cycles needed to Write in RAM
								}
								Pointer1 = Pointer1->Next1;
							}	
							while (Pointer1->Next1 != 0);

							fprintf(OutputFilePointer, "       ---          ---      ----     -------         -----------         -----             ---  ");
							fprintf(OutputFilePointer,"PENALTY CYCLES: %d\n"  , penalty_cycles);
						}

						//Write Back & No Write Allocate
						else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
							//We use do- while so that we cover the case of the last node
							do{
								//Every node that has Dirty =1, it means that it has to write the old data of the block in the ram before it'll be deleted in Cache
								if (Pointer1->Dirty1 == 1){
									penalty_cycles = MCWR; //Cycles needed to Write in RAM
									total_penalty_cycles = total_penalty_cycles + MCWR; //Cycles needed to Write in RAM
								}
								Pointer1 = Pointer1->Next1;
							}
							while (Pointer1->Next1 != 0);	

							fprintf(OutputFilePointer, "       ---          ---      ----     -------         -----------         -----             ---  ");
							fprintf(OutputFilePointer,"PENALTY CYCLES: %d\n"  , penalty_cycles);
						}

						Pointer1 = FirstNode1; //Go to first node

						//We use do- while so that we cover the case of the last node
						do{
								Pointer1->Valid1 = 0; 
								for (r=0; r<10; r++){
									Pointer1->BlockOffset1[r]= -1; //"Deletes" the context of the BlockOffsetArray
									Pointer1->Dirty1 = 0; //Set the Dirty to 0 value
								}
								Pointer1 = Pointer1->Next1;
						}	
						while (Pointer1->Next1 != 0);

						Pointer1 = FirstNode1; //Go to first node


						if (EnableVictim==1){//Flush the Victim cache
							NodeV = FirstNodeV;

							//We use do- while so that we cover the case of the last node
							do{
								NodeV->Valid1 = 0; 
								for (r=0; r<10; r++){
									NodeV->BlockOffset1[r]= -1; //"Deletes" the context of the BlockOffsetArray
									NodeV->Dirty1 = 0; //Set the Dirty to 0 value
								}
								NodeV = NodeV->Next1;
							}	
							while (NodeV->Next1 != 0);

							NodeV = FirstNodeV; //Go to first node
						}

					}

					//"Deletes" the list if the architecture is Fully Associative
					else if ((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1)){
						Pointer2 = FirstNode2;
						PointerFifo = FirstNode2;

						if (WT ==1)
							fprintf(OutputFilePointer,"\n");

						///Write Back & Write Allocate
						if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
							//We use do- while so that we cover the case of the last node
							do{
								//Every node that has Dirty =1, it means that it has to write the old data of the block in the RAM before it'll be deleted in Cache
								if (Pointer2->Dirty2 == 1){
									penalty_cycles = MCWR; //Cycles needed to Write in RAM
									total_penalty_cycles = total_penalty_cycles + MCWR; //Cycles needed to Write in RAM
								}
								Pointer2 = Pointer2->Next2;
							}	
							while (Pointer2->Next2 != 0);

							fprintf(OutputFilePointer, "       ---          ---      ----     -------         -----------              -----           ---  ");
							fprintf(OutputFilePointer,"PENALTY CYCLES: %d\n"  , penalty_cycles);
						}

						//Write Back & No Write Allocate
						else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){

							//We use do- while so that we cover the case of the last node
							do{
								//Every node that has Dirty =1, it means that it has to write the old data of the block in the ram before it'll be deleted in Cache
								if (Pointer2->Dirty2 == 1){
									penalty_cycles = MCWR; //Cycles needed to Write in RAM
									total_penalty_cycles = total_penalty_cycles + MCWR; //Cycles needed to Write in RAM
								}
								Pointer2 = Pointer2->Next2;
							}
							while (Pointer2->Next2 != 0);	

							fprintf(OutputFilePointer, "       ---          ---      ----     -------         -----------              -----           ---  ");
							fprintf(OutputFilePointer,"PENALTY CYCLES: %d\n"  , penalty_cycles);
						}

						Pointer2 = FirstNode2; //Go to first node

						//We use do- while so that we cover the case of the last node
						do{
								Pointer2->Valid2 = 0;
								Pointer2->Tag2 = 0;
								for (r=0; r<10; r++){
									Pointer2->BlockOffset2[r]= -1; //"Deletes" the context of the BlockOffsetArray
									Pointer2->Dirty2 = 0; //Set the Dirty to 0 value
								}
								Pointer2 = Pointer2->Next2;
						}	
						while (Pointer2->Next2 != 0);

						Pointer2 = FirstNode2; //Go to first node

					}	
	

					//"Deletes" the list if the architecture is N-Way Set Associative
					else if ((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
						Pointer3 = FirstNode3;

						if (WT ==1)
							fprintf(OutputFilePointer,"\n");

						//Write Back & Write Allocate
						if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
							//We use do- while so that we cover the case of the last node
							while (Pointer3->Next3 != 0){
								//Every node that has Dirty =1, it means that it has to write the old data of the block in the RAM before it'll be deleted in Cache
								if (Pointer3->Dirty3 == 1){
									penalty_cycles = MCWR; //Cycles needed to Write in RAM
									total_penalty_cycles = total_penalty_cycles + MCWR; //Cycles needed to Write in RAM
								}
								Pointer3 = Pointer3->Next3;
							}	
							

							fprintf(OutputFilePointer, "       ---          ---      ----     -------         -----------       -----          --------       ---  ");
							fprintf(OutputFilePointer,"PENALTY CYCLES: %d\n"  , penalty_cycles);
						}

						//Write Back & No Write Allocate
						else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
							//We use do- while so that we cover the case of the last node
							while (Pointer3->Next3 != 0){
								//Every node that has Dirty =1, it means that it has to write the old data of the block in the ram before it'll be deleted in Cache
								if (Pointer3->Dirty3 == 1){
									penalty_cycles = MCWR; //Cycles needed to Write in RAM
									total_penalty_cycles = total_penalty_cycles + MCWR; //Cycles needed to Write in RAM
								}
								Pointer3 = Pointer3->Next3;
							}
								

							fprintf(OutputFilePointer, "       ---          ---      ----     -------         -----------       -----         --------       ---  ");
							fprintf(OutputFilePointer,"PENALTY CYCLES: %d\n"  , penalty_cycles);
						}

						Pointer3 = FirstNode3; //Go to first node

						//We use do- while so that we cover the case of the last node
						while (Pointer3->Next3 != 0){
								Pointer3->Valid3 = 0; 
								Pointer3->Way = -1;
								Pointer3->Tag3 = -1;

								for (r=0; r<10; r++){
									Pointer3->BlockOffset3[r]= -1; //"Deletes" the context of the BlockOffsetArray
									Pointer3->Dirty3 = 0; //Set the Dirty to 0 value
								}
								Pointer3 = Pointer3->Next3;
						}	
						Pointer3 = FirstNode3; //Go to first node
					}//end of N-Way

					break;
					  }

	//-----------------------------------------------------------------------------------------------------------------------------------------------//

			//If the action is Read
			case 'R' : {
					fprintf(OutputFilePointer, " READ    the Address: ");
					fscanf_s(DataFilePointer, "%c", &Space, 1); //Reads one character from the Data File. This character is a space.
					fprintf(OutputFilePointer, "  "); //Pritns Spaces
					//Do a loop for the lenght of the address bits that we found before
					for(k=0; k<AddBits; k++){
						fscanf_s(DataFilePointer, "%c", &Bit, 1); //Reads one character from the Data File. This character represents one bit.
						fprintf(OutputFilePointer, "%c", Bit); //Writes in the output file the Bit that is read in the for loop
						if((char)Bit=='0'){Binary_Parser[k]=0;} //Make casting of the char '0'
						if((char)Bit=='1'){Binary_Parser[k]=1;} //Make casting of the char '1'			
					}

					Address = BinaryToDecimal(Binary_Parser, k-1);

					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.


					fprintf(OutputFilePointer, "   TAG:  ");
					//Write in the Output File the bits that shows the tag, according to the bits that are needed for tag (is returned from the Structures.h)
					for (n=0; n < tag ; n++){
						fprintf(OutputFilePointer,"%d", Binary_Parser[n]);
						TagBits[n]=Binary_Parser[n];
					}

					TagDec= BinaryToDecimal(TagBits, n-1); //call the Function that will convert the tag binary bits into decimal
					//printf("TAG %d ", TagDec);

					//Write in the Output File the bits that shows the index, according to the bits that are needed for index (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   INDEX: ");
					
					//If we have fully Associative architecture we don't have Index
					if (index==0){
						fprintf(OutputFilePointer,"   - ");
					}
					else{
						for (p=tag; p < (tag+index) ; p++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[p]);
							IndexBits[p]=Binary_Parser[p];
						}

					IndexDec= BinaryToDecimal(IndexBits, p-1); //call the Function that will convert the Index binary bits into decimal
					//printf("INDEX %d ", IndexDec);
					}

					//Write in the Output File the bits that shows the block offset,  according to the bits that are needed for block offset (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   BLOCK OFFSET: ");
					 
					
					//If We have only one block of words, we don't need block offset
					if (offset==0)
							fprintf(OutputFilePointer,"   - ");
					else{
						for (q=tag+index; q < (tag+index+offset) ; q++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[q]);
							BlockOffsetBits[q]=Binary_Parser[q];
						}
						BlockOffsetDec= BinaryToDecimal(BlockOffsetBits, q-1); //call the Function that will convert the BO binary bits into decimal
						//printf("BO %d \n", BlockOffsetDec);
					}

						//The case that our architecture is N_Way_Set_Associative
					if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
						//Ways are more or equal to 1
						if (Way != 0){
									Which_Way = rand() %(Way + 1);; //Decides the way between the numbers 1 and
									//The number of ways must be > 0, So there isn't a Way 0
									if (Which_Way == 0){
											Which_Way = 1;
									}
							fprintf(OutputFilePointer, "   WAY:  ");
							fprintf(OutputFilePointer,"%d", Which_Way);
						}
					}
					

					//The case that our architecture is Direct Mapped
					if((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0)){
						Pointer1 = FirstNode1;
						do{
							//Same Index
							if (IndexDec == Pointer1->Index1){
								//Valid=1
								if (Pointer1->Valid1 == 1){
									//Same Tag
									if (TagDec == Pointer1->Tag1){
										//Same Block Offset
										if (Pointer1->BlockOffset1[BlockOffsetDec] == Address){

											//Write Back & Write Allocate => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												penalty_cycles = ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
											}
											
											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
												penalty_cycles = (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
											}

											//Write Through & Write Allocate => Write in Cache and RAM (WT) 
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) (BUT you can't write in Cache (WB))
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
												penalty_cycles = (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
											}

											Pointer1->Dirty1 = 1; //For Write Back
											hits++;

											//code to write to the output file
											fprintf(OutputFilePointer, "   HIT  ");
											fprintf(OutputFilePointer, " ---        ");
											fprintf(OutputFilePointer, "  ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											/*for (y=0; y<AddBits; y++)
												printf("%d", Binary_Parser[y]);
												printf("\n");
												*/
											}
										//Different BlockOffset
										else{ 
											old = Pointer1->BlockOffset1[BlockOffsetDec];				
											
											if  (EnableVictim == 1){
												VTemp = FirstNodeV;
											  while (VTemp->Next1 != 0){
												//check if it is in victim
												if ((NodeV->Tag1 == TagDec) && (NodeV->Index1 == IndexDec) && (NodeV->BlockOffset1[BlockOffsetDec] == Address)){
													//put the old data in victim
													NodeV->Tag1 = Pointer1->Tag1;
													NodeV->Index1 = Pointer1->Index1;
													NodeV->BlockOffset1[BlockOffsetDec] = old;
													//send the correct data that found in victim, in L1 cache
													Pointer1->Tag1 = TagDec;
													Pointer1->Index1 = IndexDec;
													Pointer1->BlockOffset1[BlockOffsetDec] =  Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from VICTIM because we didn't find the correct Address in Cache and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												
												inVictim = 1;
												Pointer1->Dirty1 = 1; //For Write Back
												VictimHits++;
												hits++;

												//code to write to the output file
												fprintf(OutputFilePointer, "  VICTIM ");
												fprintf(OutputFilePointer, "HIT        "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											VTemp = VTemp->Next1;
											}//end of while

												
											if (inVictim == 0){
												//put the old data in victim cache
												NodeV->Tag1 = Pointer1->Tag1;
												NodeV->Index1 = Pointer1->Index1;
												NodeV->BlockOffset1[BlockOffsetDec] = old;
												//write the correct data from RAM
												Pointer1->Tag1 = TagDec;
												old = Pointer1->BlockOffset1[BlockOffsetDec];

												NodeV = NodeV->Next1;
												if (NodeV->Next1 ==0)
													NodeV = FirstNodeV;
										
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV+ MCRR + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCRR + MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV +  MCRR + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												

												Pointer1->Dirty1 = 1; //For Write Back
												VictimMiss++;
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   VICTIM ");
												fprintf(OutputFilePointer, "MISS       "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											//no Victim Cache
											}
											else{
												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												}
												
												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												}
	
												Pointer1->Dirty1 = 1; //For Write Back
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   MISS ");
												fprintf(OutputFilePointer, "Conflict   "); //because there was a different address from the address that we have now
												fprintf(OutputFilePointer, "  ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
											
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;
											} //end of no victim
									}//end of BO
									}
									else{ //case where tag wasnt correct. add the penalty cycles and write the new address
										if (Pointer1->Dirty1 == 1){
												total_penalty_cycles = total_penalty_cycles + (MCWR);
												penalty_cycles = (MCWR);
											}
										
										
											old = Pointer1->BlockOffset1[BlockOffsetDec];				
											
											if  (EnableVictim == 1){
												VTemp = FirstNodeV;
											  while (VTemp->Next1 != 0){
												//check if it is in victim
												if ((NodeV->Tag1 == TagDec) && (NodeV->Index1 == IndexDec) && (NodeV->BlockOffset1[BlockOffsetDec] == Address)){
													//put the old data in victim
													NodeV->Tag1 = Pointer1->Tag1;
													NodeV->Index1 = Pointer1->Index1;
													NodeV->BlockOffset1[BlockOffsetDec] = old;
													//send the correct data that found in victim, in L1 cache
													Pointer1->Tag1 = TagDec;
													Pointer1->Index1 = IndexDec;
													Pointer1->BlockOffset1[BlockOffsetDec] =  Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from VICTIM because we didn't find the correct Address in Cache and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												
												inVictim = 1;
												Pointer1->Dirty1 = 1; //For Write Back
												VictimHits++;
												hits++;

												//code to write to the output file
												fprintf(OutputFilePointer, "  VICTIM ");
												fprintf(OutputFilePointer, "HIT        "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											VTemp = VTemp->Next1;
											}//end of while

												
											if (inVictim == 0){
												//put the old data in victim cache
												NodeV->Tag1 = Pointer1->Tag1;
												NodeV->Index1 = Pointer1->Index1;
												NodeV->BlockOffset1[BlockOffsetDec] = old;
												//write the correct data from RAM
												Pointer1->Tag1 = TagDec;
												old = Pointer1->BlockOffset1[BlockOffsetDec];

												NodeV = NodeV->Next1;
												if (NodeV->Next1 ==0)
													NodeV = FirstNodeV;

												for (r=0; r<10; r++)
													Pointer1->BlockOffset1[r]= -1;
										
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV+ MCRR + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCRR + MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV +  MCRR + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												

												Pointer1->Dirty1 = 1; //For Write Back
												VictimMiss++;
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   VICTIM ");
												fprintf(OutputFilePointer, "MISS       "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											//no Victim Cache
											}
											else{
												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												}
												
												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												}
	
												Pointer1->Dirty1 = 1; //For Write Back
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   MISS ");
												fprintf(OutputFilePointer, "Conflict   "); //because there was a different address from the address that we have now
												fprintf(OutputFilePointer, "  ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
											
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;
											}

									}//end of tag
								}
								else{//if valid = 0
									
											//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
											}
												
												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
											}

											//Write Through & Write Allocate  => Write in Cache and RAM (WT)
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
											}

												//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //We read from the Cache and write in the RAM 
												penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
											}

									//code to write to the output file
									fprintf(OutputFilePointer, "   MISS ");
									fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
									fprintf(OutputFilePointer, "   ---  ");
									fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
									fprintf(OutputFilePointer,"\n"); //Print a new line
								
									Pointer1->Valid1 = 1; //Change Valid
									Pointer1->Tag1 = TagDec; //Write the correct Tag
									Pointer1->BlockOffset1[BlockOffsetDec] = Address; //Write the correct Address

									Pointer1->Dirty1 = 1; //For Write Back
									misses++;
								}//end of Valid
						}

							Pointer1 = Pointer1->Next1;
					}
					while ((Pointer1->Next1 != 0)); // || (Pointer1->Index1 > IndexDec)); //end of do while for W 

					Pointer1 = FirstNode1; //go to first Node
					}


					//The case that our arcitecture is Fully Associative
					else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1)){
							Pointer2 = FirstNode2;

							do{
								//Valid=1
								if (Pointer2->Valid2 == 1){
									//Same Tag
									if (Pointer2->Tag2 == TagDec){
										//Same BlockOffset
										if (Pointer2->BlockOffset2[BlockOffsetDec] == Address){

											//Write Back & Write Allocate => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												penalty_cycles = ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
											}
											
											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
												penalty_cycles = (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
											}

											//Write Through & Write Allocate => Write in Cache and RAM (WT) 
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) (BUT you can't write in Cache (WB))
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
												penalty_cycles = (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
											}

											Pointer2->Dirty2 = 1; //For Write Back
											hits++;
											inCache = 1; //When =1 it means that the address found or been written in Cache because Cache had free space
											Pointer2->Age = 0;

											//code to write to the output file
											fprintf(OutputFilePointer, "   HIT  ");
											fprintf(OutputFilePointer, " ---        ");
											fprintf(OutputFilePointer, "  ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											break;
										}
										//Different Block Offset
										else{
											//It means that you have free space to write in the block offset of the node
											if (Pointer2->BlockOffset2[BlockOffsetDec]== -1){

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												}

												old = Pointer2->BlockOffset2[BlockOffsetDec];
												Pointer2->BlockOffset2[BlockOffsetDec] = Address;
												Pointer2->Age = 0;
												Pointer2->Dirty2 = 1; //For Write Back
												misses++;
												inCache = 1; //When =1 it means that the address found or been written in Cache 

												//code to write to the output file
												fprintf(OutputFilePointer, "   MISS ");
												fprintf(OutputFilePointer, "Conflict   "); //because there was a different address from the address that we have now
												fprintf(OutputFilePointer, "   ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line

												break;
												}
											//Can't write the Address in a free BlockOffset in this node
											else{
												//new pointer so that we won't loose the position of Pointer2
												Pointer2b = FirstNode2; //Initialize the Pointer of the node as the first node
												
												do{
													//There is a free node to write in it
													if (Pointer2b->Valid2 == 0){
														for (r=0; r<10; r++)
															Pointer2b->BlockOffset2[r] = -1; //Free the context of the array

														//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
														if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
															penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
														}

														//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
															penalty_cycles = (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
														}

														//Write Through & Write Allocate  => Write in Cache and RAM (WT)
														else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
															penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
														}

														//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
															penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
														}

														Pointer2b->Valid2 = 1;
														Pointer2b->BlockOffset2[BlockOffsetDec] = Address;
														Pointer2b->Age = 0;
														Pointer2b->Dirty2 = 1; //For Write Back
														misses++;

														//code to write to the output file
														fprintf(OutputFilePointer, "   MISS ");
														fprintf(OutputFilePointer, "Compulsory  "); //because there was a different address from the address that we have now
														fprintf(OutputFilePointer, "  ---  ");
														fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
														fprintf(OutputFilePointer,"\n"); //Print a new line
														
														break;
													}
													Pointer2b = Pointer2b->Next2;
												}
												while (Pointer2b->Next2 != 0);

												Pointer2b = FirstNode2; //Initialize the Pointer of the node as the first node
												break;
											}
										}

									}
								}
								//Valid=0
								else if (Pointer2->Valid2 == 0){
											//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
											}

											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												penalty_cycles = (MCRL1C + MCRR + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
											}

											//Write Through & Write Allocate  => Write in Cache and RAM (WT)
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
											}


										//code to write to the output file
										fprintf(OutputFilePointer, "   MISS ");
										fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
										fprintf(OutputFilePointer, "   ---  ");
										fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
										fprintf(OutputFilePointer,"\n"); //Print a new line
								
										Pointer2->Valid2 = 1; //Change Valid
										Pointer2->Tag2 = TagDec; //Write the correct Tag
										Pointer2->BlockOffset2[BlockOffsetDec] = Address; //Write the correct Address
										Pointer2->Age = 0;
										Pointer2->Dirty2 = 1; //For Write Back
										misses++;
										inCache = 1; //When =1 it means that the address found or been written in Cache 

										break;
									}
									//Different Tag
									else{

										inCache = 0; //You have to use a policy in order to decide in which node to write the Address 

										break;	
									}

								Pointer2 = Pointer2->Next2;
							}
							while (Pointer2->Next2 != 0);

						////if there are equal it means that We have a Compulsory Miss which means that all the nodes where blank before this address
						//if(FA_Valid_Counter == CacheWords){
						//	total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR);
						//	penalty_cycles = (MCRC + MCRR);
						//	//code to write to the output file
						//	fprintf(OutputFilePointer, "   MISS ");
						//	fprintf(OutputFilePointer, "Compulsory ");
						//	fprintf(OutputFilePointer, "   ---  ");
						//	fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
						//	fprintf(OutputFilePointer,"\n"); //Print a new line
						//	}

						//We couldn't find the Address NoWhere
						if (inCache == 0){
							//total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR);
							//penalty_cycles = (MCRC + MCRR);
							////code to write to the output file
							//fprintf(OutputFilePointer, "   MISS ");
							//fprintf(OutputFilePointer, "(not in Cache) ");
							//fprintf(OutputFilePointer, " ---  ");
							//fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
							//fprintf(OutputFilePointer,"\n"); //Print a new line
							if (FIFO == 1){
															
								old = PointerFifo->BlockOffset2[BlockOffsetDec];

								//Write the new address
								for (r=0; r<10; r++)
									PointerFifo->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerFifo->Valid2 = 1;
								PointerFifo->Tag2 = TagDec;
								PointerFifo->BlockOffset2[BlockOffsetDec] = Address;
								PointerFifo->Age = 1;
								PointerFifo->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								if (PointerFifo->Next2 == 0)
									PointerFifo = FirstNode2;
								else
									PointerFifo = PointerFifo->Next2;
								break;
							}

							else if (LRU == 1){
								PointerLru = FirstNode2;
								MostUsed = FirstNode2;
								MaxUsed = PointerLru->Age; //holds the age of the node

								do{
									if (PointerLru->Age > MaxUsed){
										MaxUsed = PointerLru->Age;
										MostUsed = PointerLru;
									}
									PointerLru = PointerLru->Next2;
								}
								while (PointerLru->Next2 != 0);

								//Write the new address
								for (r=0; r<10; r++)
									MostUsed->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								MostUsed->Valid2 = 1;
								MostUsed->Tag2 = TagDec;
								old = MostUsed->BlockOffset2[BlockOffsetDec];
								MostUsed->BlockOffset2[BlockOffsetDec] = Address;
								MostUsed->Age = 1;
								MostUsed->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								break;
							}

							else if (RANDOM == 1){
								RandomNumber = rand() % (CacheEntriesL1-2);
								PointerRandom = FirstNode2;
								
								do{
									RandomNumber--;
									PointerRandom = PointerRandom->Next2;
								}
								while (RandomNumber > 0);

								//Write the new address
								for (r=0; r<10; r++)
									PointerRandom->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerRandom->Valid2 = 1;
								PointerRandom->Tag2 = TagDec;
								old = PointerRandom->BlockOffset2[BlockOffsetDec];
								PointerRandom->BlockOffset2[BlockOffsetDec] = Address;
								PointerRandom->Age = 1;
								PointerRandom->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								break;
							}

						}
					}

					else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
						Pointer3 = FirstNode3;

						while (Pointer3->Next3 != 0){
							//same index
							if (Pointer3->Index3 == IndexDec){
								//Valid=1
								if (Pointer3->Valid3 == 1){
									//Same Tag
									if (Pointer3->Tag3 == TagDec){
										//Same Block Offset
										if (Pointer3->BlockOffset3[BlockOffsetDec] == Address){

											//Write Back & Write Allocate => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												penalty_cycles = ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
											}
											
											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
												penalty_cycles = (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
											}

											//Write Through & Write Allocate => Write in Cache and RAM (WT) 
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) (BUT you can't write in Cache (WB))
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
												penalty_cycles = (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
											}

											Pointer3->Dirty3 = 1; //For Write Back
											hits++;
											inCache = 1; //When =1 it means that the address found or been written in Cache because Cache had free space
											Pointer3->Age3 = 0;
											WayInNode= Pointer3->Way;
											
											//code to write to the output file
											fprintf(OutputFilePointer, "   HIT  ");
											fprintf(OutputFilePointer, " ---        ");
											fprintf(OutputFilePointer, "  ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											/*for (y=0; y<AddBits; y++)
											printf("%d", Binary_Parser[y]);
											printf("\n");
											*/
											break;
										}
									}
									//Different Tag
									else{
											inCache = 0; //You have to use a policy in order to decide in which node to write the Address 
										break;
									}
								}
								//Valid=0
								else{
											//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
											}

											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												penalty_cycles = (MCRL1C + MCRR + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
											}

											//Write Through & Write Allocate  => Write in Cache and RAM (WT)
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
											}


										//code to write to the output file
										fprintf(OutputFilePointer, "   MISS ");
										fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
										fprintf(OutputFilePointer, "   ---  ");
										fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
										fprintf(OutputFilePointer,"\n"); //Print a new line
								
										Pointer3->Valid3 = 1; //Change Valid
										Pointer3->Tag3 = TagDec; //Write the correct Tag
										Pointer3->BlockOffset3[BlockOffsetDec] = Address; //Write the correct Address
										Pointer3->Age3 = 0;
										Pointer3->Dirty3 = 1; //For Write Back
										misses++;
										inCache = 1; //When =1 it means that the address found or been written in Cache 

										break;
									}
							}
							Pointer3 = Pointer3->Next3;
						}//end of while

						//We couldn't find the Address NoWhere
						if (inCache == 0){
													
							if (FIFO == 1){
								/*PointerFifo3 = FirstNode3; 

								//Write the new address
								for (r=0; r<10; r++)
									PointerFifo3->BlockOffset3[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerFifo3->Valid2 = 1;
								PointerFifo3->Tag2 = TagDec;
								PointerFifo3->BlockOffset2[BlockOffsetDec] = Address;
								PointerFifo3->Age = 1;
								PointerFifo3->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWR); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRC + MCWR); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								
									*/
							}

							else if (LRU == 1){
								/*PointerLru3 = FirstNode3;
								MostUsed = FirstNode2;
								MaxUsed = PointerLru->Age; //holds the age of the node

								do{
									if (PointerLru3->Age > MaxUsed){
										MaxUsed = PointerLru->Age;
										MostUsed = PointerLru;
									}
									PointerLru = PointerLru->Next2;
								}
								while (PointerLru->Next2 != 0);

								//Write the new address
								for (r=0; r<10; r++)
									MostUsed->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								MostUsed->Valid2 = 1;
								MostUsed->Tag2 = TagDec;
								MostUsed->BlockOffset2[BlockOffsetDec] = Address;
								MostUsed->Age = 1;
								MostUsed->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWR); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRC + MCWR); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line
							*/
							}

							else if (RANDOM == 1){
								PointerRandom3 = FirstNode3;

								while (PointerRandom3->Next3!=0){
									if (PointerRandom3->Index3 == IndexDec){
										RandomNumber = rand() % (Way);

										if (RandomNumber >= 0){
											NodeNumber = PointerRandom3->Number;
											RandomNumber--;
										}
									}
										PointerRandom3 = PointerRandom3->Next3;
				
								}
								PointerRandom3 = FirstNode3;

								while (PointerRandom3->Number != NodeNumber)
									PointerRandom3 = PointerRandom3->Next3;
									
								//Write the new address
								for (r=0; r<10; r++)
									PointerRandom3->BlockOffset3[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerRandom3->Valid3 = 1;
								PointerRandom3->Tag3 = TagDec;
								old = PointerRandom3->BlockOffset3[BlockOffsetDec];
								PointerRandom3->BlockOffset3[BlockOffsetDec] = Address;
								PointerRandom3->Age3 = 1;
								PointerRandom3->Dirty3 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity   "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line
							}//end of random

						}//end of in-cache
					}//end of N-Way

					break;
					   }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------//

			//If the action is Write
			case 'W' : {
					fprintf(OutputFilePointer, " WRITE   the Address:  ");
					fscanf_s(DataFilePointer, "%c", &Space, 1); //Reads one character from the Data File. This character is a space.
					fprintf(OutputFilePointer, " "); //Pritns Spaces
					//Do a loop for the lenght of the address bits that we found before
					for(l=0; l<AddBits; l++){
						fscanf_s(DataFilePointer, "%c", &Bit, 1); //Reads one character from the Data File. This character represents one bit.
						fprintf(OutputFilePointer, "%c", Bit); //Writes in the output file the Bit that is read in the for loop
						if((char)Bit=='0'){Binary_Parser[l]=0;}
						if((char)Bit=='1'){Binary_Parser[l]=1;}
					}

					Address = BinaryToDecimal(Binary_Parser, l-1);

					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.
					

					fprintf(OutputFilePointer, "   TAG:  ");
					//Write in the Output File the bits that shows the tag, according to the bits that are needed for tag (is returned from the Structures.h)
					for (n=0; n < tag ; n++){
						fprintf(OutputFilePointer,"%d", Binary_Parser[n]);
						TagBits[n]=Binary_Parser[n];
					}

					TagDec= BinaryToDecimal(TagBits, n-1); //call the Function that will convert the tag binary bits into decimal
					//printf("TAG %d ", TagDec);

					//Write in the Output File the bits that shows the index, according to the bits that are needed for index (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   INDEX: ");
					
					//If we have fully Associative architecture we don't have Index
					if (index==0){
						fprintf(OutputFilePointer,"   - ");
					}
					else{
						for (p=tag; p < (tag+index) ; p++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[p]);
							IndexBits[p]=Binary_Parser[p];
						}

						IndexDec= BinaryToDecimal(IndexBits, p-1); //call the Function that will convert the Index binary bits into decimal
						//printf("INDEX %d ", IndexDec);
					}

					//Write in the Output File the bits that shows the block offset,  according to the bits that are needed for block offset (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   BLOCK OFFSET: ");
					 
					
					//If We have only one block of words, we don't need block offset
					if (offset==0)
							fprintf(OutputFilePointer,"   - ");
					else{
						for (q=tag+index; q < (tag+index+offset) ; q++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[q]);
							BlockOffsetBits[q]=Binary_Parser[q];
						}
						BlockOffsetDec= BinaryToDecimal(BlockOffsetBits, q-1); //call the Function that will convert the BO binary bits into decimal
						//printf("BO %d \n", BlockOffsetDec);
					}

						//The case that our architecture is N_Way_Set_Associative
					if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
						//Ways are more or equal to 1
						if (Way != 0){
									Which_Way = rand() %(Way + 1); //Decides the way between the numbers 1 and
									//The number of ways must be > 0, So there isn't a Way 0
									if (Which_Way == 0){
											Which_Way = 1;
									}
							fprintf(OutputFilePointer, "   WAY:  ");
							fprintf(OutputFilePointer,"%d", Which_Way);
						}
					}


					//The case that our architecture is Direct Mapped
					if((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0)){
						Pointer1 = FirstNode1;
						do{
							//Same Index
							if (IndexDec == Pointer1->Index1){
								//Valid=1
								if (Pointer1->Valid1 == 1){
									//Same Tag
									if (TagDec == Pointer1->Tag1){
										//Same Block Offset
										if (Pointer1->BlockOffset1[BlockOffsetDec] == Address){

											//Write Back & Write Allocate => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												penalty_cycles = ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
											}
											
											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
												penalty_cycles = (MCRL1C); //NWA means that we don't write in the Cache so we only read from it
											}

											//Write Through & Write Allocate => Write in Cache and RAM (WT) 
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) (BUT you can't write in Cache (WB))
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
												penalty_cycles = (MCRL1C + MCWR); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
											}

											Pointer1->Dirty1 = 1; //For Write Back
											hits++;

											//code to write to the output file
											fprintf(OutputFilePointer, "   HIT  ");
											fprintf(OutputFilePointer, " ---        ");
											fprintf(OutputFilePointer, "  ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											/*for (y=0; y<AddBits; y++)
												printf("%d", Binary_Parser[y]);
												printf("\n");
												*/
											}
										//Different BlockOffset
										else{ 
											old = Pointer1->BlockOffset1[BlockOffsetDec];				
											
											if  (EnableVictim == 1){
												VTemp = FirstNodeV;
											  while (VTemp->Next1 != 0){
												//check if it is in victim
												if ((NodeV->Tag1 == TagDec) && (NodeV->Index1 == IndexDec) && (NodeV->BlockOffset1[BlockOffsetDec] == Address)){
													//put the old data in victim
													NodeV->Tag1 = Pointer1->Tag1;
													NodeV->Index1 = Pointer1->Index1;
													NodeV->BlockOffset1[BlockOffsetDec] = old;
													//send the correct data that found in victim, in L1 cache
													Pointer1->Tag1 = TagDec;
													Pointer1->Index1 = IndexDec;
													Pointer1->BlockOffset1[BlockOffsetDec] =  Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from VICTIM because we didn't find the correct Address in Cache and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												
												inVictim = 1;
												Pointer1->Dirty1 = 1; //For Write Back
												VictimHits++;
												hits++;

												//code to write to the output file
												fprintf(OutputFilePointer, "  VICTIM ");
												fprintf(OutputFilePointer, "HIT        "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											VTemp = VTemp->Next1;
											}//end of while

												
											if (inVictim == 0){
												//put the old data in victim cache
												NodeV->Tag1 = Pointer1->Tag1;
												NodeV->Index1 = Pointer1->Index1;
												NodeV->BlockOffset1[BlockOffsetDec] = old;
												//write the correct data from RAM
												Pointer1->Tag1 = TagDec;
												old = Pointer1->BlockOffset1[BlockOffsetDec];

												NodeV = NodeV->Next1;
												if (NodeV->Next1 ==0)
													NodeV = FirstNodeV;
										
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV+ MCRR + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCRR + MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV +  MCRR + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												

												Pointer1->Dirty1 = 1; //For Write Back
												VictimMiss++;
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   VICTIM ");
												fprintf(OutputFilePointer, "MISS       "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											//no Victim Cache
											}
											else{
												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												}
												
												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												}
	
												Pointer1->Dirty1 = 1; //For Write Back
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   MISS ");
												fprintf(OutputFilePointer, "Conflict   "); //because there was a different address from the address that we have now
												fprintf(OutputFilePointer, "  ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
											
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;
											} //end of no victim
									}//end of BO
									}
									else{ //case where tag wasnt correct. add the penalty cycles and write the new address
										if (Pointer1->Dirty1 == 1){
												total_penalty_cycles = total_penalty_cycles + (MCWR);
												penalty_cycles = (MCWR);
											}
										
										
											old = Pointer1->BlockOffset1[BlockOffsetDec];				
											
											if  (EnableVictim == 1){
												VTemp = FirstNodeV;
											  while (VTemp->Next1 != 0){
												//check if it is in victim
												if ((NodeV->Tag1 == TagDec) && (NodeV->Index1 == IndexDec) && (NodeV->BlockOffset1[BlockOffsetDec] == Address)){
													//put the old data in victim
													NodeV->Tag1 = Pointer1->Tag1;
													NodeV->Index1 = Pointer1->Index1;
													NodeV->BlockOffset1[BlockOffsetDec] = old;
													//send the correct data that found in victim, in L1 cache
													Pointer1->Tag1 = TagDec;
													Pointer1->Index1 = IndexDec;
													Pointer1->BlockOffset1[BlockOffsetDec] =  Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCWL1C); //We read from Cache, Read from VICTIM because we didn't find the correct Address in Cache and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												
												inVictim = 1;
												Pointer1->Dirty1 = 1; //For Write Back
												VictimHits++;
												hits++;

												//code to write to the output file
												fprintf(OutputFilePointer, "  VICTIM ");
												fprintf(OutputFilePointer, "HIT        "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											VTemp = VTemp->Next1;
											}//end of while

												
											if (inVictim == 0){
												//put the old data in victim cache
												NodeV->Tag1 = Pointer1->Tag1;
												NodeV->Index1 = Pointer1->Index1;
												NodeV->BlockOffset1[BlockOffsetDec] = old;
												//write the correct data from RAM
												Pointer1->Tag1 = TagDec;
												old = Pointer1->BlockOffset1[BlockOffsetDec];

												NodeV = NodeV->Next1;
												if (NodeV->Next1 ==0)
													NodeV = FirstNodeV;

												for (r=0; r<10; r++)
													Pointer1->BlockOffset1[r]= -1;
										
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV+ MCRR + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCRR + MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV +  MCRR + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												

												Pointer1->Dirty1 = 1; //For Write Back
												VictimMiss++;
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   VICTIM ");
												fprintf(OutputFilePointer, "MISS       "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											//no Victim Cache
											}
											else{
												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												}
												
												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												}
	
												Pointer1->Dirty1 = 1; //For Write Back
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   MISS ");
												fprintf(OutputFilePointer, "Conflict   "); //because there was a different address from the address that we have now
												fprintf(OutputFilePointer, "  ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
											
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;
											}

									}//end of tag
								}
								else{//if valid = 0
									
											//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
											}
												
												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
											}

											//Write Through & Write Allocate  => Write in Cache and RAM (WT)
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
											}

												//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR); //We read from the Cache and write in the RAM 
												penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
											}

									//code to write to the output file
									fprintf(OutputFilePointer, "   MISS ");
									fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
									fprintf(OutputFilePointer, "   ---  ");
									fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
									fprintf(OutputFilePointer,"\n"); //Print a new line
								
									Pointer1->Valid1 = 1; //Change Valid
									Pointer1->Tag1 = TagDec; //Write the correct Tag
									Pointer1->BlockOffset1[BlockOffsetDec] = Address; //Write the correct Address

									Pointer1->Dirty1 = 1; //For Write Back
									misses++;
								}//end of Valid
						}

							Pointer1 = Pointer1->Next1;
					}
					while ((Pointer1->Next1 != 0)); // || (Pointer1->Index1 > IndexDec)); //end of do while for W 

					Pointer1 = FirstNode1; //go to first Node
					}


					//The case that our arcitecture is Fully Associative
					else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1)){
							Pointer2 = FirstNode2;

							do{
								//Valid=1
								if (Pointer2->Valid2 == 1){
									//Same Tag
									if (Pointer2->Tag2 == TagDec){
										//Same BlockOffset
										if (Pointer2->BlockOffset2[BlockOffsetDec] == Address){

											//Write Back & Write Allocate => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												penalty_cycles = ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
											}
											
											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C+ MCWL1C); //NWA means that we don't write in the Cache so we only read from it
												penalty_cycles = (MCRL1C+ MCWL1C); //NWA means that we don't write in the Cache so we only read from it
											}

											//Write Through & Write Allocate => Write in Cache and RAM (WT) 
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) (BUT you can't write in Cache (WB))
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR + MCWL1C); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
												penalty_cycles = (MCRL1C + MCWR + MCWL1C); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
											}

											Pointer2->Dirty2 = 1; //For Write Back
											hits++;
											inCache = 1; //When =1 it means that the address found or been written in Cache because Cache had free space
											Pointer2->Age = 0;

											//code to write to the output file
											fprintf(OutputFilePointer, "   HIT  ");
											fprintf(OutputFilePointer, " ---        ");
											fprintf(OutputFilePointer, "  ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											/*for (y=0; y<AddBits; y++)
											printf("%d", Binary_Parser[y]);
											printf("\n");
											*/
											break;
										}
										//Different Block Offset
										else{
											//It means that you have free space to write in the block offset of the node
											if (Pointer2->BlockOffset2[BlockOffsetDec]== -1){

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												}

												Pointer2->BlockOffset2[BlockOffsetDec] = Address;
												Pointer2->Age = 0;
												Pointer2->Dirty2 = 1; //For Write Back
												misses++;
												inCache = 1; //When =1 it means that the address found or been written in Cache 

												//code to write to the output file
												fprintf(OutputFilePointer, "   MISS ");
												fprintf(OutputFilePointer, "Conflict   "); //because there was a different address from the address that we have now
												fprintf(OutputFilePointer, "   ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line

												break;
												}
											//Can't write the Address in a free BlockOffset in this node
											else{
												//new pointer so that we won't loose the position of Pointer2
												Pointer2b = FirstNode2; //Initialize the Pointer of the node as the first node
												
												do{
													//There is a free node to write in it
													if (Pointer2b->Valid2 == 0){
														for (r=0; r<10; r++)
															Pointer2b->BlockOffset2[r] = -1; //Free the context of the array

														//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
														if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
															penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
														}

														//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
															penalty_cycles = (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
														}

														//Write Through & Write Allocate  => Write in Cache and RAM (WT)
														else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
															penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
														}

														//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
															penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
														}

														Pointer2b->Valid2 = 1;
														Pointer2b->BlockOffset2[BlockOffsetDec] = Address;
														Pointer2b->Age = 0;
														Pointer2b->Dirty2 = 1; //For Write Back
														misses++;

														//code to write to the output file
														fprintf(OutputFilePointer, "   MISS ");
														fprintf(OutputFilePointer, "Compulsory  "); //because there was a different address from the address that we have now
														fprintf(OutputFilePointer, "  ---  ");
														fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
														fprintf(OutputFilePointer,"\n"); //Print a new line
														
														break;
													}
													Pointer2b = Pointer2b->Next2;
												}
												while (Pointer2b->Next2 != 0);

												Pointer2b = FirstNode2; //Initialize the Pointer of the node as the first node
												break;
											}
										}

									}
								}
								//Valid=0
								else if (Pointer2->Valid2 == 0){
											//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
											}

											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												penalty_cycles = (MCRL1C + MCRR + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
											}

											//Write Through & Write Allocate  => Write in Cache and RAM (WT)
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
											}


										//code to write to the output file
										fprintf(OutputFilePointer, "   MISS ");
										fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
										fprintf(OutputFilePointer, "   ---  ");
										fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
										fprintf(OutputFilePointer,"\n"); //Print a new line
								
										Pointer2->Valid2 = 1; //Change Valid
										Pointer2->Tag2 = TagDec; //Write the correct Tag
										Pointer2->BlockOffset2[BlockOffsetDec] = Address; //Write the correct Address
										Pointer2->Age = 0;
										Pointer2->Dirty2 = 1; //For Write Back
										misses++;
										inCache = 1; //When =1 it means that the address found or been written in Cache 

										break;
									}
									//Different Tag
									else{
										//if (Pointer2->Dirty2 == 1){
										//	total_penalty_cycles = total_penalty_cycles + (MCWR);
										//	penalty_cycles = (MCWR);
										//}
										//
										//	//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
										//	if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
										//		total_penalty_cycles = total_penalty_cycles + ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
										//		penalty_cycles = ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
										//	} 
										//	//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
										//	else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
										//		total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
										//		penalty_cycles = (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
										//	}

										//	//Write Through & Write Allocate  => Write in Cache and RAM (WT)
										//	else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
										//		total_penalty_cycles = total_penalty_cycles + (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
										//		penalty_cycles = (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
										//	}

										//	//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
										//	else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
										//		total_penalty_cycles = total_penalty_cycles + (MCRC + MCWR); //We read from the Cache and write in the RAM 
										//		penalty_cycles = (MCRC + MCWR); //We read from the Cache and write in the RAM 
										//	}

										//	
										//Pointer2->Dirty2 = 1; //For Write Back
										//misses++;		

										inCache = 0; //You have to use a policy in order to decide in which node to write the Address 

										//	//code to write to the output file
										//	fprintf(OutputFilePointer, "   MISS ");
										//	fprintf(OutputFilePointer, "Conflict   "); //
										//	fprintf(OutputFilePointer, "   ---  ");
										//	fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
										//	fprintf(OutputFilePointer,"\n"); //Print a new line
										//	Pointer2->Tag2 = TagDec;

										//for (r=0; r<10; r++)
										//	Pointer2->BlockOffset2[r]= -1;
										//
										//Pointer2->BlockOffset2[BlockOffsetDec] = Address;
										break;
										
									}

								Pointer2 = Pointer2->Next2;
							}
							while (Pointer2->Next2 != 0);

						////if there are equal it means that We have a Compulsory Miss which means that all the nodes where blank before this address
						//if(FA_Valid_Counter == CacheWords){
						//	total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR);
						//	penalty_cycles = (MCRC + MCRR);
						//	//code to write to the output file
						//	fprintf(OutputFilePointer, "   MISS ");
						//	fprintf(OutputFilePointer, "Compulsory ");
						//	fprintf(OutputFilePointer, "   ---  ");
						//	fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
						//	fprintf(OutputFilePointer,"\n"); //Print a new line
						//	}

						//We couldn't find the Address NoWhere
						if (inCache == 0){
							//total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR);
							//penalty_cycles = (MCRC + MCRR);
							////code to write to the output file
							//fprintf(OutputFilePointer, "   MISS ");
							//fprintf(OutputFilePointer, "(not in Cache) ");
							//fprintf(OutputFilePointer, " ---  ");
							//fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
							//fprintf(OutputFilePointer,"\n"); //Print a new line
							if (FIFO == 1){

								//Write the new address
								for (r=0; r<10; r++)
									PointerFifo->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerFifo->Valid2 = 1;
								PointerFifo->Tag2 = TagDec;
								PointerFifo->BlockOffset2[BlockOffsetDec] = Address;
								PointerFifo->Age = 1;
								PointerFifo->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								if (PointerFifo->Next2 == 0)
									PointerFifo = FirstNode2;
								else
									PointerFifo = PointerFifo->Next2;
								break;
							}

							else if (LRU == 1){
								PointerLru = FirstNode2;
								MostUsed = FirstNode2;
								MaxUsed = PointerLru->Age; //holds the age of the node

								do{
									if (PointerLru->Age > MaxUsed){
										MaxUsed = PointerLru->Age;
										MostUsed = PointerLru;
									}
									PointerLru = PointerLru->Next2;
								}
								while (PointerLru->Next2 != 0);

								//Write the new address
								for (r=0; r<10; r++)
									MostUsed->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								MostUsed->Valid2 = 1;
								MostUsed->Tag2 = TagDec;
								MostUsed->BlockOffset2[BlockOffsetDec] = Address;
								MostUsed->Age = 1;
								MostUsed->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								break;
							}

							else if (RANDOM == 1){
								RandomNumber = rand() % (CacheEntriesL1-2);
								PointerRandom = FirstNode2;
								
								do{
									RandomNumber--;
									PointerRandom = PointerRandom->Next2;
								}
								while (RandomNumber > 0);

								//Write the new address
								for (r=0; r<10; r++)
									PointerRandom->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerRandom->Valid2 = 1;
								PointerRandom->Tag2 = TagDec;
								PointerRandom->BlockOffset2[BlockOffsetDec] = Address;
								PointerRandom->Age = 1;
								PointerRandom->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								break;
							}

						}
					}

					else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
						Pointer3 = FirstNode3;

						while (Pointer3->Next3 != 0){
							//same index
							if (Pointer3->Index3 == IndexDec){
								//Valid=1
								if (Pointer3->Valid3 == 1){
									//Same Tag
									if (Pointer3->Tag3 == TagDec){
										//Same Block Offset
										if (Pointer3->BlockOffset3[BlockOffsetDec] == Address){

											//Write Back & Write Allocate => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												penalty_cycles = ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCWL1C); //We read from the cache and we write in the cache
											}
											
											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C+ MCWL1C); //NWA means that we don't write in the Cache so we only read from it
												penalty_cycles = (MCRL1C+ MCWL1C); //NWA means that we don't write in the Cache so we only read from it
											}

											//Write Through & Write Allocate => Write in Cache and RAM (WT) 
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the cache, we write in the cache and in RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) (BUT you can't write in Cache (WB))
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR + MCWL1C); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
												penalty_cycles = (MCRL1C + MCWR + MCWL1C); //NWA means that we don't write in the Cache so we only read from it and write in in RAM and in Cache
											}

											Pointer3->Dirty3 = 1; //For Write Back
											hits++;
											inCache = 1; //When =1 it means that the address found or been written in Cache because Cache had free space
											Pointer3->Age3 = 0;
											WayInNode= Pointer3->Way;
											
											//code to write to the output file
											fprintf(OutputFilePointer, "   HIT  ");
											fprintf(OutputFilePointer, " ---        ");
											fprintf(OutputFilePointer, "  ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											/*for (y=0; y<AddBits; y++)
											printf("%d", Binary_Parser[y]);
											printf("\n");
											*/
											break;
										}
									}
									//Different Tag
									else{
											inCache = 0; //You have to use a policy in order to decide in which node to write the Address 
										break;
									}
								}
								//Valid=0
								else{
											//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
											if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
												penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
											}

											//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
												penalty_cycles = (MCRL1C + MCRR + MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
											}

											//Write Through & Write Allocate  => Write in Cache and RAM (WT)
											else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
											}

											//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
											else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
											}


										//code to write to the output file
										fprintf(OutputFilePointer, "   MISS ");
										fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
										fprintf(OutputFilePointer, "   ---  ");
										fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
										fprintf(OutputFilePointer,"\n"); //Print a new line
								
										Pointer3->Valid3 = 1; //Change Valid
										Pointer3->Tag3 = TagDec; //Write the correct Tag
										Pointer3->BlockOffset3[BlockOffsetDec] = Address; //Write the correct Address
										Pointer3->Age3 = 0;
										Pointer3->Dirty3 = 1; //For Write Back
										misses++;
										inCache = 1; //When =1 it means that the address found or been written in Cache 

										break;
									}
							}
							Pointer3 = Pointer3->Next3;
						}//end of while

						//We couldn't find the Address NoWhere
						if (inCache == 0){
													
							if (FIFO == 1){
								/*PointerFifo3 = FirstNode3; 

								//Write the new address
								for (r=0; r<10; r++)
									PointerFifo3->BlockOffset3[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerFifo3->Valid2 = 1;
								PointerFifo3->Tag2 = TagDec;
								PointerFifo3->BlockOffset2[BlockOffsetDec] = Address;
								PointerFifo3->Age = 1;
								PointerFifo3->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWR); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRC + MCWR); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line

								
									*/
							}

							else if (LRU == 1){
								/*PointerLru3 = FirstNode3;
								MostUsed = FirstNode2;
								MaxUsed = PointerLru->Age; //holds the age of the node

								do{
									if (PointerLru3->Age > MaxUsed){
										MaxUsed = PointerLru->Age;
										MostUsed = PointerLru;
									}
									PointerLru = PointerLru->Next2;
								}
								while (PointerLru->Next2 != 0);

								//Write the new address
								for (r=0; r<10; r++)
									MostUsed->BlockOffset2[BlockOffsetDec] = -1; //clear the previous addresses
								
								MostUsed->Valid2 = 1;
								MostUsed->Tag2 = TagDec;
								MostUsed->BlockOffset2[BlockOffsetDec] = Address;
								MostUsed->Age = 1;
								MostUsed->Dirty2 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRC + MCRR + MCWC); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRC + MCRR); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRC + MCWC + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRC + MCWR); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRC + MCWR); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line
							*/
							}

							else if (RANDOM == 1){
								PointerRandom3 = FirstNode3;

								while (PointerRandom3->Next3!=0){
									if (PointerRandom3->Index3 == IndexDec){
										RandomNumber = rand() % (Way);

										if (RandomNumber >= 0){
											NodeNumber = PointerRandom3->Number;
											RandomNumber--;
										}
									}
										PointerRandom3 = PointerRandom3->Next3;
				
								}
								PointerRandom3 = FirstNode3;

								while (PointerRandom3->Number != NodeNumber)
									PointerRandom3 = PointerRandom3->Next3;
									
								//Write the new address
								for (r=0; r<10; r++)
									PointerRandom3->BlockOffset3[BlockOffsetDec] = -1; //clear the previous addresses
								
								PointerRandom3->Valid3 = 1;
								PointerRandom3->Tag3 = TagDec;
								PointerRandom3->BlockOffset3[BlockOffsetDec] = Address;
								PointerRandom3->Age3 = 1;
								PointerRandom3->Dirty3 = 1;
								misses++;

								//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
								if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
									penalty_cycles = ( MCRL1C + MCRR + MCWL1C); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
								}
												
								//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
								}

								//Write Through & Write Allocate  => Write in Cache and RAM (WT)
								else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
									penalty_cycles = (MCRL1C + MCWL1C + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
								}

								//Write Back & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
								else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
									penalty_cycles = (MCRL1C + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
								}

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Capacity   "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line
							}//end of random

						}//end of in-cache
					}//end of N-Way

					break;
					   }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------//

			//If the action is Modify
			case 'M' : {
					fprintf(OutputFilePointer, " MODIFY  the Address: ");
					fscanf_s(DataFilePointer, "%c", &Space,1); //Reads one character from the Data File. This character is a space.
					fprintf(OutputFilePointer, "  "); //Pritns Spaces
					//Do a loop for the lenght of the address bits that we found before
					for(m=0; m<AddBits; m++){
						fscanf_s(DataFilePointer, "%c", &Bit, 1); //Reads one character from the Data File. This character represents one bit.
						fprintf(OutputFilePointer, "%c", Bit); //Writes in the output file the Bit that is read in the for loop
						if((char)Bit=='0'){Binary_Parser[m]=0;}
						if((char)Bit=='1'){Binary_Parser[m]=1;}
					}

					Address = BinaryToDecimal(Binary_Parser, m-1);

					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.
					

					fprintf(OutputFilePointer, "   TAG:  ");
					//Write in the Output File the bits that shows the tag, according to the bits that are needed for tag (is returned from the Structures.h)
					for (n=0; n < tag ; n++){
						fprintf(OutputFilePointer,"%d", Binary_Parser[n]);
						TagBits[n]=Binary_Parser[n];
					}

					TagDec= BinaryToDecimal(TagBits, n-1); //call the Function that will convert the tag binary bits into decimal
					//printf("TAG %d ", TagDec);

					//Write in the Output File the bits that shows the index, according to the bits that are needed for index (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   INDEX: ");
					
					//If we have fully Associative architecture we don't have Index
					if (index==0){
						fprintf(OutputFilePointer,"   - ");
					}
					else{
						for (p=tag; p < (tag+index) ; p++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[p]);
							IndexBits[p]=Binary_Parser[p];
						}
					IndexDec= BinaryToDecimal(IndexBits, p-1); //call the Function that will convert the Index binary bits into decimal
					//printf("INDEX %d ", IndexDec);
					}


					//Write in the Output File the bits that shows the block offset,  according to the bits that are needed for block offset (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   BLOCK OFFSET: ");
					 
					
					//If We have only one block of words, we don't need block offset
					if (offset==0)
							fprintf(OutputFilePointer,"   - ");
					else{
						for (q=tag+index; q < (tag+index+offset) ; q++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[q]);
							BlockOffsetBits[q]=Binary_Parser[q];
						}
					BlockOffsetDec= BinaryToDecimal(BlockOffsetBits, q-1); //call the Function that will convert the BO binary bits into decimal
					//printf("BO %d \n", BlockOffsetDec);
					}

						//The case that our architecture is N_Way_Set_Associative
						if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
						//Ways are more or equal to 1
						if (Way != 0){
									Which_Way = rand() %(Way + 1); //Decides the way between the numbers 1 and %way
									//The number of ways must be > 0, So there isn't a Way 0
									if (Which_Way == 0){
											Which_Way = 1;
									}
							fprintf(OutputFilePointer, "   WAY:  ");
							fprintf(OutputFilePointer,"%d", Which_Way);
						}
					}

					
					//The case that our architecture is Direct Mapped
					if((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0)){
						Pointer1 = FirstNode1; //Go to first node
						//We use do- while so that we cover the case of the last node
						do {
							//Same Index
							if (IndexDec == Pointer1->Index1){
								//Valid=1
								if (Pointer1->Valid1 == 1){
									//Same Tag
									if (TagDec == Pointer1->Tag1){
										//Same Block Offset
											if (Pointer1->BlockOffset1[BlockOffsetDec] == Address){
													penalty_cycles = MCRL1C; //We have to read from the cache
													total_penalty_cycles = (total_penalty_cycles + MCRL1C); //We have to read from the cache
													hits++;

													//code to write to the output file
													fprintf(OutputFilePointer, "   HIT  ");
													fprintf(OutputFilePointer, " ---        ");
													fprintf(OutputFilePointer, "  ---  ");
													fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
													fprintf(OutputFilePointer,"\n"); //Print a new line
													//for (y=0; y<AddBits; y++)
													//	printf("%d", Binary_Parser[y]);
													//printf("\n");
											}
											//Different Block Offset
											else{

												old = Pointer1->BlockOffset1[BlockOffsetDec];				
											
												if  (EnableVictim == 1){
													VTemp = FirstNodeV;

													  while (VTemp->Next1 != 0){
														//check if it is in victim
														if ((NodeV->Tag1 == TagDec) && (NodeV->Index1 == IndexDec) && (NodeV->BlockOffset1[BlockOffsetDec] == Address)){
													
														//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
														if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
															penalty_cycles = ( MCRL1C + MCRV); //We read from Cache, Read from VICTIM because we didn't find the correct Address in Cache and writeit in Cache
														}

														//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
															penalty_cycles = (MCRL1C + MCRV); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
														}

														//Write Through & Write Allocate  => Write in Cache and RAM (WT)
														else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV); //We read from the Cache, Write in the Cache and write in the RAM too
															penalty_cycles = (MCRL1C + MCRV); //We read from the Cache, Write in the Cache and write in the RAM too
														}

														//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV); //We read from the Cache and write in the RAM 
															penalty_cycles = (MCRL1C + MCRV); //We read from the Cache and write in the RAM 
														} 
												
														inVictim = 1;
														Pointer1->Dirty1 = 1; //For Write Back
														VictimHits++;
														hits++;

														//code to write to the output file
														fprintf(OutputFilePointer, "  VICTIM ");
														fprintf(OutputFilePointer, "HIT        "); //because we found the address in victim cache
														fprintf(OutputFilePointer, " ---  ");
														fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
														fprintf(OutputFilePointer,"\n"); //Print a new line
														}

													VTemp = VTemp->Next1;
													}//end of while

													  if (inVictim == 0){
												
														NodeV = NodeV->Next1;
														if (NodeV->Next1 ==0)
															NodeV = FirstNodeV;
										
														Pointer1->BlockOffset1[BlockOffsetDec] = Address;

														//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
														if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
															penalty_cycles = ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
															}
	
														//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
															penalty_cycles = (MCRL1C + MCRV+ MCRR + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
															}

														//Write Through & Write Allocate  => Write in Cache and RAM (WT)
														else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
															penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
															}

														//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
														else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
															total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCRR + MCWL1C); //We read from the Cache and write in the RAM 
															penalty_cycles = (MCRL1C + MCRV +  MCRR + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
															} 
												

														Pointer1->Dirty1 = 1; //For Write Back
														VictimMiss++;
														misses++;

														//code to write to the output file
														fprintf(OutputFilePointer, "   VICTIM ");
														fprintf(OutputFilePointer, "MISS       "); //because we found the address in victim cache
														fprintf(OutputFilePointer, " ---  ");
														fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
														fprintf(OutputFilePointer,"\n"); //Print a new line
														}
																						
												}//No victim
												else
												{
													penalty_cycles = (MCRL1C+MCRR); //We have to read from the cache
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
													misses++;
												
													//code to write to the output file
													fprintf(OutputFilePointer, "   MISS ");
													fprintf(OutputFilePointer, "           "); //because there was a different address from the address that we have now
													fprintf(OutputFilePointer, "   ---  ");
													fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
													fprintf(OutputFilePointer,"\n"); //Print a new line
												}
										}//different BO
									}
									//Different Tag
									else{
										
										old = Pointer1->BlockOffset1[BlockOffsetDec];				
											
										if  (EnableVictim == 1){
												VTemp = FirstNodeV;
											  while (VTemp->Next1 != 0){
												//check if it is in victim
												if ((NodeV->Tag1 == TagDec) && (NodeV->Index1 == IndexDec) && (NodeV->BlockOffset1[BlockOffsetDec] == Address)){
													
												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV); //We read from Cache, Read from RAM because we didn't find the correct Address in Cache and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV); //We read from Cache, Read from VICTIM because we didn't find the correct Address in Cache and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCRV); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV); //We read from the Cache and write in the RAM 
												} 
												
												inVictim = 1;
												Pointer1->Dirty1 = 1; //For Write Back
												VictimHits++;
												hits++;

												//code to write to the output file
												fprintf(OutputFilePointer, "  VICTIM ");
												fprintf(OutputFilePointer, "HIT        "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												}
											VTemp = VTemp->Next1;
											}//end of while

											  if (inVictim == 0){
												
												NodeV = NodeV->Next1;
												if (NodeV->Next1 ==0)
													NodeV = FirstNodeV;
										
												Pointer1->BlockOffset1[BlockOffsetDec] = Address;

												//Write Back & Write Allocate  => Don't Write in RAM (WB) and you can write in Cache (WA)
												if ((( WA == 1) && (NWA == 0)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
													penalty_cycles = ( MCRL1C + MCRV + MCRR + MCWL1C); //We read from Cache, Read from VICTIM and read from RAM because we didn't find the correct Address in either of them and writeit in Cache
												}

												//Write Back & No Write Allocate => Don't Write in RAM (WB) and don't write in Cache (NWA)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 0) && (WB == 1))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCRR+ MCWL1C); //We read from Cache and read from RAM because we didn't find it in RAM. We don't bring it in Cache memory
													penalty_cycles = (MCRL1C + MCRV+ MCRR + MCWL1C); //We read from Cache and read from VICTIM because we didn't find it in RAM. We don't bring it in Cache memory
												}

												//Write Through & Write Allocate  => Write in Cache and RAM (WT)
												else if ((( WA == 1) && (NWA == 0)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
													penalty_cycles = (MCRL1C + MCWL1C + MCRV + MCRR + MCWR); //We read from the Cache, Write in the Cache and write in the RAM too
												}

												//Write Through & No Write Allocate => Write in Cache and RAM (WT) BUT you can't write in Cache (WB)
												else if ((( WA == 0) && (NWA == 1)) && ( (WT == 1) && (WB == 0))){
													total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRV + MCWR+ MCRR + MCWL1C); //We read from the Cache and write in the RAM 
													penalty_cycles = (MCRL1C + MCRV +  MCRR + MCWR+ MCWL1C); //We read from the Cache and write in the RAM 
												} 
												

												Pointer1->Dirty1 = 1; //For Write Back
												VictimMiss++;
												misses++;

												//code to write to the output file
												fprintf(OutputFilePointer, "   VICTIM ");
												fprintf(OutputFilePointer, "MISS       "); //because we found the address in victim cache
												fprintf(OutputFilePointer, " ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
											}//stop in Victim = 0
								
								}
								else
								{
									penalty_cycles = (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR + MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
									misses++;
										
									//code to write to the output file
									fprintf(OutputFilePointer, "   MISS ");
									fprintf(OutputFilePointer, "           "); //
									fprintf(OutputFilePointer, "   ---  ");
									fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
									fprintf(OutputFilePointer,"\n"); //Print a new line
								}
							}
							}
							//Valid=0
							else{								
									penalty_cycles = (MCRL1C + MCRR + MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //
									misses++;
									
									//code to write to the output file
									fprintf(OutputFilePointer, "   MISS ");
									fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
									fprintf(OutputFilePointer, "   ---  ");
									fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
									fprintf(OutputFilePointer,"\n"); //Print a new line
								}//end of valid
							}//end of index
							
							Pointer1 = Pointer1-> Next1;
						}
						while ((Pointer1->Next1 != 0)); //|| ((Pointer1->Index1) > IndexDec)); //end of do while R

					Pointer1 = FirstNode1;
					
					} //end of if DM


										//The case that our arcitecture is Fully Associative
					else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1)){
						Pointer2 = FirstNode2; //Go to first node

						//We use do- while so that we cover the case of the last node
						do {		
							//Valid=1 and Same Tag
							if (Pointer2->Valid2 == 1){
								if (Pointer2->Tag2 == TagDec){
									//Same Block Offset
									if (Pointer2->BlockOffset2[BlockOffsetDec] == Address){
										penalty_cycles = MCRL1C; //We have to read from the cache
										total_penalty_cycles = (total_penalty_cycles + MCRL1C); //We have to read from the cache
										hits++;
										inCache = 1; //When =1 it means that the address found in Cache

										//code to write to the output file
										fprintf(OutputFilePointer, "   HIT  ");
										fprintf(OutputFilePointer, " ---        ");
										fprintf(OutputFilePointer, "  ---  ");
										fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
										fprintf(OutputFilePointer,"\n"); //Print a new line
										//for (y=0; y<AddBits; y++)
										//	printf("%d", Binary_Parser[y]);
										//printf("\n");
										break;
									}
									//Different Block Offset
									else{
										penalty_cycles = (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
										total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
										misses++;
										inCache = 1; //When =1 it means that the address found or been written in Cache 

										//code to write to the output file
										fprintf(OutputFilePointer, "   MISS ");
										fprintf(OutputFilePointer, "           "); //because there was a different address from the address that we have now
										fprintf(OutputFilePointer, "   ---  ");
										fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
										fprintf(OutputFilePointer,"\n"); //Print a new line
										break;
										}
									}
								//Different Tag
								else{
									penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
									total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
									misses++;
									inCache = 1; //When =1 it means that the address found or been written in Cache 

									//code to write to the output file
									fprintf(OutputFilePointer, "   MISS ");
									fprintf(OutputFilePointer, "           "); //
									fprintf(OutputFilePointer, "   ---  ");
									fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
									fprintf(OutputFilePointer,"\n"); //Print a new line
									break;
									}
							}
							//Valid=0
							else{
								penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //
								total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //
								misses++;
								inCache = 1; //When =1 it means that the address found or been written in Cache 

								//code to write to the output file
								fprintf(OutputFilePointer, "   MISS ");
								fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
								fprintf(OutputFilePointer, "   ---  ");
								fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
								fprintf(OutputFilePointer,"\n"); //Print a new line
								break;
								}
							
							Pointer2 = Pointer2->Next2;
						}
						while ((Pointer2->Next2 != 0));
						
						////if there are equal it means that We have a Compulsory Miss which means that all the nodes where blank before this address
						//if(FA_Valid_Counter == CacheWords){
						//	total_penalty_cycles = total_penalty_cycles + (MCRC + MCRR);
						//	penalty_cycles = (MCRC + MCRR);
						//	//code to write to the output file
						//	fprintf(OutputFilePointer, "   MISS ");
						//	fprintf(OutputFilePointer, "Compulsory ");
						//	fprintf(OutputFilePointer, "   ---  ");
						//	fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
						//	fprintf(OutputFilePointer,"\n"); //Print a new line
						//	}

						//We couldn't find the Address NoWhere
						if (inCache == 0){
							total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C);
							penalty_cycles = (MCRL1C + MCRR);
							//code to write to the output file
							fprintf(OutputFilePointer, "   MISS ");
							fprintf(OutputFilePointer, "(not in Cache) ");
							fprintf(OutputFilePointer, " ---  ");
							fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
							fprintf(OutputFilePointer,"\n"); //Print a new line
							break;
						}

						Pointer2 = FirstNode2;
					}//end of if FA

					//The case that our arcitecture is N-Way Set Associative
					else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
						//fprintf(OutputFilePointer, "\n");
						Pointer3 = FirstNode3;

						while (Pointer3->Next3 != 0){
							//Same Index
							if (Pointer3->Index3 == IndexDec){
								//Valid=1
								if (Pointer3->Valid3 == 1){
									//Same Tag
									if (Pointer3->Tag3 == TagDec){
										//Same Block OFFset
										if (Pointer3->BlockOffset3[BlockOffsetDec] == Address){
											penalty_cycles = MCRL1C; //We have to read from the cache
											total_penalty_cycles = (total_penalty_cycles + MCRL1C); //We have to read from the cache
											hits++;
											inCache = 1; //When =1 it means that the address found in Cache

											//code to write to the output file
											fprintf(OutputFilePointer, "   HIT  ");
											fprintf(OutputFilePointer, " ---        ");
											fprintf(OutputFilePointer, "  ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											//for (y=0; y<AddBits; y++)
											//	printf("%d", Binary_Parser[y]);
											//printf("\n");
											
											WayInNode = Pointer3->Way;
											break;
										}
										//Different Block Offset
										else{
											if (ReadModify == 0){
												penalty_cycles = (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
												total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
												misses++;
												inCache = 1; //When =1 it means that the address found or been written in Cache 
												ReadModify =1;
											}
											//code to write to the output file
											fprintf(OutputFilePointer, "   MISS ");
											fprintf(OutputFilePointer, "Conflict   "); //because there was a different address from the address that we have now
											fprintf(OutputFilePointer, "   ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											//}
											break;
										}
									}
									//Different Tag
									else{
										if (ReadModify == 0){
											penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
											total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //We have to read from the cache and read from RAM after that because we didn't find the right address in the cache
											misses++;
											inCache = 1; //When =1 it means that the address found or been written in Cache 
											ReadModify =1;
										}
											if (MissRM == 0){
												//code to write to the output file
												fprintf(OutputFilePointer, "   MISS ");
												fprintf(OutputFilePointer, " Conflict  "); //
												fprintf(OutputFilePointer, "   ---  ");
												fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
												fprintf(OutputFilePointer,"\n"); //Print a new line
												MissRM = 1;
											}
										break;
									}
								}
								//Valid=0
								else{
									//to not be repeated every time
									if (ReadModify == 0){
										penalty_cycles =  (MCRL1C + MCRR+ MCWL1C); //
										total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C); //
										misses++;
										inCache = 1; //When =1 it means that the address found or been written in Cache 
										ReadModify =1;
									}
										if (MissRM == 0){
											//code to write to the output file
											fprintf(OutputFilePointer, "   MISS ");
											fprintf(OutputFilePointer, "Compulsory "); //The Valid = 0. That means that we don't have a previous address in this node
											fprintf(OutputFilePointer, "   ---  ");
											fprintf(OutputFilePointer,"PENALTY CYCLES: %d", penalty_cycles);
											fprintf(OutputFilePointer,"\n"); //Print a new line
											MissRM =1 ;
										 }
									break;
								}
							}
							Pointer3 = Pointer3->Next3; //go to next node
						}
					
					//We couldn't find the Address NoWhere
					if (inCache == 0){
						total_penalty_cycles = total_penalty_cycles + (MCRL1C + MCRR+ MCWL1C);
						penalty_cycles = (MCRL1C + MCRR+ MCWL1C);
						//code to write to the output file
						fprintf(OutputFilePointer, "   MISS ");
						fprintf(OutputFilePointer, "(not in Cache) ");
						fprintf(OutputFilePointer, " ---  ");
						fprintf(OutputFilePointer,"PENALTY CYCLES: %d"  , penalty_cycles);
						fprintf(OutputFilePointer,"\n"); //Print a new line
						break;
					}

				Pointer3 = FirstNode3; //Take the pointer to the first Node again
				
				}//end of NWay
					
				break;
			    } //end of M
			}; //end of case
	}
	} // end of file

	//Print the total results
	total_hits = ((hits/(temp*RamWords))*100);
	total_misses = ((misses/(temp*RamWords))*100);
	total_VictimHits = ((VictimHits / (temp*RamWords))*100);
	total_VictimMiss = ((VictimMiss / (temp*RamWords))*100);
	printf("The number of references in the memory is = %d \n",temp*RamWords);
	printf("The total amount of the penalty cycles for these references is = %d \n", total_penalty_cycles);
	printf("Total L1 Cache HIT rate= %.2f %% \n", total_hits);
	printf("Total L1 Cache MISS rate= %.2f %% \n", total_misses);

	if (EnableVictim == 1){
		printf("Total VICTIM HIT rate= %.2f %% \n", total_VictimHits);
		printf("Total VICTIM MISS rate= %.2f %% \n", total_VictimMiss);
	}
		
	
//Free the Space that we created with Malloc
	
if((Direct_Mapped == 1)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 0)){
	Pointer1 = FirstNode1;

	while(Pointer1->Next1!=0){
	tmp1=Pointer1;
	Pointer1=Pointer1->Next1;
	free(tmp1);
	}

	if (EnableVictim == 1 ){
		VTemp = FirstNodeV;
		while(VTemp->Next1!=0){
			tmp4=VTemp;
			VTemp=VTemp->Next1;
			free(tmp4);
		}
	}
	
}
else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 0)&&(Fully_Associative == 1)){
	Pointer2 = FirstNode2;

	while(Pointer2->Next2!=0){
	tmp2=Pointer2;
	Pointer2=Pointer2->Next2;
	free(tmp2);
	}
	
}
else if((Direct_Mapped == 0)&&(N_Way_Set_Associative == 1)&&(Fully_Associative == 0)){
	Pointer3 = FirstNode3;
	
	while(Pointer3->Next3!=0){
	
	tmp3=Pointer3;
	Pointer3=Pointer3->Next3;
	free(tmp3);
	}
	
}

	fclose(DataFilePointer); //Closes the data file
	fclose(OutputFilePointer); //Closes the output file
}