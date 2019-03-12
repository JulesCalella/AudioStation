// PIC24EP128MC206 Configuration Bit Settings

// 'C' source line config statements

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration (Allow only one reconfiguration)
#pragma config FCKSM = CSECME           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config PWMLOCK = ON             // PWM Lock Enable bit (Certain PWM registers may only be written after key sequence)
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <p24EP128MC206.h>
#include "hardwareConfig.h"

// LED Outputs
#define LED1 LATGbits.LATG6
#define LED2 LATGbits.LATG7
#define LED3 LATGbits.LATG8
#define LED4 LATGbits.LATG9
#define LED5 LATCbits.LATC1
#define LED6 LATCbits.LATC2

// IO Pins
#define A LATCbits.LATC11
#define B LATEbits.LATE12
#define C LATEbits.LATE13
#define D LATEbits.LATE14
#define E LATEbits.LATE15
#define READ_A PORTCbits.RC11
#define READ_B PORTEbits.RE12
#define READ_C PORTEbits.RE13
#define READ_D PORTEbits.RE14
#define READ_E PORTEbits.RE15

// Buttons
#define BTN1 PORTDbits.RD8   // 42 - D8
#define BTN2 PORTCbits.RC10   // 45 - C10
#define BTN3 PORTCbits.RC13   // 47 - C13
#define BTN4 PORTCbits.RC6   // 50 - C6
#define BTN5 PORTCbits.RC7   // 51 - C7
#define BTN6 PORTCbits.RC8   // 52 - C8
#define BTN7 PORTDbits.RD5   // 53 - D5
#define BTN8 PORTDbits.RD6   // 54 - D6
#define BTN9 PORTCbits.RC9   // 55 - C9

// Digital to Analog Converter Outputs
#define DAC0 LATAbits.LATA7     // 1 - A7
#define DAC1 LATBbits.LATB14    // 2 - B14
#define DAC2 LATBbits.LATB15    // 3 - B15
#define DAC3 LATAbits.LATA8     // 31 - A8
#define DAC4 LATBbits.LATB4     // 32 -B4
#define DAC5 LATBbits.LATB5     // 43 - B5
#define DAC6 LATBbits.LATB6     // 44 - B6
#define DAC7 LATBbits.LATB7     // 46 - B7
#define DAC8 LATBbits.LATB8     // 48 - B8
#define DAC9 LATBbits.LATB9     // 49 - B9
#define DAC10 LATBbits.LATB10   // 60 - B10
#define DAC11 LATBbits.LATB11   // 61 - B11
#define DAC12 LATBbits.LATB12   // 62 - B12
#define DAC13 LATBbits.LATB13   // 63 - B13
#define DAC14 LATAbits.LATA10   // 64 - A10
#define DAC15 LATAbits.LATA12   // 11 - A12

// Function Prototypes
void delay(int value);
void lightCount(uint16_t count);
void ledSweepLeft();
void ledSweepRight();
void pong(uint8_t move);
void timerInit();
void dacOutput(uint16_t output);

// Global Variables
volatile uint16_t timerCounter = 0;
volatile int interruptCalled = 1;

/*******************************************************************************
 * Timer 1 Interrupt
 * 
 * Sampling Frequency = 8kHz
 ******************************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{    
    interruptCalled = 1;
    
    IFS0bits.T1IF = 0;
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/
int main(void)
{
    PTGCONbits.PTGWDT = 0;  // Disable Watchdog timer
    RCONbits.SWDTEN = 0;
    
    // Potentiometers
    // Make A11,A0,A1,B1,C0 intputs
    TRISAbits.TRISA11 = 1;
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISBbits.TRISB1 = 1;
    TRISCbits.TRISC0 = 1;
    
    // Buttons
    TRISDbits.TRISD8 = 1;    // 42 - D8
    TRISCbits.TRISC10 = 1;   // 45 - C10
    TRISCbits.TRISC13 = 1;   // 47 - C13
    TRISCbits.TRISC6 = 1;    // 50 - C6
    TRISCbits.TRISC7 = 1;    // 51 - C7
    TRISCbits.TRISC8 = 1;    // 52 - C8
    TRISDbits.TRISD5 = 1;    // 53 - D5
    TRISDbits.TRISD6 = 1;    // 54 - D6
    TRISCbits.TRISC9 = 1;    // 55 - C9
    
    // LEDs
    // Make G6,G7,G8,G9,C1,C2,C11,E12,E13,E14,E15 outputs
    TRISGbits.TRISG6 = 0;   // LED1
    TRISGbits.TRISG7 = 0;   // LED2
    TRISGbits.TRISG8 = 0;   // LED3
    TRISGbits.TRISG9 = 0;   // LED4
    TRISCbits.TRISC1 = 0;   // LED5
    TRISCbits.TRISC2 = 0;   // LED6
    
    // IO pins
    TRISCbits.TRISC11 = 0;
    TRISEbits.TRISE12 = 0;
    TRISEbits.TRISE13 = 0;
    TRISEbits.TRISE14 = 0;
    TRISEbits.TRISE15 = 0;
    
    // DACs
    TRISAbits.TRISA7 = 0;     // 1 - A7
    TRISBbits.TRISB14 = 0;    // 2 - B14
    TRISBbits.TRISB15 = 0;    // 3 - B15
    TRISAbits.TRISA8 = 0;     // 31 - A8
    TRISBbits.TRISB4 = 0;     // 32 -B4
    TRISBbits.TRISB5 = 0;     // 43 - B5
    TRISBbits.TRISB6 = 0;     // 44 - B6
    TRISBbits.TRISB7 = 0;     // 46 - B7
    TRISBbits.TRISB8 = 0;     // 48 - B8
    TRISBbits.TRISB9 = 0;     // 49 - B9
    TRISBbits.TRISB10 = 0;   // 60 - B10
    TRISBbits.TRISB11 = 0;   // 61 - B11
    TRISBbits.TRISB12 = 0;   // 62 - B12
    TRISBbits.TRISB13 = 0;   // 63 - B13
    TRISAbits.TRISA10 = 0;   // 64 - A10
    TRISAbits.TRISA12 = 0;
    
    // Turn LEDs off
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
    LED5 = 0;
    LED6 = 0;
    
    A = 1;
    B = 1;
    C = 0;
    D = 1;
    E = 0;
    
    LED2 = 1;
    oscillatorInit();
    LED2 = 0;
    
    timerInit();
    
    // Set pin 12 to analog 
    ANSELAbits.ANSA11 = 1;
    
    adcInit();
    
    int pot1 = 0;
    int val1 = 0x3FF/6;
    int val2 = val1*2;
    int val3 = val1*3;
    int val4 = val1*4;
    int val5 = val1*5;
    int btnPressed;
    int countUp;
    
    while(1)
    {
        Nop();
        
        // Read AN9
        if(AD1CON1bits.DONE == 1)
        {
            AD1CON1bits.DONE = 0;
            pot1 = ADC1BUF0;
        }
    }
    
    return 0;
}


/******************************************************************************* 
 * DELAY 
 ******************************************************************************/
void delay(int value)
{
    int i,j;
    
    for(i=0;i<value;i++)
    {
        for(j=0;j<5000;j++)
        {
            // Do nothing
        }
    }
}

/*******************************************************************************
 * LIGHT COUNT
 ******************************************************************************/
void lightCount(uint16_t count)
{
    if((count & 0x01) != 0){
        LED1 = 1;
    } else {
        LED1 = 0;
    }

    if((count & 0x02) != 0){
        LED2 = 1;
    } else {
        LED2 = 0;
    }

    if((count & 0x04) != 0){
        LED3 = 1;
    } else {
        LED3 = 0;
    }

    if((count & 0x08) != 0){
        LED4 = 1;
    } else {
        LED4 = 0;
    }

    if((count & 0x10) != 0){
        LED5 = 1;
    } else {
        LED5 = 0;
    }

    if((count & 0x20) != 0){
        LED6 = 1;
    } else {
        LED6 = 0;
    }
}

/*******************************************************************************
 * LED SWEEP LEFT
 ******************************************************************************/
void ledSweepLeft()
{
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    LED5 = 1;
    LED6 = 1;
    
    delay(5);
    LED6 = 0;
    delay(5);
    LED5 = 0;
    delay(5);
    LED4 = 0;
    delay(5);
    LED3 = 0;
    delay(5);
    LED2 = 0;
    delay(5);
    LED1 = 0;
    delay(5);
}

/*******************************************************************************
 * LED SWEEP LEFT
 ******************************************************************************/
void ledSweepRight()
{
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    LED5 = 1;
    LED6 = 1;
    
    delay(5);
    LED1 = 0;
    delay(5);
    LED2 = 0;
    delay(5);
    LED3 = 0;
    delay(5);
    LED4 = 0;
    delay(5);
    LED5 = 0;
    delay(5);
    LED6 = 0;
    delay(5);
}

/*******************************************************************************
 * PONG
 ******************************************************************************/
void pong(uint8_t move)
{
    if(move == 0)
    {
        LED1 = 1;
        LED2 = 1;
        LED3 = 0;
        LED4 = 0;
        LED5 = 0;
        LED6 = 1;
    }
    else if((move == 1)||(move == 5))
    {
        LED1 = 1;
        LED2 = 0;
        LED3 = 1;
        LED4 = 0;
        LED5 = 0;
        LED6 = 1;
    }
    else if((move == 2)||(move == 4))
    {
        LED1 = 1;
        LED2 = 0;
        LED3 = 0;
        LED4 = 1;
        LED5 = 0;
        LED6 = 1;
    }
    else if(move == 3)
    {
        LED1 = 1;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        LED5 = 1;
        LED6 = 1;
    }
}

/*******************************************************************************
 * DAC Output
 ******************************************************************************/
void dacOutput(uint16_t output)
{
    if((output & 0x0001) != 0) DAC15 = 1;
    else DAC15 = 0;
    
    if((output & 0x0002) != 0) DAC14 = 1;
    else DAC14 = 0;
    
    if((output & 0x0004) != 0) DAC13 = 1;
    else DAC13 = 0;
    
    if((output & 0x0008) != 0) DAC12 = 1;
    else DAC12 = 0;
    
    if((output & 0x0010) != 0) DAC11 = 1;
    else DAC11 = 0;
    
    if((output & 0x0020) != 0) DAC10 = 1; 
    else DAC10 = 0;
    
    if((output & 0x0040) != 0) DAC9 = 1;
    else DAC9 = 0;
    
    if((output & 0x0080) != 0) DAC8 = 1;
    else DAC8 = 0;
    
    if((output & 0x0100) != 0) DAC7 = 1;
    else DAC7 = 0;
    
    if((output & 0x0200) != 0) DAC6 = 1;
    else DAC6 = 0;
    
    if((output & 0x0400) != 0) DAC5 = 1;
    else DAC5 = 0;
    
    if((output & 0x0800) != 0) DAC4 = 1;
    else DAC4 = 0;
    
    if((output & 0x1000) != 0) DAC3 = 1;
    else DAC3 = 0;
    
    if((output & 0x2000) != 0) DAC2 = 1;
    else DAC2 = 0;
    
    if((output & 0x4000) != 0) DAC1 = 1;
    else DAC1 = 0;
    
    if((output & 0x8000) != 0) DAC0 = 1;
    else DAC0 = 0;
}