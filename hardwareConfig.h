/* 
 * File:   hardwareConfig.h
 * Author: Jules
 *
 * Created on March 8, 2019, 5:46 PM
 */

#ifndef HARDWARECONFIG_H
#define	HARDWARECONFIG_H

// LED Outputs
#define LED1 LATGbits.LATG6
#define LED2 LATGbits.LATG7
#define LED3 LATGbits.LATG8
#define LED4 LATGbits.LATG9
#define LED5 LATCbits.LATC1
#define LED6 LATCbits.LATC2

// IO Pins
#define AIO LATCbits.LATC11
#define BIO LATEbits.LATE12
#define CIO LATEbits.LATE13
#define DIO LATEbits.LATE14
#define EIO LATEbits.LATE15
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

int oscillatorInit();
void pinInit();
void timerInit();
void adcInit();
void dacOutput(uint16_t output);

#endif	/* HARDWARECONFIG_H */

