#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <ctype.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "math.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "DriverLib/inc/hw_ssi.h"
#include <hw_nvic.h>
#include <intrinsics.h>
//#include <string.h>


#define DEBUG_SERIAL_OUTPUT                                                     // if defined data is allowed out debug serial port

#define VERSION_MAJOR 	1
#define VERSION_MINOR 	0
#define VERSION_ALPHA 	2

#define BOARD_CU1       1
#define BOARD_RU1       2

// Gen2 double display
#define DISPLAY_OFF             16
#define SEGMENT_HIGH            1
#define SEGMENT_LOW             0

#define DECIMAL_BYPASS          8
#define DECIMAL_BOTH_OFF        7
#define DECIMAL_BOTH_ON         6
#define DECIMAL_TENS_OFF        5
#define DECIMAL_TENS_ON         4
#define DECIMAL_ONES_OFF        3
#define DECIMAL_ONES_ON         2
#define DECIMAL_ON              1
#define DECIMAL_OFF             0


//#define DECIMAL         1
//#define NO_DECIMAL      0

#define LOOP_TEST_CU1_PASS      0
#define LOOP_TEST_CU1_FAIL      1

#define LOOP_TEST_RU1_PASS      0
#define LOOP_TEST_RU1_FAIL      1
#define LOOP_TEST_RU1_INVERT    2

#define DISPLAY

#define ALPHA_BLANK 0x00
#define ALPHA_c  0x58                                                           // 0101 1000				
#define ALPHA_u	 0x1C                                                           // 0001 1100				
#define ALPHA_r  0x50                                                           // 0101 0000				
#define ALPHA_P  0x73                                                           // 0111 0011

#define MFG_STRING         "[Professor Wiseacres]"
#define MODEL_STRING_CU1       "[CU1]"
#define MODEL_STRING_RU1       "[RU1]"

//#define NULL                    0x00
#define ON                      0x01
#define OFF                     0x00
#define LF                      0x0A                                            // ASCII Line Feed
#define CR                      0x0D                                            // ASCII Cariage Return
#define DUMMY                   0x00
#define TRUE                    0x01
#define FALSE                   0x00
#define STX                     0x02
#define ETX                     0x03
#define RS485_BROADCAST_ON      0x01
#define RS485_BROADCAST_OFF     0x00
#define SPACE                   0x20

// Size limits
#define RX_BUFFER_SIZE          60                                              // max size of received string from RS485 command
#define TX_BUFFER_SIZE          60                                              // max size of a transmited string 
#define RX_MAX_COMMAND_LENGTH   60

#define SOLENOID_ON_TIME        200                                              // 2 seconds, x100 because function is called 10x a second
#define SOLENOID_OFFSET_B       200                                             // offset InjectorB by 2 seconds so if InjectorA=InjectorB they will not fire at the same time         
#define NO_ZONE_INPUT_COUNT     60                                              // must be 20 no zone input detections in a row to be called "no zone input"
#define ACTIVE_ZONE_INPUT_COUNT     10                                          // count consecutive same zone found
#define ZONE_INACTIVE_FREQUENCY  100                                            // 10 seconds, compared against variable incremented at 0.1S rate, 10x10=100

// Define for how long a stream decode sits idle before it is reset
#define SERIAL_DECODE_TIMEOUT   2

//#define DELAY_5_MICRO           40                                            // 60nS per loop @ 50MHz, 5us/60nS=83, seems to run half speed
//#define DELAY_20_MICRO          160                                           // 60nS per loop @ 50MHz, 20us/60nS=333, seems to run half speed
#define DELAY_5_MICRO           67                                              // 37.5nS per loop @ 80MHz, 5us/37.5nS=133
#define DELAY_20_MICRO          533                                             // 37.5nS per loop @ 80MHz, 20us/37.5nS=533

#define I2C_PHYSICAL_1          0x05                                            // Using hard pull up/down with R/W bit not considered        
#define I2C_PHYSICAL_2          0x15                                            // Using hard pull up/down with R/W bit not considered
#define I2C_PHYSICAL_EEPROM     0x50                                            // 1010 xxxR 24LC01 EEPROM, address >> 1 = 0x50, physical address lines not used

#define ADC_SAMPLE_SEQUENCE     1                                               // 0=8 samples, 1=4 samples, 2=4 samples, 3=1 samples
#define ADC_VOLTS_PER_BIT_4096        .0008                                     // 12 bit converter with 3.3V reference TM4C1233

#define DEFAULT_FAN_TRIGGER     40                                              // temperature in C that triggers FAN to turn on (RU1).  40C is 104F

// ******************************************
// ********* EEPROM Map Address *************
// ******************************************
//
// The part being used is: AT24C01D-SSHM
// This part is a 128x8, so 128 addresses
// Maximum address is 0x7F

#define EA_INIT                 0x00                                            // 0xA5 if initialized
//#define EA_SN3                  0x01                                            // serial number 4 bytes, MSB
//#define EA_SN2                  0x02
//#define EA_SN1                  0x03
//#define EA_SN0                  0x04
#define EA_BOARD                0x05                                            // Type of unit, BOARD_CU1, BOARD_RU1
#define EA_VARIANT              0x06
#define EA_EXPAND               0x07                                            // 0x00=Normal, 0xA5=Slave

#define EA_Z1A                   0x11                                           // storage of injection rate, 0=off, 1-9, 1st injector (A)
#define EA_Z2A                   0x12
#define EA_Z3A                   0x13
#define EA_Z4A                   0x14
#define EA_Z5A                   0x15
#define EA_Z6A                   0x16
#define EA_Z7A                   0x17
#define EA_Z8A                   0x18
#define EA_Z9A                   0x19
#define EA_Z10A                  0x1A
#define EA_Z11A                  0x1B
#define EA_Z12A                  0x1C

#define EA_Z1B                   0x21                                           // storage of injection rate, 0=off, 1-9, 2nd injector (B)
#define EA_Z2B                   0x22
#define EA_Z3B                   0x23
#define EA_Z4B                   0x24
#define EA_Z5B                   0x25
#define EA_Z6B                   0x26
#define EA_Z7B                   0x27
#define EA_Z8B                   0x28
#define EA_Z9B                   0x29
#define EA_Z10B                  0x2A
#define EA_Z11B                  0x2B
#define EA_Z12B                  0x2C

#define EA_SN_START             0x30                                            // 16 locations for an ASCII serial number
#define EA_SN_STOP              0x3F

#define EA_MAXTEMP              0x40                                            // max temperature that the RU1 has encountered
#define EA_IACNT                0x48                                            // 48,49,4A,4B Injection A count, RAM variable globalInjectionCountA, 0x48=MSB
#define EA_IBCNT                0x4C                                            // 4C,4D,4E,4F Injection B count, RAM variable globalInjectionCountB, 0x4C=MSB

// ******************************************
// *********** PIN Descriptions *************
// ******************************************

#define PORT_A                  GPIO_PORTA_BASE                                 // CU1 pins all inputs
#define PIN_IN1                 GPIO_PIN_0
#define PIN_IN2                 GPIO_PIN_1
#define PIN_IN3                 GPIO_PIN_2
#define PIN_IN4                 GPIO_PIN_3
#define PIN_IN5                 GPIO_PIN_4
#define PIN_IN6                 GPIO_PIN_5
#define PIN_IN7                 GPIO_PIN_6
#define PIN_IN8                 GPIO_PIN_7

#define PIN_RU1_SOL1            GPIO_PIN_0                                      // RU1 pins all inputs       
#define PIN_RU1_SOL2            GPIO_PIN_1
#define PIN_RU1_SOL3            GPIO_PIN_2
#define PIN_RU1_INJA            GPIO_PIN_3                                      // RU1 injector A
#define PIN_RU1_INJB            GPIO_PIN_4                                      // RU1 injector B
#define PIN_RU1_SAFETY          GPIO_PIN_7


#define PORT_B                  GPIO_PORTB_BASE
#define PIN_DEBUG_RX            GPIO_PIN_0
#define PIN_DEBUG_TX            GPIO_PIN_1
#define PIN_I2C0SCL             GPIO_PIN_2
#define PIN_I2C0SDA             GPIO_PIN_3
#define PIN_PB4_ADC10           GPIO_PIN_4
#define PIN_IN_SEL              GPIO_PIN_5
#define PIN_FAN_CTRL            GPIO_PIN_5
#define PIN_LED1                GPIO_PIN_6
#define PIN_LED2                GPIO_PIN_7

#define PORT_C	                GPIO_PORTC_BASE
#define PIN_LEDA                GPIO_PIN_4
#define PIN_LEDB	            GPIO_PIN_5
#define PIN_LEDC	            GPIO_PIN_6
#define PIN_LEDD	            GPIO_PIN_7

#define PORT_D                  GPIO_PORTD_BASE
#define PIN_LEDE                GPIO_PIN_0
#define PIN_LEDF                GPIO_PIN_1
#define PIN_LEDG                GPIO_PIN_2
#define PIN_LEDDP               GPIO_PIN_3
#define PIN_USBP                GPIO_PIN_4
#define PIN_USBM                GPIO_PIN_5
#define PIN_SWITCH1             GPIO_PIN_6
#define PIN_SWITCH2             GPIO_PIN_7

#define PORT_E                  GPIO_PORTE_BASE
#define PIN_BLE_RX              GPIO_PIN_0
#define PIN_BLE_TX              GPIO_PIN_1
#define PIN_BLE_RSTN            GPIO_PIN_2                                      // CU1 output, is BLE reset
#define PIN_PE2_ADC01           GPIO_PIN_2                                      // RU1 input, ADC01 input current measurement
#define PIN_RS485_EN            GPIO_PIN_3
#define PIN_RS485_RX            GPIO_PIN_4
#define PIN_RS485_TX            GPIO_PIN_5

#define PORT_F                  GPIO_PORTF_BASE
#define BOARD_TYPE              GPIO_PIN_0
#define PIN_SWITCH3             GPIO_PIN_1                                      // if CU1
//#define PIN_FLOW                GPIO_PIN_1                                      // if RU1
#define PIN_TENSCLK             GPIO_PIN_2                                      // CU1 10's seven seg digit
#define PIN_ONESCLK             GPIO_PIN_3                                      // CU1 1's seven seg digit
#define PIN_LEDOE               GPIO_PIN_4

// ******************************************
// ********** Macro definitions *************
// ******************************************

#define FAST_GPIOPinWrite(ulPort, ucPins, ucVal) HWREG(ulPort + (GPIO_O_DATA + (ucPins << 2))) = ucVal
 
// ******************************************
// ************** Variables *****************
// ******************************************

extern unsigned char txBuffer[TX_BUFFER_SIZE];                                                     // string buffer used to send data out serial debug buffer
extern const unsigned char hexArray[];
extern unsigned char stringBuffer[20];                     
extern unsigned int globalPhysicalAdd;                                             // address of this physical tile
//extern unsigned int globalSerialNumber;
//extern unsigned char globalIRAddressMatch;
//extern unsigned char flagMasterTile;                                               // set if this tile is a master tile
extern unsigned char flagEEPROMInitialized;                                        // SET if EEPROM has been programmed
extern const unsigned char defaultLEDTrim[];
extern unsigned int mSecondCounter;                                                // variable used for priority 
//extern unsigned char flagCrossFade;
//extern unsigned char flagRandomIndependent;
//extern unsigned char flagRandomTogether;
//extern unsigned char flagJingleType;
//extern unsigned char flagJingleDwell;
//extern unsigned char flagJingleDwellRandom;
//extern unsigned char flagPowerOn;

// ******************************************
// ************* Prototypes *****************
// ******************************************

unsigned char CalculateTemperature(unsigned int localADCCnt);

void RU1GetADCValue(void);
void InquireChooser (unsigned char localSource);
void CommandProcessTemp(void);    

void OutputInjector(unsigned char zoneIn);
void OutputSolenoid(void);
void GlobalActiveTimeCountCheck(void);
void SolenoidSafetyCheck(void);
void SendZoneViaRS485(void);
void TurnOffInjectorA(void);
void TurnOffInjectorB(void);
void TurnOffInjectorAB(void);
// void SendFlowRequestViaRS485(void);
void FlowControlMonitor(void);
void HexToDecimalStringIntWithPad(unsigned char *recString, unsigned int recVar);
void BLEReset(void);
void SendStringBLE(unsigned char *rec);
//void LEDPortWriteDecimal(unsigned char recData, unsigned recDecimal);
void ProcessSerialInput(unsigned char localSource);
void SwitchInputsDebounce(void);
void CheckProgramMode(void);
void CheckZoneInputs(void);
void EEPROMWriteZoneInjection(void);
void EEPROMReadZoneInjection(void);
void SysTickIntHandler(void);
unsigned int CheckTimerDifference (unsigned int recCompare, unsigned int recPrevious);
void Setup (void);
void SendStringDebug(unsigned char *rec);
void DelayMilliSecondsBlocking(unsigned int length);						   // max value of 6500 (mS)
void DelaySecondsBlocking(unsigned int length);								   // max value of 256 (seconds)
void HexToDecimalString(unsigned char *recString, unsigned char rec);
void StringCopy(unsigned char *localDestination, unsigned char *localSource);
void LongToHexString (unsigned char *recString, unsigned int recLongVar);
void StringCat (unsigned char *localDestination, unsigned char *localSource);
void IntToHexString (unsigned char *recString, unsigned int recIntVar);
void CharToHexString (unsigned char *recString, unsigned char recCharVar);
void DelaySecondsBlocking(unsigned int length);
void HexToDecimalStringInt(unsigned char *recString, unsigned int recVar);
unsigned int DJPFindStringLength(unsigned char *ch);
void StringCatWithCR (unsigned char *localDestination);
void VersionByLED(void);
//void SendMfgModelVerOutSerial(void);
void DumpEEPROMValues(void);
void InitialCondition(void);
void UART5IntHandler(void);
void UART7IntHandler(void);
void UART1IntHandler(void);
void SerialFlagCheck(void);
void GPIOPortDIntHandler(void);
unsigned char I2CReadEEPROMSingle(unsigned char recAddress, unsigned char recI2CSlaveAddress );
void I2CWriteEEPROMSingle(unsigned char recAddress,unsigned char recI2CData, unsigned char recI2CSlaveAddress );
//void EEPROMReadSetup(void);
//void EEPROMLoopTillConfigured(void);
unsigned int Convert3ASCIIDecimalToDecimal(unsigned char *lString, unsigned char lIndex);
unsigned char Convert2ASCIIHexToDecimal(unsigned char lIndex);
//unsigned char Convert2ASCIIHexToDecimal(unsigned char *lString, unsigned char lIndex);
// void SendConfigureResponse(void);
void SendStringRS485NoChecking(unsigned char *rec);
void GPIOPortDIntHandler(void);
void CheckForTimeouts(void);
void VersionBy7Seg(void);
void SendStringRS485Master(unsigned char *rec);
void StringToUpper(unsigned char *localString);
void SendAck(unsigned char localSource);
void FlushRxBuffer(void);
unsigned char DJPStringCompare(unsigned char *localString1, unsigned char localStart1, unsigned char *localString2, unsigned char localStart2, unsigned char localCount);
void ProcessSerialInputBLE(void);

void CommandProcessSerialZoneInput(unsigned char localSource);
void CommandProcessFChooser(unsigned char localSource);
void CommandProcessEEPROMRead(unsigned char localSource);       
void CommandProcessEEPROMWrite(unsigned char localSource);
void CommandProcessVersion(unsigned char localSource);
void CommandProcessSetZone(unsigned char localSource);
void CommandProcessAllZone(unsigned char localSource);
void CommandProcessCommit(unsigned char localSource);
void CommandProcessQuiet(unsigned char localSource);
void CommandProcessHello(unsigned char localSource);
void CommandProcessGet(void);
void CommandProcessTest(unsigned char localSource);

void BLECmdModeSequence(void);
int DJPStringSearch(unsigned char *mainString, unsigned char *subString);
unsigned char BLEExtractNameFromDump(void);
unsigned char BLEWaitForCmdResponseBlocking(void);
unsigned char BLEExtractVersion(void);
unsigned char BLEExtractNameFromDump(void);
unsigned char BLEExtractBTAFromDump(void);
unsigned char BLEEnterCommandMode(void);
unsigned char BLEExitCommandMode(void);
void CommandProcessB(unsigned char localSource);
void CommandProcessExpand(unsigned char localSource);
void SetBLEName(unsigned char *localString);
void CommandProcessSerialNumber(unsigned char localSource);
void GetSerialNumberFromEEPROM(void);

void LEDDualDecimal(unsigned char recData, unsigned recDecimal);
void LEDDualClear(void);
void LEDDualHex(unsigned char recData, unsigned recHighLow, unsigned recDecimal);
void LEDClockTens(void);
void LEDClockOnes(void);
void DebugPortZoneValue(void);
void RU1FanControl(void);
void GPIOPortFIntHandler(void);
void RU1InitInjectorA(void);
void RU1InitInjectorB(void);
void LoopTest(unsigned char localSource);
void InitLoopTest(void);
void LoopTestResult(void);
void CommandProcessCChooser(unsigned char localSource);                                // "C" Commands                        
void CommandProcessClear(unsigned char localSource);          
void InquireTempAllViaRS485(void);
void CU1DebugDisplayRU1Data(void);

void StoreIntToEEPROM(unsigned char localAdd, unsigned int localData);
unsigned int ReadIntFromEEPROM(unsigned char localAdd);
void InquireInjectorCount(void);
void CommandProcessInjectorCount(void);
unsigned char ConvertAsciiToDecimal(unsigned char inputVar);

void SendStringBLETest(void);
void RU1ADCConvertCntToTemperature();
void CurrentEstablishAverage(void);
unsigned int CurrentMeasureMaxMin(void);
//void CurrentTest(void);
void CurrentTestAllOutputs(void);
void CommandProcessCurrent(void);
void InquireCurrent(void);


#endif
