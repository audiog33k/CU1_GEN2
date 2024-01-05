#include "main.h"



// ****************************************************************************
// *************************** I2CWriteDAC ************************************
// ****************************************************************************
//
//
void I2CWriteDAC(unsigned char recHighByte, unsigned char recLowByte, unsigned char recI2CSlaveAddress )
{	
    unsigned int localInt;
    
    // make an int from two bytes
    localInt = recHighByte;
    localInt = localInt << 8;
    localInt = localInt | recLowByte;
    localInt = localInt << 2;                       //  10 bit dac does not use bits 0 and 1, go figure
    // seperate back out
    recLowByte = localInt;
    recHighByte = localInt >> 8;      
    recHighByte = recHighByte & 0x3F;               // 0011 1111, Make sure bits 7 and 6 zero which makes it a DAC write command
    
    
//        StringCopy(txBuffer, "DAC:     ");
//        stringBuffer[0] = hexArray[recHighByte & 0x0000000F];
//        stringBuffer[1] = hexArray[(recLowByte >> 4) & 0x0000000F];
//        stringBuffer[2] = hexArray[recLowByte & 0x0000000F];
//        stringBuffer[3] = NULL;
//        StringCat(txBuffer, stringBuffer);
//        SendString(txBuffer);
  
        while (I2CMasterBusy(I2C3_BASE));
    
        I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, false);
        I2CMasterDataPut(I2C3_BASE, recHighByte);
        I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    
        while (I2CMasterBusy(I2C3_BASE));
    
        I2CMasterDataPut(I2C3_BASE, recLowByte);
        I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
 
}
// ****************************************************************************
// **************************** I2CReadADC ************************************
// ****************************************************************************
// 
// This function reads a microchip I2C 10 bit ADC
// Data is presented 0000 DDDD DDDD DDXX,  so it needs a shift of two bits to the right
// In this application, the ADC is connected to 5V.  5V/1024 = 0.00489 V per bit
//
unsigned int I2CReadADC (unsigned char recI2CSlaveAddress)
{
    unsigned char localHighByte;
    unsigned char localLowByte;
    unsigned int returnVar;
    
	while (I2CMasterBusy(I2C3_BASE));
        
	//send control byte with read bit set
    I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, true);

    // get the most significant byte
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
	while (I2CMasterBusy(I2C3_BASE));
    localHighByte = I2CMasterDataGet(I2C3_BASE);
        
    // get the least significant byte
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while (I2CMasterBusy(I2C3_BASE));
    localLowByte = I2CMasterDataGet(I2C3_BASE); 
        
    returnVar = localHighByte;
    returnVar = returnVar << 8;
    returnVar = returnVar | localLowByte;
    returnVar = returnVar >> 2;
    
    return returnVar;
}

// ****************************************************************************
// *************************** I2CWriteDACConfig ************************************
// ****************************************************************************
//
// 7654 3210
// 100x xxxx    Command 
// xxx4 xxxx    VREF1   1 = Vref pin buffered, 0=Connect to Vdd
// xxxx 3xxx    VREF0   1 = Vref pin buffered, 0=Connect to Vdd
// xxxx x2xx    PD1     0 = Normal operation
// xxxx xx1x    PD0     0 = Normal operation
// xxxx xxx0    G       0 = Gain of 1, 1 = gain of 2
// 1001 1000    0x98
//
void I2CWriteDACConfig(unsigned char recI2CData, unsigned char recI2CSlaveAddress )
{	  
        while (I2CMasterBusy(I2C3_BASE));
    
        I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, false);
        I2CMasterDataPut(I2C3_BASE, recI2CData);
        I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_SEND);   
}
// ****************************************************************************
// *************************** I2CReadDSP *************************************
// ****************************************************************************
unsigned char I2CReadDSP(unsigned char recHighAddress, unsigned char recLowAddress,unsigned char recI2CSlaveAddress )
{	
    unsigned char returnVar;
    
	while (I2CMasterBusy(I2C3_BASE));

        // send control byte and address
        I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, false);

	I2CMasterDataPut(I2C3_BASE, recHighAddress);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	while (I2CMasterBusy(I2C3_BASE));

	I2CMasterDataPut(I2C3_BASE, recLowAddress);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	while (I2CMasterBusy(I2C3_BASE));
        
	//send control byte again with read bit set
        I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, true);

	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

	while (I2CMasterBusy(I2C3_BASE));

        returnVar = I2CMasterDataGet(I2C3_BASE);
        
        return returnVar;        
}

// ****************************************************************************
// ************************ I2CReadDataEEPROM *********************************
// ****************************************************************************
//
//#define I2C_MASTER_CMD_SINGLE_SEND              0x00000007
//#define I2C_MASTER_CMD_SINGLE_RECEIVE           0x00000007
//#define I2C_MASTER_CMD_BURST_SEND_START         0x00000003
//#define I2C_MASTER_CMD_BURST_SEND_CONT          0x00000001
//#define I2C_MASTER_CMD_BURST_SEND_FINISH        0x00000005
//#define I2C_MASTER_CMD_BURST_SEND_ERROR_STOP    0x00000004
//#define I2C_MASTER_CMD_BURST_RECEIVE_START      0x0000000b
//#define I2C_MASTER_CMD_BURST_RECEIVE_CONT       0x00000009
//#define I2C_MASTER_CMD_BURST_RECEIVE_FINISH     0x00000005
//#define I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP 0x00000005
//
unsigned char I2CReadDataEEPROM(unsigned char recHighAddress, unsigned char recLowAddress, unsigned char recI2CSlaveAddress )
{	
    unsigned char returnVar;
    
	while (I2CMasterBusy(I2C3_BASE));

        // send control byte and address
        I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, false);

	I2CMasterDataPut(I2C3_BASE, recHighAddress);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	while (I2CMasterBusy(I2C3_BASE));

	I2CMasterDataPut(I2C3_BASE, recLowAddress);
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	while (I2CMasterBusy(I2C3_BASE));
        
	//send control byte again with read bit set
        I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, true);

	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

	while (I2CMasterBusy(I2C3_BASE));

        returnVar = I2CMasterDataGet(I2C3_BASE);
        
        return returnVar;        
}
// ****************************************************************************
// ************************ I2CSendDataEEPROM *********************************
// ****************************************************************************
//
// This function sends a single byte to an I2C EEPROM location.
// When write is finished, the location will be read till the data is valid.
// A timeout "postTimeOutCount" increments to let the read bailout if an error.
//
void I2CSendDataEEPROM(unsigned char recHighAddress, unsigned char recLowAddress,unsigned char recI2CData, unsigned char recI2CSlaveAddress )
{	
    unsigned char readVar;
    unsigned char flagReadOk = 0;
    unsigned int postTimeOutCount = 0;     
    
        while (I2CMasterBusy(I2C3_BASE));
    
        I2CMasterSlaveAddrSet(I2C3_BASE, recI2CSlaveAddress, false);
        I2CMasterDataPut(I2C3_BASE, recHighAddress);
        I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START);

        while (I2CMasterBusy(I2C3_BASE));

        I2CMasterDataPut(I2C3_BASE, recLowAddress);
        I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
    
        while (I2CMasterBusy(I2C3_BASE));
    
        I2CMasterDataPut(I2C3_BASE, recI2CData);
        I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
 
        while (flagReadOk == 0)
        {   
            readVar = I2CReadDataEEPROM(recHighAddress, recLowAddress, recI2CSlaveAddress); 
            if (readVar == recI2CData)
            {
              flagReadOk = 1;  
            } 
            postTimeOutCount++;
            if(postTimeOutCount>10000)
            {
                flagReadOk = 2;  
            }
        }
}
// ****************************************************************************
// ************************ I2CReadEEPROMSingle *******************************
// ****************************************************************************
//     SysCtlDelay(1000);

unsigned char I2CReadEEPROMSingle(unsigned char recAddress, unsigned char recI2CSlaveAddress )
{	
  
    unsigned char returnVar = 0;
    
    while (I2CMasterBusy(I2C0_BASE));

    // send control byte and address
    I2CMasterSlaveAddrSet(I2C0_BASE, recI2CSlaveAddress, false);
    SysCtlDelay(1000);
    while (I2CMasterBusy(I2C0_BASE));

    I2CMasterDataPut(I2C0_BASE, recAddress);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    SysCtlDelay(1000);
    while (I2CMasterBusy(I2C0_BASE));
        
    //send control byte again with read bit set
    I2CMasterSlaveAddrSet(I2C0_BASE, recI2CSlaveAddress, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    SysCtlDelay(1000);
    while (I2CMasterBusy(I2C0_BASE));

    returnVar = I2CMasterDataGet(I2C0_BASE);
        
    return returnVar;   
/*  
    unsigned char returnVar = 0;
    
    while (I2CMasterBusy(I2C0_BASE));
    __no_operation();

    // send control byte and address
    I2CMasterSlaveAddrSet(I2C0_BASE, recI2CSlaveAddress, false);
    __no_operation();

    I2CMasterDataPut(I2C0_BASE, recAddress);
    __no_operation();

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    __no_operation();

    while (I2CMasterBusy(I2C0_BASE));
    __no_operation();
        
    //send control byte again with read bit set
    I2CMasterSlaveAddrSet(I2C0_BASE, recI2CSlaveAddress, true);
    __no_operation();

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    __no_operation();

    while (I2CMasterBusy(I2C0_BASE));
    __no_operation();

    returnVar = I2CMasterDataGet(I2C0_BASE);
        
    return returnVar;   
*/     
}
// ****************************************************************************
// *********************** I2CWriteEEPROMSingle *******************************
// ****************************************************************************
//
// This function sends a single byte to an I2C EEPROM location.
// When write is finished, the location will be read till the data is valid.
// A timeout "postTimeOutCount" increments to let the read bailout if an error.
//
void I2CWriteEEPROMSingle(unsigned char recAddress,unsigned char recI2CData, unsigned char recI2CSlaveAddress )
{	
    unsigned char readVar;
    unsigned char flagReadOk = 0;
    unsigned int postTimeOutCount = 0;     
    
        while (I2CMasterBusy(I2C0_BASE));
    
        I2CMasterSlaveAddrSet(I2C0_BASE, recI2CSlaveAddress, false);
        SysCtlDelay(1000);
        while (I2CMasterBusy(I2C0_BASE));

        I2CMasterDataPut(I2C0_BASE, recAddress);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        SysCtlDelay(1000);
        while (I2CMasterBusy(I2C0_BASE));
    
        I2CMasterDataPut(I2C0_BASE, recI2CData);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        SysCtlDelay(1000);
        while (I2CMasterBusy(I2C0_BASE));

        while (flagReadOk == 0)
        {   
            readVar = I2CReadEEPROMSingle(recAddress, recI2CSlaveAddress); 
            if (readVar == recI2CData)
            {
              flagReadOk = 1;  
            } 
            postTimeOutCount++;
            if(postTimeOutCount>10000)
            {
                flagReadOk = 2;  
            }
        }
}
// ***********************************************************
// ******************* I2CLEDTileFast ************************
// ***********************************************************
//
// This function is a faster version of I2CLEDTile.  It gains speed by using continued write
// function of the LED driver.  Measured speed at 0.66mS at I2C=400KHz and Processor at 80MHz.
//
// 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
// R  G  B  R  G  B  R  G  B  R  G  B  W  W  W  W
//
// 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
// R  G  B  R  G  B  R  G  B  R  G  B  W  W  W  W
//
// Locations of LEDs (at corners)
// Red   00, 03, 06, 09, 16, 19, 22, 25
// Green 01, 04, 07, 10, 17, 20, 23, 26
// Blue  02, 05, 08, 11, 18, 21, 24, 27
//
void I2CLEDTileFast(unsigned char localRed, unsigned char localGreen, unsigned localBlue)
{ 
    unsigned char n;
    unsigned char localArray[12];

        localArray[0] = localRed; 
        localArray[1] = localGreen; 
        localArray[2] = localBlue; 
        localArray[3] = localRed; 
        localArray[4] = localGreen; 
        localArray[5] = localBlue; 
        localArray[6] = localRed; 
        localArray[7] = localGreen; 
        localArray[8] = localBlue; 
        localArray[9] = localRed; 
        localArray[10] = localGreen; 
        localArray[11] = localBlue; 
        
        // first LED controller
        while (I2CMasterBusy(I2C1_BASE));    
        I2CMasterSlaveAddrSet(I2C1_BASE, I2C_PHYSICAL_1, false);
        I2CMasterDataPut(I2C1_BASE, 0x08 | 0x80);                                   // 1000 1000, or in bit 7 for auto increment                                                                 
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    

        for(n=0; n<11; n++)
        {
            while (I2CMasterBusy(I2C1_BASE));    
            I2CMasterDataPut(I2C1_BASE, localArray[n]);
            I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        }
        
        while (I2CMasterBusy(I2C1_BASE));    
        I2CMasterDataPut(I2C1_BASE, localArray[11]);
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

        // second controller
        while (I2CMasterBusy(I2C1_BASE));    
        I2CMasterSlaveAddrSet(I2C1_BASE, I2C_PHYSICAL_2, false);
        I2CMasterDataPut(I2C1_BASE, 0x08 | 0x80);                                   // 1000 1000, or in bit 7 for auto increment                                                                 
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    

        for(n=0; n<11; n++)
        {
            while (I2CMasterBusy(I2C1_BASE));    
            I2CMasterDataPut(I2C1_BASE, localArray[n]);
            I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        }
        
        while (I2CMasterBusy(I2C1_BASE));    
        I2CMasterDataPut(I2C1_BASE, localArray[11]);
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
   
}

// ***********************************************************
// ********************* I2CContinueWrite ********************
// ***********************************************************
void I2CContinueWrite(unsigned char localData)
{
    while (I2CMasterBusy(I2C1_BASE));
    I2CMasterDataPut(I2C1_BASE, localData);
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
}
// ***********************************************************
// ******************* I2CLEDDriverSetup *********************
// ***********************************************************
//
// This function configures the NXP PCA9955 16 LED Driver.
// Global blink is enabled with a 50% duty cycle and 0.4second duty which is only used for configuration blink.
//
void I2CLEDDriverSetup(unsigned char localPhysicalAddress)
{
    unsigned char regAddr;
    
   // Allow all LEDs to have global blink
    for(regAddr = 0x02; regAddr < 0x06; regAddr++)
    {    
        while (I2CMasterBusy(I2C1_BASE));    
        I2CMasterSlaveAddrSet(I2C1_BASE, localPhysicalAddress, false);
        I2CMasterDataPut(I2C1_BASE, regAddr);
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    
        while (I2CMasterBusy(I2C1_BASE));   
        I2CMasterDataPut(I2C1_BASE, 0xFF);
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    }
    // Duty cycle of blink
    while (I2CMasterBusy(I2C1_BASE));    
    I2CMasterSlaveAddrSet(I2C1_BASE, localPhysicalAddress, false);
    I2CMasterDataPut(I2C1_BASE, 0x06);                                       // GRPPWM Group duty cycle
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    
    while (I2CMasterBusy(I2C1_BASE));   
    I2CMasterDataPut(I2C1_BASE, 0x7F);                                       // 0111 1111 7F - 50% duty cycle
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    // Frequency of blink
    while (I2CMasterBusy(I2C1_BASE));    
    I2CMasterSlaveAddrSet(I2C1_BASE, localPhysicalAddress, false);
    I2CMasterDataPut(I2C1_BASE, 0x07);                                       // GRPFREQ Group frequency blink                                       // GRPPWM Group duty cycle
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    
    while (I2CMasterBusy(I2C1_BASE));   
    I2CMasterDataPut(I2C1_BASE, 0x05);                                       // frequency, (x+1)/15.26, (5+1)/15.26 = 0.4 seconds
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

    // mode 1 register
    // default is 0x89 1000 1001
    // bit 7 is read only
    while (I2CMasterBusy(I2C1_BASE));    
    I2CMasterSlaveAddrSet(I2C1_BASE, localPhysicalAddress, false);
    I2CMasterDataPut(I2C1_BASE, 0x00);                                        
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    
    while (I2CMasterBusy(I2C1_BASE));   
    I2CMasterDataPut(I2C1_BASE, 0x89);                                       // 1000 1001 is default                                        
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

}
// *****************************************************************************
// ********************** LEDLoadDefaultCurrentTrim *****************************
// *****************************************************************************
//
// This function will load the current/trim values to all channels of the LED driver.
// There is a trim value for each channel.
// Values are loaded from a constant table.
// Address for first LED is 0x22.
//
void LEDLoadDefaultCurrentTrim(unsigned char localPhysicalAddress)
{
    unsigned char cnt;

    for(cnt=0; cnt<16; cnt++)
    { 
        while (I2CMasterBusy(I2C1_BASE));    
        I2CMasterSlaveAddrSet(I2C1_BASE, localPhysicalAddress, false);
        I2CMasterDataPut(I2C1_BASE, 0x18 + cnt);
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    
        while (I2CMasterBusy(I2C1_BASE));   
        I2CMasterDataPut(I2C1_BASE, defaultLEDTrim[cnt]);
        I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);       
    }   
}
// *****************************************************************************
// ********************** I2CLEDBrightnessWriteAll *****************************
// *****************************************************************************
//
// This function controls all the PWM (brightness) setting on all channels at same time.
// Passed value is 0 to 255, 255 being max brightness.
//
void I2CLEDBrightnessWriteAll(unsigned char localPhysicalAddress, unsigned char LEDAllIntensity)
{
    while (I2CMasterBusy(I2C1_BASE));    
    I2CMasterSlaveAddrSet(I2C1_BASE, localPhysicalAddress, false);
    I2CMasterDataPut(I2C1_BASE, 0x44);
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);    
    while (I2CMasterBusy(I2C1_BASE));   
    I2CMasterDataPut(I2C1_BASE, LEDAllIntensity);
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
}

