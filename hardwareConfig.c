#include <xc.h>
#include <stdint.h>
#include "hardwareConfig.h"

// **UNFINISHED
void oscillatorInit()
{
    if(OSCCONbits.COSC == 0x00) return;
    
    __builtin_write_OSCCONH(0x00);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    
    while(OSCCONbits.LOCK != 1);
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
    
    PR1 = 500; // Counts to 1000
    
    T1CONbits.TON = 1;  // Turn on timer
}

// **UNFINISHED
void adcInit()
{
    AD1CON1bits.ADON = 0;
    
    AD1CON1bits.ADSIDL = 0; // Continues in idle mode
    AD1CON1bits.AD12B = 0;  // 4-channel, 10-bit
    AD1CON1bits.FORM = 0x0; // integer
    AD1CON1bits.SSRCG = 0;
    AD1CON1bits.SSRC = 0x7; // internal counter
    AD1CON1bits.SIMSAM = 0; // Sampled multiple channels in sequence
    AD1CON1bits.ASAM = 1;   // Samples immediately after last conversion
    
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
    
    AD1CON1bits.SAMP = 1;   // Start first sample
}