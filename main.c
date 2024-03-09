/*
Project: 	Professor Wiseacres - FertiWiser
Author:		Dan Patten
Date:           01-08-2024
Compiler        IAR 9.50.1
Target CPU:     ARM32 M4 Tiva
CPU Model:      TM4C1233D5PMI
                xxxxxxxxDxxxx   Flash C=32K D=64K E=128K H=256K
                xxxxxxxxx5xxx   RAM 3=12K 5=24K 6=32K
                xxxxxxxxxxPMx   Package PM=64QFP PZ=100QFP PGE=144QFP ZRB=157BGA
                xxxxxxxxxxxxI   I=-40C to 85C   T=-40C to 105C

=====================
PCB Notes and changes
=====================

Gen2 Hardware Changes
CU1
- Added 2nd 7seg LED.  Controlled with 574 latches now.
- Added 4 more inputs, now 12 inputs, 1,2,3,4,5,6,7,8,9,A,B,C now transmitted over RS485 bus.
- Added 3rd switch which is intended to perform test injection.
- Removed injector output.
- Found that capacitor in parallel with optocoupler was causing problems.  Causing false zone inputs. Removed.


RU1
- Added 2nd 7seg LED.  Controlled with 574 latches now.
- Added 3rd switch which is intended to perform test injection.
- Added fan controller.  AC/24VDC linear supply and FET.
- Added thermal sensor that reports temperature.
- Removed flow sensor input
 
Notes:




=====================
Version History
=====================
Version Zip     Date
0.2.0           12-31-2020      Most features implemented, 2nd injector added for RU1    
0.2.2   BUP9    01-01-2021      Add 2 injector storage for CU1, add initialization EEPROM B injector, add EEPROM error checking on init
0.2.3   BUP11   01-01-2021      Cleanup of injector control, fix/test RS485 commands: reboot/version/EERPOM read, start flow sensor framework, 
                                    fix zone change while injector active bug
0.2.4   BUP12   01-03-2021      Flow sensor implemented, CU1 requests data and RU1 replies with correct data.  
0.2.5   BUP13   01-04-2021      Change flow reporting to gallons per hour
0.3.0   BUP19   02-13-2021      BLE commands implemented, flow request implemented, tx/rx of zone/settings, commit RAM zone/settings to EEPROM
0.3.1   BUP20   02-13-2021      Framed all debug information with '[' and ']'
0.3.2   BUP21   02-27-2021      Added [ALLRD] and [ALLWR] commands.  Fixed a few bugs.
0.3.3   BUP22   02-27-2021      Added [TEST] command and cleaned up code.  Added ack to all commands that do not return something.  
                                    Also added ack to BLE commands
0.3.4   BUP23   02-28-2021      Added code so that data is only returned to the source (who asked for it), this got tricky 

0.3.5   ???     ???             ???

0.3.6   BUP24   03-13-2021      Convert [TEST] to [TESTA] and [TESTB] so two different commands can be used to trigger either injector.  
                                    Change RS485 to slower baud rate 19200 to possibly get better reliabilty of long distances.

0.3.7   BUP26   03-28-2021      Fix BLE module reset bug (wrong port), add BLE sequence to get into/out of command mode and change
                                    BLE name, this was alot of work.

0.3.8   BUP27   03-28-2021      Add serial commands: [BLEBOOT] [BLENAME?][BLEBTA?][BLEVER?]

0.3.9   BUP28   03-28-2021      Change to trigger solenoid imediately on zone sense.  Changes made in OutputInjector() and CheckZoneInputs() DJPQ3

0.4.0   BUP29   03-29-2021      Fixed timeout bug in BLEWaitForCmdResponseBlocking(). Added [BLESETNAME>xxxxx] command.

0.4.1   BUP30   03-29-2021      Added ack to [BLEBOOT] and [BLESETNAME>xxxx].  Refined BLESETNAME routine.  
                                    Fixed short TESTA/TESTB injection with "if (flagTestInjector)" in function TurnOffInjectorAB()

0.4.2   BUP32   04-03-2021      Added flagExpand (1=expansion) EEPROM location EA_EXPAND = 0xA5 if expansion.  Added [EXPAND] command.  Added a suffix
                                    to the Z:1,IB:8,E] command which is either 'P' Primary or 'E' Expansion.  OutputSolenoid() modified to only replicate solenoids
                                    if zone command comes from a CU1 that is primary. If expansion selected, the BLE name is changed to "FertiWiser-E".  Default
                                    zone values with unitialized EEPROM are 99999990 and 99999999.

0.4.3   BUP33   04-10-2021      - Modified OutputSolenoid() to turn off replicated solenoids to avoid stuck solenoid when a CU1 is switched from primary to expansion
                                - Added BLECmdModeSequence()to CommandProcessExpand() to check for new name
                                - Changed BLEExtractNameFromDump() changed BLE name to stop search after "_"

0.4.4   BUP34   04-11-2021      - Fixed [BOOT] command not working on RU1.  Added [NSET---] Serial Number set command

0.4.5   BUP35   04-11-2021      - Sometimes the txBuffer output would have a CR (carriage return) inserted when performing rapid [ALLRD?] commands from BLE.  Do not know why.  
                                    Made a local dedicated transmit buffer in CommandProcessAllZone() and problem went away.

0.5.0   BUP061922   06-19-2022  - Added priority to inputs in CheckZoneInputs().  If two zones are on at same time lower input takes presedence.
                                - Added commands to request temperature [INQTEMPNOW] and [INQTEMPMAX]

0.5.1   BUP062622   06-26-2022  - Added capbility to read ADC RU1 and linearize temperature data from a lookup table

*** Gen2 Hardware ***

0.6.0   BUP021923   02-19-2023  - Gen2 hardware.  Added code for dual displays and 9-12 inputs, removed injector on CU1, added fan control on RU1
                                - Has debug serial input bug.  Appears that serial input not getting processed fast enough.

0.6.1   BUP022023   02-20-2023  - Re-worked all three serial interrupts.  Each input has its own dedicated input string and the string is now built in the interrupt.   
                                - Fixed Bug1

0.6.2   BUP022323   02-23-2023  - Added Switch3 input to trigger injection or send injection command.

0.6.3   BUP031223   03-12-2023  - Added detection and display of broken communication.

0.7.0   BUP032223   03-22-2023  - Fixed bugs with broken communication.

0.7.1   BUP032723   03-27-2023  - Fixed more bugs in communication.
                                - Fixed BLE comms bug in BLE interrupt.  Had deleted "if (flagRxCmdInProcessBLE == TRUE)" somehow.
                                - Re-worked I2C EEPROM read/write routines
                                - Made ALLRD and ALLWR commands work with 12 zones
                                - Added FANON and FANOFF commands.  FANON turns fan on regardless of temperature.  It is volatile and will reset to temperature mode on reboot

0.7.2   BUP032823   03-28-2023  - Rework SW3 interrupt to fix random double send od RS485 [TESTA].  Stop disable/enable of interrupt after timeout and just use a counter.

0.7.3   BUP032923   03-29-2023  - Added "," as allowed character in RS485 interupt

0.7.4   BUP040123   04-01-2023  - Added flagZoneChange = 1; in LoopTestResult to get CU1 display back after comms error
                                - Found that capacitor in parallel with optocoupler was causing problems.  Causing false zone inputs.
                                - Moved "GPIOPinWrite(PORT_B, PIN_LED2, 0)" to "else if (flagBoardType == BOARD_RU1)" so injector LED does not turn on if CU1.
                                - General cleanup.

0.7.5   BUP042123   04-21-2023  - Added globalZoneOnCount so that zone detection must pass multiple consecutive detections that are the same. This was added so when two
                                    zones are on at same time, the higher zone does not glitch the detection of lower zone through zero cross.
0.8.0   BUP042223   04-22-2023  - Added commands to reset the max recorded temperature [CLEARMAX] received by CU1 which sends RS485 command [485CLEARMAX] to RU1

0.8.1   BUP051323   05-13-2023  - Added command [GETTEMPALL?] so BLE can request both temperatures at same time.  Also changed CU1/RU1 command to request both temperatures.

0.8.2   BUP052023   05-20-2023  - Added debug output of current temperature on CU1.

0.8.3   ???

0.9.0   BUP071923   07-19-2023  - Fixed bug where reading EEPROM only reported back to debug port. Was issue with function CommandProcessEEPROMRead not having localSource
                                    passed to it.

0.9.2               12-02-2023  - Made Switch3 part of interrupt.
                                - CU1 Switch3 allows front panel control of injector B, Injector test function removed.
                                - RU1 Switch3 now is test of injectors A and B at the same time.
                                - Cleaned up temperature reporting.  Removed "NOW" and "MAX" commands, can only request "ALL" ie both at same time.
                                - [GETINJECT?] command added.  Gets injection count.
                                - [CLEARINJECT] command added.  Clears injection count.

0.9.3   BUP121323   12-13-2023  - Changed SendZoneViaRS485() to send InjectorA and InjectorB data at same time [Z:1,IA:4,IB:5,P].  Modified CommandProcessSerialZoneInput() 
                                    and OutputInjector() to allow simultaneous injector.
                                - Changed decimal points to indicate if injectors are active.  Tens decimal = InjectorA, Ones decimal = InjectorB

0.9.4   BUP121323A  12-13-2023  - Added feature in  OutputInjector() so InjectorB starts 2 seconds after InjectorA by adding #define SOLENOID_OFFSET_B       

0.9.5   BUP121623   12-16-2023  - Bug CU1 zone input not displayed when no injection programmed. Fix in OutputInjector() else statement, add LEDDualDecimal().
                                - Bug RU1. If active zone is followed my new zone with no injection, previous zone not turned off.  Add TurnOffInjectorA() in same location
                                    as bug above.

0.9.6   BUP096      12-17-2023  - Added delay SysCtlDelay(1000) before BLE transmit busy check in function SendStringBLE()

0.9.7   BUP097      12-18-2023  - CU1 was turning on old injector LED which is no longer valid. Fixed.
                                - Reworked injector test feature by simulating Zone1 set to 10, much simpler, changes in OutputInjector() and controlled by flagTestInjectorA


0.9.8   BUP098      12-19-2023  - Changed [CLEARINJECT] to two seperate commands [CLEARINJECTA] & [CLEARINJECTB] so counters can be cleared independently
                                - [485CLEARINJECTA] & [485CLEARINJECTB] 485 commands added to clear injector counts
                                - Updated temperature ADC count to C table to be more accurate
                                - Moved [485CLEARMAX],[485CLEARINJECTA],[485CLEARINJECTB] from 5 second scheduler to 1 second scheduler

0.9.9   BUP099      12-28-2023  - CommandProcessAllZone() was not converting ASCII 0-9,A-F to 0-15 decimal properly.
                                - CommandProcessSetZone() was not converting ASCII 0-9,A-F to 0-15 decimal properly.

1.0.0   BUP100      12-31-2023  - Release candidate.   
                                - Add some documentation. 
                                - Fix bug in StoreIntToEEPROM(), only low byte was stored.
                          
1.0.1   BUP101      01-06-2024  - Add current read functions.  The RU1 will read the current sensor individually enabling outputs and measures prior to while(1) loop starts.  The values are transmitted in 5 second scheduler
                                    permanently anticiapting more current code that is activley measuring.  Commands RS485 function [INQCURRENT?] and BLE command [GETCURRENT?] added.  Functions: CurrentTestAllOutputs(),
                                    CurrentMeasureMaxMin(), CurrentEstablishAverage(), InquireCurrent() added.

1.0.2   BUP102      01-08-2024  - Reverse Injector A and B indication on the 7 segment display. Changes made noted by "DJP8" functions CheckProgramMode() and 
                                    OutputInjector modified. LEDDualDecimal() function modified with "bypass" variable passed to it.
    
1.0.3   BUP103      03-03-2024  - Add function CommandProcessCommit() to the AllWrite command so that the commit command is not required to move RAM to EEPROM. Requested
                                Jon Lips.


======================
Notes
======================

- A 2 second injection is approximately 1mL
- InjectorA uses a 20L jug of fertilizer
- InjectorB uses a 10L jug of fertilizer
- Max temperature of injectors is 150F
- Turn on fan at 104F (40C)
- App temperature text turns yellow at 110F
- App temperature text turns red at 150F
- App fertilizer percentage text turns yellow at 20%
- App fertilizer percentage text turns red at 5%

======================
Code To Do (after meeting with Jon and Eric)
======================
- Dim 7seg for prettiness sake
--------------
Done
- Change CU1 test button to program B injector only
- Change/Add a 10th pulse
- Count injections and store in EEPROM
- Add command to read and reset injection count
- Allow both injectors to run at same time.  Injector used to have presidence

======================
Questions
======================

- If RS485 is reversed and RU1 receives junk for too long,  RU1 RS485 uart seems to lock up.  If the E1 error occurs, should I do a self reset?
- Should the serial output triggered by pushing switch 3 be scheduled to go out rather than async?
- Implement watchdog?
- Rework all commands to have a destination prefix or suffix.  instead of [v] for version make it [VCU1] and [VRU1] etc?
- DIM up/down on display to look cool

=====================
Questions/Comments
=====================
+added code
-deleted commented out code

DJPQ1   Do I really need to turn off solenoids here?  CommandProcessSerialZoneInput
DJPQ2
DJPQ3   Changes to make injector trigger on initial zone sense

DJP4+   Increase from 9 to 10 on injection setting
DJP5+   Change injection speed display to 2 digits
DJP6+   Changed injector B output "else if" to "if"  in OutputInjector(), this will allow both injectors to run at same time, ie InjectorA no longer has presidence
DJP7    Injector test changes
DJP8    Swap of 7 seg decimal points for injector A and injector B

======================
Files modified in this program:
======================
startup.c uVectorEntry

======================
Notes about this project development...
======================

Faster way to write to the port.
#define FAST_GPIOPinWrite(ulPort, ucPins, ucVal) HWREG(ulPort + (GPIO_O_DATA + (ucPins << 2))) = ucVal
back to back GPIOPinWrite at 80MHz results in a 220nS assertion time or 2.27MHz frequency
back to back GPIOPinWrite using a macro at 80MHz is 196nS on/off = 5MHz

Port F.0  This port is also the NMI and requires the goofy unlock procedure to be used.  This pin was used for the strobe and found that it only responded to rising
edges regardless of setting to rise or fall.

GPIOPinWrite(PORT_A, PIN_LED_DEBUG, ~GPIOPinRead(PORT_A, PIN_LED_DEBUG)); // toggle the debug LED            
GPIOPinWrite(PORT_A, PIN_LED_DEBUG,0);
SysCtlDelay(1000);   // each iteration is 3 assembly instructions, (1/80MHz)(3)(1000)=37.5uS   
                                            

===================
Segment Details
===================
    A
   ---
F |   | B
   ---     G (middle segment)
E |   | C
   --- *   P (decimal point)
    D
		 GFE DCBA
SEG_0 0x3F	0011 1111
SEG_1 0x06	0000 0110
SEG_2 0x5B	0101 1011
SEG_3 0x4F	0100 1111
SEG_4 0x66	0110 0110
SEG_5 0x6D	0110 1101
SEG_6 0x7C	0111 1101
SEG_7 0x07	0000 0111
SEG_8 0x7F	0111 1111
SEG_9 0x67	0110 0111
SEG_A 0x77	0111 0111
SEG_B 0x7C	0111 1100
SEG_C 0x39	0011 1001
SEG_D 0x5E	0101 1110
SEG_E 0x79	0111 1001
SEG_F 0x73	0111 0001

#define ALPHA_c  0101 1000				0x58
#define ALPHA_u	 0001 1100				0x1C
#define ALPHA_r  0101 0000				0x50
#define ALPHA_P  0111 0011				0x73

====================
CU1 Gen 2 (after meeting with Jon/Eric 07/xx/2023)
====================

Set     Detail  ON      OFF     mL Min
0       Off     
1       Min     2       58      1
2       -       2       28      2
3       -       2       18      3
4       -       2       13      4
5       -       2       10      5
6       -       2       8       6
7       -       2       7       6.7
8       -       2       6       7.5
9       -       2       5       8.6
10      Max     2       4       10

const unsigned int pulseIntervalOff[]={0,5800,2800,1800,1300,1000,800, 700, 600, 500, 400};  

====================
Error Codes
====================

CU1
E1	No communication to RU1
E3      Possible RU1 power problem (use App for more detail)
E4      Possible RU1 solenoid/injector problem (use App for more detail)

RU1
E1	No Communication to CU1
E2	Possible reversal of RS485 wires
E3      Possible RU1 power problem (use App for more detail)
E4      Possible RU1 solenoid/injector problem (use App for more detail)



*/
#include "main.h"

// 7 segment lookup table, 0 through F and OFF
const unsigned char segArray[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7c,0x07,0x7F,0x67,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00}; 

// hex conversion lookup table
const unsigned char hexArray[]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};        

// Lookup table for the injector cycle times
// - The fastest injection cycle possible is 6 seconds. 2 seconds on and 4 seconds to exhaust. Anything shorter will be only a partial injection.  
// - This number is in 10mS steps.  So (400)(0.010s) = 4 seconds.  This is the OFF time
// - The on time of the injector is fixed at 2 seconds
// - This lookup will often be indexed by an array globalZoneArray[zoneIn]
// - The on time is stored in define: SOLENOID_ON_TIME.  Also in 10mS resolution
//
const unsigned int pulseIntervalOff[]={0, 5800, 2800, 1800, 1300, 1000, 800, 700, 600, 500, 400};       // 10 settings

//
// This is a constant lookup table for a 10K at 25C NTC.  Murata NCU18XH103D60RB.  A resistor divider
// 10K on top and the NTC on bottom.  The values are for a 3.3V referenced ADC with 12bit (4096) resolution.
// The range of this lookup is from 0C to 90C.
// Note that the 26th element is 2048 which is half the 12 bit resolution.
// The NTC response is not linear and this table was provided from Murata, SimSurfing tool.
//
// Note voltage decreases as temperature increases.
//
// Example:
// My lair is currently 16C and voltage to ADC is 1.51V. ADC count is 1820d
// 3.3V/4096 = 0.000805V per count
// 0.000805 * 1820 = 1.45V which is close to what was measured with DMM on input to ADC
//

// Top resistor loaded 10K, 10K simulation, NTC bottom
const unsigned int tempLookup[]=
{
2709,                                                                           // 0C
2678,2645,2613,2580,2546,2513,2479,2444,2410,2375,                              //  1C to 10C
2340,2305,2270,2235,2200,2164,2129,2093,2058,2023,                              // 11C to 20C
1987,1952,1917,1883,1848,1813,1778,1743,1708,1674,                              // 21C to 30C
1640,1606,1573,1540,1508,1476,1444,1413,1382,1352,                              // 31C to 40C
1322,1292,1263,1235,1207,1179,1152,1126,1100,1074,                              // 41C to 50C
1049,1024,1000, 976, 953, 931, 908, 887, 865, 845,                              // 51C to 60C
 824, 804, 785, 766, 747, 729, 711, 694, 677, 661,                              // 61C to 70C
 645, 629, 614, 599, 584, 570, 556, 542, 529, 516,                              // 71C to 80C
 504, 491, 480, 468, 457, 446, 435, 424, 414, 404,                              // 81C to 90C
 394                                                                            // 91C
};



unsigned int mSecondCounter = 0;                                                // variable used for priority 
unsigned char txBuffer[TX_BUFFER_SIZE];                                         // string buffer used to send data out serial ports
unsigned char txBufferBLE[TX_BUFFER_SIZE];                                         // string buffer used to send data out serial ports
unsigned char rxBuffer[RX_BUFFER_SIZE];                                         // string buffer that is loaded from serial input interrupt        
unsigned char rxBufferDebug[RX_BUFFER_SIZE];                                         // string buffer that is loaded from serial input interrupt        
unsigned char rxBufferBLE[RX_BUFFER_SIZE];                                         // string buffer that is loaded from serial input interrupt        
unsigned char cmdBuffer[RX_BUFFER_SIZE];                                         // string buffer that is loaded from serial input interrupt        

unsigned char rxBufferBLECmd[100];
unsigned char rxBufferBLECmdCnt = 0;
unsigned char globalPreRxVarBLE;                                                // Previous char from BLE module
unsigned char globalPrePreRxVarBLE;                                             // Previous Previous from BLE module :)
unsigned char globalDebugCnt;
unsigned char bleFirmwareVersion[6];
unsigned char bleModuleBTAString[13];
unsigned char bleModuleNameString[17];
unsigned char bleModuleConnectString[4];
unsigned char globalSerialNumberString[17];                                     // 0-15 are serial number, 16 is NULL

unsigned char stringBuffer[20];
unsigned char stringDebug[20];
unsigned char I2CWriteBuffer[10];
unsigned char rxVar;
unsigned char rxVarDebug;
unsigned char rxVarBLE;
unsigned char globalRxCnt = 0;
unsigned char globalRxCntDebug = 0;
unsigned char globalRxCntBLE = 0;

//unsigned int globalSerialNumber = 0;
unsigned char globalSerialTimeout = 0;                                          // counts every second, when terminal reached, stream processing reset
unsigned char globalSerialTimeoutDebug = 0;                                     // counts every second, when terminal reached, stream processing reset
unsigned char globalSerialTimeoutBLE = 0;                                       // counts every second, when terminal reached, stream processing reset
//unsigned int globalSecondCounter = 0;
unsigned char globalSerialSource;                                               // where is data coming from? D=debug, 4=485, U=USB, B=BLE
unsigned char globalSwitch1DebounceCount;
unsigned char globalSwitch2DebounceCount;
unsigned char globalSwitch3DebounceCount;
unsigned char globalZoneActive = 0;                                             // 0=no active zone, 1-8 are possible values
unsigned char globalInjectVar = 0;                                              // 0=no injection, 1-9 are possible values
unsigned char globalZoneArrayA[14];                                             // Zone 1-12 contains injection variable for each zone, location [0] not used, contains value 0-10 (0-A)
unsigned char globalZoneArrayB[14];                                              // as above but for 2nd (B) injector
unsigned int globalZoneOffCount;                                                // used to filter out zero cross spike on AC input optocoupler's output, consecutive NO zone input count
unsigned int globalZoneOnCount;                                                // used to filter out zero cross spike on AC input optocoupler's output, consecutive same zone count
unsigned int globalInjectionTimeCountA = 0;                                     // 0.1 second counter of injection on and off times
unsigned int globalInjectionTimeCountB = 0;                                     // 0.1 second counter of injection on and off times

unsigned char globalActiveZone;                                                 // active zone 
unsigned char globalCurrentZone;

unsigned int globalActiveTimeCount;                                             // counts how long after a valid serial command has been received, inc at 0.1S rate
unsigned int globalFlowInputRU1;                                                // count of water flow, continual update, RU1
unsigned int globalFlowInputCU1 = 0;                                            // CU1 version of variable above, received from RU1 every 5 seconds
unsigned char globalFlowStringCU1[8] = {'0','0','0','0','0',NULL,NULL,NULL};    // ASCII version of globalFlowInputCU1

unsigned char globalTempMaxInputRU1 = 0;                                         // current max temperature, RU1
//unsigned char globalTempMaxInputCU1 = 0;                                         // CU1 version of max variable above,
unsigned char globalTempNowInputRU1 = 0;                                         // current now temperature, RU1
//unsigned char globalTempNowInputCU1 = 0;                                         // CU1 version of now variable above,
//unsigned char globalTempMaxStringCU1[4] = {'0','0','0',NULL};                   // ASCII version of globalTempMaxInputCU1
//unsigned char globalTempNowStringCU1[4] = {'0','0','0',NULL};                   // ASCII version of globalTempNowInputCU1
unsigned char globalCU1TemperatureString[20];                                        // string version of both values stored on CU1

unsigned int globalADCVar[4];
unsigned char globalRU1RXDropCnt = 0;                                           // count of how many seconds RU1 has counted without comms
unsigned char globalLastZone;

// Injection count stored in 32 bit integer.
// Most fast cycle is an injection ever 6 seconds.
// How many seconds in a year?
// 60*60*24=86,400 seconds in a day
// 86,400x365 = 31,536,000 seconds in a year
unsigned int globalInjectionCountA = 0;                                         // how many pulses has the injector A performed
unsigned int globalInjectionCountB = 0;                                         // how many pulses has the injector B performed
unsigned char globalCU1InjectionString[30];                                     // string version of both values stored on CU1

unsigned char globalCU1CurrentString[45];                                       // string version of values below
unsigned int globalCurrentAverage = 0;
unsigned int globalCurrentSol1 = 0;
unsigned int globalCurrentSol2 = 0;
unsigned int globalCurrentSol3 = 0;
unsigned int globalCurrentInjA = 0;
unsigned int globalCurrentInjB = 0;
unsigned int globalCurrentFan = 0;

// ***************** Flags *********************

unsigned char flagBoardType = 0;                                                // 1=CU1, 2=RU1
//unsigned char flagRxProcess = 0;
//unsigned char flagRxProcessDebug = 0;
//unsigned char flagRxProcessBLE = 0;
unsigned char flagRxCmdInProcess = 0;
unsigned char flagRxCmdInProcessDebug = 0;
unsigned char flagRxCmdInProcessBLE = 0;
unsigned char flagEEPROMInitialized = 0;                                        // SET if EEPROM has been programmed
unsigned char flagSwitch1Pressed = 0;
unsigned char flagSwitch2Pressed = 0;
unsigned char flagSwitch3Pressed = 0;
unsigned char flagSwitch1Released = 0;
unsigned char flagSwitch2Released = 0;
unsigned char flagSwitch3Released = 0;
unsigned char flagSwitch1Long = 0;
unsigned char flagProgramMode = 0;
unsigned char flagQuiet = 0;                                                    // SET to stop CU1 from sending to RU1
unsigned char flagTestInjectorA = 0;                                             // Set to force a single injection
unsigned char flagTestInjectorB = 0;                                             // Set to force a single injection

unsigned char flagBLECmdResponse = 0;                                           // set when a BLE Command response has been received
unsigned char flagExpand = 0;                                                   // set if the CU1 is in expansion (slave) mode.  In expansion mode, the CU1 does not do solenoid replication
unsigned char flagLoopTestCU1 = 0;                                              // 1=pass
unsigned char flagLoopTestRU1 = 0;                                              // 1=pass

unsigned char flagClearLEDOnce = 0;                                             // set to allow clearing lEDs, prevents repetitive calls to clear
unsigned char flagClearLEDOnceLoopTestRU1 = 0;                                  // set to allow clearing lEDs, prevents repetitive calls to clear
unsigned char flagClearLEDOnceLoopTestCU1 = 0;                                  // set to allow clearing lEDs, prevents repetitive calls to clear
unsigned char flagRU1FanOverride = 0;                                           // if set, turn on fan regardless of temperature
unsigned char flagZoneChange = 0;                                               // will be set on a zone change and imediately cleared so that new zone stuff only runs once
unsigned char flagCU1ClearAfterError = 0;
unsigned char flagZoneActive = 0;                                               // flag set if multiple consecutive same zone input is reached
unsigned char flagScheduleClearMaxTemp = 0;                                     // set to schedule command on RS485, clear max temperature on RU1
unsigned char flagScheduleInjectA = 0;                                           // set to schedule command on RS485, clear inject A values on RU1
unsigned char flagScheduleInjectB = 0;                                           // set to schedule command on RS485, clear inject B values on RU1

unsigned char flagDecimalOnes = 0;
unsigned char flagDecimalTens = 0;

// ****************************************************************************
// ********************************* main *************************************
// ****************************************************************************
//
int main(void)
{ 
    unsigned int mSecondCounter1xSecond = 0;
    unsigned int mSecondCounter10xSecond = 0;
    unsigned int mSecondCounter100xSecond = 0;
    unsigned int mSecondCounter5Seconds = 0;    
    unsigned int secondCounter = 0;                                             // 60secondsx10minutes=600, 6x24x365=52,560 executions per year

    unsigned requestCount = 0;                                                  // counter to schedule how often CU1 requests data from RU1
    
    Setup();
    TurnOffInjectorAB();                                                        // turn off all injectors
    GPIOPinWrite(PORT_B, PIN_LED2, PIN_LED2);                                   // turn off injection LED (low asserted)
    GPIOPinWrite(PORT_F, PIN_LEDOE, 0);                                         // turn on seven seg LED buffers (low asserted)
    GPIOPinWrite(PORT_B, PIN_FAN_CTRL,PIN_FAN_CTRL);                            // SET is fan off.
    globalActiveZone = 0;                                                       // no zones active on init
    OutputSolenoid();                                                           // turn off all solenoids
    BLEReset();                                                                 // reset the BLE module
    BLECmdModeSequence();                                                       // enter BLE command mode, get version, name, serial number
    GetSerialNumberFromEEPROM();
    CommandProcessVersion('D');                                                 // Send config data out debug port
    EEPROMReadZoneInjection();                                                  // get setups from EEPROM   
    DumpEEPROMValues();
    VersionBy7Seg();
    GPIOIntEnable(PORT_D, PIN_SWITCH1 | PIN_SWITCH2);                           // enable switch1 and switch2 interrupt    
    GPIOIntEnable(PORT_F, PIN_SWITCH3);                                         // enable switch3 interrupt    
    CurrentEstablishAverage();                                                  // check current sensor, sample and average    
    CurrentTestAllOutputs();                                                    // measure current for solenoids, injectors and fan

    
    while (1)
    {                                    
        // *************** RUN EVERY 1/100 SECOND (10mS) ******************
        if (CheckTimerDifference(10, mSecondCounter100xSecond))		        // executed 100 times second
        {
            mSecondCounter100xSecond  = mSecondCounter;  
            CheckZoneInputs();                                                  // look at the 12 optocoupler inputs
        }

        // *************** RUN EVERY 1/10 SECOND (100mS) ******************
        if (CheckTimerDifference(100, mSecondCounter10xSecond))		        // executed 10 times second (100mS)
        {
            mSecondCounter10xSecond = mSecondCounter;     
            SwitchInputsDebounce();                                             // Zone and +/- switch inputs
            CheckProgramMode();                                                 // CU1 only, mode when changing zone configuration via physical buttons
            GlobalActiveTimeCountCheck();                                       // check to see if active zone timer as expired
            SolenoidSafetyCheck();                                              // tickle the oneshot to keep optocoupler sink active
            RU1GetADCValue();                                                   // look at ADC value for NTC (temperature) and current
        }   
        // *************** RUN EVERY SECOND ******************
        if (CheckTimerDifference(1000, mSecondCounter1xSecond))		        // execute 1x second
        {
            mSecondCounter1xSecond  = mSecondCounter; 
            globalRU1RXDropCnt++;
            if (globalRU1RXDropCnt > 10) flagLoopTestRU1 = LOOP_TEST_RU1_FAIL;
            CheckForTimeouts();                                                 // check for serial input data timeouts
            GPIOPinWrite(PORT_B, PIN_LED1, ~GPIOPinRead(PORT_B, PIN_LED1));     // toggle the debug LED 
            OutputSolenoid();                                                   // control replicated solenoids on RU1 only
            RU1ADCConvertCntToTemperature();                                    // convert ADC cnt to temperature
            RU1FanControl(); 
            secondCounter++;
            if (secondCounter > 600)                                            // 10 minutes has elapsed?  We do not want to wearout EEPROM, so only save every 10 minutes
            {
                if (flagBoardType == BOARD_RU1)                                 // are we RU1 controller?
                { 
                    StoreIntToEEPROM(EA_IACNT, globalInjectionCountA);  
                    StoreIntToEEPROM(EA_IBCNT, globalInjectionCountB);  
                }
                secondCounter = 0;                                              // reset second counter, and start counting to 10 minutes again
            }
            
            if (flagScheduleClearMaxTemp)
            {
                SendStringRS485NoChecking("[485CLEARMAX]");                                      
                flagScheduleClearMaxTemp = 0;
            }
            else if (flagScheduleInjectA)
            {
                SendStringRS485NoChecking("[485CLEARINJECTA]");                                      
                flagScheduleInjectA = 0;
            }
            else if (flagScheduleInjectB)
            {
                SendStringRS485NoChecking("[485CLEARINJECTB]");                                      
                flagScheduleInjectB = 0;
            }
        }
        // *************** RUN EVERY 5 SECONDS ******************
        if (CheckTimerDifference(5000, mSecondCounter5Seconds))		        // execute every seconds
        {
            mSecondCounter5Seconds  = mSecondCounter; 
            SendZoneViaRS485();                                                 // CU1 to RU1 only, send active zone data
            
            // schedule status requests from RU1

            requestCount++;
            if(requestCount==1)
            {
                InitLoopTest();
            }
            else if(requestCount==2)
            {
                LoopTestResult();                                               // results of loop test                
            }  
            else if(requestCount==3)
            {
                InquireTempAllViaRS485();                                       // CU1 to RU1 only, get current and max temperature
            } 
            else if(requestCount==4)
            {
                InquireInjectorCount();                                         // get injector count
            }             
            else if(requestCount==5)
            {
                InquireCurrent();                                               // get current
                requestCount=0;                                                 // reset the count
            }             
        }
    } 
} 
// ****************************************************************************
// ************************** CurrentTestAllOutputs ***************************
// ****************************************************************************
// 
// This function measures current for Solenoid 1,2,3, InjectorA, Injector B and Fan. This is
// blocking function and is called before the while(1) main loop starts.  Global variables
// are loaded that will be transmitted to the CU1.
//
void CurrentTestAllOutputs(void)
{
    if (flagBoardType == BOARD_RU1)                                             // are we a RU1?
    {    
        GPIOPinWrite(PORT_A, PIN_RU1_SOL1, PIN_RU1_SOL1);                       // turn on opto LED (high asserted)   
        globalCurrentSol1 = CurrentMeasureMaxMin();
        GPIOPinWrite(PORT_A, PIN_RU1_SOL1, 0);                                  // turn off 
        DelayMilliSecondsBlocking(100);                                         

        GPIOPinWrite(PORT_A, PIN_RU1_SOL2, PIN_RU1_SOL2);                       // turn on opto LED (high asserted)   
        globalCurrentSol2 = CurrentMeasureMaxMin();
        GPIOPinWrite(PORT_A, PIN_RU1_SOL2, 0);                                  // turn off 
        DelayMilliSecondsBlocking(100);                                         
        
        GPIOPinWrite(PORT_A, PIN_RU1_SOL3, PIN_RU1_SOL3);                       // turn on opto LED (high asserted)   
        globalCurrentSol3 = CurrentMeasureMaxMin();
        GPIOPinWrite(PORT_A, PIN_RU1_SOL3, 0);                                  // turn off 
        DelayMilliSecondsBlocking(100);                                        

        GPIOPinWrite(PORT_A, PIN_RU1_INJA, PIN_RU1_INJA);                       // turn on opto LED (high asserted)   
        globalCurrentInjA = CurrentMeasureMaxMin();
        GPIOPinWrite(PORT_A, PIN_RU1_INJA, 0);                                  // turn off 
        DelayMilliSecondsBlocking(100);                                         

        GPIOPinWrite(PORT_A, PIN_RU1_INJB, PIN_RU1_INJB);                       // turn on opto LED (high asserted)   
        globalCurrentInjB = CurrentMeasureMaxMin();
        GPIOPinWrite(PORT_A, PIN_RU1_INJB, 0);                                  // turn off 
        DelayMilliSecondsBlocking(100);                                       

        FAST_GPIOPinWrite(PORT_B, PIN_FAN_CTRL,0);                              // CLR is fan on.
        globalCurrentFan = CurrentMeasureMaxMin();
        FAST_GPIOPinWrite(PORT_B, PIN_FAN_CTRL,PIN_FAN_CTRL);                   // SET is fan off.

        // transmit results out debug port
        txBuffer[0] = 'S';
        txBuffer[1] = 'o';
        txBuffer[2] = 'l';
        txBuffer[3] = '1';
        txBuffer[4] = ':';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[((globalCurrentSol1 >> 12) & 0x0F)];
        txBuffer[7] = hexArray[((globalCurrentSol1 >> 8) & 0x0F)];
        txBuffer[8] = hexArray[((globalCurrentSol1 >> 4) & 0x0F)];
        txBuffer[9] = hexArray[globalCurrentSol1 & 0x0F];
        txBuffer[10] = NULL;
        SendStringDebug(txBuffer);

        txBuffer[0] = 'S';
        txBuffer[1] = 'o';
        txBuffer[2] = 'l';
        txBuffer[3] = '2';
        txBuffer[4] = ':';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[((globalCurrentSol2 >> 12) & 0x0F)];
        txBuffer[7] = hexArray[((globalCurrentSol2 >> 8) & 0x0F)];
        txBuffer[8] = hexArray[((globalCurrentSol2 >> 4) & 0x0F)];
        txBuffer[9] = hexArray[globalCurrentSol2 & 0x0F];
        txBuffer[10] = NULL;
        SendStringDebug(txBuffer);

        txBuffer[0] = 'S';
        txBuffer[1] = 'o';
        txBuffer[2] = 'l';
        txBuffer[3] = '3';
        txBuffer[4] = ':';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[((globalCurrentSol3 >> 12) & 0x0F)];
        txBuffer[7] = hexArray[((globalCurrentSol3 >> 8) & 0x0F)];
        txBuffer[8] = hexArray[((globalCurrentSol3 >> 4) & 0x0F)];
        txBuffer[9] = hexArray[globalCurrentSol3 & 0x0F];
        txBuffer[10] = NULL;
        SendStringDebug(txBuffer);

        txBuffer[0] = 'I';
        txBuffer[1] = 'n';
        txBuffer[2] = 'j';
        txBuffer[3] = 'A';
        txBuffer[4] = ':';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[((globalCurrentInjA >> 12) & 0x0F)];
        txBuffer[7] = hexArray[((globalCurrentInjA >> 8) & 0x0F)];
        txBuffer[8] = hexArray[((globalCurrentInjA >> 4) & 0x0F)];
        txBuffer[9] = hexArray[globalCurrentInjA & 0x0F];
        txBuffer[10] = NULL;
        SendStringDebug(txBuffer);

        txBuffer[0] = 'I';
        txBuffer[1] = 'n';
        txBuffer[2] = 'J';
        txBuffer[3] = 'B';
        txBuffer[4] = ':';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[((globalCurrentInjB >> 12) & 0x0F)];
        txBuffer[7] = hexArray[((globalCurrentInjB >> 8) & 0x0F)];
        txBuffer[8] = hexArray[((globalCurrentInjB >> 4) & 0x0F)];
        txBuffer[9] = hexArray[globalCurrentInjB & 0x0F];
        txBuffer[10] = NULL;
        SendStringDebug(txBuffer);

        txBuffer[0] = 'F';
        txBuffer[1] = 'a';
        txBuffer[2] = 'n';
        txBuffer[3] = ':';
        txBuffer[4] = ' ';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[((globalCurrentFan >> 12) & 0x0F)];
        txBuffer[7] = hexArray[((globalCurrentFan >> 8) & 0x0F)];
        txBuffer[8] = hexArray[((globalCurrentFan >> 4) & 0x0F)];
        txBuffer[9] = hexArray[globalCurrentFan & 0x0F];
        txBuffer[10] = NULL;
        SendStringDebug(txBuffer);  
    }
}
// ****************************************************************************
// ************************** CurrentMeasureMaxMin ****************************
// ****************************************************************************
// 
// This is a blocking function that has forced delays.
//
// This function will measure a maximum and minimum current and calculate difference between the two. 
// The difference is returned.  The Hall effect current sensor output is centered on 2.5V and since the 
// current is AC, max readings will be above and below the center point.   The max and min is measured
// which coresonds to a current peak to peak.
//
// Typical returned values are shown below.
//
//      Avg     Now     Max     Min     Diff
//      0x0C36  0x0CB7  0x0CC4  0x0C36  0x008E          No load
//      0x0C36  0x0BBE  0x0D28  0x0C36  0x00F2          Solenoid Rainbird PN 235430
//      0x0C36  0x0BDF  0x0E5F  0x0C36  0x0229          2x Solenoid Rainbird PN 235430
//      0x0C36  0x0C3D  0x0D2D  0x0C36  0x00F7          Orbit
//      0x0C3F  0x0C24  0x0E17  0x0C3F  0x01D8          Fan
//      0x0C3F  0x0C24  0x0E17  0x0C3F  0x01D8          Fan adding solenoid.  This is odd, max current does not increase
//

unsigned int CurrentMeasureMaxMin(void)
{
    unsigned int localCurrentNow;
    unsigned int localCnt;
    unsigned int localCurrentDif;
    unsigned int localCurrentMax;
    unsigned int localCurrentMin;
    
    if (flagBoardType == BOARD_RU1)                                             // are we a RU1?
    {
        localCurrentMax = globalCurrentAverage;                                 // start min and max values at a central point
        localCurrentMin = globalCurrentAverage;
        
        GPIOPinWrite(PORT_A, PIN_RU1_SAFETY, ~GPIOPinRead(PORT_A, PIN_RU1_SAFETY)); // toggle the safety pin
        GPIOPinWrite(PORT_A, PIN_RU1_SAFETY, ~GPIOPinRead(PORT_A, PIN_RU1_SAFETY)); // toggle the safety pin
        DelayMilliSecondsBlocking(250);                                         // Allow a little time for current to settle before measuring
        GPIOPinWrite(PORT_A, PIN_RU1_SAFETY, ~GPIOPinRead(PORT_A, PIN_RU1_SAFETY)); // toggle the safety pin
        GPIOPinWrite(PORT_A, PIN_RU1_SAFETY, ~GPIOPinRead(PORT_A, PIN_RU1_SAFETY)); // toggle the safety pin
        DelayMilliSecondsBlocking(250);                                         // Allow a little time for current to settle before measuring
       
        for(localCnt=0; localCnt < 500; localCnt++)
        {          
            if (ADCIntStatus(ADC0_BASE, ADC_SAMPLE_SEQUENCE, true))	        // Has a conversion finished
            {
                ADCIntClear(ADC0_BASE, ADC_SAMPLE_SEQUENCE);			// Clear the ADC interrupt flag.
                ADCSequenceDataGet(ADC0_BASE, ADC_SAMPLE_SEQUENCE, globalADCVar);	// Read ADC Value.
                ADCProcessorTrigger(ADC0_BASE, ADC_SAMPLE_SEQUENCE);		// Trigger another ADC conversion

                localCurrentNow =  globalADCVar[0];
                DelayMilliSecondsBlocking(1);                                   // force our sampling rate to be 1Kz.

                if (localCurrentNow > localCurrentMax)                          // is value greater than max?
                {
                    localCurrentMax = localCurrentNow;                          // store max
                }
                else if (localCurrentNow < localCurrentMin)
                {
                    localCurrentMin = localCurrentNow;
                }
                
                GPIOPinWrite(PORT_A, PIN_RU1_SAFETY, ~GPIOPinRead(PORT_A, PIN_RU1_SAFETY)); // toggle the safety pin
            }
        }        
        
        localCurrentDif = localCurrentMax - localCurrentMin;                    // calculate difference between Max and Min

/*                
            txBuffer[0] = ' ';
            txBuffer[1] = ' ';
            txBuffer[2] = hexArray[((globalCurrentAverage >> 12) & 0x0F)];
            txBuffer[3] = hexArray[((globalCurrentAverage >> 8) & 0x0F)];
            txBuffer[4] = hexArray[((globalCurrentAverage >> 4) & 0x0F)];
            txBuffer[5] = hexArray[globalCurrentAverage & 0x0F];
            txBuffer[6] = ' ';
            txBuffer[7] = hexArray[((localCurrentNow >> 12) & 0x0F)];
            txBuffer[8] = hexArray[((localCurrentNow >> 8) & 0x0F)];
            txBuffer[9] = hexArray[((localCurrentNow >> 4) & 0x0F)];
            txBuffer[10] = hexArray[localCurrentNow & 0x0F];
            txBuffer[11] = ' ';
            txBuffer[12] = hexArray[((localCurrentMax >> 12) & 0x0F)];
            txBuffer[13] = hexArray[((localCurrentMax >> 8) & 0x0F)];
            txBuffer[14] = hexArray[((localCurrentMax >> 4) & 0x0F)];
            txBuffer[15] = hexArray[localCurrentMax & 0x0F];
            txBuffer[16] = ' ';
            txBuffer[17] = hexArray[((localCurrentMin >> 12) & 0x0F)];
            txBuffer[18] = hexArray[((localCurrentMin >> 8) & 0x0F)];
            txBuffer[19] = hexArray[((localCurrentMin >> 4) & 0x0F)];
            txBuffer[20] = hexArray[localCurrentMin & 0x0F];
            txBuffer[21] = ' ';
            txBuffer[22] = hexArray[((localCurrentDif >> 12) & 0x0F)];
            txBuffer[23] = hexArray[((localCurrentDif >> 8) & 0x0F)];
            txBuffer[24] = hexArray[((localCurrentDif >> 4) & 0x0F)];
            txBuffer[25] = hexArray[localCurrentDif & 0x0F];
            txBuffer[26] = NULL;
            SendStringDebug(txBuffer);
*/
                
    }              
    return localCurrentDif;
}
// ****************************************************************************
// ************************** CurrentEstablishAverage *************************
// ****************************************************************************
// 
// This function will setablish average current draw prior to any injectors or fans turning on.
// 256 samples will be acquired and accumulated, then a 8 bit shift right occurs to perform
// the divide by 256.
//
void CurrentEstablishAverage(void)
{
    unsigned int localCurrentAccumulate;
    unsigned char localCnt;
    
    if (flagBoardType == BOARD_RU1)                                             // are we a RU1?
    {
        if (ADCIntStatus(ADC0_BASE, ADC_SAMPLE_SEQUENCE, true))			// Has a conversion finished
        {
            for(localCnt=0; localCnt < 255; localCnt++)
            {          
                ADCIntClear(ADC0_BASE, ADC_SAMPLE_SEQUENCE);			// Clear the ADC interrupt flag.
                ADCSequenceDataGet(ADC0_BASE, ADC_SAMPLE_SEQUENCE, globalADCVar);	// Read ADC Value.
                ADCProcessorTrigger(ADC0_BASE, ADC_SAMPLE_SEQUENCE);		// Trigger another ADC conversion

                localCurrentAccumulate = localCurrentAccumulate + globalADCVar[0];
                DelayMilliSecondsBlocking(10);
            }
            localCurrentAccumulate = localCurrentAccumulate >> 8;               // divide by 256
            globalCurrentAverage = localCurrentAccumulate;
        }
    }   
//    UARTCharPut(UART1_BASE,'A');
//    UARTCharPut(UART1_BASE,':');                                                // debug, display raw current ADC count
//    HexToDecimalStringInt(txBuffer,globalCurrentAverage); 
//    SendStringDebug(txBuffer);
}
// ****************************************************************************
// *************************** CU1DebugDisplayRU1Data *************************
// ****************************************************************************
// 
void CU1DebugDisplayRU1Data(void)
{     
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 controller?
    { 
/*
        txBuffer[0] =  'T';                                                     
        txBuffer[1] =  'N';
        txBuffer[2] =  'O';
        txBuffer[3] =  'W';
        txBuffer[4] =  ':';
        txBuffer[5] = globalTempNowStringCU1[0];
        txBuffer[6] = globalTempNowStringCU1[1];
        txBuffer[7] = globalTempNowStringCU1[2];
        txBuffer[8] =  ' ';
        txBuffer[9] =  'T';
        txBuffer[10] = 'M';
        txBuffer[11] = 'A';
        txBuffer[12] = 'X';
        txBuffer[13] = ':';
        txBuffer[14] = globalTempMaxStringCU1[0];
        txBuffer[15] = globalTempMaxStringCU1[1];
        txBuffer[16] = globalTempMaxStringCU1[2];
        txBuffer[17] =  NULL;
        SendStringDebug(txBuffer);    
*/
        SendStringDebug(globalCU1TemperatureString);
        SendStringDebug(globalCU1InjectionString);        
    }
}
// ****************************************************************************
// ************************* GPIOPortFIntHandler ******************************
// ****************************************************************************
// 
// This function is an interrupt.  It is for the physical switch tied to PF.1 that is SW3.  
// CU1 this switch is used for programming injector B
// RU1 this switch activate test of solenoids
//
// The function checks the mask to determine if appropriate switch was pressed & clears the int 
//
void GPIOPortFIntHandler(void)
{
    unsigned char localVar;
    
    localVar = GPIOIntStatus(PORT_F, 1);                                        // get masked interrupt status
    if (localVar & PIN_SWITCH3)
    {
        if (flagBoardType == BOARD_RU1)                                         // are we RU1 (remote)?
        {
//            GPIOPinWrite(PORT_B, PIN_LED2, 0);                                  // turn on injection LED (low asserted)  
            flagTestInjectorA = 1;                                              // set InjectorA flag
            flagTestInjectorB = 1;                                              // set InjectorB flag
        }
        else if (flagBoardType == BOARD_CU1)                                    // are we CU1 (master)?
        {
            // nothing to do here
        }
        GPIOIntClear(PORT_F, PIN_SWITCH3);                                                                                    
        flagSwitch3Pressed = TRUE;        
    }
}
// ****************************************************************************
// ************************* GPIOPortDIntHandler ******************************
// ****************************************************************************
// 
// This function is an interrupt.  SW1 and SW2 for physical switches tied to PD6 and PD7.
//
void GPIOPortDIntHandler(void)
{
    unsigned char localVar;
    
    localVar = GPIOIntStatus(PORT_D, 1);
    if (localVar & PIN_SWITCH1)
    {
        GPIOIntClear(PORT_D, PIN_SWITCH1);                                         
        flagSwitch1Pressed = TRUE;        
    }
    else if (localVar & PIN_SWITCH2)
    {
        GPIOIntClear(PORT_D, PIN_SWITCH2);                                         
        flagSwitch2Pressed = TRUE;       
    }  
}
// ****************************************************************************
// ************************** LoopTestResult **********************************
// ****************************************************************************
// 
//
void LoopTestResult(void)
{
//    UARTCharPut(UART1_BASE,'L');                                        // @@@ DEBUG RU1
//    UARTCharPut(UART1_BASE,flagLoopTestRU1+0x30);                       // @@@ DEBUG RU1
//    UARTCharPut(UART1_BASE,',');                                        // @@@ DEBUG RU1

    if (flagBoardType == BOARD_CU1)                                             // are we CU1 controller?
    {
        if(flagLoopTestCU1 == LOOP_TEST_CU1_PASS)
        {
            SendStringDebug("Loop OK");
            if (flagCU1ClearAfterError)                                         // only clear display once
            {
                LEDDualClear(); 
                flagCU1ClearAfterError = 0;                                     // clear flag 
                flagZoneChange = 1;                                             // update the display
            }
        }
        else if (flagLoopTestCU1 == LOOP_TEST_CU1_FAIL)
        {
            flagCU1ClearAfterError = 1;                                         // set flag to clear LED once
            SendStringDebug("Loop Broke");
            LEDDualHex(segArray[0x0E], SEGMENT_HIGH, DECIMAL_OFF);
            LEDDualHex(segArray[0x01], SEGMENT_LOW, DECIMAL_OFF);             
        }
    }
    else if (flagBoardType == BOARD_RU1)                                        // are we RU1 slave
    {
        if(flagLoopTestRU1 == LOOP_TEST_RU1_PASS)                               // set in the RS485 interrupt when a "[" has been received
        {
            SendStringDebug("RU1 RX OK");
            if (flagClearLEDOnceLoopTestRU1)                                    // only clear once
            {
                LEDDualClear();                                                 
                flagClearLEDOnceLoopTestRU1 = 0;                                // clear the flag
            }
        }
        else if (flagLoopTestRU1 == LOOP_TEST_RU1_FAIL)
        {
            SendStringDebug("RU1 RX Broke");
            LEDDualHex(segArray[0x0E], SEGMENT_HIGH, DECIMAL_OFF);
            LEDDualHex(segArray[0x01], SEGMENT_LOW, DECIMAL_OFF);             
            flagClearLEDOnceLoopTestRU1 = 1;                                    // set the flag to indicate a clear should happen next time this function is called
        }
        else if (flagLoopTestRU1 == LOOP_TEST_RU1_INVERT)                       // set in the RS485 interrupt when a received char is out of range may indicate swapped RS485 A/B lines
        {
            SendStringDebug("RU1 RX Invert");
            LEDDualHex(segArray[0x0E], SEGMENT_HIGH, DECIMAL_OFF);
            LEDDualHex(segArray[0x02], SEGMENT_LOW, DECIMAL_OFF);             
            flagClearLEDOnceLoopTestRU1 = 1;                                    // set the flag to indicate a clear should happen next time this function is called
        }
    }         
}
// ****************************************************************************
// ********************************* LoopTest *********************************
// ****************************************************************************
//
// Function called by rx of RS485 commands
// If we are a RU1 and a "[LOOPTEST?]" command was receieved, send a "[LOOPPASS*]" back to the CU1.
//
// 01234567890
// [LOOPTEST?]  received by RU1
// [LOOPPASS*]  received by CU1
//
void LoopTest(unsigned char localSource)
{
    // RS485 command received by RU1
    if(cmdBuffer[2]=='O' && cmdBuffer[3]=='O' && cmdBuffer[4]=='P' && cmdBuffer[5]=='T' && cmdBuffer[6]=='E' && cmdBuffer[7]=='S' && cmdBuffer[8]=='T' && cmdBuffer[9]=='?')
    {
        if (flagBoardType == BOARD_RU1)                                         // are we RU1 slave?
        {
            txBuffer[0] =  '[';
            txBuffer[1] =  'L';
            txBuffer[2] =  'O';
            txBuffer[3] =  'O';
            txBuffer[4] =  'P';
            txBuffer[5] =  'P';
            txBuffer[6] =  'A';
            txBuffer[7] =  'S';
            txBuffer[8] =  'S';
            txBuffer[9] =  '*';
            txBuffer[10] =  ']';
            txBuffer[11] =  NULL;            
            SendStringRS485NoChecking(txBuffer);   
            flagLoopTestRU1 = LOOP_TEST_RU1_PASS;                               // "LOOPTEST" received, clear the error flag  
        }
    }
    // RS485 command received by CU1
    else if(cmdBuffer[2]=='O' && cmdBuffer[3]=='O' && cmdBuffer[4]=='P' && cmdBuffer[5]=='P' && cmdBuffer[6]=='A' && cmdBuffer[7]=='S' && cmdBuffer[8]=='S' && cmdBuffer[9]=='*')
    {
        if (flagBoardType == BOARD_CU1)                                         // are we CU1 master?
        {
            flagLoopTestCU1 = LOOP_TEST_CU1_PASS;
        }
    }
}
// ****************************************************************************
// **************************** InitLoopTest **********************************
// ****************************************************************************
// 
// This function, if a CU1 sends a text string to the RU1 which will return "PASS".  
// A error flag is set and es expected to be cleared by sucesful completion of the loop.
//
void InitLoopTest(void)
{
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 controller?
    {
        flagLoopTestCU1 = LOOP_TEST_CU1_FAIL;                                   // set flag to fail and see if it gets cleared
        txBuffer[0] =  '[';
        txBuffer[1] =  'L';
        txBuffer[2] =  'O';
        txBuffer[3] =  'O';
        txBuffer[4] =  'P';
        txBuffer[5] =  'T';
        txBuffer[6] =  'E';
        txBuffer[7] =  'S';
        txBuffer[8] =  'T';
        txBuffer[9] =  '?';
        txBuffer[10] =  ']';
        txBuffer[11] =  NULL;            
        SendStringRS485NoChecking(txBuffer);                                      
    }
}

// ****************************************************************************
// *************************** RU1InitInjectorA ****************************
// ****************************************************************************
//
// This function turns on Injector A.  This is only called from a qualified RU1 portion
// of code.  It will be shut down in Main.
//
void RU1InitInjectorA(void)
{ 
    GPIOPinWrite(PORT_A, PIN_RU1_INJA, PIN_RU1_INJA);               // turn on AC optocoupler (high asserted)
    GPIOPinWrite(PORT_B, PIN_LED2, 0);                              // turn on injection LED (low asserted)    
} 
// ****************************************************************************
// *************************** RU1InitInjectorB ****************************
// ****************************************************************************
//
// This function turns on Injector B.  This is only called from a qualified RU1 portion
// of code.  It will be shut down in Main.
//
void RU1InitInjectorB(void)
{ 
    GPIOPinWrite(PORT_A, PIN_RU1_INJB, PIN_RU1_INJB);               // turn on AC optocoupler (high asserted)
    GPIOPinWrite(PORT_B, PIN_LED2, 0);                              // turn on injection LED (low asserted)                    
} 
// ****************************************************************************
// **************************** CommandProcessTest ****************************
// ****************************************************************************
//
// This function is the result of [TESTA] or [TESTB] being received from debug command, RS485 (if RU1) and 
// BLE (if CU1).
//
// If CU1 it will send command via RS485 to RU1 to activate injector A or injector B.
//
// The Injectors are activated in this function but are turned off in Main loop, run every second.
//
void CommandProcessTest(unsigned char localSource)
{
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
    {
        if(cmdBuffer[2]=='E' && cmdBuffer[3]=='S' && cmdBuffer[4]=='T')                              
        {
            if(cmdBuffer[5]=='A')                                               // turn on injector A on CU1
            {
                SendStringRS485NoChecking("[TESTA]");                           // send command via RS485 to the RU1            
                SendAck(localSource);                
            }
            else if(cmdBuffer[5]=='B')
            {
                SendStringRS485NoChecking("[TESTB]");                           // send command via RS485 to the RU1            
                SendAck(localSource);                                
            }
        }
    }
    else if (flagBoardType == BOARD_RU1)                                        // are we CU1 (controller)
    {
        if(cmdBuffer[2]=='E' && cmdBuffer[3]=='S' && cmdBuffer[4]=='T')                              
        {
            
            if(cmdBuffer[5]=='A')                                               // turn on injector A on CU1
            {
//                RU1InitInjectorA();  
//                UARTCharPut(UART1_BASE,'1');
                flagTestInjectorA = 1;
                globalInjectionTimeCountA = 0;
            }
            else if(cmdBuffer[5]=='B')
            {
//                RU1InitInjectorB();  
                flagTestInjectorB = 1;              
//                UARTCharPut(UART1_BASE,'2');
            }            
        }        
    }
}
// ****************************************************************************
// **************************** UART1IntHandler *******************************
// ****************************************************************************
//
// Debug Input
// This function is the debug serial interrupt. An input string is built that is exclusive 
// to this interrupt.
//
void UART1IntHandler(void)
{
    unsigned long ulStatus;  
        
    ulStatus = UARTIntStatus(UART1_BASE, true);                                 // Get the interrrupt status
    UARTIntClear(UART1_BASE, ulStatus);                                         // Clear the asserted interrupts.
    rxVarDebug = UARTCharGet(UART1_BASE);                                       // get the input data

//    UARTCharPut(UART1_BASE, hexArray[(rxVarDebug >> 4) & 0x000F]);                   // @@@ debug
//    UARTCharPut(UART1_BASE, hexArray[rxVarDebug & 0x000F]);
//    UARTCharPut(UART1_BASE,CR);
//    UARTCharPut(UART1_BASE,LF);
//    UARTCharPut(UART1_BASE,rxVarDebug);                                         // @@@ debug

//    UARTCharPut(UART1_BASE,rxVarBLE);                                         // @@@ debug
   
    if(rxVarDebug == '[')                                                       // check for start of string indicator
    {
        globalRxCntDebug = 0;                                                   // new string, reset the indexer into the array
        flagRxCmdInProcessDebug = TRUE;
    }
    else if(rxVarDebug == ']')
    {
        rxBufferDebug[globalRxCntDebug] = rxVarDebug;                
        rxBufferDebug[globalRxCntDebug+ 1] = NULL;                              // terminate string with a zero
        globalRxCntDebug = 0;
        flagRxCmdInProcessDebug = FALSE;
        ProcessSerialInput('D');                                                // process the complete string and indicate that it came from the debug 'D' port
    }

    rxBufferDebug[globalRxCntDebug] = rxVarDebug;                
    globalRxCntDebug++;

    if(globalRxCntDebug > RX_MAX_COMMAND_LENGTH)
    {
        globalRxCntDebug = 0;
        SendStringDebug("MAX RX LENGTH DEBUG");               
    }        
    globalSerialTimeoutDebug = 0;                                               // active data coming, reset timeout counter
} 
// ****************************************************************************
// **************************** UART5IntHandler *******************************
// ****************************************************************************
//
// RS485 Input
// This function is the RS485 serial interrupt. An input string is built that is exclusive 
// to this interrupt.
//
void UART5IntHandler(void)
{
    unsigned long ulStatus;  

    ulStatus = UARTIntStatus(UART5_BASE, true);                                 // Get the interrrupt status
    UARTIntClear(UART5_BASE, ulStatus);                                         // Clear the asserted interrupts.
    rxVar = UARTCharGet(UART5_BASE);

    if(rxVar == '[')                                                            // check for start of string indicator
    {
        globalRxCnt = 0;
        flagRxCmdInProcess = TRUE;
        if (flagBoardType == BOARD_RU1) globalRU1RXDropCnt = 0;                 // reset the count of how long RU1 can go without data
    }
    else if(rxVar == ']')
    {
        rxBuffer[globalRxCnt] = rxVar;                
        rxBuffer[globalRxCnt+1] = NULL;                                         // terminate string with a zero
        globalRxCnt = 0;
        flagRxCmdInProcess = FALSE;
        ProcessSerialInput('4');                                                // process the complete string and indicate that it came from the RS485 '4' port
    }  
    
    if ((rxVar >= '0' && rxVar <= ']') || rxVar == '*' || rxVar == ',' || rxVar == CR || rxVar == LF )  // check range of data
    {
        rxBuffer[globalRxCnt] = rxVar;                                                         
        globalRxCnt++;        
    }
    else                                                                        // if out of range, possible RS485 data polarity is swapped
    {
//        UARTCharPut(UART1_BASE,'<');
//        UARTCharPut(UART1_BASE, hexArray[(rxVar >> 4) & 0x000F]);
//        UARTCharPut(UART1_BASE, hexArray[rxVar & 0x000F]);
//        UARTCharPut(UART1_BASE,'>');
//        UARTCharPut(UART1_BASE,CR);
//        UARTCharPut(UART1_BASE,LF);
        
        flagLoopTestRU1 = LOOP_TEST_RU1_INVERT;  
    }
    if(globalRxCnt > RX_MAX_COMMAND_LENGTH)
    {
        globalRxCnt = 0;
        SendStringDebug("MAX RX LENGTH RS485");               
    }        
    globalSerialTimeout = 0;                                                    // active data coming, reset timeout counter
} 
// ****************************************************************************
// **************************** UART7IntHandler *******************************
// ****************************************************************************
//
// BLE Input
// This function is the BLE serial interrupt. An input string is built that is exclusive 
// to this interrupt.
//

void UART7IntHandler(void)
{
    unsigned long ulStatus;  
    
    ulStatus = UARTIntStatus(UART7_BASE, true);                                 // Get the interrrupt status
    UARTIntClear(UART7_BASE, ulStatus);                                         // Clear the asserted interrupts.
    globalPrePreRxVarBLE = globalPreRxVarBLE;                                   // store rx char 2 deep
    globalPreRxVarBLE = rxVarBLE;                                               // store rx char 1 deep
    rxVarBLE = UARTCharGet(UART7_BASE);

//    UARTCharPut(UART1_BASE,rxVarBLE);                                         // @@@ debug
    
    // Build a dedicated string of BLE data when we are in command mode
    // If the characters "MD>" or "END" have been received the input string is saved and a flag is set
    rxBufferBLECmd[rxBufferBLECmdCnt] = rxVarBLE;
    rxBufferBLECmdCnt++;        
    if((globalPrePreRxVarBLE == 'M' && globalPreRxVarBLE == 'D' && rxVarBLE == '>') || (globalPrePreRxVarBLE == 'E' && globalPreRxVarBLE == 'N' && rxVarBLE == 'D'))
    {        
        rxBufferBLECmd[rxBufferBLECmdCnt] = NULL;                               // terminate the string
        rxBufferBLECmdCnt = 0;                                                  // reset the indexer
        flagBLECmdResponse = 1;                                                 // flag that complete string is ready
    }
    
    // normal BLE input    
    if(rxVarBLE == '[')                                                         // check for start of string
    {
        globalRxCntBLE = 0;
        flagRxCmdInProcessBLE = TRUE;                                           // start of string detected, set the flag to start building a string
    }
    else if(rxVarBLE == ']')
    {
        rxBufferBLE[globalRxCntBLE] = rxVarBLE;                
        globalRxCntBLE++;
        rxBufferBLE[globalRxCntBLE] = NULL;                                     // terminate string with a zero
        globalRxCntBLE = 0;                                                     // zero the counter and get ready for next string input
        flagRxCmdInProcessBLE = FALSE;
        ProcessSerialInput('B');                                                // process the complete string and indicate that it came from the BLE 'B' port                                            
    }
    
    if (flagRxCmdInProcessBLE == TRUE)                                          // we are building string, save it and inc the counter
    {
        rxBufferBLE[globalRxCntBLE] = rxVarBLE;                
        globalRxCntBLE++;
    }
    
    if(globalRxCntBLE > RX_MAX_COMMAND_LENGTH)
    {
        globalRxCntBLE = 0;
        SendStringDebug("MAX RX LENGTH BLE");               
    }
    
    globalSerialTimeoutBLE = 0;                                                 // active data coming, reset timeout counter          
}  
// ****************************************************************************
// ****************************** CheckForTimeouts ****************************
// ****************************************************************************
//
// This function is called every second and increments a counter if a serial 
// command is in process.  It also checks if a predetermined count is matched and if
// it is exceeded, the serial stream process is restarted.
//
void CheckForTimeouts(void)
{
    // RS485 serial input timeout stuff
    if (flagRxCmdInProcess == TRUE)                                             // RS485 serial stream is in process, INC counter
    {    
        globalSerialTimeout++;
    }
    if(globalSerialTimeout > SERIAL_DECODE_TIMEOUT)                             // if timeout is exceeded, reset processing of string to beginning
    {
        globalSerialTimeout = 0;                                                // reset the timeout counter
        flagRxCmdInProcess = FALSE;
        globalRxCnt= 0;                                                         // reset the indexer into the string
        SendStringDebug("TIMEOUT RS485");
    }   

    // BLE serial input timeout stuff
    if (flagRxCmdInProcessBLE == TRUE)                                          // BLE serial stream is in process, INC counter
    {    
        globalSerialTimeoutBLE++;
    }
    if(globalSerialTimeoutBLE > SERIAL_DECODE_TIMEOUT)                          // if timeout is exceeded, reset processing of string to beginning
    {
        globalSerialTimeoutBLE = 0;                                             // reset the timeout counter
        flagRxCmdInProcessBLE = FALSE;
        globalRxCntBLE= 0;                                                      // reset the indexer into the string
        SendStringDebug("TIMEOUT BLE");
    }   

    // Debug serial input timeout stuff
    if (flagRxCmdInProcessDebug == TRUE)                                        // Debug serial stream is in process, INC counter
    {    
        globalSerialTimeoutDebug++;
    }
    if(globalSerialTimeoutDebug > SERIAL_DECODE_TIMEOUT)                        // if timeout is exceeded, reset processing of string to beginning
    {
        globalSerialTimeoutDebug = 0;                                           // reset the timeout counter
        flagRxCmdInProcessDebug = FALSE;
        globalRxCntDebug = 0;                                                   // reset the indexer into the string
        SendStringDebug("TIMEOUT DEBUG");
    }      
}
// ****************************************************************************
// ******************************* RU1FanControl ******************************
// ****************************************************************************
//
// This function will turn on the 24VDC fan (RU1 only) when a defined temperature
// has been reached.  Will also turn on if flagRU1FanOverride = 1.
//
void RU1FanControl(void)
{
    if (flagBoardType == BOARD_RU1)                                             // are we a RU1?
    {
        if ((globalTempNowInputRU1 > DEFAULT_FAN_TRIGGER) || flagRU1FanOverride)// temperature exceeded, turn on fan
        {
            FAST_GPIOPinWrite(PORT_B, PIN_FAN_CTRL,0);                          // CLR is fan on.
        }
        else
        {
            if (flagRU1FanOverride)                                             // if flag is set do not let fan turn off
            {
            }
            else
            {
                FAST_GPIOPinWrite(PORT_B, PIN_FAN_CTRL,PIN_FAN_CTRL);           // SET is fan off.
            }
        }
    }
}  
// ****************************************************************************
// ******************************* LEDDualHex *********************************
// ****************************************************************************
//
// This function will write to a single display of the dual display specified by recHighLow.
// Data must be cconverted to 7 segment prior to calling this function
// This will display hex values
// If a number 0 1 2 3 4 5 6 7 8 9 A B C D E F BLANK (0 - 16).
// Data is split between 2 ports, PC7-PC4 and PD3-PD0
//
void LEDDualHex(unsigned char recData, unsigned recHighLow, unsigned recDecimal)
{
    unsigned char localPC;
    unsigned char localPD;
       
    localPC = recData << 4;                                                     // DCBA #### <- ### #DCBA
    localPC = ~localPC & 0xF0;                                                  // mask low nibble
    localPD = recData >> 4;                                                     // #GFE #### -> #### #GFE
    localPD = ~localPD & 0x07;                                                  // mask high nibble and decimal point
   
    if (recDecimal == DECIMAL_ON)
    {
        localPD = (localPD & 0xF7);                                             // 1111 0111 low so decimal is turned on
    }
    else if (recDecimal == DECIMAL_OFF)
    {
        localPD = (localPD | 0x08);                                             // 0000 1000 high so decimal is turned off
    }

    GPIOPinWrite(PORT_C, 0xF0, localPC);							                    
    GPIOPinWrite(PORT_D, 0x0F, localPD);

    if (recHighLow == SEGMENT_HIGH)
    {
        LEDClockTens();    
    }
    else if (recHighLow == SEGMENT_LOW)
    {
        LEDClockOnes();    
    }   
}
// ****************************************************************************
// ****************************** LEDClockTens ********************************
// ****************************************************************************
//
void LEDClockTens(void)
{
    GPIOPinWrite(PORT_F, PIN_TENSCLK, PIN_TENSCLK);                             // Tens digit, clock the 574
    GPIOPinWrite(PORT_F, PIN_TENSCLK, 0);
    GPIOPinWrite(PORT_F, PIN_TENSCLK, PIN_TENSCLK);                     
}
// ****************************************************************************
// ****************************** LEDClockOnes ********************************
// ****************************************************************************
//
void LEDClockOnes(void)
{
    GPIOPinWrite(PORT_F, PIN_ONESCLK, PIN_ONESCLK);                             // Lows digit, clock the 574
    GPIOPinWrite(PORT_F, PIN_ONESCLK, 0);
    GPIOPinWrite(PORT_F, PIN_ONESCLK, PIN_ONESCLK);     
}
// ****************************************************************************
// ****************************** LEDDualClear ********************************
// ****************************************************************************
//
// This function clears the dual display.
// Data is split between 2 ports, PC7-PC4 and PD3-PD0.
//
void LEDDualClear(void)
{
    GPIOPinWrite(PORT_C, 0xF0, 0xFF);	                                        // all segments off						                    
    GPIOPinWrite(PORT_D, 0x0F, 0xFF);                                           // all segments off
    LEDClockTens();    
    LEDClockOnes();    
}
// ****************************************************************************
// ******************************** LEDDualDecimal ****************************
// ****************************************************************************
//
// This function receives a number and converts it to decimal and is displayed over two seven segment displays. 
// This function does not display hex values.
// Data is split between 2 ports, PC7-PC4 and PD3-PD0
//
// recDecimal can be: DECIMAL_ONES_ON, DECIMAL_ONES_OFF, DECIMAL_TENS_ON, DECIMAL_TENS_OFF, DECIMAL_BOTH_OFF, DECIMAL_BYPASS
// 
//            UARTCharPut(UART1_BASE,'*');       
void LEDDualDecimal(unsigned char recData, unsigned recDecimal)
{
    unsigned char localPC;
    unsigned char localPD;
    unsigned char localTens;
    unsigned char localOnes;

    // set/clear global flags that track the decimal points
    if (recDecimal == DECIMAL_ONES_ON)
    {
        flagDecimalOnes = 1;
    }
    else if (recDecimal == DECIMAL_ONES_OFF) 
    {
        flagDecimalOnes = 0;
    }
    else if (recDecimal == DECIMAL_TENS_ON) 
    {
        flagDecimalTens = 1;
    }
    else if (recDecimal == DECIMAL_TENS_OFF) 
    {
        flagDecimalTens = 0;
    }
    else if (recDecimal == DECIMAL_BOTH_ON) 
    {
        flagDecimalOnes = 1;
        flagDecimalTens = 1;
    }
    else if (recDecimal == DECIMAL_BOTH_OFF) 
    {
        flagDecimalOnes = 0;
        flagDecimalTens = 0;
    }
    else if (recDecimal == DECIMAL_BYPASS)
    {
        // do not adjust flags
    }
    
    if(recData > 9)                                                             // numbers greater than nine
    {
        localTens = recData/10;                                                 // how many tens?
        localOnes = recData - (localTens*10);                                   // how many ones?   
       
        localOnes = segArray[localOnes];                                        // convert ones to 7seg mapping
        localTens = segArray[localTens];                                        // convert tens to 7seg mapping
        
        // LED write is split between 2 ports
        localPC = localOnes << 4;                                               // DCBA #### <- ### #DCBA
        localPC = ~localPC & 0xF0;                                              // mask low nibble
        localPD = localOnes >> 4;                                               // #GFE #### -> #### #GFE
        localPD = ~localPD & 0x0F;                                              // mask high nibble and decimal point

        // ONES
        // decimal point on ones display
        if (flagDecimalOnes)
        {
            localPD = (localPD & 0xF7);                                         // 1111 0111 low so decimal is turned on
        }
        else
        {
            localPD = (localPD | 0x08);                                         // 0000 1000 high so decimal is turned off
        }

        GPIOPinWrite(PORT_C, 0xF0, localPC);					// write data to 574 latch inputs		                    
        GPIOPinWrite(PORT_D, 0x0F, localPD);    
        LEDClockOnes();    

        localPC = localTens << 4;                                               // DCBA #### <- ### #DCBA
        localPC = ~localPC & 0xF0;                                              // mask low nibble
        localPD = localTens >> 4;                                               // #GFE #### -> #### #GFE
        localPD = ~localPD & 0x0F;                                              // mask high nibble and decimal point

        // TENS
        // decimal point on tens display
        if (flagDecimalTens)
        {
            localPD = (localPD & 0xF7);                                         // 1111 0111 low so decimal is turned on
        }
        else
        {
            localPD = (localPD | 0x08);                                         // 0000 1000 high so decimal is turned off
        }

        GPIOPinWrite(PORT_C, 0xF0, localPC);							                    
        GPIOPinWrite(PORT_D, 0x0F, localPD);
        LEDClockTens();    
    }  
    else                                                                        // number smaller than 10
    {
        recData = segArray[recData];
        localPC = recData << 4;                                                 // DCBA #### <- ### #DCBA
        localPC = ~localPC & 0xF0;                                              // mask low nibble
        localPD = recData >> 4;                                                 // #GFE #### -> #### #GFE
        localPD = ~localPD & 0x0F;                                              // mask high nibble and decimal point
        
        // ONES
        // decimal point on ones display
        if (flagDecimalOnes)
        {
            localPD = (localPD & 0xF7);                                         // 1111 0111 low so decimal is turned on
        }
        else
        {
            localPD = (localPD | 0x08);                                         // 0000 1000 high so decimal is turned off
        }

        GPIOPinWrite(PORT_C, 0xF0, localPC);	                                // write ones data						                    
        GPIOPinWrite(PORT_D, 0x0F, localPD);
        LEDClockOnes();    

        // TENS
        localPC = 0xFF;                                                         // all segments off on tens display
        localPD = 0xFF;                                                         // all segments off on tens display        
        // decimal point on tens display
        if (flagDecimalTens)
        {
            localPD = (localPD & 0xF7);                                         // 1111 0111 low so decimal is turned on
//            UARTCharPut(UART1_BASE,'A');         
        }
        else
        {
            localPD = (localPD | 0x08);                                         // 0000 1000 high so decimal is turned off
//            UARTCharPut(UART1_BASE,'a');         
        }        
        GPIOPinWrite(PORT_C, 0xF0, localPC);                                    // get data on ports	                                						                    
        GPIOPinWrite(PORT_D, 0x0F, localPD);                                       
        LEDClockTens();    
    }      
}
// ****************************************************************************
// ************************ CalculateTemperature ******************************
// ****************************************************************************
//
// This function receives an ADC count and then steps through and array of values and attempts
// to find a match.  If a match is not found it will round up or down, if not in range 0xFE (254)
// is returned.
//
// "indexer" is the returned temperature in C
// ADC count, high ADC count = low temperature, low ADC count = high temperature
//
// Example: 
// indexer 00 = 3002
// indexer 24 = 2087
// indexer 25 = 2048
// indexer 26 = 2010
// indever 91 =  453
//
// for ADC cnt = 2020 temperature is slightly hotter than 25C
// for indexer = 25
// localDif = 2048 - 2010 = 38
// localLowWindow = 2048 - 38/2 = 2029
//
unsigned char CalculateTemperature(unsigned int localADCCnt)
{
    unsigned char indexer;
    unsigned char returnTemp;                                                   // returned temperature from this function
    unsigned int localDif;                                                      // difference between current lookup entry and next entry
    unsigned int localLowWindow;                                                // half way point
    unsigned char localStop;
  
    indexer = 0;
    localStop = 1;                                                              

    if(localADCCnt > tempLookup[0])                                             // too cold, ADC count is large
    {
                returnTemp = 0xFC;                                              // load error
//                UARTCharPut(UART1_BASE,'C');       
    }
    else if (localADCCnt < tempLookup[91])                                      // too hot, ADC count is small
    {
                returnTemp = 0xFD;                                              // load error
//                UARTCharPut(UART1_BASE,'H');      
    }
    else
    {
        while (localStop)                                                           // loop till localStop cleared
        {
            localDif = tempLookup[indexer] - tempLookup[indexer+1];                 
            localLowWindow = (tempLookup[indexer] - (localDif/2));  
           
            if(localADCCnt == tempLookup[indexer])                                  // ADC count matches EXACTLY lookup table
            {
//                UARTCharPut(UART1_BASE,'=');
                returnTemp = indexer;
                localStop = 0;
            }
            else if(localADCCnt < tempLookup[indexer] && localADCCnt >= localLowWindow) // ADC count is lower (hotter), round down
            {
//                UARTCharPut(UART1_BASE,'<');
                returnTemp = indexer;      
                localStop = 0;
            }
            else if(localADCCnt < localLowWindow && localADCCnt > tempLookup[indexer+1])// round up
            {
//                UARTCharPut(UART1_BASE,'>');
                returnTemp = indexer+1;      
                localStop = 0;
            }  
            indexer++;
            if (indexer > 90)
            {    
//                UARTCharPut(UART1_BASE,'?');                                        // match not found
                returnTemp = 0xFE;                                                  // load error value for temperature
                localStop = 0;            
            }
        }        
    }
    return returnTemp;
}
// ****************************************************************************
// ****************************** RU1GetADCValue ******************************
// ****************************************************************************
//
//  The RU1 has added:
//      - NTC (temperature measurement) on PIN58, PB4, AIN10
//      - Current sensor on PIN7, PE1, AIN1
//
void RU1GetADCValue(void)
{
  
//    unsigned int localCurrentNow;

    if (flagBoardType == BOARD_RU1)                                             // are we a RU1?
    {
        if (ADCIntStatus(ADC0_BASE, ADC_SAMPLE_SEQUENCE, true))			// Has a conversion finished
        {
            ADCIntClear(ADC0_BASE, ADC_SAMPLE_SEQUENCE);			// Clear the ADC interrupt flag.
            ADCSequenceDataGet(ADC0_BASE, ADC_SAMPLE_SEQUENCE, globalADCVar);	// Read ADC Value.
            ADCProcessorTrigger(ADC0_BASE, ADC_SAMPLE_SEQUENCE);		// Trigger another ADC conversion

//            localCurrentNow = globalADCVar[0];                                  // get current 
            globalTempNowInputRU1 = CalculateTemperature(globalADCVar[1]);      // get temperature

         }
    }
} 
// ****************************************************************************
// *********************** RU1ADCConvertCntToTemperature **********************
// ****************************************************************************
// 
void RU1ADCConvertCntToTemperature()
{
    if (flagBoardType == BOARD_RU1)                                             // are we a RU1?
    {
//            UARTCharPut(UART1_BASE,'A');                                        // debug, display raw temperature ADC count
//            UARTCharPut(UART1_BASE,'T');
//            HexToDecimalStringInt(txBuffer,globalADCVar[1]); 
//            SendStringDebug(txBuffer);


//            UARTCharPut(UART1_BASE,'T');
//            UARTCharPut(UART1_BASE,'e');
//            UARTCharPut(UART1_BASE,'m');
//            UARTCharPut(UART1_BASE,'p');
//            HexToDecimalString(txBuffer,globalTempNowInputRU1);
//            SendStringDebug(txBuffer);
        
        if (globalTempNowInputRU1 < 90)                                     // is current temp within in range?
        {
            if (globalTempNowInputRU1 > globalTempMaxInputRU1)              // is current temp greater than max?
            {
                globalTempMaxInputRU1 = globalTempNowInputRU1;
                I2CWriteEEPROMSingle(EA_MAXTEMP,globalTempMaxInputRU1,I2C_PHYSICAL_EEPROM);
            }          
        }
//            UARTCharPut(UART1_BASE,'M');
//            UARTCharPut(UART1_BASE,'a');
//            UARTCharPut(UART1_BASE,'x');
//            UARTCharPut(UART1_BASE,' ');
//            HexToDecimalString(txBuffer,globalTempMaxInputRU1);
//            SendStringDebug(txBuffer);            
    }    
}
// ****************************************************************************
// ************************** BLECmdModeSequence ******************************
// ****************************************************************************
//
// This function communicates with BLE module in command mode.  It does the following stuff:
// - Enter command mode
// - Get BLE firmware version
// - Dumps data to get:
// - BLE module name
// - BLE module serial number (BTA)
// - Change name to FertiWiser if not already set
// - exit command mode
//
void BLECmdModeSequence(void)
{

    int stringIndex;                                                            // 0 to positive indicates where match occured, negative if no match
    unsigned char flagProceed = 0;
    unsigned char test;
    
    if (flagBoardType == BOARD_CU1)                                             // are we a CU1?
    {
        DelaySecondsBlocking(1);
 
        // *** enter command mode ***
        flagProceed = BLEEnterCommandMode();
        // *** get firmware version ***
        if (flagProceed)
        {
            UARTCharPut(UART7_BASE,'V');
            UARTCharPut(UART7_BASE,CR);
            flagProceed = BLEWaitForCmdResponseBlocking();           
            flagProceed = BLEExtractVersion();            
        }
        // *** dump data ***
        if (flagProceed)
        {
            UARTCharPut(UART7_BASE,'D');
            UARTCharPut(UART7_BASE,CR);
            flagProceed = BLEWaitForCmdResponseBlocking();           
            flagProceed = BLEExtractNameFromDump();
            flagProceed = BLEExtractBTAFromDump();           
        } 
        // *** check for proper BLE name ***
        if (flagProceed)
        {        
            test = DJPStringCompare(bleModuleNameString,1,"FertiWiser",1,10);   
            if(test == 0)                                                       // 0=match, I know this is goofy return
            {
//                SendStringDebug("Name Match");               
            }
            else
            {
//                SendStringDebug("Name Wrong");
                SendStringBLE("S-,FertiWiser");                                 // rename the BLE Module 
                flagProceed =  BLEWaitForCmdResponseBlocking();           
                stringIndex = DJPStringSearch(rxBufferBLECmd,"AOK");            // command accepted?
                if (stringIndex >=0)                                            // "AOK" was rx, lets check it
                {
//                    SendStringDebug("Name Change AOK");
                    UARTCharPut(UART7_BASE,'D');
                    UARTCharPut(UART7_BASE,CR);
                    flagProceed = BLEWaitForCmdResponseBlocking();           
                    flagProceed = BLEExtractNameFromDump();
                }
            }
        }
        // *** exit command mode ***
        if (flagProceed)
        {
            flagProceed = BLEExitCommandMode();   
        }
    }
}
// ****************************************************************************
// **************************** BLEExitCommandMode ***************************
// ****************************************************************************
//
// This function exits the BLE command by sending "---CR".
// Will return 1 if succesful, 0 if not.
//
unsigned char BLEExitCommandMode(void)
{
    unsigned char returnVar;
    int stringIndex;
    
    UARTCharPut(UART7_BASE,'-');
    UARTCharPut(UART7_BASE,'-');
    UARTCharPut(UART7_BASE,'-');
    UARTCharPut(UART7_BASE,CR);
    
    BLEWaitForCmdResponseBlocking();           
    stringIndex = DJPStringSearch(rxBufferBLECmd,"END");                        // search for "END" to see if we are in command mode
    if (stringIndex >= 0)                                                       // command entry worked, negative if no match
    {
        returnVar = 1;
//        SendStringDebug("Data Mode");            
    }
    else
    {
        returnVar = 0;
    } 
    return returnVar;
}

// ****************************************************************************
// **************************** BLEEnterCommandMode ***************************
// ****************************************************************************
//
// This function enters the BLE command by sending $$$ (with no CR).
// Will return 1 if succesful, 0 if not.
//
unsigned char BLEEnterCommandMode(void)
{
    unsigned char returnVar;
    int stringIndex;
    
    UARTCharPut(UART7_BASE,'$');
    UARTCharPut(UART7_BASE,'$');
    UARTCharPut(UART7_BASE,'$');                
    BLEWaitForCmdResponseBlocking();           
    stringIndex = DJPStringSearch(rxBufferBLECmd,"CMD>");                   // search for "CMD>" to see if we are in command mode
    if (stringIndex >= 0)                                                   // command entry worked, negative if no match
    {
        returnVar = 1;
//        SendStringDebug("Command Mode");            
    }
    else
    {
        returnVar = 0;
    }   
    return returnVar;
}
// ****************************************************************************
// *********************** BLEWaitForCmdResponseBlocking **********************
// ****************************************************************************
//
// This function waits for a response from the BLE module.  This function will be typically
// called after the Tiva sends a command to the BLE module.  It will wait for a flag
// to be set that indicates a string was formed from the BLE module that was terminated
// by "CMD>".  The flag is set in an interrupt
//
// It will timeout after 2 seconds and return 0 if exit by timeout or 1 if exit by 
// flag.
//
unsigned char BLEWaitForCmdResponseBlocking(void)
{
    unsigned char localTimeout = 0;
    unsigned char returnVar;
    unsigned char exitWhile = 1;
    
    while(exitWhile)
    {
        DelayMilliSecondsBlocking(100);
        localTimeout++;
        if(flagBLECmdResponse==1) exitWhile = 0;
        if(localTimeout > 10) exitWhile = 0;        
    }
    
    if (flagBLECmdResponse == 1)
    {
        flagBLECmdResponse = 0; 
        returnVar = 1;  
    }
    else
    {
        returnVar = 0;
    }
    
    return returnVar;
}
// ****************************************************************************
// ***************************** BLEExtractVersion ****************************
// ****************************************************************************
//
// This function will extract the BLE module firmware version from the "V" command string when
// in command mode.  Depending on the BLE fimrware, the module sometimes reports name differently.
//
// RN4870 V1.xx        As shipped from factory
// RN4871 V1.xx        After 1.41 firmware update
//
// If succesful, the function returns 1, 0 if not.  The verion is stored in global string.
//
// Version is 0-4 chars, V1.41 bleFirmwareVersion[6]
// 01234
// V1.41 
//
unsigned char BLEExtractVersion(void)
{
    unsigned char stringIndex;
    unsigned char returnVar;
    
    stringIndex = DJPStringSearch(rxBufferBLECmd,"RN487");                      // search for version
    if (stringIndex)                                                            // version command worked
    {
        bleFirmwareVersion[0] = rxBufferBLECmd[stringIndex+7];                  // V
        bleFirmwareVersion[1] = rxBufferBLECmd[stringIndex+8];                  // 1
        bleFirmwareVersion[2] = rxBufferBLECmd[stringIndex+9];                  // .
        bleFirmwareVersion[3] = rxBufferBLECmd[stringIndex+10];                 // 4
        bleFirmwareVersion[4] = rxBufferBLECmd[stringIndex+11];                 // 1
        returnVar = 1;
    }
    else
    {
        returnVar = 0;
    }
    bleFirmwareVersion[5] = NULL;
//    SendStringDebug(bleFirmwareVersion);            
    return returnVar;
}
// ****************************************************************************
// ************************ BLEExtractBTAFromDump ****************************
// ****************************************************************************
//
// This function will extract the BLE BTA "Blue Tooth Address" from the "D" dump string when
// in command mode.
//
// BTA can only be 0-11 characters, bleModuleBTAString[13]
//
//     000000000011
//     012345678901
// BTA=049162A6E6C6
//
unsigned char BLEExtractBTAFromDump(void)
{
    int startIndex;                                                             // start index
    int newIndex = 0;                                                           // index for new string
    unsigned char returnVar;
    
    startIndex = DJPStringSearch(rxBufferBLECmd,"BTA=");                        // search for "BTA="
    if (startIndex >= 0)                                                        // "BTA=" found
    {    
        startIndex += 4;                                                        // get past "BTA=" 
        
        for (newIndex=0; newIndex<12; newIndex++)
        {
            bleModuleBTAString[newIndex] = rxBufferBLECmd[startIndex];
            startIndex++;
        }
        returnVar = 1;
    }
    else
    {
        returnVar = 0;
    }
    bleModuleBTAString[12] = NULL;                                              // safety sake, make sure it is terminated
//    SendStringDebug(bleModuleBTAString);                
    return returnVar;    
}
// ****************************************************************************
// ************************ BLEExtractNameFromDump ****************************
// ****************************************************************************
//
// This function will extract the BLE module name from the "D" dump string when
// in command mode.
//
// Name=RN4870-xxxx         As shipped from factory
// Name=RN4871_xxxx         After 1.41 firmware update (this is real name of module)
// Name=FertiWiser_xxxx     Desired name
// Name=FertiWiser-E_xxxx   When in expansion mode
//
// The BLE module auto appends the "_xxxx" with the "xxxx" being the last two bytes of the BTA (Blue Tooth Address)
// 
// name can only be 0-15 characters, bleModuleNameString[17];
//
unsigned char BLEExtractNameFromDump(void)
{
    int startIndex;                                                             // start index
    int newIndex = 0;                                                           // index for new string
    unsigned char testChar;
    unsigned char searchFlag = 1;
    unsigned char returnVar;
    
    startIndex = DJPStringSearch(rxBufferBLECmd,"Name=");                       // search for name
    if (startIndex >= 0)                                                        // "Name=" found
    {    
        startIndex += 5;                                                        // get past "Name="    
        while(searchFlag)
        {            
            testChar = rxBufferBLECmd[startIndex];
//            if (testChar == '_' || testChar == '-')
            if (testChar == '_')
            {
                searchFlag = 0;                                                 // stop search, end of name found
                bleModuleNameString[newIndex] = NULL;                           // terminate string                
            }
            else
            {
                bleModuleNameString[newIndex] = testChar;
            }
            newIndex++;
            startIndex++;
            if(newIndex > 15) searchFlag = 0;                                   // stop search, name too large            
        }
        returnVar = 1;
    }
    else
    {
        returnVar = 0;
    }
    bleModuleNameString[16] = NULL;
//    SendStringDebug(bleModuleNameString);                
    return returnVar;    
}
// ****************************************************************************
// **************************** DJPStringSearch *******************************
// ****************************************************************************
//
// This function will search the mainString passed to it for a subString that is 
// also passed.  The start location (zero based) is returned. If no match is found
// negative number is returned (-1).
//
// 00000000001111111111
// 01234567890123456789
// quick fat fox jumps
//           fox
//
// Return is decimal 10
//
int DJPStringSearch(unsigned char *mainString, unsigned char *subString)
{   
    unsigned int msi = 0;                                                       // main string index
    unsigned int ssi = 0;                                                       // sub string index
    unsigned char matchFlag;
    unsigned char returnVar;
    unsigned int msLength;                                                      // main string length
    unsigned int ssLength;                                                      // sub string length
    unsigned int matchLength = 0;
    
    msLength = DJPFindStringLength(mainString);                                 // get string length
    ssLength = DJPFindStringLength(subString);                                  // get string length
       
    for(msi = 0; msi<msLength; msi++)                                           // start indexing through mainString
    {      
        if (mainString[msi] == subString[ssi])                                  // a single (first) letter match has been found
        {
            for(ssi = 1; ssi<ssLength; ssi++)                                   // start indexing through subString, but start at index 1 because match at index 0 brought us here
            {
                msi++;
                if (mainString[msi] == subString[ssi])                          // if first letter matched, resume looking at rest
                {
                    matchFlag = 1;
                    matchLength++;
                }
                else                                                            // not a full match
                {
                    matchLength = 0;
                    matchFlag = 0;
                    ssi = 0;
                    break;                                                      // break out of subString search
                }            
            }
            if(matchLength == ssLength-1)                                       // break out of mainString "for loop" search, no need to keep searching
            {
                break;
            }
        }       
    }
    if (matchFlag == 1)
    {
        returnVar = (msi - (ssLength-1));                                       // calculate first character match location
    }
    else
    {
        returnVar = -1;                                                         // no match, indicate by negative number
    }
    return returnVar;   
}
// ****************************************************************************
// ********************************* BLEReset *********************************
// ****************************************************************************
//
// Reset the BLE module.  Reset is low asserted.  Datasheet specifies 63nS minimum reset time.
//
void BLEReset(void)
{
    if (flagBoardType == BOARD_CU1)                                                 // are we a CU1?
    {
        GPIOPinWrite(PORT_E, PIN_BLE_RSTN,  PIN_BLE_RSTN);
        DelayMilliSecondsBlocking(1);            
        GPIOPinWrite(PORT_E, PIN_BLE_RSTN,  0);
        DelayMilliSecondsBlocking(1);            
        GPIOPinWrite(PORT_E, PIN_BLE_RSTN,  PIN_BLE_RSTN);   
    }
}
// ****************************************************************************
// *************************** FlowControlMonitor *****************************
// ****************************************************************************
// 
// This function monitors connection to pin PF1 which is flow control if RU1. This pin
// timer 0, section B.  It is a countdown event timer which is preloaded with 10K
// every second and the difference is calculated every 5 seconds.
//
// Instead of looking at the counter every second, it was switched to every 5 seconds to increase resolution.
// We will also change to calculating gallons per hour.
// 
// Flow with FS300A-G3.4" is supposed to be:
// Frequency = Flow*5.5 (flow in liters per minute) or 
// 1 liter = 0.264 gallons
// 5.5/0.264 = 20.83
// Flow(liters per minute) = Freq/5.5
// Flow(gallons per minute) = Freq/20.83
//
// 1Hz = 0.04 gallons minute
// 10Hz = 28 gallons minute
// 100Hz = 288 gallons minute
// 
void FlowControlMonitor(void)
{
    if (flagBoardType == BOARD_RU1)                                                 // are we a RU1 (remote)?
    {
        globalFlowInputRU1 = TimerValueGet(TIMER0_BASE, TIMER_B);        
        TimerLoadSet(TIMER0_BASE, TIMER_B, 10000);
        globalFlowInputRU1 = 10000 - globalFlowInputRU1;                                  // find diference between preload and decremented value

        globalFlowInputRU1 *= 12;                                                      // convert to minutes
        globalFlowInputRU1 *= 60;                                                      // convert to hours
        globalFlowInputRU1 = (unsigned int)floor(globalFlowInputRU1/(20.83*60));          // calculate gallons per hour
                
//        HexToDecimalStringIntWithPad(stringBuffer,globalFlowInputRU1);           // convert latest flow data to ascii          
//        StringCopy(txBuffer,"RU1 Flow: ");                                      // format string to send back to CU1
//        StringCat(txBuffer,stringBuffer);
//        SendStringDebug(txBuffer);     
    }  
}
// ****************************************************************************
// ************************ SolenoidSafetyCheck ************************
// ****************************************************************************
// 
// This function is a safety check to help avoid stuck solenoids.  This function must be 
// called every second to trigger a hardware one-shot to keep it active.  This hardware
// one-shot drives a transistor which is a sink to all of the optocouplers.
//
void SolenoidSafetyCheck(void)
{
    if (flagBoardType == BOARD_RU1)                                                 // are we a RU1 (remote)?
    {
        GPIOPinWrite(PORT_A, PIN_RU1_SAFETY, ~GPIOPinRead(PORT_A, PIN_RU1_SAFETY)); // toggle the safety pin
    }   
}
// ****************************************************************************
// ************************ GlobalActiveTimeCountCheck ************************
// ****************************************************************************
// 
// Idea of this function is that a timeout counter is incremented every 0.1seconds (that 
// is how often this function should be called).  If 10 seconds elapses without an external
// reset of the globalActiveTimeCount, the active zone will be turned off.  The count
// is reset in the serial input processor function: CommandProcessSerialZoneInput().
//
void GlobalActiveTimeCountCheck(void)
{
    if (flagBoardType == BOARD_RU1)                                             // are we a RU1 (remote)?
    {
        globalActiveTimeCount++;                                        
        if (globalActiveTimeCount > ZONE_INACTIVE_FREQUENCY)                    // has timeout been reached
        {
            globalActiveTimeCount = 0;                                          // reset the global counter
            globalActiveZone = 0;                                               // reset active zone to none (0)
        } 
    }
}
// ****************************************************************************
// ************************ EEPROMWriteZoneInjection **************************
// ****************************************************************************
//
// This function transfers the contents of the two injector RAM arrays (A&B) to the
// I2C EEPROM.  If a value is found to be out of range prior to EEPROM write, it is zeroed.
//
void EEPROMWriteZoneInjection(void)
{
    unsigned char localAdd;
    unsigned char localZone;
    unsigned char localVar;
    
    localZone = 1;                                                               // zones 1-8, start at 1
    for(localAdd=EA_Z1A; localAdd<EA_Z1A+12; localAdd++)
    {        
        localVar = globalZoneArrayA[localZone];
        if(localVar > 9)                                                        // check for out of range data
        {
            localVar = 0;                                                       // zero if out of range
        }        
        I2CWriteEEPROMSingle(localAdd,localVar,I2C_PHYSICAL_EEPROM);
        localZone++;
    }
    
    localZone = 1;                                                               // zones 1-8, start at 1
    for(localAdd=EA_Z1B; localAdd<EA_Z1B+12; localAdd++)
    {        
        localVar = globalZoneArrayB[localZone];
        if(localVar > 9)                                                        // check for out of range data
        {
            localVar = 0;                                                       // zero if out of range
        }        
        I2CWriteEEPROMSingle(localAdd,localVar,I2C_PHYSICAL_EEPROM);
        localZone++;
    }
}
// ****************************************************************************
// ************************* EEPROMReadZoneInjection **************************
// ****************************************************************************
//
// This function reads the I2C EEPROM and loads the RAM array with injector values for 
// each zone.
//
// Both arrays (A & B) are loaded and values are checked coming from the EEPROM for proper range.  If
// an out of range is read, a zero is loaded in to RAM array as well as written back to the EEPROM.
//
// Change to make initial testing easier on first boot.
//
//              000000000111
// Injector A   123456789012
//              999999999900
//
//              000000000111
// Injector B   123456789012
//              999999999990
//
// This way with a blank EEPROM all zones will to default to "9" max setting so the injector pulse is easier to see. Exception is zone 11 & 12 with:
// 11 A=Off B=On
// 12 A=Off B=Off 
//
void EEPROMReadZoneInjection(void)
{
    unsigned char localAdd;
    unsigned char localZone;
    unsigned char localCheck;
    
    // get injector A settings from EEPROM, loop 12x with starting location of "A" injector
    localZone = 1;
    for(localAdd=EA_Z1A; localAdd<EA_Z1A+12; localAdd++)
    {
        localCheck = I2CReadEEPROMSingle(localAdd,I2C_PHYSICAL_EEPROM);
        if (localCheck > 9)                                                     // check for out of range (above 9), is 0xFF if EEPROM has not been programmed
        {
            if (localAdd == EA_Z11A || localAdd == EA_Z12A)
            {
                localCheck = 0;                                                 // make min setting of 0. if out of range, for zone 12
            }
            else
            {
                localCheck = 9;                                                 // make max setting of 9, if out of range, for zone 1-11
            }            
            I2CWriteEEPROMSingle(localAdd,localCheck,I2C_PHYSICAL_EEPROM);      // store it
        }
        globalZoneArrayA[localZone] = localCheck;
        localZone++;
    }

    // get injector B settings from EEPROM, loop 12x with starting locaction of "B" injector
    localZone = 1;
    for(localAdd=EA_Z1B; localAdd<EA_Z1B+12; localAdd++)
    {
        localCheck = I2CReadEEPROMSingle(localAdd,I2C_PHYSICAL_EEPROM);
        if (localCheck > 9)                                                     // check for out of range (above 9), is 0xFF if EEPROM has not been programmed
        {
            if (localAdd == EA_Z12B)
            {
                localCheck = 0;                                                 // make min setting of 0. if out of range, for zone 12
            }
            else
            {
                localCheck = 9;                                                 // make max setting of 9, if out of range, for zone 1-11
            }            
            I2CWriteEEPROMSingle(localAdd,localCheck,I2C_PHYSICAL_EEPROM);      // store it
        }        
        globalZoneArrayB[localZone] = localCheck;
        localZone++;
    }    
}
// ****************************************************************************
// **************************** CheckZoneInputs ******************************
// ****************************************************************************
//
// This function reads Port A (Zone Inputs) and then converts a bit position to a number 1-C, 0 is no input.
// This function should be called every 0.01 seconds
// Ignoring the zero cross AC input glitch is tricky...
//      To do this I count how many times in a row that there is no input.
//      This is tracked by variable globalZoneInOffCount, if there is an input this variable is reset
//      The threshold for no zone input detections is currently 20 (20x 0.1=2S), defined as NO_ZONE_INPUT_COUNT
//      Also added is a tracking of how many consecutive zone detections
//
//      Active zone input results in zero cross glitch which is  approx 1ms high with 7ms low.  
//
//      Current scheduler is 1S, 100mS and 10mS
//  
//
void CheckZoneInputs(void)
{
    unsigned char zoneInRaw;                                                    // inputs 1-8
    unsigned char zoneInRaw912;                                                 // inputs 9-12        
    unsigned char localFlagZoneActive = 0;                                      // set if any of the zone inputs are active
    
    if (flagBoardType == BOARD_CU1)                                             // are we a CU1 (control) or RU1 (remote)
    {
        if(!flagProgramMode)
        {
            FAST_GPIOPinWrite(PORT_B, PIN_IN_SEL,0);                            // Look at inputs 1-12, change mux input select
            SysCtlDelay(1000);                                                  
            zoneInRaw = GPIOPinRead(PORT_A, 0xFF);                              // read the whole port  
            FAST_GPIOPinWrite(PORT_B, PIN_IN_SEL,PIN_IN_SEL);                   // Look at inputs 9-12, change mux input select
            SysCtlDelay(1000);                                                  
            zoneInRaw912 = GPIOPinRead(PORT_A, 0x0F);                           // read low nibble  
            zoneInRaw912 = zoneInRaw912 & 0x0F;                                 // mask upper nibble, just in case stuff is there

            globalLastZone = globalCurrentZone;                                 // store previous zone input

            if (zoneInRaw != 0xFF)                                              // 0xFF = no activity on 1-8
            {
                localFlagZoneActive = 1;
                globalZoneOffCount = 0;                                         // reset the "off" count
            }
            if (zoneInRaw912 != 0x0F)                                           // 0x0F = no activity on 9-12
            {
                localFlagZoneActive = 1;
                globalZoneOffCount = 0;                                         // reset the "off" count
            }
                       
            if (localFlagZoneActive)   // is a zone active?
            {   
                if      ((zoneInRaw & 0x01) == 0) globalCurrentZone = 0x01;
                else if ((zoneInRaw & 0x02) == 0) globalCurrentZone = 0x02;
                else if ((zoneInRaw & 0x04) == 0) globalCurrentZone = 0x03;
                else if ((zoneInRaw & 0x08) == 0) globalCurrentZone = 0x04;
                else if ((zoneInRaw & 0x10) == 0) globalCurrentZone = 0x05;
                else if ((zoneInRaw & 0x20) == 0) globalCurrentZone = 0x06;
                else if ((zoneInRaw & 0x40) == 0) globalCurrentZone = 0x07;
                else if ((zoneInRaw & 0x80) == 0) globalCurrentZone = 0x08;
                else if ((zoneInRaw912 & 0x01) == 0) globalCurrentZone = 0x09;   // 0000 1110 check
                else if ((zoneInRaw912 & 0x02) == 0) globalCurrentZone = 0x0A;   // 0000 1101
                else if ((zoneInRaw912 & 0x04) == 0) globalCurrentZone = 0x0B;   // 0000 1011
                else if ((zoneInRaw912 & 0x08) == 0) globalCurrentZone = 0x0C;   // 0000 0111                            
            }
            else                                                                // no zone active (or a zero cross glitch)
            {
                if(globalZoneOffCount < NO_ZONE_INPUT_COUNT)                    // start counting how many times no zone input has been found in a row
                {    
                    globalZoneOffCount++;
                }
                if(globalZoneOffCount >= NO_ZONE_INPUT_COUNT)                   // too many "no inputs" in a row
                { 
                    flagZoneActive = 0;                                  
                    globalZoneOnCount = 0;
//                    UARTCharPut(UART1_BASE,'*');                                // @@@ debug
                    globalActiveZone = 0;                                       // we have had a bunch of no zone detections in a row, lets call it zero
                    if (flagClearLEDOnce)
                    {
                        LEDDualClear();                                         
                        flagClearLEDOnce = 0;                                   // clear the flag
                    }
                    TurnOffInjectorAB();
                    globalInjectionTimeCountA=0;                                 // reset injection timer DJPQ3+
                    globalInjectionTimeCountB=0;                                 // reset injection timer DJPQ3+
                }                
            }

            // if we have an active zone, start tracking how many consecutive same zone detections have occured
            if (localFlagZoneActive)                                            // is a zone active?
            {   
                if (globalLastZone == globalCurrentZone)
                {
                    if (globalZoneOnCount < ACTIVE_ZONE_INPUT_COUNT)            // consecutive same detections below threshold?
                    {
                        globalZoneOnCount++;
                    }
                    else
                    {
                        flagZoneChange = 1;                                     // zone has changed
                        flagZoneActive = 1;                                     // consecutive detections reached, set flag
                        globalActiveZone = globalCurrentZone;
//                        UARTCharPut(UART1_BASE,'#');                          // @@@ debug
//                        if (globalActiveZone < 10)
//                        {
//                            UARTCharPut(UART1_BASE,globalActiveZone+0x30);    // @@@ debug
//                        }
//                        else 
//                        {
//                            UARTCharPut(UART1_BASE,globalActiveZone+0x37);    // @@@ debug                  
//                        }

                    }
                }
            }
                        
            if (flagZoneChange)
            {
                flagClearLEDOnce = 1;
                flagZoneChange = 0;                                             // zone has changed
                OutputInjector(globalActiveZone);                               // control of injector              
            }            
        }   // if !flagProgramMode
    }
    else if (flagBoardType == BOARD_RU1)
    {
        OutputInjector(globalActiveZone);        
    }
}
// ****************************************************************************
// **************************** TurnOffInjectorA ******************************
// ****************************************************************************
//
void TurnOffInjectorA(void)
{
    if (flagBoardType == BOARD_RU1)
    {
        GPIOPinWrite(PORT_A, PIN_RU1_INJA, 0);                                  // turn off RU1 AC optocoupler, injector A, (high asserted)
        GPIOPinWrite(PORT_B, PIN_LED2, PIN_LED2);                               // turn off injection LED (low asserted)                             
    }
}
// ****************************************************************************
// **************************** TurnOffInjectorB ******************************
// ****************************************************************************
//
void TurnOffInjectorB(void)
{
    if (flagBoardType == BOARD_RU1)
    {
        GPIOPinWrite(PORT_A, PIN_RU1_INJB, 0);                                  // turn off RU1 AC optocoupler, injector B, (high asserted)
        GPIOPinWrite(PORT_B, PIN_LED2, PIN_LED2);                               // turn off injection LED (low asserted)                             
    }
}
// ****************************************************************************
// *************************** TurnOffInjectorAB ******************************
// ****************************************************************************
//
// This function turns off the injectors and bottom LED (LED2). If the flagTestInjector is set, the LED 
// and injector are left as to prevent an accidental premature turn off
// by the process that is automatically occuring.
//
void TurnOffInjectorAB(void)
{
//    if (flagTestInjector)                                                       // is manual injection triggered by a TESTA or TESTB
//    {
//        // if so, do not turn off injectors here
//    }
//    else                                                                        // normal operation (not in test mode) allow injectors to turn off

//    {
        if (flagBoardType == BOARD_RU1)
        {
            GPIOPinWrite(PORT_B, PIN_LED2, PIN_LED2);                           // turn off injection LED (low asserted)                             
            GPIOPinWrite(PORT_A, PIN_RU1_INJA, 0);                              // turn off RU1 AC optocoupler, injector A, (high asserted)
            GPIOPinWrite(PORT_A, PIN_RU1_INJB, 0);                              // turn off RU1 AC optocoupler, injector B, (high asserted)
        }
//    }
}

// ****************************************************************************
// ************************** InquireTempAllViaRS485 **************************
// ****************************************************************************
//
// This function sends a request for temperature information (from CU1 to RU1)
// If flagExpand is set, we are an expansion unit, do not send flow request command 
//
void InquireTempAllViaRS485(void)
{
    if (flagBoardType == BOARD_CU1)                                             
    {
        if (flagExpand)                                                         // flagExpand is set, do not send request
        {
            
        }
        else                                                                    // flagExpand not set, go ahead request current temperature
        {
            SendStringRS485NoChecking("[INQTEMPALL?]");   
        }
    }
}
// ****************************************************************************
// ************************** InquireInjectorCount **************************
// ****************************************************************************
//
// This function sends a request for injector count information (from CU1 to RU1)
// If flagExpand is set, we are an expansion unit, do not send flow request command 
//
void InquireInjectorCount(void)
{
    if (flagBoardType == BOARD_CU1)                                             
    {
        if (flagExpand)                                                         // flagExpand is set, do not send request
        {
            
        }
        else                                                                    // flagExpand not set, go ahead request injector
        {
            SendStringRS485NoChecking("[INQINJECT?]");   
        }
    }
}
// ****************************************************************************
// ****************************** InquireCurrent ******************************
// ****************************************************************************
//
// This function is a RS485 command only where the CU1 request current data from the RU1.
//
void InquireCurrent(void)
{
    if (flagBoardType == BOARD_CU1)                                             
    {
        if (flagExpand)                                                         // flagExpand is set, do not send request
        {
            
        }
        else                                                                    // flagExpand not set, go ahead request currents
        {
            SendStringRS485NoChecking("[INQCURRENT?]");  
        }
    }
}
// ****************************************************************************
// **************************** SendZoneViaRS485 ******************************
// ****************************************************************************
//
// This function sends zone input data via RS485 from CU1 to RU1.
//
// 00000000001111111
// 01234567890123456
// [Z:1,IA:4,IB:5,P]     Zone1, Injector A = 4, Injector B = 5, Primary
// [Z:1,IA:4,IB:5,E]     Expansion
//
// This function sends status of the RU1 zone inputs along with injection value out the RS485 port.  A
// zone can have an "A" injection value or "B" injection value or none.  It will send a string every 5 seconds
// if a zone input is sensed.  
//
void SendZoneViaRS485(void)
{
    if (flagBoardType == BOARD_CU1)                                             // Only CU1 sends zone information via RS485
    {
        if (globalZoneOffCount<NO_ZONE_INPUT_COUNT)                             // if below threshold of consecutive no detections, it must be an input
        {
            txBuffer[0] = '[';
            txBuffer[1] = 'Z';
            txBuffer[2] = ':';
            txBuffer[3] = hexArray[globalActiveZone];                           // zone that was triggered
            txBuffer[4] = ',';

            txBuffer[5] = 'I';
            txBuffer[6] = 'A';
            txBuffer[7] = ':';
            txBuffer[8] = hexArray[globalZoneArrayA[globalActiveZone]];     // how much A injection
            txBuffer[9] = ',';

            txBuffer[10] = 'I';
            txBuffer[11] = 'B';
            txBuffer[12] = ':';
            txBuffer[13] = hexArray[globalZoneArrayB[globalActiveZone]];     // how much B injection
            txBuffer[14] = ',';                       

            if (flagExpand)                                                     // flagExpand is set, do not send flow request
            {
                txBuffer[15] = 'E';                                             // command sent from expansion mode CU1                
            }
            else                                                                // flagExpand not set, go ahead request flow data
            {
                txBuffer[15] = 'P';                                             // command sent from primary mode CU1                
            }
            txBuffer[16] = ']';                       
            txBuffer[17] = NULL;
            SendStringRS485NoChecking(txBuffer);   
        }
    }
}
// ****************************************************************************
// **************************** CheckProgramMode ******************************
// ****************************************************************************
//
// If we are in Program Mode setup zones
// globalZoneInjectArray[13] stores injection rate for each zone.  [0] location not used for ease of coding...
// Switch1 increments zones 1-12
// Switch2 increments injectorA rate 0-10
// Switch3 increments injectorB rate 0-10
// When is data stored to EEPROM?  It is stored when exiting program mode which oocurs in SwitchInputsDebounce()
//
void 
CheckProgramMode(void)
{
    if (flagBoardType == BOARD_CU1)                                             // are we a CU1 (control) or RU1 (remote)
    {
        if(flagProgramMode)
        {
            // Zone increment
            if(flagSwitch1Released)                                             // zone is changing                                        
            {
                globalZoneActive++;                                             // 1-12, increment zone to setup
                if(globalZoneActive > 12)                                       // wrap back to zone 1
                {
                    globalZoneActive = 1;
                }
                LEDDualDecimal(globalZoneActive, DECIMAL_OFF);
//                DebugPortZoneValue();
                flagSwitch1Released = 0;                                        // switch released, allow another switch release
            }
            // Injector A rate increment
            if(flagSwitch2Released)
            {
                globalZoneArrayA[globalZoneActive]++;
                if(globalZoneArrayA[globalZoneActive] > 10)                     // DJP4+ (was 9) now 10, max injection rate reached wrap to 0.  
                {
                    globalZoneArrayA[globalZoneActive] = 0;
                }
//                flagDecimalTens = 0;                                                  // clear tens flag
//                LEDDualDecimal(globalZoneArrayA[globalZoneActive], DECIMAL_ONES_ON); 
                flagDecimalOnes = 0;                                                    // DJP8,clear ones flag
                flagDecimalTens = 1;                                                    // DJP8, set tens flag
                LEDDualDecimal(globalZoneArrayA[globalZoneActive], DECIMAL_BYPASS);    // DJP8, make InjectorA on the left
                flagSwitch2Released = 0;   
            }  
            // Injector B rate increment
            if(flagSwitch3Released)
            {
                globalZoneArrayB[globalZoneActive]++;
                if(globalZoneArrayB[globalZoneActive] > 10)                             // DJP4+ (was 9) now 10, max injection rate reached wrap to 0.  
                {
                    globalZoneArrayB[globalZoneActive] = 0;
                }
//                flagDecimalOnes = 0;                                                  // clear ones flag                
//                LEDDualDecimal(globalZoneArrayB[globalZoneActive], DECIMAL_TENS_ON); 
                flagDecimalOnes = 1;                                                    // DJP8, clear ones flag
                flagDecimalTens = 0;                                                    // DJP8, set tens flag
                LEDDualDecimal(globalZoneArrayB[globalZoneActive], DECIMAL_BYPASS);    // DJP8, make InjectorB on the right
                flagSwitch3Released = 0;   
            }        
        }
        else
        {
            //  nothing to do here
        }
    }
}
// ****************************************************************************
// **************************** DebugPortZoneValue ****************************
// ****************************************************************************
//
void DebugPortZoneValue(void)
{
    unsigned char localIndex;
    unsigned char localVar;
    
    for(localIndex=1; localIndex<13; localIndex++)
    {
        localVar = globalZoneArrayA[localIndex];
        UARTCharPut(UART1_BASE,hexArray[localVar >> 4]);
        UARTCharPut(UART1_BASE,hexArray[localVar & 0x0F]);
        UARTCharPut(UART1_BASE,0x20);  
    } 
    UARTCharPut(UART1_BASE,CR);
    UARTCharPut(UART1_BASE,LF);  
}

// ****************************************************************************
// ************************** SwitchInputsDebounce ****************************
// ****************************************************************************
//
// This function debounces the switch inputs. 
// This function is entered when a flag is set by interrupt service and called by Main().
// Flags are set in this function and must be cleared elsewhere when used.
// Flags set are:
// flagSwitch#Released
// flagSwitch#Long
//
void SwitchInputsDebounce(void)
{
    // Switch #1
    if (flagSwitch1Pressed)
    {
        if (GPIOPinRead(PORT_D, PIN_SWITCH1))                                   // released, it went high
        {  
            if (globalSwitch1DebounceCount > 1 && globalSwitch1DebounceCount < 10) // released and held long enough but not too long
            {
                flagSwitch1Released = 1;
            }
            else                                                                // released but not held long enough
            {                
            }
            globalSwitch1DebounceCount = 0; 
            flagSwitch1Pressed = 0;
        }
        else                                                                    // still being pressed
        {
            globalSwitch1DebounceCount++; 
            if(globalSwitch1DebounceCount > 50)                                 // 5 seconds have passed, enter program mode                
            {
                flagSwitch1Long = 1;
                globalSwitch1DebounceCount = 0; 
                flagSwitch1Pressed = 0;
                if (flagProgramMode)                                            // Toggle mode, Exit program modem
                {
                    flagProgramMode = 0;
                    LEDDualClear();
                    GPIOPinWrite(PORT_B, PIN_LED2, PIN_LED2);                   // no longer in program mode, turn off LED
                    EEPROMWriteZoneInjection();                                 // save to EEPROM injection settings                    
                }
                else                                                            // Toggle mode, enter program mode
                {
                    flagProgramMode = 1;                                        // we are now in program mode
                    globalZoneActive = 1;                                       // work with zone 1 first
                    GPIOPinWrite(PORT_B, PIN_LED2, 0);                          // turn on LED
                    LEDDualDecimal(globalZoneActive, DECIMAL_OFF);                    
                }   
            }
        }
    }
    
    // Switch #2
    if (flagSwitch2Pressed)
    {
        if (GPIOPinRead(PORT_D, PIN_SWITCH2))                                   // released, it went high
        {  
            if (globalSwitch2DebounceCount > 1 && globalSwitch2DebounceCount < 10) // released and held long enough but not too long
            {
                flagSwitch2Released = 1;
            }        
            else                                                                // released but not held long enough
            {                
            }
            globalSwitch2DebounceCount = 0; 
            flagSwitch2Pressed = 0;
        }
        else                                                                    // still being pressed
        {
            globalSwitch2DebounceCount++;                    
        }
    }
    
    // Switch #3
    if (flagSwitch3Pressed)
    {
        if (GPIOPinRead(PORT_F, PIN_SWITCH3))                                   // released, it went high
        {  
            if (globalSwitch3DebounceCount > 1 && globalSwitch3DebounceCount < 10) // released and held long enough but not too long
            {
                flagSwitch3Released = 1;
            }        
            else                                                                // released but not held long enough
            {                
            }
            globalSwitch3DebounceCount = 0; 
            flagSwitch3Pressed = 0;
        }
        else                                                                    // still being pressed
        {
            globalSwitch3DebounceCount++;                    
        }
    }    
}
/*
// ****************************************************************************
// *************************** LEDPortWriteDecimal ****************************
// ****************************************************************************
//
// If a number 0 1 2 3 4 5 6 7 8 9 A B C D E F BLANK (0 - 16).
// Data sent to this function is passed through lookup "segArray[]" table first.
// Data is split between 2 ports, PC7-PC4 and PD3-PD0
// Decimal point is PD3
//
void LEDPortWriteDecimal(unsigned char recData, unsigned recDecimal)
{
    unsigned char localPC;                                                      // local port C
    unsigned char localPD;                                                      // local port D
    
    localPC = recData << 4;                                                     // DCBA #### <- ### #DCBA
    localPC = ~localPC & 0xF0;                                                  // mask low nibble
    localPD = recData >> 4;                                                     // #GFE #### -> #### #GFE
    localPD = ~localPD & 0x07;                                                  // mask high nibble and decimal point
   
    if (recDecimal == DECIMAL_ONES || recDecimal == DECIMAL_TENS)
    {
        localPD = (localPD & 0xF7);                                             // 1111 0111 low so decimal is turned on
    }
    else if (recDecimal == DECIMAL_OFF)
    {
        localPD = (localPD | 0x08);                                             // 0000 1000 high so decimal is turned off
    }

    GPIOPinWrite(PORT_C, 0xF0, localPC);							                    
    GPIOPinWrite(PORT_D, 0x0F, localPD);

    GPIOPinWrite(PORT_F, PIN_ONESCLK, PIN_ONESCLK);
    GPIOPinWrite(PORT_F, PIN_ONESCLK, 0);
    GPIOPinWrite(PORT_F, PIN_ONESCLK, PIN_ONESCLK);   
}

*/
// ****************************************************************************
// ************************ ProcessSerialInput ********************************
// ****************************************************************************
//
// Process serial input from RS485, RS232 (debug) and BLE UARTs
// 4 = RS485
// D = Debug
// B = BLE (Bluetooth Low Energy)
// 
// Command              Description             CU1     RU1
//                                              In      Out     In      Out
// ==========           =============           ====    ====    ====    ====
// [Z:-,I-:-,-]         Zone                    4       4                       Zone command, example: Z:1,IA:8,P = set zone#1 injectorA to a setting of 8, Primary
//                                                                              This is data stream used when the CU1 transmits to the RU1
//                                                                       
// [H]                  Hello World             D,B,4   D,B,4   D,B     D,B       
// [WD---D---]          EEPROM Wr Dec           D,B     D,B     D,B             Example write location 0x33 with data 0x55 = [WH33H55]
// [WH--H--]            EEPROM Wr Hex   
// [RD---]              EEPROM Rd Dec           D,B     D,B     D,B             Example, read hex location 0x03 = [RD03]                      
// [RH--]               EEPROM Rd Hex                                           Return value: [A:0x-- D:0x--]
// [V]                  Version                 D,B,4   D,B,4   D,4    D,4
// [BOOT]               Reboot                  D,B             D,B
// [S:-,I-:-]           Set zone                D,B                             Example: [S:1,IA:8] = set CU1 zone#1 Injector A to 8
//
// [ALLRD?]             Read ALL zones          D,B     D,B                     Read all zone injector settings
//                                                                              Return value: [ALLRD* A A A A A A A A A A A A B B B B B B B B B B B B]
//
// [ALLWR>]             Write ALL zones         D,B                             Write all zone injector settings
//                                                                              Write value: [ALLWR> A A A A A A A A A A A A B B B B B B B B B B B B]
//
// [COMMIT]             Commit EEPROM           D,B                             Write all zone injector settings to EEPROM
// [CLEARMAX]           Clear max temp          D,B                             Clear max stored max temperature on RU1
// [CLEARINJECT]        Clear inector count                                     Clear current count of injections on RU1
// [CLEARINJECTA]       Clear injection cnt A                                   Clear current count of injections A on RU1
// [CLEARINJECTB]       Clear injection cnt B                                   Clear current count of injections B on RU1
//
// [QON]                Quiet On                D,B                             [QON] or [QOFF], QON will stop data transmitting on RS485 bus
// [QOFF]               Quiet Off
//
// [GETTEMPALL?]        Get Both                D,B,4   D,B                     Debug/BLE requests CU1 temperature, returns [GETALL*000,000]
// [GETINJECT?]         Get Injector count      D,B,4                           Debug/BLE request Injector A&B count Return value: [INQINJECT*xxxxxxxx,xxxxxxxx]
// [GETCURRENT?]        Get Current             D,B,4                           Debug/BLE request Current.  Return value: [INQCURRENT*xxxxxxxx,xxxxxxxx]
//
// [TESTA]              Test Injector           D,B                             This will issue a single pulse of the injector A                                           
// [TESTB]              Test Injector           D,B                             This will issue a single pulse of the injector B                                           
//
// [BLEBOOT]            Reset BLE               D,B                             Reboots the BLE module
// [BLENAME?]           BLE Name?               D,B                             Returns BLE name that Tiva gets upon boot.  Name is from Tiva RAM
// [BLEBTA?]            BLE BTA?                D,B                             Returns BLE address that Tiva gets upon boot.  Name is from Tiva RAM
// [BLEVER?]            BLE VER?                D,B                             Returns BLE module firmware version
// [BLESETNAME>xxxxx]                           D,B                             Sets the BLE module name
//
// [EXPAND]     Make slave                      D,B                             Turns a CU1 into a expansion unit so it does not transmit valve replication    
//
// [NSET------] SN SET                                                          Set the serial number
//
// [FANON]
// [FANOFF]
//
// ========================
// Exclusive RS485 Commands
// ========================
//
// [INQTEMPALL?]         Temperature             4               4              Board to board only, CU1 requests RU1 current and max temperature
// [INQINJEC?T]                                  4               4              Board to board only, CU1 request RU1 injector count
// [INQCURRENT?]                                                                Board to board only, CU1 request RU1 current measurements
// [LOOP] [BACK]
// [485CLEARMAX]                                                                RU1 received, clears max temperature
// [485CLEARINJECTA]                                                            RU1 received, clears injection A counts
// [485CLEARINJECTB]                                                            RU1 received, clears injection B counts
//
void ProcessSerialInput(unsigned char localSource)
{   
    if(localSource == 'B')
    {
        StringCopy(cmdBuffer,rxBufferBLE);   
        SendStringDebug(cmdBuffer);      // @@@ DJP
    }
    else if(localSource == '4')
    {
        StringCopy(cmdBuffer,rxBuffer);         
    }
    else if(localSource == 'D')
    {
        StringCopy(cmdBuffer,rxBufferDebug);    
    }

    //    StringToUpper(cmdBuffer);
        
    switch(cmdBuffer[1])
    {
        case 'Z':                                                               // Zone input command                   
            CommandProcessSerialZoneInput(localSource);
            break;
        case 'H':                                                               // receive 'H' and reply with "WORLD"
            CommandProcessHello(localSource);                                             
            break;    
        case 'W':                                                               // EEPROM Write
            CommandProcessEEPROMWrite(localSource);
            break;
        case 'R':                                                               // EEPROM Read
            CommandProcessEEPROMRead(localSource);       
            break;
        case 'V':                                                               // Version    
            CommandProcessVersion(localSource); 
            break;
        case 'F':
            CommandProcessFChooser(localSource);                                // "F" commands
            break;
        case 'S':
            CommandProcessSetZone(localSource);                                 // set injector value for a specific zone
            break;
        case 'A':
            CommandProcessAllZone(localSource);                                 // read or write all zones injector settings to/from RAM
            break;
        case 'C':
            CommandProcessCChooser(localSource);                                // "C" Commands                        
            break;
        case 'Q':
            CommandProcessQuiet(localSource);                                   // quiet the CU1 sending data to RU1                       
            break;
        case 'G':
            CommandProcessGet();                                                // get values stored on CU1                       
            break;            
        case 'T':
            CommandProcessTest(localSource);                                    // Test Injector                      
            break;  
        case 'B':
            CommandProcessB(localSource);                                       // "B" Commands, BOOT and BLE                   
            break;             
        case 'E':
            CommandProcessExpand(localSource);                                  // "E" Expand or Slave                  
            break;             
        case 'N':
            CommandProcessSerialNumber(localSource);                            // Serial Number entry                 
            break;  
        case 'I':
            InquireChooser(localSource);                                        // Inquire about temperature, injector
            break;
        case 'L':                                                               // loop test of RS485 link
            LoopTest(localSource);
            break;
        case '4':                                                               // Clear commands
            CommandProcessClear(localSource);
            break;
                   
    }  
}

// ****************************************************************************
// *************************** CommandProcessClear ****************************
// ****************************************************************************
//
// This function is processed only by RU1.  Max temperature and injection count is cleared.
//
// 0000000000111111
// 0123456789010123
// [485CLEARMAX]        
// [485CLEARINJECTA]
// [485CLEARINJECTB]
//
void CommandProcessClear(unsigned char localSource)
{
    if (flagBoardType == BOARD_RU1)                                             // are we CU1 (controller)
    {
        if(cmdBuffer[2]=='8' && cmdBuffer[3]=='5' && cmdBuffer[4]=='C' && cmdBuffer[5]=='L' && cmdBuffer[6]=='E' && cmdBuffer[7]=='A' && cmdBuffer[8]=='R'  && cmdBuffer[9]=='M' && cmdBuffer[10]=='A' && cmdBuffer[11]=='X')
        { 
            SendStringDebug("Clear Max");
            I2CWriteEEPROMSingle(EA_MAXTEMP,0,I2C_PHYSICAL_EEPROM);
            globalTempMaxInputRU1 = 0;
        }
        else if(cmdBuffer[2]=='8' && cmdBuffer[3]=='5' && cmdBuffer[4]=='C' && cmdBuffer[5]=='L' && cmdBuffer[6]=='E' && cmdBuffer[7]=='A' && cmdBuffer[8]=='R'  && cmdBuffer[9]=='I' && cmdBuffer[10]=='N' && cmdBuffer[11]=='J' && cmdBuffer[12]=='E' && cmdBuffer[13]=='C' && cmdBuffer[14]=='T' && cmdBuffer[15]=='A')
        { 
            SendStringDebug("Clear Inject A");
            globalInjectionCountA = 0;                                          // zero how many pulses has the injector A performed
            StoreIntToEEPROM(EA_IACNT, globalInjectionCountA);                  // store the zero value now
        }
        else if(cmdBuffer[2]=='8' && cmdBuffer[3]=='5' && cmdBuffer[4]=='C' && cmdBuffer[5]=='L' && cmdBuffer[6]=='E' && cmdBuffer[7]=='A' && cmdBuffer[8]=='R'  && cmdBuffer[9]=='I' && cmdBuffer[10]=='N' && cmdBuffer[11]=='J' && cmdBuffer[12]=='E' && cmdBuffer[13]=='C' && cmdBuffer[14]=='T' && cmdBuffer[15]=='B')
        { 
            SendStringDebug("Clear Inject B");
            globalInjectionCountB = 0;                                          // zero how many pulses has the injector B performed
            StoreIntToEEPROM(EA_IBCNT, globalInjectionCountB);                  // store the zero value now
        }
    }
}
// ****************************************************************************
// *************************** InquireChooser *********************************
// ****************************************************************************
//
// Commands that RU1 receives via RS485 input
//
// 000000000010
// 012345678901
// [INQTEMP]        
// [INQINJECT]        
// [INQCURRENT]
//
void InquireChooser(unsigned char localSource)
{

    if(cmdBuffer[2]=='N' && cmdBuffer[3]=='Q')     
    {        
        if (cmdBuffer[4]=='T' && cmdBuffer[5]=='E' && cmdBuffer[6]=='M' && cmdBuffer[7]=='P')
        {
            CommandProcessTemp();    
        }
        else if (cmdBuffer[4]=='I' && cmdBuffer[5]=='N' && cmdBuffer[6]=='J' && cmdBuffer[7]=='E' && cmdBuffer[8]=='C' && cmdBuffer[9]=='T')
        {
            CommandProcessInjectorCount(); 
        }  
        else if (cmdBuffer[4]=='C' && cmdBuffer[5]=='U' && cmdBuffer[6]=='R' && cmdBuffer[7]=='R' && cmdBuffer[8]=='E' && cmdBuffer[9]=='N' && cmdBuffer[10]=='T')
        {
            CommandProcessCurrent(); 
        }  
    }
}
// ****************************************************************************
// *********************** CommandProcessSerialNumber *************************
// ****************************************************************************
//
// This function sets the serial number that is saved in the EEPROM.  The serial number can be set
// via EEPROM write commands, this function just makes it easier.
//
// Serial number form:
// 
// 0000000000111111111121
// 0123456789012345678901
// [NSETCU1-------------]     Device number
// [NSET---V3-----------]     PCB version number
// [NSETCU1V300000000001]
// 
void CommandProcessSerialNumber(unsigned char localSource)
{
    unsigned char sni = 0;                                                      // serial number index
    unsigned char cbi = 0;                                                      // command buffer index
    unsigned char ea = EA_SN_START;                                             // eeprom address
    
    if(cmdBuffer[2]=='S' && cmdBuffer[3]=='E' && cmdBuffer[4]=='T' && (cmdBuffer[5]=='C' || cmdBuffer[5]=='R')  && cmdBuffer[6]=='U' && cmdBuffer[7]=='1')
    { 
        cbi = 5;
        for (sni=0; sni<16; sni++)
        {
            globalSerialNumberString[sni] = cmdBuffer[cbi]; 
            I2CWriteEEPROMSingle(ea,cmdBuffer[cbi],I2C_PHYSICAL_EEPROM);
            cbi++;                                                              // inc the command (input) string index
            ea++;
        }
        globalSerialNumberString[16] = NULL;                                    // make sure string is terminated
        SendAck(localSource);                                              
    }
}
// ****************************************************************************
// ************************ GetSerialNumberFromEEPROM *************************
// ****************************************************************************
//
// This function reads the EEPROM and loads serial number into global string
//
void GetSerialNumberFromEEPROM(void)
{
    unsigned char ea = EA_SN_START;                                             // eeprom index
    unsigned char sni = 0;                                                      // string index         
        
    for (sni=0; sni<16; sni++)
    {
        globalSerialNumberString[sni] = I2CReadEEPROMSingle(ea,I2C_PHYSICAL_EEPROM);
        ea++;            
    }
    globalSerialNumberString[16] = NULL;                                        // make sure string is terminated         
}
// ****************************************************************************
// ************************** CommandProcessExpand ****************************
// ****************************************************************************
//
// This function handles [EXPAND] command.  It will turn a 2nd CU1 into an expansion unit.
// ie 2 or more CU1 working together for a 16 valve timer box.
// The expansion unit will:
//  - Transmit injector info only.  No valve replication data is sent.
//  - Will not poll for flow data
// The expansion unit will determine if it is Primary or Expansion by reading EEPROM location.
//
// 01234567
// [EXPAND]
//
void CommandProcessExpand(unsigned char localSource)
{
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
    {
        if(cmdBuffer[2]=='X' && cmdBuffer[3]=='P' && cmdBuffer[4]=='A' && cmdBuffer[5]=='N' && cmdBuffer[6]=='D')
        {    
    
            if (flagExpand == 0)                                                // if Expansion flag not set, toggle it                                               
            {
                flagExpand = 1;
//                LEDPortWriteDecimal(segArray[14],DECIMAL_OFF);                   // display "E" for expansion
                LEDDualHex(segArray[14], SEGMENT_LOW, DECIMAL_OFF);
                I2CWriteEEPROMSingle(EA_EXPAND,0xA5,I2C_PHYSICAL_EEPROM);       // store expansion is set in EEPROM
                SetBLEName("S-,FertiWiser-E");
                BLEReset();                                                     // reset the BLE module
                StringCopy(txBuffer,"[Expand]");
                if(localSource == 'D') SendStringDebug(txBuffer);
                if(localSource == 'B') SendStringBLE(txBuffer);
                if(localSource == '4') SendStringRS485NoChecking(txBuffer);    
                DelaySecondsBlocking(1);  
            }
            else                                                                // if Expansion flag set, toggle it                                               
            {
                flagExpand = 0;
//                LEDPortWriteDecimal(ALPHA_P,DECIMAL_OFF);                        // display "P" for primary
                LEDDualHex(ALPHA_P, SEGMENT_LOW, DECIMAL_OFF);
                I2CWriteEEPROMSingle(EA_EXPAND,0x00,I2C_PHYSICAL_EEPROM);       // store expansion is not set in EEPROM
                SetBLEName("S-,FertiWiser");
                BLEReset();                                                     // reset the BLE module
                StringCopy(txBuffer,"[Primary]");
                if(localSource == 'D') SendStringDebug(txBuffer);
                if(localSource == 'B') SendStringBLE(txBuffer);
                if(localSource == '4') SendStringRS485NoChecking(txBuffer);    
                DelaySecondsBlocking(1);            
            }

            BLECmdModeSequence();                                               // get config data from module again since name has changed
        }
    }    
}
// ****************************************************************************
// **************************** CommandProcessB *******************************
// ****************************************************************************
//
// This function handles "B" commands
// [BLEBOOT]
// [BLENAME?]
// [BLEBTA?]
// [BLEVER?]
// [BLESETNAME>xxxxx]
// [BOOT]
//
void CommandProcessB(unsigned char localSource)
{
    unsigned char n;
    unsigned char localIndex;
    unsigned char localString[20];
//    unsigned char flagProceed = 0;
    
    if(cmdBuffer[2]=='L' && cmdBuffer[3]=='E')                              
    {
        if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
        {   
            if(cmdBuffer[4]=='B' && cmdBuffer[5]=='O' && cmdBuffer[6]=='O' && cmdBuffer[7]=='T')
            {    
                BLEReset();                                                     // reset the BLE module  
                SendAck(localSource);                                
            }
            else if(cmdBuffer[4]=='N' && cmdBuffer[5]=='A' && cmdBuffer[6]=='M' && cmdBuffer[7]=='E' && cmdBuffer[8]=='?')
            {
                StringCopy(txBuffer, "[");
                StringCat(txBuffer,bleModuleNameString);    
                StringCat(txBuffer,"]");    
                if(localSource == 'D') SendStringDebug(txBuffer);
                if(localSource == 'B') SendStringBLE(txBuffer);
                if(localSource == '4') SendStringRS485NoChecking(txBuffer);                            
            }
            else if(cmdBuffer[4]=='B' && cmdBuffer[5]=='T' && cmdBuffer[6]=='A' && cmdBuffer[7]=='?')
            {
                StringCopy(txBuffer, "[");
                StringCat(txBuffer,bleModuleBTAString);    
                StringCat(txBuffer,"]");    
                if(localSource == 'D') SendStringDebug(txBuffer);
                if(localSource == 'B') SendStringBLE(txBuffer);
                if(localSource == '4') SendStringRS485NoChecking(txBuffer);                            
            }
            else if(cmdBuffer[4]=='V' && cmdBuffer[5]=='E' && cmdBuffer[6]=='R' && cmdBuffer[7]=='?')
            {
                StringCopy(txBuffer, "[");
                StringCat(txBuffer,bleFirmwareVersion);    
                StringCat(txBuffer,"]");    
                if(localSource == 'D') SendStringDebug(txBuffer);
                if(localSource == 'B') SendStringBLE(txBuffer);
                if(localSource == '4') SendStringRS485NoChecking(txBuffer);                            
            }
            else if(cmdBuffer[4]=='S' && cmdBuffer[5]=='E' && cmdBuffer[6]=='T')
            {
                if(cmdBuffer[7]=='N' && cmdBuffer[8]=='A' && cmdBuffer[9]=='M' && cmdBuffer[10]=='E' && cmdBuffer[11]=='>')    
                {
                    // build the string
                    localString[0] = 'S';
                    localString[1] = '-';
                    localString[2] = ',';
                    localIndex=3;

                    for(n=12; cmdBuffer[n]!=']'; n++)
                    {
                        localString[localIndex] = cmdBuffer[n];  
                        localIndex++;
                    }

                    localString[localIndex] = NULL;
                    SetBLEName(localString);
                    SendAck(localSource);                                   
                }
            }
        }
    }
    else if (cmdBuffer[2] == 'O' && cmdBuffer[3] == 'O' && cmdBuffer[4] == 'T')
    {
        SendAck(localSource);
        HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;       
    }  
    
}
// ****************************************************************************
// ******************************** SetBLEName ********************************
// ****************************************************************************
//
// This function sets the BLE name with a string passed to it.
// String should look like this: S-,FertiWiser
//
void SetBLEName(unsigned char *localString)
{
    unsigned char flagProceed = 0;

    flagProceed = BLEEnterCommandMode();                                        // enter command mode 

    if (flagProceed)
    {
        SendStringBLE(localString); 
        BLEWaitForCmdResponseBlocking();
        BLEExitCommandMode();  
    }
}
// ****************************************************************************
// ************************* CommandProcessGet ****************************
// ****************************************************************************
//
// This function returns (when requested) the flow value that is gathered from the RU1.
// The CU1 is continually requesting data from RU1.
//
// globalFlowStringCU1 is ascii string of globalFlowInputCU1
//
// 0000000000111
// 0123456789012
// [GETTEMPALL?]        Returns [TEMPALL*000,000] current,max
// [GETINJECT?]         Returns 
// [GETCURRENT?]        Returns 
//
//
void CommandProcessGet(void)
{
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
    {
        if(cmdBuffer[2]=='E' && cmdBuffer[3]=='T')                              
        {
            if(cmdBuffer[4]=='T' && cmdBuffer[5]=='E' && cmdBuffer[6]=='M' && cmdBuffer[7]=='P' && cmdBuffer[8]=='A' && cmdBuffer[9]=='L' && cmdBuffer[10]=='L' && cmdBuffer[11]=='?')                              
            {              
                SendStringBLE(globalCU1TemperatureString);                                                
                SendStringDebug(globalCU1TemperatureString);
            }
            else if(cmdBuffer[4]=='I' && cmdBuffer[5]=='N' && cmdBuffer[6]=='J' && cmdBuffer[7]=='E' && cmdBuffer[8]=='C' && cmdBuffer[9]=='T' && cmdBuffer[10]=='?')                              
            {              
                SendStringBLE(globalCU1InjectionString);                                                
                SendStringDebug(globalCU1InjectionString);
            }
            else if(cmdBuffer[4]=='C' && cmdBuffer[5]=='U' && cmdBuffer[6]=='R' && cmdBuffer[7]=='R' && cmdBuffer[8]=='E' && cmdBuffer[9]=='N' && cmdBuffer[10]=='T' && cmdBuffer[11]=='?')                              
            {              
                SendStringBLE(globalCU1CurrentString);                                                
                SendStringDebug(globalCU1CurrentString);
            }
        }
    }   
}
// ****************************************************************************
// ************************* CommandProcessHello ******************************
// ****************************************************************************
// 
// This function will reply "World" to a [H] command.  It will append where the command came from.
//
// 0123
// [H]
void CommandProcessHello(unsigned char localSource)
{
//    if(cmdBuffer[2]=='E' && cmdBuffer[3]=='L' && cmdBuffer[4]=='L' && cmdBuffer[5]=='O')                              
//    {
        if (flagBoardType == BOARD_CU1)                                         // are we CU1 (controller)
        {
            if(localSource == 'D') SendStringDebug("[World CU1 Debug]");
            if(localSource == 'B') SendStringBLE("[World CU1 BLE]");
            if(localSource == '4') SendStringRS485NoChecking("[World CU1 RS485]");    
        }
        else if (flagBoardType == BOARD_RU1)                                    // are we RU1
        {
            if(localSource == 'D') SendStringDebug("[World RU1 Debug]");
            if(localSource == '4') SendStringRS485NoChecking("[World RU1 RS485]");    
        }
//    }
}
// ****************************************************************************
// ************************* CommandProcessQuiet ******************************
// ****************************************************************************
//
// 
// This function will set the flagQuiet.  This prevents the CU1 from sending data to
// RU1.  Primarily these are zone/setting and flow reqest commands.  This flag is only 
// in ram and will be reset on next boot.
//
// 012345
// [QON]
// [QOFF]
//
void CommandProcessQuiet(unsigned char localSource)
{
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
    {
        if(cmdBuffer[2]=='O' && cmdBuffer[3]=='N')                              // error and format checking                             
        {           
            flagQuiet = 1;                                                      // SET the flag to enter quiet mode           
            SendAck(localSource);
        }        
        else if(cmdBuffer[2]=='O' && cmdBuffer[3]=='F' && cmdBuffer[4]=='F')                              
        {           
            flagQuiet = 0;                                                      // CLR the flag to exit quiet mode           
            SendAck(localSource);
        }
    }       
}


// ****************************************************************************
// *********************** CommandProcessCChooser *****************************
// ****************************************************************************
//
// This function will direct to different "C" commands
//
// 00000000001111
// 01234567890123
// [COMMIT]
// [CLEARMAX]
// [CLEARINJECTA]
// [CLEARINJECTB]
//
void CommandProcessCChooser(unsigned char localSource)
{

    if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
    {
        // error and format checking
        if(cmdBuffer[2]=='O' && cmdBuffer[3]=='M' && cmdBuffer[4]=='M' && cmdBuffer[5]=='I' && cmdBuffer[6]=='T')                              
        {
            CommandProcessCommit(localSource);
        }
        else if(cmdBuffer[2]=='L' && cmdBuffer[3]=='E' && cmdBuffer[4]=='A' && cmdBuffer[5]=='R' && cmdBuffer[6]=='M' && cmdBuffer[7]=='A' && cmdBuffer[8]=='X')                              
        {
//            SendStringDebug("Clear Max Temp");
            flagScheduleClearMaxTemp = 1;                                       // schedule sending [485CLEARMAX] on RS485 bus
            SendAck(localSource);                
        }
        else if(cmdBuffer[2]=='L' && cmdBuffer[3]=='E' && cmdBuffer[4]=='A' && cmdBuffer[5]=='R' && cmdBuffer[6]=='I' && cmdBuffer[7]=='N' && cmdBuffer[8]=='J' && cmdBuffer[9]=='E' && cmdBuffer[10]=='C' && cmdBuffer[11]=='T' && cmdBuffer[12]=='A')                              
        {
//            SendStringDebug("Clear Inject A");
            flagScheduleInjectA = 1;                                             // schedule sending [485CLEARINJECT] on RS485 bus
            SendAck(localSource);                
        }
        else if(cmdBuffer[2]=='L' && cmdBuffer[3]=='E' && cmdBuffer[4]=='A' && cmdBuffer[5]=='R' && cmdBuffer[6]=='I' && cmdBuffer[7]=='N' && cmdBuffer[8]=='J' && cmdBuffer[9]=='E' && cmdBuffer[10]=='C' && cmdBuffer[11]=='T' && cmdBuffer[12]=='B')                              
        {
//            SendStringDebug("Clear Inject B");
            flagScheduleInjectB = 1;                                             // schedule sending [485CLEARINJECT] on RS485 bus
            SendAck(localSource);                
        }
    }
}
// ****************************************************************************
// ************************ CommandProcessCommit ******************************
// ****************************************************************************
//
// This function will commit zone settings from RAM to EEPROM
//
void CommandProcessCommit(unsigned char localSource)
{
    unsigned char localAddress;                                                 // EEPROM address to write to
    unsigned char n;                                                            // index into RAM array of zone settings
        
    n = 1;                                                                      // RAM address starts at 1
    for(localAddress=EA_Z1A; localAddress<EA_Z1A+13; localAddress++)            // A injector 
    {                
        I2CWriteEEPROMSingle(localAddress,globalZoneArrayA[n],I2C_PHYSICAL_EEPROM);
        n++;                
    }
    n = 1;                                                                      // RAM address starts at 1
    for(localAddress=EA_Z1B; localAddress<EA_Z1B+13; localAddress++)            // B injector
    {                
        I2CWriteEEPROMSingle(localAddress,globalZoneArrayB[n],I2C_PHYSICAL_EEPROM);
        n++;                
    }
    SendAck(localSource);
}
// ****************************************************************************
// ********************** CommandProcessAllZone *******************************
// ****************************************************************************
//
// This function will return or write the settings for all 8 zones of injector A and injector B
//
// As of version 1.0.3, CommandProcessCommit() was added so that when external device writes to
// values to this function, data is also copied to EEPROM.  This used to take a seperate command "COMMIT"
// to do this.
//
// ALLRD? will return all zone data to the host computer
// ALLWR> will send all zone data to the host computer
//
// Command
// 01234567 
// [ALLRD?]
// [ALLWR>]
//
// Return from microcontroller
// [ALLRD* A A A A A A A A A A A A B B B B B B B B B B B B]
// 000000000011111111112222222222333333333344444444445555555555
// 012345678901234567890123456789012345678901234567890123456789

// Write to microcontroller
// [ALLWR> A A A A A A A A A A A A B B B B B B B B B B B B]
// 000000000011111111112222222222333333333344444444445555555555
// 012345678901234567890123456789012345678901234567890123456789
//
void CommandProcessAllZone(unsigned char localSource)
{
    unsigned char n;
    unsigned char i;
//    unsigned char localBuffer[TX_BUFFER_SIZE];
    
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
    {
        // Read data from microcontroller
        if(cmdBuffer[2]=='L' && cmdBuffer[3]=='L' && cmdBuffer[4]=='R' && cmdBuffer[5]=='D' && cmdBuffer[6]=='?')  // error and format checking
        {
//            UARTCharPut(UART1_BASE,'+');                                        // @@@ DJP, remove me

            txBuffer[0] = '[';
            txBuffer[1] = 'A';
            txBuffer[2] = 'L';
            txBuffer[3] = 'L';
            txBuffer[4] = 'R';
            txBuffer[5] = 'D';
            txBuffer[6] = '*';
            txBuffer[7] = ' ';

            i = 8;
            for(n=1; n<13; n++)
            {
                txBuffer[i] = hexArray[globalZoneArrayA[n]];                    // get value and convert 0-10 decimal to '0'-'A' ascii
                i++;
                txBuffer[i] = SPACE;
                i++;
            }
            for(n=1; n<13; n++)
            {
                txBuffer[i] = hexArray[globalZoneArrayB[n]];                    // get value and convert 0-10 decimal to '0'-'A' ascii
                i++;
                txBuffer[i] = SPACE;
                i++;
            }
            i--;                                                                // eliminate last space from last run of "for loop"
            txBuffer[i] = ']';                                                  // indicate end of string          
            i++;
            txBuffer[i] = NULL;                                                 // terminate the string          
                        
            if(localSource == 'D') 
            {
                SendStringDebug(txBuffer); 
            }
            if(localSource == 'B') 
            {
                SendStringBLE(txBuffer);
            }
        }
        // write data to the microcontroller
        else if(cmdBuffer[2]=='L' && cmdBuffer[3]=='L' && cmdBuffer[4]=='W' && cmdBuffer[5]=='R' && cmdBuffer[6]=='>')  // error and format checking
        {         
            globalZoneArrayA[1] =  ConvertAsciiToDecimal(cmdBuffer[8]);                          // convert from ascii and store in RAM
            globalZoneArrayA[2] =  ConvertAsciiToDecimal(cmdBuffer[10]);                         
            globalZoneArrayA[3] =  ConvertAsciiToDecimal(cmdBuffer[12]);
            globalZoneArrayA[4] =  ConvertAsciiToDecimal(cmdBuffer[14]);
            globalZoneArrayA[5] =  ConvertAsciiToDecimal(cmdBuffer[16]);
            globalZoneArrayA[6] =  ConvertAsciiToDecimal(cmdBuffer[18]);
            globalZoneArrayA[7] =  ConvertAsciiToDecimal(cmdBuffer[20]);
            globalZoneArrayA[8] =  ConvertAsciiToDecimal(cmdBuffer[22]);
            globalZoneArrayA[9] =  ConvertAsciiToDecimal(cmdBuffer[24]);
            globalZoneArrayA[10] = ConvertAsciiToDecimal(cmdBuffer[26]);
            globalZoneArrayA[11] = ConvertAsciiToDecimal(cmdBuffer[28]);
            globalZoneArrayA[12] = ConvertAsciiToDecimal(cmdBuffer[30]);
            
            globalZoneArrayB[1] =  ConvertAsciiToDecimal(cmdBuffer[32]);
            globalZoneArrayB[2] =  ConvertAsciiToDecimal(cmdBuffer[34]);
            globalZoneArrayB[3] =  ConvertAsciiToDecimal(cmdBuffer[36]);
            globalZoneArrayB[4] =  ConvertAsciiToDecimal(cmdBuffer[38]);
            globalZoneArrayB[5] =  ConvertAsciiToDecimal(cmdBuffer[40]);
            globalZoneArrayB[6] =  ConvertAsciiToDecimal(cmdBuffer[42]);
            globalZoneArrayB[7] =  ConvertAsciiToDecimal(cmdBuffer[44]);
            globalZoneArrayB[8] =  ConvertAsciiToDecimal(cmdBuffer[46]);
            globalZoneArrayB[9] =  ConvertAsciiToDecimal(cmdBuffer[48]);
            globalZoneArrayB[10] = ConvertAsciiToDecimal(cmdBuffer[50]);
            globalZoneArrayB[11] = ConvertAsciiToDecimal(cmdBuffer[52]);
            globalZoneArrayB[12] = ConvertAsciiToDecimal(cmdBuffer[54]);
           
            SendAck(localSource);
            
            CommandProcessCommit(localSource);                                  // commit RAM to EEPROM.  New as of 1.0.3.
        }
    }   
}
// ****************************************************************************
// ************************** CommandProcessSetZone ***************************
// ****************************************************************************
//
// This function will change the zone/setting values.  Only changes the static value.  These
// values must be committed to EEPROM with another command.
//
// 0123456789 
// [S:1,IA:8]
//
void CommandProcessSetZone(unsigned char localSource)
{
    unsigned char localZone; 
    unsigned char localSetting;
    
    if (flagBoardType == BOARD_CU1)                                             // are we CU1 (controller)
    {
        localZone = ConvertAsciiToDecimal(cmdBuffer[3]);                      // what zone are we working with and convert ascii to decimal
        localSetting = ConvertAsciiToDecimal(cmdBuffer[8]);                   // what setting? and convert ascii to decimal
        if (localZone >= 1 && localZone <= 8)                                   // check zone value, must be between 1 and 8
        {
            if (localSetting <= 9)                                              // check setting value, must be between 0 and 9
            {
                if(cmdBuffer[2]==':' && cmdBuffer[4]==',' && cmdBuffer[5]=='I') // error and format checking
                {
                    if(cmdBuffer[6]=='A')                                       // are we injector A?
                    {
                        globalZoneArrayA[localZone] = localSetting;
                        SendAck(localSource);
                    }
                    else if (cmdBuffer[6]=='B')
                    {
                        globalZoneArrayB[localZone] = localSetting;
                        SendAck(localSource);
                    }
                }
            }
        }
    }        
}
// ****************************************************************************
// ************************** CommandProcessTemp ******************************
// ****************************************************************************
//
// [00000000011111111112222
// 012345678901234567890123
// [INQTEMPALL?]
// [INQTEMPALL*xxxxx,xxxxx]
//
void CommandProcessTemp(void)
{
//    unsigned int local10k;
//    unsigned int local1k;
//    unsigned int local100;
//    unsigned int local10;
//    unsigned int local1;
    
    
    // RU1 receiving data request
    if (flagBoardType == BOARD_RU1)                                             // are we RU1 (remote)
    {
        if(cmdBuffer[11]=='?')                                                  // command from CU1
        {
            if (cmdBuffer[8]=='A' && cmdBuffer[9]=='L' && cmdBuffer[10]=='L')
            {
                // form string to send to CU1
                txBuffer[0] =  '[';
                txBuffer[1] =  'I';
                txBuffer[2] =  'N';
                txBuffer[3] =  'Q';
                txBuffer[4] =  'T';
                txBuffer[5] =  'E';
                txBuffer[6] =  'M';
                txBuffer[7] =  'P';
                txBuffer[8] =  'A';
                txBuffer[9] =  'L';
                txBuffer[10] = 'L';            
                txBuffer[11] = '*';                                                 // reply '*'

                HexToDecimalStringIntWithPad(stringBuffer, globalTempNowInputRU1);  // convert latest temperature data to ascii          
                txBuffer[12] = stringBuffer[0];                                     // ascii conversion of decimal globalFlowCount
                txBuffer[13] = stringBuffer[1];
                txBuffer[14] = stringBuffer[2];
                txBuffer[15] = stringBuffer[3];
                txBuffer[16] = stringBuffer[4];

                txBuffer[17] = ',';
                
                HexToDecimalStringIntWithPad(stringBuffer, globalTempMaxInputRU1);      // convert max temperature data to ascii          
                txBuffer[18] = stringBuffer[0];
                txBuffer[19] = stringBuffer[1];
                txBuffer[20] = stringBuffer[2];
                txBuffer[21] = stringBuffer[3];
                txBuffer[22] = stringBuffer[4];
                txBuffer[23] = ']';                                                 // terminate            
                txBuffer[24] = NULL;                                                // terminate            
                SendStringDebug(txBuffer);
                SendStringRS485NoChecking(txBuffer);  
            }            
        }
    }
    // CU1 receiving data from RU1
    else if (flagBoardType == BOARD_CU1)                                        // are we CU1 (remote)
    {
        if(cmdBuffer[11]=='*')                                                   
        {
            if (cmdBuffer[8]=='A' && cmdBuffer[9]=='L' && cmdBuffer[10]=='L' && cmdBuffer[17]==',')                
            {
                globalCU1TemperatureString[0] = '[';
                globalCU1TemperatureString[1] = 'T';
                globalCU1TemperatureString[2] = 'E';
                globalCU1TemperatureString[3] = 'M';
                globalCU1TemperatureString[4] = 'P';
                globalCU1TemperatureString[5] = 'A';
                globalCU1TemperatureString[6] = 'L';
                globalCU1TemperatureString[7] = 'L';
                globalCU1TemperatureString[8] = '*';
                globalCU1TemperatureString[9] = cmdBuffer[14];
                globalCU1TemperatureString[10] = cmdBuffer[15];
                globalCU1TemperatureString[11] = cmdBuffer[16];
                globalCU1TemperatureString[12] = ',';
                globalCU1TemperatureString[13] = cmdBuffer[20];
                globalCU1TemperatureString[14] = cmdBuffer[21];
                globalCU1TemperatureString[15] = cmdBuffer[22];
                globalCU1TemperatureString[16] = ']';
                globalCU1TemperatureString[17] = NULL;
                globalCU1TemperatureString[18] = NULL;
                globalCU1TemperatureString[19] = NULL;
            }
        }       
    }
}
// ****************************************************************************
// ******************** CommandProcessInjectorCount ***************************
// ****************************************************************************
//
// 00000000001111111111222222222
// 01234567890123456789012345678
// [INQINJECT?]
// [INQINJECT*xxxxxxx,xxxxxxxx]
//
void CommandProcessInjectorCount(void)
{
    
    // RU1 receiving data request
    if (flagBoardType == BOARD_RU1)                                             // are we RU1 (remote)
    {
        if(cmdBuffer[10]=='?')                                                  // command from CU1
        {
            txBuffer[0] =  '[';
            txBuffer[1] =  'I';
            txBuffer[2] =  'N';
            txBuffer[3] =  'Q';
            txBuffer[4] =  'I';
            txBuffer[5] =  'N';
            txBuffer[6] =  'J';
            txBuffer[7] =  'E';
            txBuffer[8] =  'C';
            txBuffer[9] =  'T';
            txBuffer[10] = '*';                                                 // reply '*'

            LongToHexString (stringBuffer, globalInjectionCountA);
            txBuffer[11] = stringBuffer[0];                                     // ascii conversion of decimal globalFlowCount
            txBuffer[12] = stringBuffer[1];
            txBuffer[13] = stringBuffer[2];
            txBuffer[14] = stringBuffer[3];
            txBuffer[15] = stringBuffer[4];
            txBuffer[16] = stringBuffer[5];
            txBuffer[17] = stringBuffer[6];
            txBuffer[18] = stringBuffer[7];

            txBuffer[19] = ',';
            
            LongToHexString (stringBuffer, globalInjectionCountB);
            txBuffer[20] = stringBuffer[0];
            txBuffer[21] = stringBuffer[1];
            txBuffer[22] = stringBuffer[2];
            txBuffer[23] = stringBuffer[3];
            txBuffer[24] = stringBuffer[4];
            txBuffer[25] = stringBuffer[5];
            txBuffer[26] = stringBuffer[6];
            txBuffer[27] = stringBuffer[7];
            txBuffer[28] = ']';                                                 // terminate            
            txBuffer[29] = NULL;                                                // terminate            
            SendStringDebug(txBuffer);
            SendStringRS485NoChecking(txBuffer);  
       }
    }
    // CU1 receiving data from RU1
    else if (flagBoardType == BOARD_CU1)                                        // are we CU1 (remote)
    {
        if(cmdBuffer[10]=='*')                                                   
        {
            StringCopy(globalCU1InjectionString, cmdBuffer);
//            SendStringDebug("Inject data back from RU1");
//            SendStringDebug(globalInjectionString);
        }       
    }
}
// ****************************************************************************
// ************************** CommandProcessCurrent ***************************
// ****************************************************************************
//
// 00000000001111111111222222222
// 01234567890123456789012345678
// [INQCURRENT?]
// [INQCURRENT*xxx,xxx,xxx,xxx,xxx,xxx,xxx]                                     
//
void CommandProcessCurrent(void)
{   
    // RU1 receiving data request
    // load data into string in ascii form
    if (flagBoardType == BOARD_RU1)                                             // are we RU1 (remote)
    {
        if(cmdBuffer[11]=='?')                                                  // command from CU1
        {
            txBuffer[0] =  '[';
            txBuffer[1] =  'I';
            txBuffer[2] =  'N';
            txBuffer[3] =  'Q';
            txBuffer[4] =  'C';
            txBuffer[5] =  'U';
            txBuffer[6] =  'R';
            txBuffer[7] =  'R';
            txBuffer[8] =  'E';
            txBuffer[9] =  'N';
            txBuffer[10] = 'T';
            txBuffer[11] = '*';                                                 // reply '*'
            txBuffer[12] = hexArray[((globalCurrentAverage >> 8) & 0x0F)];
            txBuffer[13] = hexArray[((globalCurrentAverage >> 4) & 0x0F)];
            txBuffer[14] = hexArray[globalCurrentAverage & 0x0F]; 
            txBuffer[15] = ',';            
            txBuffer[16] = hexArray[((globalCurrentSol1 >> 8) & 0x0F)];
            txBuffer[17] = hexArray[((globalCurrentSol1 >> 4) & 0x0F)];
            txBuffer[18] = hexArray[globalCurrentSol1 & 0x0F]; 
            txBuffer[19] = ',';
            txBuffer[20] = hexArray[((globalCurrentSol2 >> 8) & 0x0F)];
            txBuffer[21] = hexArray[((globalCurrentSol2 >> 4) & 0x0F)];
            txBuffer[22] = hexArray[globalCurrentSol2 & 0x0F];
            txBuffer[23] = ',';
            txBuffer[24] = hexArray[((globalCurrentSol3 >> 8) & 0x0F)];
            txBuffer[25] = hexArray[((globalCurrentSol3 >> 4) & 0x0F)];
            txBuffer[26] = hexArray[globalCurrentSol3 & 0x0F];
            txBuffer[27] = ',';
            txBuffer[28] = hexArray[((globalCurrentInjA >> 8) & 0x0F)];
            txBuffer[29] = hexArray[((globalCurrentInjA >> 4) & 0x0F)];
            txBuffer[30] = hexArray[globalCurrentInjA & 0x0F];
            txBuffer[31] = ',';
            txBuffer[32] = hexArray[((globalCurrentInjB >> 8) & 0x0F)];
            txBuffer[33] = hexArray[((globalCurrentInjB >> 4) & 0x0F)];
            txBuffer[34] = hexArray[globalCurrentInjB & 0x0F];
            txBuffer[35] = ',';
            txBuffer[36] = hexArray[((globalCurrentFan >> 8) & 0x0F)];
            txBuffer[37] = hexArray[((globalCurrentFan >> 4) & 0x0F)];
            txBuffer[38] = hexArray[globalCurrentFan & 0x0F];
            txBuffer[39] = ']';                                                           
            txBuffer[40] = NULL;                                                // terminate            
            SendStringDebug(txBuffer);
            SendStringRS485NoChecking(txBuffer);  
       }
    }
    // CU1 receiving data from RU1
    // load variables with hex value converted from ascii
    else if (flagBoardType == BOARD_CU1)                                        // are we CU1 (remote)
    {
        if(cmdBuffer[11]=='*')                                                   
        {          
            globalCurrentAverage = ConvertAsciiToDecimal(cmdBuffer[12]);
            globalCurrentAverage = globalCurrentAverage << 4;
            globalCurrentAverage = ConvertAsciiToDecimal(cmdBuffer[13]) | globalCurrentAverage;
            globalCurrentAverage = globalCurrentAverage << 4;
            globalCurrentAverage = ConvertAsciiToDecimal(cmdBuffer[14]) | globalCurrentAverage;
            
            globalCurrentSol1 = ConvertAsciiToDecimal(cmdBuffer[16]);
            globalCurrentSol1 = globalCurrentSol1 << 4;
            globalCurrentSol1 = ConvertAsciiToDecimal(cmdBuffer[17]) | globalCurrentSol1;
            globalCurrentSol1 = globalCurrentSol1 << 4;
            globalCurrentSol1 = ConvertAsciiToDecimal(cmdBuffer[18]) | globalCurrentSol1;

            globalCurrentSol2 = ConvertAsciiToDecimal(cmdBuffer[20]);
            globalCurrentSol2 = globalCurrentSol2 << 4;
            globalCurrentSol2 = ConvertAsciiToDecimal(cmdBuffer[21]) | globalCurrentSol2;
            globalCurrentSol2 = globalCurrentSol2 << 4;
            globalCurrentSol2 = ConvertAsciiToDecimal(cmdBuffer[22]) | globalCurrentSol2;

            globalCurrentSol3 = ConvertAsciiToDecimal(cmdBuffer[24]);
            globalCurrentSol3 = globalCurrentSol3 << 4;
            globalCurrentSol3 = ConvertAsciiToDecimal(cmdBuffer[25]) | globalCurrentSol3;
            globalCurrentSol3 = globalCurrentSol3 << 4;
            globalCurrentSol3 = ConvertAsciiToDecimal(cmdBuffer[26]) | globalCurrentSol3;

            globalCurrentInjA = ConvertAsciiToDecimal(cmdBuffer[28]);
            globalCurrentInjA = globalCurrentInjA << 4;
            globalCurrentInjA = ConvertAsciiToDecimal(cmdBuffer[29]) | globalCurrentInjA;
            globalCurrentInjA = globalCurrentInjA << 4;
            globalCurrentInjA = ConvertAsciiToDecimal(cmdBuffer[30]) | globalCurrentInjA;
            
            globalCurrentInjB = ConvertAsciiToDecimal(cmdBuffer[32]);
            globalCurrentInjB = globalCurrentInjB << 4;
            globalCurrentInjB = ConvertAsciiToDecimal(cmdBuffer[33]) | globalCurrentInjB;
            globalCurrentInjB = globalCurrentInjB << 4;
            globalCurrentInjB = ConvertAsciiToDecimal(cmdBuffer[34]) | globalCurrentInjB;

            globalCurrentFan = ConvertAsciiToDecimal(cmdBuffer[36]);
            globalCurrentFan = globalCurrentFan << 4;
            globalCurrentFan = ConvertAsciiToDecimal(cmdBuffer[37]) | globalCurrentFan;
            globalCurrentFan = globalCurrentFan << 4;
            globalCurrentFan = ConvertAsciiToDecimal(cmdBuffer[38]) | globalCurrentFan;   
            
            StringCopy(globalCU1CurrentString, cmdBuffer);
        }       
    }
}
// ****************************************************************************
// ************************** CommandProcessFChooser **************************
// ****************************************************************************
//
// 0123456789
// [FLOW?]          CU1 sends this to RU1 from the main scheduler loop
// [FLOW*]xxxxx     RU1 sends this to CU1 as a reply.  xxxxx is ASCII data front padded with '0'
// [FANON]
// [FANOFF]
//
void CommandProcessFChooser(unsigned char localSource)
{
    unsigned int local10k;
    unsigned int local1k;
    unsigned int local100;
    unsigned int local10;
    unsigned int local1;
    
    // RU1 receiving data request
    if (flagBoardType == BOARD_RU1)                                             // are we RU1 (remote)
    {
        if(cmdBuffer[2]=='L' && cmdBuffer[3]=='O' && cmdBuffer[4]=='W' && cmdBuffer[5]=='?')            // error and format checking
        {
            HexToDecimalStringIntWithPad(stringBuffer,globalFlowInputRU1);         // convert latest flow data to ascii          
            txBuffer[0] = '[';
            txBuffer[1] = 'F';
            txBuffer[2] = 'L';
            txBuffer[3] = 'O';
            txBuffer[4] = 'W';
            txBuffer[5] = '*';                                                  // reply '*'
            txBuffer[6] = stringBuffer[0];                                      // ascii conversion of decimal globalFlowCount
            txBuffer[7] = stringBuffer[1];
            txBuffer[8] = stringBuffer[2];
            txBuffer[9] = stringBuffer[3];
            txBuffer[10] = stringBuffer[4];
            txBuffer[11] = ']';                                                 // terminate            
            txBuffer[12] = NULL;                                                 // terminate            
            SendStringRS485NoChecking(txBuffer);                            
        }
        else if (cmdBuffer[2]=='A' && cmdBuffer[3]=='N' && cmdBuffer[4]=='O' && cmdBuffer[5]=='N')
        {
            flagRU1FanOverride = 1;                                             // turn on fan override, volatile will be cleared on boot
        }
        else if (cmdBuffer[2]=='A' && cmdBuffer[3]=='N' && cmdBuffer[4]=='O' && cmdBuffer[5]=='F' && cmdBuffer[6]=='F')
        {
            flagRU1FanOverride = 0;                                             // turn off fan override
        }
    }
    // CU1 receiving data from RU1
    else if (flagBoardType == BOARD_CU1)                                        // are we CU1 (remote)
    {
        if(cmdBuffer[2]=='L' && cmdBuffer[3]=='O' && cmdBuffer[4]=='W' && cmdBuffer[5]=='*')            // error and format checking
        {
            local10k = (cmdBuffer[6]-0x30) * 10000;
            local1k = (cmdBuffer[7]-0x30) * 1000;
            local100 = (cmdBuffer[8]-0x30) * 100;
            local10 = (cmdBuffer[9]-0x30) * 10;
            local1 = (cmdBuffer[10]-0x30);               
            globalFlowInputCU1 = local10k + local1k + local100 + local10 + local1;

            globalFlowStringCU1[0] = cmdBuffer[6];
            globalFlowStringCU1[1] = cmdBuffer[7];
            globalFlowStringCU1[2] = cmdBuffer[8];
            globalFlowStringCU1[3] = cmdBuffer[9];
            globalFlowStringCU1[4] = cmdBuffer[10];
            globalFlowStringCU1[5] = NULL;
        }  
        else if (cmdBuffer[2]=='A' && cmdBuffer[3]=='N' && cmdBuffer[4]=='O' && cmdBuffer[5]=='N')
        {
            txBuffer[0] = '[';
            txBuffer[1] = 'F';
            txBuffer[2] = 'A';
            txBuffer[3] = 'N';
            txBuffer[4] = 'O';
            txBuffer[5] = 'N';                                                  
            txBuffer[6] = ']';                                      
            txBuffer[7] = NULL;
            SendStringRS485NoChecking(txBuffer);
            SendAck(localSource);                
        }
        else if (cmdBuffer[2]=='A' && cmdBuffer[3]=='N' && cmdBuffer[4]=='O' && cmdBuffer[5]=='F' && cmdBuffer[6]=='F')
        {
            txBuffer[0] = '[';
            txBuffer[1] = 'F';
            txBuffer[2] = 'A';
            txBuffer[3] = 'N';
            txBuffer[4] = 'O';
            txBuffer[5] = 'F';                                                  
            txBuffer[6] = 'F';                                      
            txBuffer[7] = ']';
            txBuffer[8] = NULL;
            SendStringRS485NoChecking(txBuffer);                            
            SendAck(localSource);                
        }        
    }
}
// ****************************************************************************
// ********************** CommandProcessSerialZoneInput ***********************
// ****************************************************************************
//
// Gen2 boards now have:
// 12 zones and received zone data can now be ASCII 1,2,3,4,5,6,7,8,9,A,B,C
// 10 injector settings which can be 0,1,2,3,4,5,6,7,8,9,A
//
// The suffix 'P' or 'E' is added which signals the receiving RU1 that the command was sent by a CU1 in "expansion mode".  Thus the RU1
// should not replicate the solenoid with this command.
//
// 00000000001111111
// 01234567890123456
// [Z:1,IA:4,IB:5,P]    Example Zone1, Injector A = 4, Injector B = 5, Primary
//    x                 Zone input 1-C (1-12 decimal)
//         x            Injector A value = 0-A (0-10 decimal)
//              x       Injector B value = 0-A (0-10 decimal)
//                x     P=Primary or E=Expansion
//
void CommandProcessSerialZoneInput(unsigned char localSource)
{ 
    if (flagBoardType == BOARD_RU1)                                             // are we RU1 (remote)
    {
        if(cmdBuffer[2]==':' && cmdBuffer[4]==',' && cmdBuffer[5]=='I' && cmdBuffer[7]==':' && cmdBuffer[9]==',' && cmdBuffer[10]=='I' &&  cmdBuffer[12]==':' && cmdBuffer[14]==',')         // error and format checking
        {
            globalActiveTimeCount = 0;                                          // reset timeout counter, a zone command has been recieved

            globalActiveZone = ConvertAsciiToDecimal(cmdBuffer[3]);                     // zones are 1 based, convert to decimal
            globalZoneArrayA[globalActiveZone] = ConvertAsciiToDecimal(cmdBuffer[8]);   // store the Injector A value
            globalZoneArrayB[globalActiveZone] = ConvertAsciiToDecimal(cmdBuffer[13]);  // store the Injector B value                
            SendAck(localSource);

            if (cmdBuffer[15]=='E')                                             // command received from 'E' expansion CU1
            {
                flagExpand = 1;
            }
            if (cmdBuffer[15]=='P')                                             // command received from 'P' primary CU1
            {
                flagExpand = 0;
            }           
        }
    }
}
// ****************************************************************************
// ************************* ConvertAsciiToDecimal ****************************
// ****************************************************************************
//
// This function converts an ascii 0-9,A-F to decimal 0-15
//
unsigned char ConvertAsciiToDecimal(unsigned char inputVar)
{
    unsigned char returnVar;
    
    if (inputVar >= '0' && inputVar <= '9')                                     // check range, is ascii 0-9?
    {
        returnVar = inputVar - 0x30;                                            // convert to decimal 
    }
    else if (inputVar >= 'A' && inputVar <= 'F')                                // check range, is ascii A-F?
    {
        returnVar = inputVar - 0x37;                                            // convert ASCII 'A' to decimal 10, 'B' to 11 etc
    }
    return returnVar;   
}
// ****************************************************************************
// *************************** OutputInjector *********************************
// ****************************************************************************
//
//
void OutputInjector(unsigned char zoneIn)
{ 
    unsigned char localInjValA;
    unsigned char localInjValB;
    
/*    
    stringDebug[0] = 'I';    
    stringDebug[1] = ':';
    stringDebug[2] = hexArray[zoneIn >> 4];
    stringDebug[3] = hexArray[zoneIn & 0x0F];
    stringDebug[4] = ' ';
    stringDebug[5] = hexArray[globalZoneArrayA[zoneIn] >> 4];
    stringDebug[6] = hexArray[globalZoneArrayA[zoneIn] & 0x0F];
    stringDebug[7] = ' ';
    stringDebug[8] = hexArray[globalZoneArrayB[zoneIn] >> 4];
    stringDebug[9] = hexArray[globalZoneArrayB[zoneIn] & 0x0F];
    stringDebug[10] = CR;
    stringDebug[11] = LF;
    stringDebug[12] = 0x00;
    SendStringDebug(stringDebug);
*/   
  
    if(flagTestInjectorA)
    {
        localInjValA = 10;                                                      // simulate globalZoneArrayA[zoneIn], Zone1 with injector value set to 10;
        zoneIn = 1;                                                             // simulate zone1 input
    }
    else
    {
        localInjValA = globalZoneArrayA[zoneIn];                                // normal programmed value
    }

    if(flagTestInjectorB)
    {
        localInjValB = 10;                                                      // simulate globalZoneArrayB[zoneIn], Zone1 with injector value set to 10;
        zoneIn = 1;                                                             // simulate zone1 input
    }
    else
    {
        localInjValB = globalZoneArrayB[zoneIn];                                // normal programmed value
    }
  

    // *** INJECTOR A ***
//    if (globalZoneArrayA[zoneIn])                                             // if active zone InjectorA not zero execute
    if (localInjValA)                                                           // if active zone InjectorA not zero execute
    {
//        UARTCharPut(UART1_BASE,'A');                                          // @@@ Debug
//        LEDDualDecimal(zoneIn,DECIMAL_ONES_ON);                                 // yes injection, indicate with decimal point
        LEDDualDecimal(zoneIn,DECIMAL_TENS_ON);                                 // DJP8 yes injection, indicate with decimal point
        globalInjectionTimeCountA++;                                            // increment at 0.01s (frequency of this routine being called)

        if (globalInjectionTimeCountA == 1)                                     // DJPQ3+
        {
            if (flagBoardType == BOARD_CU1)                                     // are we a CU1 (control) or RU1 (remote)
            {
                // no injector CU1 Gen2
            }
            else if (flagBoardType == BOARD_RU1)                                // is RU1 (remote)
            {
                GPIOPinWrite(PORT_B, PIN_LED2, 0);                              // turn on injection LED (low asserted)  
                GPIOPinWrite(PORT_A, PIN_RU1_INJA, PIN_RU1_INJA);               // turn on AC optocoupler (high asserted) 
                globalInjectionCountA++;                                        // count how many times we have done this
            }               
        }

        // solenoid off time + on time has been reached, restart the count
        if (globalInjectionTimeCountA == SOLENOID_ON_TIME)                       // DJPQ3+
        {
            TurnOffInjectorA();
            flagTestInjectorA = 0;                                              // clear the test injection flag 
        }
        //  this is the time between activations + injector time on
//        if (globalInjectionTimeCountA >= (pulseIntervalOff[globalZoneArrayA[zoneIn]] + SOLENOID_ON_TIME)) // DJPQ3+
        if (globalInjectionTimeCountA >= (pulseIntervalOff[localInjValA] + SOLENOID_ON_TIME)) // DJPQ3+
        {
            globalInjectionTimeCountA=0;                                        // DJPQ3+ reset timer
        }
        flagClearLEDOnce = 1;                                                   // set the flag
    }
    else
    {
//        if (zoneIn != 0) LEDDualDecimal(zoneIn,DECIMAL_ONES_OFF);               // display zone input with no injection on decimal points
        if (zoneIn != 0) 
        {
            if (localInjValB)                                                   // DJP8 was there B injection? 
            {
                flagDecimalOnes = 1;                                            // turn on ONES(B) decimal point
                flagDecimalTens = 0;                                            // turn off TENS(A) decimal point
            }
            else
            {
                flagDecimalOnes = 0;                                            // turn off ONES(B) decimal point
                flagDecimalTens = 0;                                            // turn off TENS(A) decimal point
            }          
            LEDDualDecimal(zoneIn,DECIMAL_BYPASS);                              // DJP8 display zone input with no injection on decimal points
        }
        TurnOffInjectorA();            
        GPIOPinWrite(PORT_B, PIN_LED2, PIN_LED2);                               // turn off injection LED (low asserted)                             
    }
    // *** INJECTOR B ***
//    if(globalZoneArrayB[zoneIn])                                              // if active zone Injector B not zero execute
    if(localInjValB)                                                            // if active zone Injector B not zero execute
    {
//        UARTCharPut(UART1_BASE,'B');                                           // @@@ Debug
//        LEDDualDecimal(zoneIn,DECIMAL_TENS_ON);                                 // yes injection, indicate with decimal point on "tens" 7 seg display
        LEDDualDecimal(zoneIn,DECIMAL_ONES_ON);                                 // DJP8 yes injection, indicate with decimal point on "tens" 7 seg display
        globalInjectionTimeCountB++;                                            // increment at 0.01s (frequency of this routine being called)
        if (globalInjectionTimeCountB == 1+SOLENOID_OFFSET_B) 
        {
            if (flagBoardType == BOARD_RU1)                                     // is RU1 (remote), CU1 does not have 2nd injector B
            {
                GPIOPinWrite(PORT_A, PIN_RU1_INJB, PIN_RU1_INJB);               // turn on AC optocoupler (high asserted)
                GPIOPinWrite(PORT_B, PIN_LED2, 0);                              // turn on injection LED (low asserted)                  
                globalInjectionCountB++;                                        // count how many times we have done this
            }  
        }
        // solenoid off time + on time has been reached, restart the count
        if (globalInjectionTimeCountB == SOLENOID_ON_TIME + SOLENOID_OFFSET_B)  // DJPQ3+
        {
            TurnOffInjectorB();
            flagTestInjectorB = 0;                                              // clear the test injection flag     
        } 
        //  this is the time between activations + injector time on
        if (globalInjectionTimeCountB >= (pulseIntervalOff[localInjValB] + SOLENOID_ON_TIME + SOLENOID_OFFSET_B)) // DJPQ3+
        {
            globalInjectionTimeCountB=SOLENOID_OFFSET_B;                        // DJPQ3+ reset timer
        }
        flagClearLEDOnce = 1;                                                   // set the flag        
    }
    else                                                                        // No B injection
    {
        if (zoneIn != 0) 
        {  
            if (localInjValA)                                                   // DJP8 was there A injection? 
            {
                flagDecimalOnes = 0;                                            // turn off ONES(B) decimal point
                flagDecimalTens = 1;                                            // turn on TENS(A) decimal point
            }
            else
            {
                flagDecimalOnes = 0;                                            // turn off ONES(B) decimal point
                flagDecimalTens = 0;                                            // turn off TENS(A) decimal point
            }          
            LEDDualDecimal(zoneIn,DECIMAL_BYPASS);                              // DJP8 display zone input with no injection on decimal points
        }
        TurnOffInjectorB();
        GPIOPinWrite(PORT_B, PIN_LED2, PIN_LED2);                               // turn off injection LED (low asserted)                             
    }
    // *** NO INJECTION ***
    if(zoneIn == 0)
    {
        if (flagClearLEDOnce)
        {
            LEDDualClear();                                         
            flagClearLEDOnce = 0;                                               // clear the flag
        }
        TurnOffInjectorAB();
    }
    else
    {
        flagClearLEDOnce = 1;                                                   // set the flag
    }
}
// ****************************************************************************
// *************************** OutputSolenoid *********************************
// ****************************************************************************
//
// This function controls the solenoids attached to the RU1 (remote), these are the solenoids 
// that are being replicated from sprinkler controller to the CU1 over RS485 to the RU1.  These are
// not the injectors.  Only 3 solenoids are replicated, because this system needs 4 wires.
//
// Note that the RU1 (remote) controls the source side of the LED on the optocouplers, where the CU1
// controls the sink side of the LED on the optocoupler.
//
void OutputSolenoid(void)
{ 
    if (flagBoardType == BOARD_CU1)                                             // are we a CU1 (control) or RU1 (remote)
    {
    // no solenoids are controlled by CU1 Gen2   
    }
    else if (flagBoardType == BOARD_RU1)                                        // is RU1 (remote)
    {
        if (globalActiveZone == 1)
        {
            if (flagExpand == 0)                                                // flagExpand is not set, go ahead and replicate solenoid
            {
                GPIOPinWrite(PORT_A, PIN_RU1_SOL1, PIN_RU1_SOL1);               // turn on opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL2, 0);                          // turn off replicated solenoid 2 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL3, 0);                          // turn off replicated solenoid 3 opto LED (high asserted)
            }
            else
            {
                GPIOPinWrite(PORT_A, PIN_RU1_SOL1, 0);                          // turn off replicated solenoid 1 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL2, 0);                          // turn off replicated solenoid 2 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL3, 0);                          // turn off replicated solenoid 3 opto LED (high asserted)
            }
        }
        else if (globalActiveZone == 2)
        {
            if (flagExpand == 0)                                                // flagExpand is not set, go ahead and replicate solenoid
            {
                GPIOPinWrite(PORT_A, PIN_RU1_SOL1, 0);                          // turn off replicated solenoid 1 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL2, PIN_RU1_SOL2);               // turn on opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL3, 0);                          // turn off replicated solenoid 3 opto LED (high asserted)
            }
            else
            {
                GPIOPinWrite(PORT_A, PIN_RU1_SOL1, 0);                          // turn off replicated solenoid 1 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL2, 0);                          // turn off replicated solenoid 2 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL3, 0);                          // turn off replicated solenoid 3 opto LED (high asserted)
            }
        }
        else if (globalActiveZone == 3)
        {
            if (flagExpand == 0)                                                // flagExpand is not set, go ahead and replicate solenoid
            {
                GPIOPinWrite(PORT_A, PIN_RU1_SOL1, 0);                          // turn off replicated solenoid 1 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL2, 0);                          // turn off replicated solenoid 2 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL3, PIN_RU1_SOL3);               // turn on opto LED (high asserted)
            }
            else
            {
                GPIOPinWrite(PORT_A, PIN_RU1_SOL1, 0);                          // turn off replicated solenoid 1 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL2, 0);                          // turn off replicated solenoid 2 opto LED (high asserted)
                GPIOPinWrite(PORT_A, PIN_RU1_SOL3, 0);                          // turn off replicated solenoid 3 opto LED (high asserted)
            }
        }
        else
        {
            GPIOPinWrite(PORT_A, PIN_RU1_SOL1, 0);                              // turn off replicated solenoid 1 opto LED (high asserted)
            GPIOPinWrite(PORT_A, PIN_RU1_SOL2, 0);                              // turn off replicated solenoid 2 opto LED (high asserted)
            GPIOPinWrite(PORT_A, PIN_RU1_SOL3, 0);                              // turn off replicated solenoid 3 opto LED (high asserted)
        }                  
    }
}
// ****************************************************************************
// *************************** CommandProcessVersion **************************
// ****************************************************************************
//
// This function sends version and serial number out a UART defined by the passed variable
//
// Looks like this:
//
// [Professor Wiseacres]                                                      
// [CU1]                                                                         
// [Ver: 0.3.6]                                                                  
// [SN: 00000000]                                                              
// [Speed KHz: 80000]
// [BLE Name: xxxx]
// [BLE Firmware: xxxx]
// [BLE BTA: xxxx]
//
unsigned char bleFirmwareVersion[6];
unsigned char bleModuleBTAString[13];
unsigned char bleModuleNameString[17];

void CommandProcessVersion(unsigned char localSource)
{
    if (flagBoardType == BOARD_CU1)                                             // are we a CU1 (control) or RU1 (remote)
    {
        StringCopy(txBuffer, MFG_STRING); 
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);    
        
        StringCopy(txBuffer,"[CU1 ");         
        if(flagExpand)
        {
            StringCat(txBuffer,"Expansion");
        }
        else
        {
            StringCat(txBuffer,"Primary");
        }
        StringCat(txBuffer,"]");               
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);    

        txBuffer[0] = '[';
        txBuffer[1] = 'V';
        txBuffer[2] = 'e';
        txBuffer[3] = 'r';
        txBuffer[4] = ':';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[VERSION_MAJOR];
        txBuffer[7] = '.';
        txBuffer[8] = hexArray[VERSION_MINOR];
        txBuffer[9] = '.';
        txBuffer[10] = hexArray[VERSION_ALPHA];
        txBuffer[11] = ']';                                                    // terminate with a NULL
        txBuffer[12] = NULL;                                                    // terminate with a NULL
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);    
        
        StringCopy(txBuffer, "[SN: ");
        StringCat(txBuffer, globalSerialNumberString);
        StringCat(txBuffer, "]");       
//        StringCopy(txBuffer, "[SN: ");
//        LongToHexString(stringBuffer, globalSerialNumber);
//        StringCat(txBuffer, stringBuffer);
//        StringCat(txBuffer, "]");
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);    
        
        StringCopy(txBuffer, "[Speed KHz: ");
        HexToDecimalStringInt (stringBuffer, SysCtlClockGet()/1000);       
        StringCat(txBuffer,stringBuffer);    
        StringCat(txBuffer,"]");    
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);    
                
        StringCopy(txBuffer, "[BLE Name: ");
        StringCat(txBuffer,bleModuleNameString);    
        StringCat(txBuffer,"]");    
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);    
        
        StringCopy(txBuffer, "[BLE BTA: ");
        StringCat(txBuffer,bleModuleBTAString);    
        StringCat(txBuffer,"]");    
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);    

        StringCopy(txBuffer, "[BLE Firmware: ");
        StringCat(txBuffer,bleFirmwareVersion);    
        StringCat(txBuffer,"]");    
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);           
    }
    else if (flagBoardType == BOARD_RU1)
    {
        StringCopy(txBuffer, MFG_STRING); 
        SendStringDebug(txBuffer);

        StringCopy(txBuffer, MODEL_STRING_RU1); 
        SendStringDebug(txBuffer);

        txBuffer[0] = '[';
        txBuffer[1] = 'V';
        txBuffer[2] = 'e';
        txBuffer[3] = 'r';
        txBuffer[4] = ':';
        txBuffer[5] = ' ';
        txBuffer[6] = hexArray[VERSION_MAJOR];
        txBuffer[7] = '.';
        txBuffer[8] = hexArray[VERSION_MINOR];
        txBuffer[9] = '.';
        txBuffer[10] = hexArray[VERSION_ALPHA];
        txBuffer[11] = ']';
        txBuffer[12] = NULL;                                                    // terminate with a NULL
        SendStringDebug(txBuffer);

        StringCopy(txBuffer, "[SN: ");
        StringCat(txBuffer, globalSerialNumberString);
        StringCat(txBuffer, "]");       
//        StringCopy(txBuffer, "[SN: ");
//        LongToHexString(stringBuffer, globalSerialNumber);
//        StringCat(txBuffer, stringBuffer);
//        StringCat(txBuffer, "]");
        SendStringDebug(txBuffer);
        
        StringCopy(txBuffer, "[Speed KHz: ");
        HexToDecimalStringInt (stringBuffer, SysCtlClockGet()/1000);       
        StringCat(txBuffer,stringBuffer);    
        StringCat(txBuffer,"]");    
        SendStringDebug(txBuffer);
    }
}
// ****************************************************************************
// ******************** CommandProcessEEPROMWrite ***************************
// ****************************************************************************
//
// 0123456789    
// [WD---D---]   EEPROM Write with DECIMAL values
// [WH--H--]     EEPROM Write with HEX values
//
void CommandProcessEEPROMWrite(unsigned char localSource)
{  
    unsigned char localAddress;
    unsigned char localData;
    
    // Write with decimal values
    if(cmdBuffer[2] == 'D')   
    {
        if (cmdBuffer[6] == 'D')
        {
            localAddress = Convert3ASCIIDecimalToDecimal(cmdBuffer, 3); 
            localData = Convert3ASCIIDecimalToDecimal(cmdBuffer, 7);
            I2CWriteEEPROMSingle(localAddress,localData,I2C_PHYSICAL_EEPROM);
            SendAck(localSource);                                                      
        }
    }
    // write with hex values
    else if(cmdBuffer[2] == 'H')   
    {
        if (cmdBuffer[5] == 'H')
        {
            localAddress = Convert2ASCIIHexToDecimal(3); 
            localData = Convert2ASCIIHexToDecimal(6);
            I2CWriteEEPROMSingle(localAddress,localData,I2C_PHYSICAL_EEPROM);
            SendAck(localSource);    
        }        
    }  
}
// ****************************************************************************
// *********************** CommandProcessEEPROMRead ***************************
// ****************************************************************************
//
// 012345
// [RD---]    EEPROM Read with DECIMAL values
// [RH--]    EEPROM Read with HEX values
// 
// Return value: [A:0x-- D:0x--]
//
void CommandProcessEEPROMRead(unsigned char localSource)
{
    unsigned char localRead = 0;
    unsigned char localFlag = 0;
    unsigned char localAddress = 0xf1;

    if(cmdBuffer[2] == 'D')   
    {
       localAddress = Convert3ASCIIDecimalToDecimal(cmdBuffer, 3); 
       localFlag = 1;
    }
    else if(cmdBuffer[2] == 'H')   
    {
       localAddress = Convert2ASCIIHexToDecimal(3); 
       localFlag = 1;
    }
    
    
    if (localFlag == 1)
    {
        localRead = I2CReadEEPROMSingle(localAddress,I2C_PHYSICAL_EEPROM);  
        
        txBuffer[0] = '[';
        txBuffer[1] = 'A';
        txBuffer[2] = ':';
        txBuffer[3] = '0';
        txBuffer[4] = 'x';
        txBuffer[5] = hexArray[localAddress >> 4];
        txBuffer[6] = hexArray[localAddress & 0x0F];
        txBuffer[7] = ' ';
        txBuffer[8] = 'D';
        txBuffer[9] = ':';
        txBuffer[10] = '0';
        txBuffer[11] = 'x';
        txBuffer[12] = hexArray[localRead >> 4];
        txBuffer[13] = hexArray[localRead & 0x0F];
        txBuffer[14] = ']';        
        txBuffer[15] = 0;
//        SendStringDebug(txBuffer);
        if(localSource == 'D') SendStringDebug(txBuffer);
        if(localSource == 'B') SendStringBLE(txBuffer);
        if(localSource == '4') SendStringRS485NoChecking(txBuffer);  
    }
}
// ****************************************************************************
// ******************* Convert3ASCIIDecimalToDecimal **************************
// ****************************************************************************
//
// Converts 3 ASCII characters in a string pointed to by lIndex, checks and converts 
// to a decimal.  Max number input is 999.
//
unsigned int Convert3ASCIIDecimalToDecimal(unsigned char *lString, unsigned char lIndex)
{
    unsigned int x;

    if ((cmdBuffer[lIndex] >= '0' && cmdBuffer[lIndex] <= '9') && 
        (cmdBuffer[lIndex+1] >= '0' && cmdBuffer[lIndex+1] <= '9') &&
         (cmdBuffer[lIndex+2] >= '0' && cmdBuffer[lIndex+2] <= '9'))
    {
    
        x = (lString[lIndex++]-0x30) * 100;
        x += (lString[lIndex++]-0x30) * 10;
        x += (lString[lIndex])-0x30;
    }   
    return x;
}
// ****************************************************************************
// *********************** Convert2ASCIIHexToDecimal **************************
// ****************************************************************************
//
// Converts 2 ASCII characters in a string pointed to by lIndex.  The ASCII characters
// are hexidecimal.  For example a "FF" in the string will be converted to 255.  Upper
// and lower case alpha characters are checked as well as range.
//
unsigned char Convert2ASCIIHexToDecimal(unsigned char lIndex)
{
    unsigned char highNibble;
    unsigned char lowNibble;
    
    if (cmdBuffer[lIndex] >= '0' && cmdBuffer[lIndex] <= '9')
    {
        highNibble = cmdBuffer[lIndex] - 0x30;
    }
    else if (cmdBuffer[lIndex] >= 'A' && cmdBuffer[lIndex] <= 'F')
    {
        highNibble = cmdBuffer[lIndex] - 0x37;    
    }
    highNibble <<= 4;   
    lIndex++;

    if (cmdBuffer[lIndex] >= '0' && cmdBuffer[lIndex] <= '9')
    {
        lowNibble = cmdBuffer[lIndex] - 0x30;
    }
    else if (cmdBuffer[lIndex] >= 'A' && cmdBuffer[lIndex] <= 'F')
    {
        lowNibble = cmdBuffer[lIndex] - 0x37;    
    }

    highNibble = highNibble | lowNibble;     
    
    return highNibble;
}
// ****************************************************************************
// ************************* CheckTimerDifference *****************************
// ****************************************************************************
//
// This function checks the difference between the current system timer which increments
// 1000x a second and a value of the counter at a previous moment in time.  If the
// difference has been exceeded a "1" is returned.
//
// recPrevious = system timer value stored at an earlier time
// recCompare = difference value that is being compared
//
unsigned int CheckTimerDifference (unsigned int recCompare, unsigned int recPrevious)
{
    unsigned char returnVar=0;

    if (mSecondCounter < recPrevious)					        // a wrap has occured
    {
        if ((mSecondCounter + (65535-recPrevious)) >= recCompare)
        {
                returnVar = 1;
        }
        else
        {
                returnVar = 0;
        }
    }
    else						                        // a wrap has not occured
    {
        if ((mSecondCounter - recPrevious) >= recCompare)
        {
                returnVar = 1;
        }
        else
        {
                returnVar = 0;
        }
    }
    return returnVar;
}
// ****************************************************************************
// ********************** DelayMilliSecondsBlocking ***************************
// ****************************************************************************
void DelayMilliSecondsBlocking(unsigned int length)
{
    unsigned int previousValue;
    unsigned char flagExit = 0;

    previousValue = mSecondCounter;											   // store latest value of mS counter incremented by interrupt

    while (flagExit == 0)
    {
        if (CheckTimerDifference(length, previousValue))
        {
                previousValue = mSecondCounter;
                flagExit = 1;
        }
    }
}
// ****************************************************************************
// *************************** DelaySecondsBlocking ***************************
// ****************************************************************************
void DelaySecondsBlocking(unsigned int length)
{
	unsigned char seconds;

	for (seconds=0; seconds<length; seconds++)
	{
		DelayMilliSecondsBlocking(1000);
	}
}
// ****************************************************************************
// *************************** SysTickIntHandler ******************************
// ****************************************************************************
//
// This is an interrupt that fires 1000 times a second.  This function name is
// added to the vector table in "startup.c"  mSecondCounter is an unsigned int, so the
// max mS it will count is 65536, or roughly 6.5 seconds.
//
void SysTickIntHandler(void)
{
	mSecondCounter++;
}
// ****************************************************************************
// ***************************** FlushRxBuffer ********************************
// ****************************************************************************
void FlushRxBuffer(void)
{
    unsigned char n;
    
    for(n=0; n<RX_BUFFER_SIZE; n++)                                             // zero out the RX buffer, so next command starts with clean slate
    {
        rxBuffer[n] = 0;
    }
}
// ****************************************************************************
// *************************** SendStringDebug ********************************
// ****************************************************************************
void SendStringDebug(unsigned char *rec)
{
#ifdef DEBUG_SERIAL_OUTPUT  
	while (*rec != '\0')
	{
		UARTCharPut(UART1_BASE,*rec++);
	}
    UARTCharPut(UART1_BASE,CR);
#endif
}
// ****************************************************************************
// ******************************** SendAck ***********************************
// ****************************************************************************
// 
// This function will send an ACK back to the host computer.
//
void SendAck(unsigned char localSource)
{
//    FlushRxBuffer();
//    StringCopy(txBuffer,"OK");
//    SendStringDebug("OK");  
//    SendStringBLE("OK");
    if(localSource == 'D') SendStringDebug("[OK]");
    if(localSource == 'B') SendStringBLE("[OK]");
//    if(localSource == '4') SendStringRS485NoChecking("[Hello World]");    

}
// ****************************************************************************
// ********************** SendStringRS485NoChecking ***************************
// ****************************************************************************
// 
// This function will send data out the RS485 port.  This function will block till
// the data has finished shifting out the port.  It checks for a flag (flagQuiet)
// and if set, will not transmit.
//
void SendStringRS485NoChecking(unsigned char *rec)
{   
    if (flagQuiet)
    {
        // nothing here, flag is set do not send data
    }
    else
    {
        GPIOPinWrite(PORT_E, PIN_RS485_EN, PIN_RS485_EN);                       // enable RS485 transmit, high enabled         
        while (*rec != '\0')
        {
            UARTCharPut(UART5_BASE,*rec++);
            UARTCharGet(UART5_BASE);                                            // flush received data as it is sent
        }
        UARTCharPut(UART5_BASE,CR);
        UARTCharGet(UART5_BASE);                                                // flush received data as it is sent
        while (UARTBusy(UART5_BASE));                                           // true while bits are being shifted out
        GPIOPinWrite(PORT_E, PIN_RS485_EN, 0);                                  // disable RS485 transmit, high enabled                
    }   
}
// ****************************************************************************
// ***************************** SendStringBLE ********************************
// ****************************************************************************
//
// SysCtlDelay(1000);
//
void SendStringBLE(unsigned char *rec)
{
    unsigned char sendVar;
    
    while (*rec != '\0')
    {
        sendVar = *rec++;

        UARTCharPut(UART7_BASE,sendVar);                                        // BLE port
//        UARTCharPut(UART1_BASE,sendVar);                                        // debug port
        SysCtlDelay(1000);
        while (UARTBusy(UART7_BASE));                                           // true while bits are being shifted out
    }
    
    UARTCharPut(UART7_BASE,CR);
    SysCtlDelay(1000);
    while (UARTBusy(UART7_BASE));                                               // true while bits are being shifted out
}
void SendStringBLETest(void)
{    
    UARTCharPut(UART7_BASE,'[');                                        // BLE port
    SysCtlDelay(1000);
    while (UARTBusy(UART7_BASE));                                           // true while bits are being shifted out

    UARTCharPut(UART7_BASE,'^');                                        // BLE port
    SysCtlDelay(1000);
    while (UARTBusy(UART7_BASE));                                           // true while bits are being shifted out

    UARTCharPut(UART7_BASE,']');                                        // BLE port
    SysCtlDelay(1000);
    while (UARTBusy(UART7_BASE));                                           // true while bits are being shifted out
    
    UARTCharPut(UART7_BASE,CR);
    SysCtlDelay(1000);
    while (UARTBusy(UART7_BASE));                                               // true while bits are being shifted out
}
// ****************************************************************************
// **************************** StoreIntToEEPROM ******************************
// ****************************************************************************
//
// This function stores a 32bit int to EEPROM starting a passed variable.
// address+0 = most significant byte
// address+3 = least significant byte
//
void StoreIntToEEPROM(unsigned char localAdd, unsigned int localData)
{
    unsigned char localVar3;    // most significant
    unsigned char localVar2;
    unsigned char localVar1;
    unsigned char localVar0;    // least significant
    
    localVar0 = localData & 0x000000FF;
    I2CWriteEEPROMSingle(localAdd+3,localVar0,I2C_PHYSICAL_EEPROM);
//    UARTCharPut(UART1_BASE,'0');
//    UARTCharPut(UART1_BASE,':');
//    UARTCharPut(UART1_BASE, hexArray[(localVar0 >> 4) & 0x0F]);
//    UARTCharPut(UART1_BASE, hexArray[localVar0 & 0x0F]);
//    UARTCharPut(UART1_BASE,' ');

    localVar1 = (localData>>8) & 0x000000FF;
    I2CWriteEEPROMSingle(localAdd+2,localVar1,I2C_PHYSICAL_EEPROM);
//    UARTCharPut(UART1_BASE,'1');
//    UARTCharPut(UART1_BASE,':');
//    UARTCharPut(UART1_BASE, hexArray[(localVar1 >> 4) & 0x0F]);
//    UARTCharPut(UART1_BASE, hexArray[localVar1 & 0x0F]);
//    UARTCharPut(UART1_BASE,' ');

    localVar2 = (localData>>16) & 0x000000FF;
    I2CWriteEEPROMSingle(localAdd+1,localVar2,I2C_PHYSICAL_EEPROM);
//    UARTCharPut(UART1_BASE,'2');
//    UARTCharPut(UART1_BASE,':');
//    UARTCharPut(UART1_BASE, hexArray[(localVar2 >> 4) & 0x0F]);
//    UARTCharPut(UART1_BASE, hexArray[localVar2 & 0x0F]);
//    UARTCharPut(UART1_BASE,' ');

    localVar3 = (localData>>24) & 0x000000FF;
    I2CWriteEEPROMSingle(localAdd+0,localVar3,I2C_PHYSICAL_EEPROM);
//    UARTCharPut(UART1_BASE,'3');
//    UARTCharPut(UART1_BASE,':');
//    UARTCharPut(UART1_BASE, hexArray[(localVar3 >> 4) & 0x0F]);
//    UARTCharPut(UART1_BASE, hexArray[localVar3 & 0x0F]);
//    UARTCharPut(UART1_BASE,' ');

}        
// ****************************************************************************
// *************************** ReadIntFromEEPROM ******************************
// ****************************************************************************
//
// This function reads a 32bit int from EEPROM starting a passed variable.
// address+0 = most significant byte
// address+3 = least significant byte
//
unsigned int ReadIntFromEEPROM(unsigned char localAdd)
{
    unsigned char local3, local2, local1, local0;
    unsigned int returnVar;

    local3 = I2CReadEEPROMSingle(localAdd+0,I2C_PHYSICAL_EEPROM);
    local2 = I2CReadEEPROMSingle(localAdd+1,I2C_PHYSICAL_EEPROM);
    local1 = I2CReadEEPROMSingle(localAdd+2,I2C_PHYSICAL_EEPROM);
    local0 = I2CReadEEPROMSingle(localAdd+3,I2C_PHYSICAL_EEPROM);

    returnVar = local3;
    returnVar = returnVar << 8;
    returnVar = returnVar | local2;
    returnVar = returnVar << 8;
    returnVar = returnVar | local1;
    returnVar = returnVar << 8;
    returnVar = returnVar | local0;

    return returnVar;
}        
 

