#include "Libraries_and_Defines.h"

char Actions[4]= {'R', 'M', 'W', 'F'}; //possible actions - Read , Modify, Write, Flush
int binary[128]; //An array that will take in each cell of it, one byte of a binary Address.The rest cells will have the value '0'. We assume that we don't have addresses that need more than 127 bits

int* DecimalToBinary(int );

//Function that converts the Decimal addresses that takes from the main, to Binary addresses
int* DecimalToBinary(int decimal){
	int rem; //The remainder of a division
	int i=0, j=0, k=0;

	//Reset the binary array when we finish with one binary address
	for (i=0; i<=128; i++){
		binary[i]=0;
	}

	//The loop is continued until the decimal take the value 0, so that it can't be divided any more.
	while (decimal!=0){
		rem = decimal %2; //We find the remainder when we divide the decimal address with the number 2
		decimal = decimal / 2; //We redivide the decimal with the number 2
		binary[j]=rem; //Save the remainder which can take the values 0 or 1, in the binary array
		j=j+1; //Increase the value of j so that the next rem, will be written in the next position of the array
	} 

	return binary; //Return the Binary address to main
}