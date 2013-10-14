/* PROJECT ARXITEKTONIKIS
MICHALIS PRODROMOU - MARIALENA SHIAMMA
*/

#include "Libraries_and_Defines.h" //It contains all the defines and the '#include's
#include "Converter.h" //The header file that we created in which there are some functions and the definitions of variables
#include "Parser.h" //The header file that parses the Data File
#include "Structures.h" //The header file that create the Cache memory depending on the Architecture of the memory


int main(){

int RandomAddress; //holds the value of the random generated RAM address
int RandomAction; //holds the value of the random generated Action 
int i, k=0, FlushesCounter=0,  FlushesFrequency = 0; // usd for counters later on
int IndexBitsAmount=0, TagBitsAmount=0, BOBitsAmount=0; //used for structures function
int AddressBitsAmount = 0; //Calculates the bits needed for each RAM reference
int RamWordCapacity = 0; //The size of RAM
int Ways=0; //Way of the N-Way set associative architecture

errno_t OpenningError= fopen_s(&DataFilePointer,"DataFile.txt","w+"); //Open (or create) the data file. also checks for opening errors. w+ == write and read permission

//Call the structures function to do the calculations
Structures(&IndexBitsAmount, &TagBitsAmount, &BOBitsAmount, &AddressBitsAmount, &RamWordCapacity, &Ways);

//used to randomize the generated addresses and actions
srand((unsigned) time(NULL)); //It is reseeding with a different value each second

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
					for (k=AddressBitsAmount-1; k>=0; k--){
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
			for (k=AddressBitsAmount-1; k>=0; k--){
				//printf("%i",binary[k]); //It prints the binary table that contain the binary address that is returned from the function DecimalToBinary
				fprintf(DataFilePointer,"%i",binary[k]); //It writes the binary table that contain the binary address that is returned from the function DecimalToBinary in the data file
			}

			//printf("  | %d \n", RandomAddress); //prints the value of the RandomAddress variable
			fprintf(DataFilePointer,"  | %d \n", RandomAddress); //writes the value of the RandomAddress variable in the data file
		}//end of "action wasnt a flush"
	}//end of for(temp*RamWordCapacity)
}//end of else
}

else if (NumberOfFlushes == 0){//User chose to have 0 flushes in the program
		for (i=0; i<(temp*RamWordCapacity); i++){ 
			
			RandomAction= rand() %3; 
			RandomAddress = rand() %((int)(RamWordCapacity)-1); //sets a random number between 0 and ramsize (given) -1 to the variable RandomAddress
		
			//printf("%c ", Actions[RandomAction]);//prints the content of the Actions table in position RandomAction (=int value)
			fprintf(DataFilePointer, "%c ", Actions[RandomAction]);//writes the content of the Actions table in position RandomAction (=int value) in the data file
		
			DecimalToBinary(RandomAddress); //call the function named DecimalToBinary so that it will convert the Random address which is Decimal to Binary
			
			//In each for loop, it returns one bit of the binary address
			for (k=AddressBitsAmount-1; k>=0; k--){
				//printf("%i",binary[k]); //It prints the binary table that contain the binary address that is returned from the function DecimalToBinary
				fprintf(DataFilePointer,"%i",binary[k]); //It writes the binary table that contain the binary address that is returned from the function DecimalToBinary in the data file
			}

			//printf("  | %d \n", RandomAddress); //prints the value of the RandomAddress variable
			fprintf(DataFilePointer,"  | %d \n", RandomAddress); //writes the value of the RandomAddress variable in the data file
		}
		}//end of if Flashes=0
//Flushes <0
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
printf("Cache Entries = %d \n", (CACHE_SIZE / WORD_SIZE));
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


Parser(AddressBitsAmount, IndexBitsAmount, TagBitsAmount, BOBitsAmount, RamWordCapacity, Ways); //call the Parser function

system("pause");   
return 0;
}//end of main