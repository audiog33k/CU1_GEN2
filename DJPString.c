#include "main.h"


//*************************************************************************
//***************************** StringToUpper *****************************
//*************************************************************************
//
// This function searches through a null terminated string and replaces lower case
// values with upper case values.  Only 'a' through 'z' are changed.  Original
// values are over written.
//
void StringToUpper(unsigned char *localString)
{
    unsigned char i;
    
    for (i = 0; localString[i] != NULL; i++) 
    {
        if(localString[i] >= 'a' && localString[i] <= 'z') 
        {
         localString[i] = localString[i] -32;
        }  
    }
}
//*************************************************************************
//***************************** DJPStringCompare *****************************
//*************************************************************************
//
// Compares two strings.  Passed variables are:
// 1. first string pointer
// 2. first string starting point, one based
// 3. second string pointer
// 4. second string starting point, one based
// 5. how many characters to compare, one based
// returns 0=match, non-zero return indicates failure and returned number is position of failure (one based)
//
unsigned char DJPStringCompare(unsigned char *localString1, unsigned char localStart1, unsigned char *localString2, unsigned char localStart2, unsigned char localCount)
{
    unsigned char passFail = 0;                                                 // assume a match and change if failure
    unsigned char n;
    
    localStart1 -= 1;                                                           // convert to zero based
    localStart2 -= 1;
    
    for(n=0; n<localCount; n++)
    {
        if(localString1[localStart1] != localString2[localStart2])              // not the same
        {
            passFail = n+1;                                                     // convert to one based
            return passFail;                                                    // return index to where the compare fails
        }
        else                                                                    // the same
        {
        }
        localStart1++;
        localStart2++;
    }
    return passFail;
}
//*************************************************************************
//***************************** StringCopy ********************************
//*************************************************************************
//
//  Copys a string from source to destination and terminates with a NULL.
//  Destination contents are over-written
//
void StringCopy(unsigned char *localDestination, unsigned char *localSource)
{
	while (*localSource != NULL)
	{
		*localDestination++ = *localSource++;
	}
	*localDestination = NULL;
}

unsigned int DJPFindStringLength(unsigned char *ch)
{
	unsigned int count = 0;
	while ((*ch != 0xFF) && (*ch != 0))
	{
		++count;
		++ch;
	}
//	return count + 1;
	return count;
}
//*************************************************************************
//*************************** CharToHexString ******************************
//*************************************************************************
//
// Converts a 8bit unsigned number into an ASCII string that is NULL terminated
//
void CharToHexString (unsigned char *recString, unsigned char recCharVar)
{
	recString[2] = NULL;
	recString[1] = hexArray[recCharVar & 0x000F];
	recString[0] = hexArray[(recCharVar >> 4) & 0x000F];
}
//*************************************************************************
//**************************** StringCat **********************************
//*************************************************************************
//
// Stringcat will add NULL terminated string to the original NULL terminated string
//
void StringCat (unsigned char *localDestination, unsigned char *localSource)
{

	while (*localDestination != NULL)										   // find the end of the string
	{
		*localDestination++;
	}
	while (*localSource != NULL)											   // now start copying data till end of source
	{
		*localDestination++ = *localSource++;
	}

	*localDestination = NULL;												   // make new string has a NULL on the end
}
//*************************************************************************
//************************** StringCatWithCR ******************************
//*************************************************************************
//
// StringCatWithCR will add a Carriage Return and NULL to a terminated string
//
void StringCatWithCR (unsigned char *localDestination)
{
	while (*localDestination != NULL)										   // find the end of the string
	{
		*localDestination++;
	}
	*localDestination++ = CR;												   // add the carriage return
	*localDestination++ = NULL;												   // make sure new string has a NULL on the end
}
//*************************************************************************
//************************** HexToDecimalString ***************************
//*************************************************************************
//
// This routine will take a unsigned char value of 0 to 255 and convert
// it to an ascii string that is null terminated.
//
void HexToDecimalString(unsigned char *recString, unsigned char recVar)
{
    if (recVar < 10)
    {
            recString[0] = '0';												   
            recString[1] = '0';												   
            recString[2] = recVar + 0x30;
            recString[3] = NULL;												   // make sure the string is null terminated
    }
    else if (recVar < 100)
    {
            recString[0] = '0';
            recString[1] = (recVar / 10);										   // how many 10's in the varible
            recString[2] = (recVar - (recString[1] * 10));						   // subtract number of 10's and see waht is left
            recString[3] = NULL;												   // make sure the string is null terminated
            recString[1] = recString[1] + 0x30;									   // convert to ascii
            recString[2] = recString[2] + 0x30;									   // convert to ascii
    }
    else if (recVar > 99)
    {
            recString[0] = (recVar / 100);										   // how many 100's in variable
            recString[1] = ((recVar - (recString[0] * 100))/10);				   // subtract 100's and see how many 10's
            recString[2] = (recVar - ((recString[1] * 10) + (recString[0] * 100)));	   // subtract 100's and 10's and see what is left
            recString[0] = recString[0] + 0x30;									   // convert to ascii
            recString[1] = recString[1] + 0x30;									   // convert to ascii
            recString[2] = recString[2] + 0x30;									   // convert to ascii
            recString[3] = NULL;												   // make sure the string is null terminated
    }
}
//*************************************************************************
//************************ HexToDecimalStringInt **************************
//*************************************************************************
//
// This routine will take a unsigned int value of 0 to 65536 and convert
// it to an ascii string that is null terminated.
//
void HexToDecimalStringInt(unsigned char *recString, unsigned int recVar)
{
    unsigned char ones;
    unsigned char tens;
    unsigned int tensOnes;   
    unsigned char hundreds;
    unsigned int hundredsTensOnes;
    unsigned char thousands;
    unsigned int thousandsHundredsTensOnes;
    unsigned char tenThousands;
 
    // make sure all elements have a NULL
    recString[0] = NULL;
    recString[1] = NULL;												   
    recString[2] = NULL;												   
    recString[3] = NULL;												   
    recString[4] = NULL;												  
    recString[5] = NULL;												  
    
    if (recVar<10)
    {
        ones = recVar;
		recString[0] = ones + 0x30;
    }
 	else if (recVar < 100)
	{
        tens = recVar/10;
        ones = recVar - (tens*10);       
        recString[0] = tens + 0x30;
        recString[1] = ones + 0x30;												   
    }  
    else if(recVar < 1000)
    {
        hundreds = recVar / 100;
        tensOnes = recVar - (hundreds*100);               
        tens =  tensOnes / 10;
        ones = tensOnes%10;
        recString[0] = hundreds + 0x30;
        recString[1] = tens + 0x30;												   
        recString[2] = ones + 0x30;												   
    }
     else if(recVar < 10000)
    {
        thousands = recVar/1000;
        hundredsTensOnes = recVar - (thousands*1000);
        hundreds = hundredsTensOnes / 100;
        tensOnes = hundredsTensOnes - (hundreds*100);               
        tens =  tensOnes / 10;
        ones = tensOnes%10;
        recString[0] = thousands + 0x30;
        recString[1] = hundreds + 0x30;												   
        recString[2] = tens + 0x30;												   
        recString[3] = ones + 0x30;												   
    }
    else
    {
        tenThousands = recVar/10000;      
        thousandsHundredsTensOnes = recVar - (tenThousands*10000);          
        thousands = thousandsHundredsTensOnes/1000;
        hundredsTensOnes = thousandsHundredsTensOnes - (thousands*1000);
        hundreds = hundredsTensOnes / 100;
        tensOnes = hundredsTensOnes - (hundreds*100);               
        tens =  tensOnes / 10;
        ones = tensOnes%10;     
        recString[0] = tenThousands + 0x30;
        recString[1] = thousands + 0x30;
        recString[2] = hundreds + 0x30;												   
        recString[3] = tens + 0x30;												   
        recString[4] = ones + 0x30;												   
    }
}
//*************************************************************************
//********************* HexToDecimalStringIntWithPad **********************
//*************************************************************************
//
// This routine will take a unsigned int value of 0 to 65536 and convert
// it to an ascii string that is null terminated. Unused digits are front padded
// with ASCII 0
//
void HexToDecimalStringIntWithPad(unsigned char *recString, unsigned int recVar)
{
    unsigned char ones;
    unsigned char tens;
    unsigned int tensOnes;   
    unsigned char hundreds;
    unsigned int hundredsTensOnes;
    unsigned char thousands;
    unsigned int thousandsHundredsTensOnes;
    unsigned char tenThousands;
 
    // make sure all elements have a NULL
    recString[0] = '0';
    recString[1] = '0';												   
    recString[2] = '0';												   
    recString[3] = '0';												   
    recString[4] = '0';												  
    recString[5] = NULL;												  
    
    if (recVar<10)
    {
        ones = recVar;
		recString[4] = ones + 0x30;
    }
 	else if (recVar < 100)
	{
        tens = recVar/10;
        ones = recVar - (tens*10);       
        recString[3] = tens + 0x30;
        recString[4] = ones + 0x30;												   
    }  
    else if(recVar < 1000)
    {
        hundreds = recVar / 100;
        tensOnes = recVar - (hundreds*100);               
        tens =  tensOnes / 10;
        ones = tensOnes%10;
        recString[2] = hundreds + 0x30;
        recString[3] = tens + 0x30;												   
        recString[4] = ones + 0x30;												   
    }
     else if(recVar < 10000)
    {
        thousands = recVar/1000;
        hundredsTensOnes = recVar - (thousands*1000);
        hundreds = hundredsTensOnes / 100;
        tensOnes = hundredsTensOnes - (hundreds*100);               
        tens =  tensOnes / 10;
        ones = tensOnes%10;
        recString[1] = thousands + 0x30;
        recString[2] = hundreds + 0x30;												   
        recString[3] = tens + 0x30;												   
        recString[4] = ones + 0x30;												   
    }
    else
    {
        tenThousands = recVar/10000;      
        thousandsHundredsTensOnes = recVar - (tenThousands*10000);          
        thousands = thousandsHundredsTensOnes/1000;
        hundredsTensOnes = thousandsHundredsTensOnes - (thousands*1000);
        hundreds = hundredsTensOnes / 100;
        tensOnes = hundredsTensOnes - (hundreds*100);               
        tens =  tensOnes / 10;
        ones = tensOnes%10;     
        recString[0] = tenThousands + 0x30;
        recString[1] = thousands + 0x30;
        recString[2] = hundreds + 0x30;												   
        recString[3] = tens + 0x30;												   
        recString[4] = ones + 0x30;												   
    }
}
//*************************************************************************
//************************** LongToHexString ******************************
//*************************************************************************
//
// Converts a 32bit unsigned number into an ASCII string that is NULL terminated
//
void LongToHexString (unsigned char *recString, unsigned int recLongVar)
{
	recString[8] = NULL;
	recString[7] = hexArray[recLongVar & 0x0000000F];
	recString[6] = hexArray[(recLongVar >> 4) & 0x0000000F];
	recString[5] = hexArray[(recLongVar >> 8) & 0x0000000F];
	recString[4] = hexArray[(recLongVar >> 12) & 0x0000000F];
	recString[3] = hexArray[(recLongVar >> 16) & 0x0000000F];
	recString[2] = hexArray[(recLongVar >> 20) & 0x0000000F];
	recString[1] = hexArray[(recLongVar >> 24) & 0x0000000F];
	recString[0] = hexArray[(recLongVar >> 28) & 0x0000000F];
}
//*************************************************************************
//*************************** IntToHexString ******************************
//*************************************************************************
//
// Converts a 16bit unsigned number into an ASCII string that is NULL terminated
//
void IntToHexString (unsigned char *recString, unsigned int recIntVar)
{
	recString[4] = NULL;
	recString[3] = hexArray[recIntVar & 0x0000000F];
	recString[2] = hexArray[(recIntVar >> 4) & 0x0000000F];
	recString[1] = hexArray[(recIntVar >> 8) & 0x0000000F];
	recString[0] = hexArray[(recIntVar >> 12) & 0x0000000F];
}
