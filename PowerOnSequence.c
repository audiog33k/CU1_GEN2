#include "main.h"

extern const unsigned char segArray[];
extern unsigned char flagBoardType;
extern unsigned char flagExpand;
extern unsigned char globalTempMaxInputRU1;
extern unsigned int globalInjectionCountA;
extern unsigned int globalInjectionCountB;

// ****************************************************************************
// ************************************* Setup ********************************
// ****************************************************************************
//
// Setup ports and peripherals
//
void Setup (void)
{
    unsigned char localVar;

    // PLL is running at 400MHz and divided by 2 = 200MHz.  
    // SYSDIV_4 -> 200/4 = 50MHz
    // Possible values are 2.5 through 16.
	SysCtlClockSet( SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );      // 80MHz
//	SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );      // 50MHz

	// Port A setup
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  
	GPIOPinTypeGPIOInput(PORT_A, PIN_IN1 | PIN_IN2 | PIN_IN3 | PIN_IN4 | PIN_IN5 | PIN_IN6 | PIN_IN7 | PIN_IN8);

	// PortB setup
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(PORT_B, PIN_LED1 | PIN_LED2 | PIN_IN_SEL);
	GPIOPinTypeGPIOInput(PORT_A, PIN_PB4_ADC10);

	// Port C setup
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);  
	GPIOPinTypeGPIOOutput(PORT_C, PIN_LEDA | PIN_LEDB | PIN_LEDC | PIN_LEDD);
//    GPIOPadConfigSet(PORT_C, PIN_SWITCH, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	// Port D Setup
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);  
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;				        // Lock register  0x00000520, write key 0x4C4F434B 
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR)   = 0xff;			                    // Commit Register 0x00000524, allow all port D pins to be modified
	GPIOPinTypeGPIOInput(PORT_D, PIN_SWITCH1 | PIN_SWITCH2 ); 
	GPIOPinTypeGPIOOutput(PORT_D, PIN_LEDE | PIN_LEDF | PIN_LEDG | PIN_LEDDP);
        GPIOPadConfigSet(PORT_D, PIN_SWITCH1 | PIN_SWITCH2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);    // needs pullup because not present on PCB

	// Port E setup
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  
	GPIOPinTypeGPIOOutput(PORT_E, PIN_BLE_RSTN | PIN_RS485_EN );

	// Port F setup
    // User manual  section 10.2.4
    // PF.0 is also NMI and is goofy to get setup as GPIO.
	// Unlock register must be written with a key and then each bit identified that can be modified.
	// This sequence is the two HWREG write
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;				        // Lock register  0x00000520, write key 0x4C4F434B 
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR)   = 0xff;			                    // Commit Register 0x00000524, allow all port F pins to be modified
	GPIOPinTypeGPIOInput(PORT_F, BOARD_TYPE | PIN_SWITCH3); 
	GPIOPinTypeGPIOOutput(PORT_F, PIN_ONESCLK | PIN_TENSCLK | PIN_LEDOE);
        GPIOPadConfigSet(PORT_F, BOARD_TYPE | PIN_SWITCH3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // pull pin up, we will look at it to see if it is pulled low external


    // UART Setup
    // UART5 is RS485 port
    // Use UART port "UART5' at PE4 and PE5
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);							   // enable the UART
    GPIOPinConfigure(GPIO_PE4_U5RX);              
    GPIOPinConfigure(GPIO_PE5_U5TX);
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART5_BASE, SysCtlClockGet(), 19200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART5_BASE);
    IntEnable(INT_UART5);
    UARTIntEnable(UART5_BASE, UART_INT_RX);
    UARTFIFOLevelSet(UART5_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);
      
    // UART7 Setup
    // UART7 is BLE interface
    // Use UART port "UART7' at PE0 and PE1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);							   // enable the UART
    GPIOPinConfigure(GPIO_PE0_U7RX);              
    GPIOPinConfigure(GPIO_PE1_U7TX);
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART7_BASE);
    IntEnable(INT_UART7);
    UARTIntEnable(UART7_BASE, UART_INT_RX);
    UARTFIFOLevelSet(UART7_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);

    // UART1 Setup
    // UART1 is debug port
    // Use UART port "UART1' at PB0 and PB1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);							   // enable the UART
    GPIOPinConfigure(GPIO_PB0_U1RX);              
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART1_BASE);
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX);
    UARTFIFOLevelSet(UART1_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);
   
    // I2C Setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    I2CMasterInitExpClk(I2C0_BASE,SysCtlClockGet(), false);                     // false=100KHz, true=400KHz
                   
    // ********* Setup system tick interrupt ***************  

    SysTickPeriodSet(80000);                                                    // for 80MHz =  1ms/12.5nS=80,000

    // Enable the SysTick Interrupt.
    SysTickIntEnable();

    // Enable SysTick.
    SysTickEnable();

    // Enable interrupts to the processor.
    IntMasterEnable();
       
    // timer 0 setup used triac drive
/*
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    GPIOPinConfigure(GPIO_PF1_T0CCP1);
    GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_1);                               // link the timer to the pin
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM );        // configure timer 0 as two seperate 16 bit timers and make time "A" PWM
    TimerEnable(TIMER0_BASE, TIMER_B);                                           // turn it on
    TimerLoadSet(TIMER0_BASE, TIMER_B, 10000);
    TimerMatchSet(TIMER0_BASE, TIMER_B, 5000);                                     
*/       
/*
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);
    TimerEnable(TIMER0_BASE, TIMER_A);
*/   
    // Configure Switch Interrupt setup on Port D, Switch 1 and Switch 2  
    GPIOIntRegister(PORT_D, GPIOPortDIntHandler);                                                  // register the function that will be used as interrupt routine
    GPIOIntTypeSet(PORT_D, PIN_SWITCH1, GPIO_FALLING_EDGE);
    GPIOIntTypeSet(PORT_D, PIN_SWITCH2, GPIO_FALLING_EDGE);
    GPIOIntClear(PORT_D, PIN_SWITCH1);
    GPIOIntClear(PORT_D, PIN_SWITCH2);

    // Configure Switch Interrupt setup on Port F, Switch 3    
    GPIOIntRegister(PORT_F, GPIOPortFIntHandler);                                                  // register the function that will be used as interrupt routine
    GPIOIntTypeSet(PORT_F, PIN_SWITCH3, GPIO_FALLING_EDGE);
    GPIOIntClear(PORT_F, PIN_SWITCH3);

    
/*
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);								   // Enable the ADC

	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);							   // Charge Voltage
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);							   // Battery Voltage

	ADCSequenceConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 2, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 3, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END | ADC_CTL_TS);
	ADCHardwareOversampleConfigure(ADC0_BASE, ADC_NUM_AVERAGES);			    // setup how many averages to run
	ADCSequenceEnable(ADC0_BASE, ADC_SAMPLE_SEQUENCE);
	ADCProcessorTrigger(ADC0_BASE, ADC_SAMPLE_SEQUENCE);						   // Trigger the ADC conversion.
	ADCIntEnable(ADC0_BASE, ADC_SAMPLE_SEQUENCE);								   // enable flag/int to show ADC has finished conversion       
    ADCReferenceSet(ADC0_BASE,ADC_REF_INT);                                 // set ADC reference to internal which is 3.0V
*/
    // display all 7 segment LEDs in sequence
    LEDDualClear();
   
    GPIOPinWrite(PORT_C, PIN_LEDA, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);

    GPIOPinWrite(PORT_C, PIN_LEDB, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);

    GPIOPinWrite(PORT_C, PIN_LEDC, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);

    GPIOPinWrite(PORT_C, PIN_LEDD, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);

    GPIOPinWrite(PORT_D, PIN_LEDE, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);

    GPIOPinWrite(PORT_D, PIN_LEDF, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);

    GPIOPinWrite(PORT_D, PIN_LEDG, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);

    GPIOPinWrite(PORT_D, PIN_LEDDP, 0);                                 
    LEDClockOnes();    
    LEDClockTens();    
    DelayMilliSecondsBlocking(100);
    LEDDualClear();

    localVar = I2CReadEEPROMSingle(EA_EXPAND,I2C_PHYSICAL_EEPROM);              // is the unit configured as an expansion unit
    if (localVar == 0xA5)                                                       // set the flag, we are an expansion unit
    {
        flagExpand = 1;
//        LEDPortWriteDecimal(segArray[14],NO_DECIMAL);                           // display "E" for expansion
    }
    else
    {
        flagExpand = 0;
//        LEDPortWriteDecimal(ALPHA_P,NO_DECIMAL);                                // display "P" for primary
    }
       
    // determine what type of board we are: RU1
    if (GPIOPinRead(PORT_F, BOARD_TYPE))                                        // check pin PF.0, high=RU1 (remote unit)
    {
        flagBoardType = BOARD_RU1;

    // ****************** Setup The ADC ********************************

    GPIOPinTypeGPIOOutput(PORT_E, PIN_RS485_EN );                               // remove PE2 from being output
    GPIOPinTypeGPIOInput(PORT_E, PIN_PE2_ADC01);                                // make PE2 an input       
        
//  The RU1 has added a NTC (temperature measurement) on PIN58, PB4, AIN10
//    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);					// Enable the ADC
    GPIOPinTypeADC(GPIO_PORTB_BASE, PIN_PB4_ADC10);	                        // Temperature PB4
    GPIOPinTypeADC(GPIO_PORTE_BASE, PIN_PE2_ADC01);	                        // Measure current PE2
    ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 0, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 1, ADC_CTL_CH10);
    ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 2, ADC_CTL_CH11 | ADC_CTL_IE | ADC_CTL_END | ADC_CTL_TS);
    
//    ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 0, ADC_CTL_CH10);
//    ADCSequenceStepConfigure(ADC0_BASE, ADC_SAMPLE_SEQUENCE, 1, ADC_CTL_CH11 | ADC_CTL_IE | ADC_CTL_END | ADC_CTL_TS);
    ADCHardwareOversampleConfigure(ADC0_BASE, 8);			        // setup how many averages to run
    ADCSequenceEnable(ADC0_BASE, ADC_SAMPLE_SEQUENCE);
    ADCProcessorTrigger(ADC0_BASE, ADC_SAMPLE_SEQUENCE);			// Trigger the ADC conversion.
    ADCIntEnable(ADC0_BASE, ADC_SAMPLE_SEQUENCE);								// enable flag/int to show ADC has finished conversion       
    ADCReferenceSet(ADC0_BASE,ADC_REF_INT);                                     // set ADC reference to internal which is 3.0V
        
        
        
        
        
        LEDDualHex(ALPHA_r,SEGMENT_HIGH, DECIMAL_OFF);
        LEDClockTens();
        LEDDualHex(ALPHA_u,SEGMENT_LOW, DECIMAL_OFF);
        LEDClockOnes();
        DelaySecondsBlocking(1);    
        // on RU1 (remote unit) these pins are outputs
        GPIOPinTypeGPIOOutput(PORT_A, PIN_RU1_SOL1 | PIN_RU1_SOL2 | PIN_RU1_SOL3 | PIN_RU1_INJA | PIN_RU1_INJB | PIN_RU1_SAFETY);
        GPIOPadConfigSet(PORT_A, PIN_RU1_SOL1 | PIN_RU1_SOL2 | PIN_RU1_SOL3 | PIN_RU1_INJA | PIN_RU1_INJB , GPIO_STRENGTH_10MA, GPIO_PIN_TYPE_STD); 

        // PF1 is input on RU1
        // was a flow sensor, removed on Gen2
        // GPIOPinTypeGPIOInput(PORT_F, PIN_FLOW);
        // GPIOPadConfigSet(PORT_F, PIN_FLOW, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // pull pin up, we will look at it to see if it is pulled low external

        // Timer setup to count flow control pulses
        // Configure PF1, T0CCP1, Pin29 for capture of edges
        // This pin is an input if RU1 
        // T0CCP1 is Timer B
        // For some reason, the TimerLoadSet can not be 0xFFFF (0xFFFE works)
        // This configuration counts down.
        // SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
        // GPIOPinTypeTimer(GPIO_PORTF_BASE, PIN_FLOW);
        // GPIOPinConfigure(GPIO_PF1_T0CCP1);
        // TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_CAP_COUNT));
        // TimerLoadSet(TIMER0_BASE, TIMER_B, 10000);
        // TimerControlEvent(TIMER0_BASE, TIMER_B, TIMER_EVENT_POS_EDGE);
        // TimerEnable(TIMER0_BASE, TIMER_B);  
        
        // look at stored max temperature
        localVar = I2CReadEEPROMSingle(EA_MAXTEMP,I2C_PHYSICAL_EEPROM);
        if (localVar>90)                                                        // not initialized, reset to zero
        {
            I2CWriteEEPROMSingle(EA_MAXTEMP,0,I2C_PHYSICAL_EEPROM);
        }
        else if (localVar<=90)                                                  // within range
        {
            globalTempMaxInputRU1 = localVar;
        }

        // get 32 injection counts and zero if EEPROM was blank
        globalInjectionCountA = ReadIntFromEEPROM(EA_IACNT);                    // get 32bit count of injection A      
        if (globalInjectionCountA == 0xFFFFFFFF)                                // if EEPROM was blank at this location, zero it
        {
            globalInjectionCountA = 0;
            StoreIntToEEPROM(EA_IACNT, globalInjectionCountA);
        }
        globalInjectionCountB = ReadIntFromEEPROM(EA_IBCNT);                    // get 32bit count of injection B      
        if (globalInjectionCountB == 0xFFFFFFFF)                                // if EEPROM was blank at this location, zero it
        {
            globalInjectionCountB = 0;
            StoreIntToEEPROM(EA_IBCNT, globalInjectionCountB);
        }       
    }
    // we are CU1
    else                                                                        // pin low = CU1
    {
        flagBoardType = BOARD_CU1;

        // on CU1 (control unit) these pins are inputs
        GPIOPinTypeGPIOInput(PORT_A, PIN_IN1 | PIN_IN2 | PIN_IN3 | PIN_IN4 | PIN_IN5 | PIN_IN6 | PIN_IN7 | PIN_IN8);
      
        LEDDualHex(ALPHA_c,SEGMENT_HIGH, DECIMAL_OFF);
        LEDClockTens();
        LEDDualHex(ALPHA_u,SEGMENT_LOW, DECIMAL_OFF);
        LEDClockOnes();
        DelaySecondsBlocking(1);
        LEDDualClear();
        
        if (flagExpand)                                                         // set the flag, we are an expansion unit
        {
            LEDDualHex(segArray[14],SEGMENT_LOW, DECIMAL_OFF);
            LEDClockOnes();
        }
        else
        {
            LEDDualHex(ALPHA_P,SEGMENT_LOW, DECIMAL_OFF);
            LEDClockOnes();
        }
        DelaySecondsBlocking(1);
    }
          
    LEDDualClear();
}
// ****************************************************************************
// ************************** DumpEEPROMValues ********************************
// ****************************************************************************
// 
// Send out contents of the EEPROM out debug port
//
// Looks like this:
//
// [0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ]                        
// [== == == == == == == == == == == == == == == == ]                            
// [FF A5 FF FF FF FF FF FF FF FF FF FF FF FF FF FF ]                            
// [FF 08 00 00 08 00 01 00 03 00 FF FF FF FF FF FF ]                            
// [FF 00 00 00 00 00 00 00 00 00 FF FF FF FF FF FF ]                            
// [FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ]                            
// [FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ]                           
// [FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ]                            
// [FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ]                            
// [FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ]

void DumpEEPROMValues(void)
{
    unsigned char x,y,add;
    unsigned char readValue;

    SendStringDebug("[0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ]");
    SendStringDebug("[== == == == == == == == == == == == == == == == ]");
    add = 0;
    for(y=0; y<8; y++)
    {            
        UARTCharPut(UART1_BASE,'[');
        for(x=0; x<16; x++)
        {    
            readValue = I2CReadEEPROMSingle(add, I2C_PHYSICAL_EEPROM);
            UARTCharPut(UART1_BASE,hexArray[readValue >> 4]);
            UARTCharPut(UART1_BASE,hexArray[readValue & 0x0F]);
            UARTCharPut(UART1_BASE,0x20);
            add++;
        } 
        UARTCharPut(UART1_BASE,']');
        UARTCharPut(UART1_BASE,CR);        
    }
}
// ****************************************************************************
// ************************** VersionByLED ************************************
// ****************************************************************************
//
// This function blinks the firmware version on the LEDs.
// Major version on Status
// Minor version on Busy
// Alpha version on Full
//
// Started for loops at 1 and added 1 to ending condition because compiler would produce
// a warning against comparing against a zero version number.
//
void VersionByLED(void)
{
    unsigned int n;
   
    for(n=1; n<VERSION_MAJOR+1; n++)
    {
        GPIOPinWrite(PORT_B, PIN_LED1, 0);                           
        DelayMilliSecondsBlocking(250);
        GPIOPinWrite(PORT_B, PIN_LED1, PIN_LED1);                           
        DelayMilliSecondsBlocking(250);     
    }
    DelaySecondsBlocking(1);
    for(n=1; n<VERSION_MINOR+1; n++)
    {
        GPIOPinWrite(PORT_B, PIN_LED1, 0);                           
        DelayMilliSecondsBlocking(250);
        GPIOPinWrite(PORT_B, PIN_LED1, PIN_LED1);                           
        DelayMilliSecondsBlocking(250);     
    }
    DelaySecondsBlocking(1);
    
    for(n=1; n<VERSION_ALPHA+1; n++)
    {
        GPIOPinWrite(PORT_B, PIN_LED1, 0);                           
        DelayMilliSecondsBlocking(250);
        GPIOPinWrite(PORT_B, PIN_LED1, PIN_LED1);                           
        DelayMilliSecondsBlocking(250);     
    }
    DelaySecondsBlocking(1);

}
// ****************************************************************************
// ************************* VersionBy7Seg ************************************
// ****************************************************************************
//
// This function blinks the firmware version on the 7 segment display.
//
void VersionBy7Seg(void)
{
    // display major and minor version
    LEDDualHex(segArray[VERSION_MAJOR],SEGMENT_HIGH, DECIMAL_ON);               // Major.
    LEDDualHex(segArray[VERSION_MINOR],SEGMENT_LOW, DECIMAL_ON);                // Minor.
    DelayMilliSecondsBlocking(1000);    
    LEDDualClear();

    // display minor minor :)
    LEDDualHex(segArray[VERSION_ALPHA],SEGMENT_HIGH, DECIMAL_OFF);              // Alpha   
    LEDDualHex(segArray[DISPLAY_OFF],SEGMENT_LOW, DECIMAL_OFF);                 // blank
    DelayMilliSecondsBlocking(1000);    
    LEDDualClear();
}
// ****************************************************************************
// *************************** EEPROMLoopTillConfigured ***********************
// ****************************************************************************
//
// This function checks for EEPROM location to be 0xA5.  It will loop till it finds that
// this location is programmed.
//
/*
void EEPROMLoopTillConfigured(void)
{
    unsigned int mSecondCounter1xSecond = 0;
    unsigned char localVar = 0;
    
    localVar = I2CReadEEPROMSingle(EA_INIT, I2C_PHYSICAL_EEPROM);          // check to see if the eeprom has been configured
    if (localVar == 0xA5)
    {
        flagEEPROMInitialized = TRUE;
    }
    if(flagEEPROMInitialized == FALSE)
    {  
        while (localVar != 0xA5)
        {
            SerialFlagCheck();     

            // *************** RUN EVERY SECOND ******************
            if (CheckTimerDifference(1000, mSecondCounter1xSecond))		                    // execute 1x second
            {
                mSecondCounter1xSecond  = mSecondCounter; 
                CheckForTimeouts();
                localVar = I2CReadEEPROMSingle(EA_INIT, I2C_PHYSICAL_EEPROM);          // check to see if the eeprom has been configured
                GPIOPinWrite(PORT_B, PIN_LED1, ~GPIOPinRead(PORT_B, PIN_LED1)); // toggle the debug LED 

            }
        }
        flagEEPROMInitialized = TRUE;
    }   
}
*/
// ****************************************************************************
// **************************** EEPROMReadSetup *******************************
// ****************************************************************************
//
/*
void EEPROMReadSetup(void)
{
    unsigned char localVar;
    unsigned char var3, var2, var1, var0;
       
    localVar = I2CReadEEPROMSingle(EA_INIT, I2C_PHYSICAL_EEPROM);            // check to see if the eeprom has been configured
    if (localVar == 0xA5)
    {       
        // read 4 bytes from EEPROM and construct the 32 bit serial number      
        var3 = I2CReadEEPROMSingle(EA_SN3, I2C_PHYSICAL_EEPROM);
        var2 = I2CReadEEPROMSingle(EA_SN2, I2C_PHYSICAL_EEPROM);
        var1 = I2CReadEEPROMSingle(EA_SN1, I2C_PHYSICAL_EEPROM);
        var0 = I2CReadEEPROMSingle(EA_SN0, I2C_PHYSICAL_EEPROM);
        globalSerialNumber = var3;
        globalSerialNumber <<= 8;
        globalSerialNumber |= var2;
        globalSerialNumber <<= 8;
        globalSerialNumber |= var1;
        globalSerialNumber <<= 8;
        globalSerialNumber |= var0;
         
        EEPROMReadZoneInjection();
      
        flagEEPROMInitialized  = 1;
    }
    else
    {
      flagEEPROMInitialized  = 0;
    }  
}
*/