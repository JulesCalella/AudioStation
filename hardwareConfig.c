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