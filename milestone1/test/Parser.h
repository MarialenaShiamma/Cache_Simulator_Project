#include "Libraries_and_Defines.h"

FILE *DataFilePointer; //The pointer to the data file
FILE *OutputFilePointer; //The pointer to the output file

//Function that is parsing the Data File and creates the Output Files with the info that we need in order to build the memories
void Parser(int AddBits, int index, int tag, int offset, int RamWords, int Way){

	errno_t OpenningError= fopen_s(&DataFilePointer,"DataFile.txt","r+"); //Open (or create) the data file. Also checks for opening errors.
	errno_t OpenningError2= fopen_s(&OutputFilePointer,"OutputFile.txt","w+"); //Open (or create) the output file. Also checks for opening errors. w+ == write and read permission

	int i=0, j=0, k=0, l=0, m=0, n=0, p=0, q=0, Bit=0, Which_Way =0;//used for counters
	char c; //A character that is useful only to find the new line character
	char Action=' '; // R, W, M, F
	char Space=' ';	
	int Binary_Parser[128]={0}; //An array that will take in each cell of it, one byte of a binary Address.The rest cells will have the value '0'. We assume that we don't have addresses that need more than 127 bits
	

	//runs for temp *Ramsize to read all the lines from the DataFile
	for (j=0; j<(temp*RamWords); j++){ 
		fscanf_s(DataFilePointer, "%c", &Action, 1); //Reads from the Data File the first character.This character represent the Action - R,W,M,F
			
			//Check the value of the Action that was read before
			switch (Action){
			//If it is a Flush action
			case 'F': {
					fprintf(OutputFilePointer, " FLUSH\n");	
					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.
					break;
					}

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

					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.


					fprintf(OutputFilePointer, "   TAG:  ");
					//Write in the Output File the bits that shows the tag, according to the bits that are needed for tag (is returned from the Structures.h)
					for (n=0; n < tag ; n++){
						fprintf(OutputFilePointer,"%d", Binary_Parser[n]);
					}

					//Write in the Output File the bits that shows the index, according to the bits that are needed for index (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   INDEX: ");
					
					//If we have fully Associative architecture we don't have Index
					if (index==0){
						fprintf(OutputFilePointer,"   - ");
					}
					else
						for (p=tag; p < (tag+index) ; p++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[p]);
						}

					//Write in the Output File the bits that shows the block offset,  according to the bits that are needed for block offset (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   BLOCK OFFSET: ");
					 
					
					//If We have only one block of words, we don't need block offset
					if (offset==0)
							fprintf(OutputFilePointer,"   - ");
					else
						for (q=tag+index; q < (tag+index+offset) ; q++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[q]);
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
					
					fprintf(OutputFilePointer,"\n"); //Print a new line
					break;
					   }

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

					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.
					

					fprintf(OutputFilePointer, "   TAG:  ");
					//Write in the Output File the bits that shows the tag, according to the bits that are needed for tag (is returned from the Structures.h)
					for (n=0; n < tag ; n++){
						fprintf(OutputFilePointer,"%d", Binary_Parser[n]);
					}
					//Write in the Output File the bits that shows the index, according to the bits that are needed for index (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   INDEX: ");
					
					//If we have fully Associative architecture we don't have Index
					if (index==0){
						fprintf(OutputFilePointer,"   - ");
					}
					else
						for (p=tag; p < (tag+index) ; p++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[p]);
						}
					//Write in the Output File the bits that shows the block offset,  according to the bits that are needed for block offset (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   BLOCK OFFSET: ");
					 
					
					//If We have only one block of words, we don't need block offset
					if (offset==0)
							fprintf(OutputFilePointer,"   - ");
					else
						for (q=tag+index; q < (tag+index+offset) ; q++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[q]);
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

					fprintf(OutputFilePointer,"\n"); //Print a new line
					break;
					   }


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

					while (c=fgetc(DataFilePointer) != '\n'){} //Reads characters and do nothing until we reach the new line character so that we can go to the next line.
					

					fprintf(OutputFilePointer, "   TAG:  ");
					//Write in the Output File the bits that shows the tag, according to the bits that are needed for tag (is returned from the Structures.h)
					for (n=0; n < tag ; n++){
						fprintf(OutputFilePointer,"%d", Binary_Parser[n]);
					}
					//Write in the Output File the bits that shows the index, according to the bits that are needed for index (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   INDEX: ");
					
					//If we have fully Associative architecture we don't have Index
					if (index==0){
						fprintf(OutputFilePointer,"   - ");
					}
					else
						for (p=tag; p < (tag+index) ; p++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[p]);
						}
					//Write in the Output File the bits that shows the block offset,  according to the bits that are needed for block offset (is returned from the Structures.h)
					fprintf(OutputFilePointer, "   BLOCK OFFSET: ");
					 
					
					//If We have only one block of words, we don't need block offset
					if (offset==0)
							fprintf(OutputFilePointer,"   - ");
					else
						for (q=tag+index; q < (tag+index+offset) ; q++){
							fprintf(OutputFilePointer,"%d", Binary_Parser[q]);
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

					fprintf(OutputFilePointer,"\n"); //Print a new line
					break;
					   }
			};
	}
		
	fclose(DataFilePointer); //Closes the data file
	fclose(OutputFilePointer); //Closes the output file
}