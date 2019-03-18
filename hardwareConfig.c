#include <xc.h>
#include <stdint.h>
#include "hardwareConfig.h"

// **UNFINISHED
int oscillatorInit()
{
    //if(OSCCONbits.COSC == 0x00) return;
    
    CLKDIV = 0x0000;
    PLLFBD = 0x0035;
    OSCTUN = 0x0000;
    
    //Unlock PLL
    __builtin_write_OSCCONH(0x78);
    __builtin_write_OSCCONH(0x9A);
    __builtin_write_OSCCONH(0x01);
    
    __builtin_write_OSCCONL(OSCCON | 0x46);
    __builtin_write_OSCCONL(OSCCON | 0x57);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    
    while(OSCCONbits.OSWEN != 0);
    
    while(OSCCONbits.LOCK != 1)
    {
        if(OSCCONbits.CF == 1){
            OSCCONbits.CF = 0;
            return -1;
        }
    }
    
    return 0;
}

// **UNFINISHED
void pinInit()
{
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
    
    // Set pins to analog 
    ANSELAbits.ANSA11 = 1;
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA1 = 1;
    ANSELBbits.ANSB1 = 1;
    ANSELCbits.ANSC0 = 1;
    
    // Turn LEDs off
    LED1 = 1;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
    LED5 = 0;
    LED6 = 0;
    
    AIO = 0;
    BIO = 0;
    CIO = 0;
    DIO = 0;
    EIO = 0;
}

// **UNFINISHED
void timerInit()
{
    // Timer configuration
    T1CONbits.TON = 0;  // Turn timer off for setup
    T1CONbits.TSIDL = 1;    // Discontinue in idle mode
    T1CONbits.TGATE = 0;    // Gated Time accumulation disabled
    T1CONbits.TCKPS = 0x0;  // 1:1 prescale value
    T1CONbits.TSYNC = 0;   
    T1CONbits.TCS = 0;  // Clock = Fosc/2
    
    // Interrupt
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    IPC0bits.T1IP = 0x02;
    
    PR1 = 3168; // 6334 @ 101.3375MHz = 8kHz
    
    T1CONbits.TON = 1;  // Turn on timer
}

// **UNFINISHED
void adcInit()
{
    AD1CON1bits.ADON = 0;
    
    AD1CON1bits.ADSIDL = 0; // Continues in idle mode
    AD1CON1bits.AD12B = 1;  // 1-channel, 12-bit
    AD1CON1bits.FORM = 0x0; // integer
    AD1CON1bits.SSRCG = 0;
    AD1CON1bits.SSRC = 0x7; // internal counter
    //AD1CON1bits.SIMSAM = 0; // Sampled multiple channels in sequence
    AD1CON1bits.ASAM = 0;   // Samples when SAMP bit is set
    
    AD1CON2bits.VCFG = 0x7;
    AD1CON2bits.CSCNA = 0;  // Does not scan inputs
    AD1CON2bits.SMPI = 0x00;    // Interrupt after every sample
    AD1CON2bits.BUFM = 0;   // Always fill buffer from start address
    AD1CON2bits.ALTS = 0;   // Always uses MUXA
    
    AD1CON3bits.ADRC = 0;   // Clock derived from internal clock
    AD1CON3bits.SAMC = 0x10;
    AD1CON3bits.ADCS = 0xFF;
    
    AD1CON4bits.ADDMAEN = 0;    // DMA not used
    
    AD1CHS0bits.CH0NA = 0;  // Negative input is Vrefl
    AD1CHS0bits.CH0SA = 0x09;   // positive input is AN9
    
    AD1CON1bits.ADON = 1;
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