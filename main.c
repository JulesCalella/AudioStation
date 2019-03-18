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
#include <math.h>
#include "hardwareConfig.h"
#include "waveform.h"

#define SCORE_SIZE_Y 100
#define SCORE_SIZE_X 10

// Function Prototypes
void delay(int value);
void lightCount(uint16_t count);
void ledSweepLeft();
void ledSweepRight();
void pong(uint8_t move);
int parseScoreString();
void fillString();
void createOutput();

// Global Variables
volatile uint16_t output = 0;
volatile uint16_t timer = 0;
volatile uint16_t tempoCounterLight = 0;
volatile uint16_t tempoCounter = 0;
volatile uint16_t tempoTripletCounter = 0;
volatile uint16_t tempoCounterMax = (SAMPLING_FREQUENCY * 60.0) / (TEMPO * 16.0); // 16 comes from 64th/4beats
volatile uint16_t tempoTripletCounterMax = (SAMPLING_FREQUENCY * 60.0) / (TEMPO * 24.0); // 24 comes from (32th/4beats)/3 triplets
volatile uint16_t measure = 0;
volatile uint16_t tempo64Counter = 0;
volatile uint16_t tempo32TripletCounter = 0;
volatile uint8_t update = 0;

uint8_t scoreString[SCORE_SIZE_Y][SCORE_SIZE_X];
uint16_t scoreElementY = 0;
uint16_t scoreElementX = 0;
volatile NOTE note1;
volatile NOTE note2;
volatile NOTE note3;
volatile NOTE note4;
volatile NOTE note5;
volatile NOTE note6;
volatile NOTE note7;
volatile NOTE note8;
volatile NOTE note9;
volatile NOTE note10;


/*******************************************************************************
 * Timer 1 Interrupt
 * 
 * Sampling Frequency = 16kHz
 ******************************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{    
    // Send output to DAC
    dacOutput(output);
    
    update = 1;
    
    // Increment tempoCounter and triplet counter
    tempoCounter++;
    tempoTripletCounter++;
    
    // IF TRUE: 1/32 triplet note duration passed
    if(tempoTripletCounter >= tempoTripletCounterMax){
        tempoTripletCounter = 0;
        tempo32TripletCounter ++;
    }
    
    // IF TRUE: 1/64th note duration passed
    if(tempoCounter >= tempoCounterMax){
        // Reset tempoCounter
        tempoCounter = 0;
        // Increment 64th note duration
        tempo64Counter++;
        // Increment tempoCounterLight 
        tempoCounterLight++;
        
        // IF 64 notes passed, 1 measure is complete
        if(tempo64Counter >= 64){
            // Reset 64ths counter and triplet counter
            tempo64Counter = 0;
            tempo32TripletCounter = 0;
            
            // Increment measure
            measure++;
            
            // TEMP: Loop first 5 measures
            if(measure >= 5){
                measure = 0;
                scoreElementY = 0;
                scoreElementX = 0;
            }
        }
        
        // Adjust the light
        if(tempoCounterLight >= 16){
            LED3 = ~LED3;
            tempoCounterLight = 0;
        }
        

    }

    IFS0bits.T1IF = 0;
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/
int main(void)
{
    PTGCONbits.PTGWDT = 0;  // Disable Watchdog timer
    RCONbits.SWDTEN = 0;
    
    pinInit();
    
    if(oscillatorInit() == -1) LED6 = 1;
    
    note1.available = 1;
    note2.available = 1;
    note3.available = 1;
    note4.available = 1;
    note5.available = 1;
    note6.available = 1;
    note7.available = 1;
    note8.available = 1;
    note9.available = 1;
    note10.available = 1;
    
    fillString();
    
    parseScoreString();
    parseScoreString();
    parseScoreString();
    parseScoreString();
    parseScoreString();
    parseScoreString();
    parseScoreString();
    parseScoreString();
    parseScoreString();
    parseScoreString();
    
    createOutput();
    
    adcInit();
    timerInit();
    setVolumes();
    
    while(1)
    {
        // Read Button
        if(BTN1 == 0)        {
            setVolumes();
        }
        
        parseScoreString();
        
        if(update == 1) createOutput();
    }
    
    return 0;
}

/******************************************************************************* 
 * PARSE SCORE STRING
 ******************************************************************************/
/* Returns -1 if the end of the score is reached
 * Returns 0 if no problems
 * Message format:
 *      FFh - message start (0)
 *      XXXXh - Measure Start Number (1 & 2)
 *      XXh - Location Start (3)
 *      XXXXh - Measure End Number (4 & 5)
 *      XXh - Location End (6)
 *      XXh - Note quality/value (7)
 *      XXh - Note pitch (8)
 *      XXh - Note volume (9)
 */
int parseScoreString()
{
    // Search for message start at 0xFF [BYTE 0]
    while(scoreString[scoreElementY][scoreElementX] != 0xFF){
        scoreElementY++;
        
        // If end of string met, move to next row
        if(scoreElementY >= SCORE_SIZE_Y){
            scoreElementY = 0;
            scoreElementX++;
            
            if((scoreElementX == SCORE_SIZE_X) && (scoreElementY == SCORE_SIZE_Y)) return -1;
        }
    }
    
    uint8_t currentElementY = scoreElementY + 1;
    uint8_t currentElementX = scoreElementX;
    uint16_t measureStartValue;
    uint8_t locationStartValue;
    uint16_t measureEndValue;
    uint8_t locationEndValue;
    uint8_t noteValue;
    int *waveform;
    uint16_t waveformMax;
    uint8_t pitchValue;
    uint8_t volumeValue;
    uint8_t isTriplet = 0;
    
    // Find measure start number [BYTES 1 & 2]
    measureStartValue = scoreString[currentElementY][currentElementX];
    currentElementY++;
    measureStartValue = (measureStartValue << 8) | scoreString[currentElementY][currentElementX];
    
    // Find location start [BYTE 3]
    currentElementY++;
    locationStartValue = scoreString[currentElementY][currentElementX];
    
    // Find measure end number [BYTES 4 & 5]
    currentElementY++;
    measureEndValue = scoreString[currentElementY][currentElementX];
    currentElementY++;
    measureEndValue = (measureEndValue << 8) | scoreString[currentElementY][currentElementX];
    
    // Find location start [BYTE 6]
    currentElementY++;
    locationEndValue = scoreString[currentElementY][currentElementX];

    // Determine which note is being played [BYTE 7]
    currentElementY++;
    noteValue = scoreString[currentElementY][currentElementX];

    switch(noteValue){
        case A:
            waveform = waveformA;
            waveformMax = totalElementsA;
            break;
        case AS:
            waveform = waveformASharp;
            waveformMax = totalElementsASharp;
            break;
        case B:
            waveform = waveformB;
            waveformMax = totalElementsB;
            break;
        case C:
            waveform = waveformC;
            waveformMax = totalElementsC;
            break;
        case CS:
            waveform = waveformCSharp;
            waveformMax = totalElementsCSharp;
            break;
        case D:
            waveform = waveformD;
            waveformMax = totalElementsD;
            break;
        case DS:
            waveform = waveformDSharp;
            waveformMax = totalElementsDSharp;
            break;
        case E:
            waveform = waveformE;
            waveformMax = totalElementsE;
            break;
        case F:
            waveform = waveformF;
            waveformMax = totalElementsF;
            break;
        case FS:
            waveform = waveformFSharp;
            waveformMax = totalElementsFSharp;
            break;
        case G:
            waveform = waveformG;
            waveformMax = totalElementsG;
            break;
        case GS:
            waveform = waveformGSharp;
            waveformMax = totalElementsGSharp;
            break;
        default:
            waveform = waveformA;
            waveformMax = totalElementsA;
            break;
    }

    // Record the pitch
    currentElementY++;
    pitchValue = scoreString[currentElementY][currentElementX];

    // Record the volume
    currentElementY++;
    volumeValue = scoreString[currentElementY][currentElementX];

    // Find an available note struct
    if(note1.available == 1){
        note1.available = 0;
        note1.pitch = pitchValue;
        note1.currentElement = 0;
        note1.measureStart = measureStartValue;
        note1.locationStart = locationStartValue;
        note1.measureEnd = measureEndValue;
        note1.locationEnd = locationEndValue;
        note1.triplet = isTriplet;
        note1.waveform = waveform;
        note1.waveformElementMax = waveformMax;
    } else if(note2.available == 1){
        note2.available = 0;
        note2.pitch = pitchValue;
        note2.currentElement = 0;
        note2.measureStart = measureStartValue;
        note2.locationStart = locationStartValue;
        note2.measureEnd = measureEndValue;
        note2.locationEnd = locationEndValue;
        note2.triplet = isTriplet;
        note2.waveform = waveform;
        note2.waveformElementMax = waveformMax;
    } else if(note3.available == 1){
        note3.available = 0;
        note3.pitch = pitchValue;
        note3.currentElement = 0;
        note3.measureStart = measureStartValue;
        note3.locationStart = locationStartValue;
        note3.measureEnd = measureEndValue;
        note3.locationEnd = locationEndValue;
        note3.triplet = isTriplet;
        note3.waveform = waveform;
        note3.waveformElementMax = waveformMax;
    } else if(note4.available == 1){
        note4.available = 0;
        note4.pitch = pitchValue;
        note4.currentElement = 0;
        note4.measureStart = measureStartValue;
        note4.locationStart = locationStartValue;
        note4.measureEnd = measureEndValue;
        note4.locationEnd = locationEndValue;
        note4.triplet = isTriplet;
        note4.waveform = waveform;
        note4.waveformElementMax = waveformMax;
    } else if(note5.available == 1){
        note5.available = 0;
        note5.pitch = pitchValue;
        note5.currentElement = 0;
        note5.measureStart = measureStartValue;
        note5.locationStart = locationStartValue;
        note5.measureEnd = measureEndValue;
        note5.locationEnd = locationEndValue;
        note5.triplet = isTriplet;
        note5.waveform = waveform;
        note5.waveformElementMax = waveformMax;
    } else if(note6.available == 1){
        note6.available = 0;
        note6.pitch = pitchValue;
        note6.currentElement = 0;
        note6.measureStart = measureStartValue;
        note6.locationStart = locationStartValue;
        note6.measureEnd = measureEndValue;
        note6.locationEnd = locationEndValue;
        note6.triplet = isTriplet;
        note6.waveform = waveform;
        note6.waveformElementMax = waveformMax;
    } else if(note7.available == 1){
        note7.available = 0;
        note7.pitch = pitchValue;
        note7.currentElement = 0;
        note7.measureStart = measureStartValue;
        note7.locationStart = locationStartValue;
        note7.measureEnd = measureEndValue;
        note7.locationEnd = locationEndValue;
        note7.triplet = isTriplet;
        note7.waveform = waveform;
        note7.waveformElementMax = waveformMax;
    } else if(note8.available == 1){
        note8.available = 0;
        note8.pitch = pitchValue;
        note8.currentElement = 0;
        note8.measureStart = measureStartValue;
        note8.locationStart = locationStartValue;
        note8.measureEnd = measureEndValue;
        note8.locationEnd = locationEndValue;
        note8.triplet = isTriplet;
        note8.waveform = waveform;
        note8.waveformElementMax = waveformMax;
    } else if(note9.available == 1){
        note9.available = 0;
        note9.pitch = pitchValue;
        note9.currentElement = 0;
        note9.measureStart = measureStartValue;
        note9.locationStart = locationStartValue;
        note9.measureEnd = measureEndValue;
        note9.locationEnd = locationEndValue;
        note9.triplet = isTriplet;
        note9.waveform = waveform;
        note9.waveformElementMax = waveformMax;
    } else if(note10.available == 1){
        note10.available = 0;
        note10.pitch = pitchValue;
        note10.currentElement = 0;
        note10.measureStart = measureStartValue;
        note10.locationStart = locationStartValue;
        note10.measureEnd = measureEndValue;
        note10.locationEnd = locationEndValue;
        note10.triplet = isTriplet;
        note10.waveform = waveform;
        note10.waveformElementMax = waveformMax;
    } else {
        // If no notes are available, return but dont increment scoreElementY
        // or scoreElementX
        return 0;
    }
    
    // Increment to the next message start
    scoreElementY += 10;
    
    // Bounds Check
    if(scoreElementY >= SCORE_SIZE_Y){
        scoreElementY = 0;
        scoreElementX++;
        
        if((scoreElementX == SCORE_SIZE_X) && (scoreElementY == SCORE_SIZE_Y)) return -1;
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

/* Test Song
 * 0xFF 0x0003 0x00 0x0003 0x10 0x03 0x02 0x7F
 * 0xFF 0x0003 0x10 0x0003 0x20 0x07 0x02 0x7F
 * 0xFF 0x0003 0x20 0x0003 0x30 0x0A 0x02 0x7F
 * 0xFF 0x0004 0x00 0x0005 0x00 0x03 0x02 0x7F
 * 0xFF 0x0004 0x00 0x0005 0x00 0x07 0x02 0x7F
 * 0xFF 0x0004 0x00 0x0005 0x00 0x0A 0x02 0x7F
 */
void fillString()
{
    // 0xFF 0x0003 0x00 0x0003 0x10 0x03 0x02 0x7F
    scoreString[0][0] = 0xFF;
    scoreString[1][0] = 0x00;
    scoreString[2][0] = 1;
    scoreString[3][0] = 0;//0x00;
    scoreString[4][0] = 0x00;
    scoreString[5][0] = 1;
    scoreString[6][0] = 8;//0x08;
    scoreString[7][0] = C;
    scoreString[8][0] = 0x02;
    scoreString[9][0] = 0x7F;
    // 0xFF 0x0003 0x10 0x0003 0x20 0x07 0x02 0x7F
    scoreString[10][0] = 0xFF;
    scoreString[11][0] = 0x00;
    scoreString[12][0] = 1;
    scoreString[13][0] = 8;//0x80;
    scoreString[14][0] = 0x00;
    scoreString[15][0] = 1;
    scoreString[16][0] = 16;//0x10;
    scoreString[17][0] = C;
    scoreString[18][0] = 0x02;
    scoreString[19][0] = 0x7F;
    // 0xFF 0x0003 0x20 0x0003 0x30 0x0A 0x02 0x7F
    scoreString[20][0] = 0xFF;
    scoreString[21][0] = 0x00;
    scoreString[22][0] = 1;
    scoreString[23][0] = 16;//0x10;
    scoreString[24][0] = 0x00;
    scoreString[25][0] = 1;
    scoreString[26][0] = 24;//0x18;
    scoreString[27][0] = G;
    scoreString[28][0] = 0x02;
    scoreString[29][0] = 0x7F;
    // 0xFF 0x0004 0x00 0x0005 0x00 0x03 0x02 0x7F
    scoreString[30][0] = 0xFF;
    scoreString[31][0] = 0x00;
    scoreString[32][0] = 1;
    scoreString[33][0] = 24;
    scoreString[34][0] = 0x00;
    scoreString[35][0] = 1;
    scoreString[36][0] = 32;
    scoreString[37][0] = G;
    scoreString[38][0] = 0x02;
    scoreString[39][0] = 0x7F;
    //  0xFF 0x0004 0x00 0x0005 0x00 0x07 0x02 0x7F
    scoreString[40][0] = 0xFF;
    scoreString[41][0] = 0x00;
    scoreString[42][0] = 1;
    scoreString[43][0] = 32;
    scoreString[44][0] = 0x00;
    scoreString[45][0] = 1;
    scoreString[46][0] = 40;
    scoreString[47][0] = A;
    scoreString[48][0] = 0x02;
    scoreString[49][0] = 0x7F;
    // 0xFF 0x0004 0x00 0x0005 0x00 0x0A 0x02 0x7F
    scoreString[50][0] = 0xFF;
    scoreString[51][0] = 0x00;
    scoreString[52][0] = 1;
    scoreString[53][0] = 40;
    scoreString[54][0] = 0x00;
    scoreString[55][0] = 1;
    scoreString[56][0] = 48;
    scoreString[57][0] = A;
    scoreString[58][0] = 0x02;
    scoreString[59][0] = 0x7F;
    
    scoreString[60][0] = 0xFF;
    scoreString[61][0] = 0x00;
    scoreString[62][0] = 1;
    scoreString[63][0] = 48;//0x00;
    scoreString[64][0] = 0x00;
    scoreString[65][0] = 2;
    scoreString[66][0] = 0;//0x08;
    scoreString[67][0] = G;
    scoreString[68][0] = 0x02;
    scoreString[69][0] = 0x7F;
    
    scoreString[70][0] = 0xFF;
    scoreString[71][0] = 0x00;
    scoreString[72][0] = 2;
    scoreString[73][0] = 0;//0x00;
    scoreString[74][0] = 0x00;
    scoreString[75][0] = 2;
    scoreString[76][0] = 8;//0x08;
    scoreString[77][0] = F;
    scoreString[78][0] = 0x02;
    scoreString[79][0] = 0x7F;
    
    scoreString[80][0] = 0xFF;
    scoreString[81][0] = 0x00;
    scoreString[82][0] = 2;
    scoreString[83][0] = 8;//0x00;
    scoreString[84][0] = 0x00;
    scoreString[85][0] = 0x02;
    scoreString[86][0] = 16;//0x08;
    scoreString[87][0] = F;
    scoreString[88][0] = 0x02;
    scoreString[89][0] = 0x7F;
    
    scoreString[90][0] = 0xFF;
    scoreString[91][0] = 0x00;
    scoreString[92][0] = 2;
    scoreString[93][0] = 16;//0x00;
    scoreString[94][0] = 0x00;
    scoreString[95][0] = 2;
    scoreString[96][0] = 24;//0x08;
    scoreString[97][0] = E;
    scoreString[98][0] = 0x02;
    scoreString[99][0] = 0x7F;
    
    scoreString[0][1] = 0xFF;
    scoreString[1][1] = 0x00;
    scoreString[2][1] = 2;
    scoreString[3][1] = 24;//0x00;
    scoreString[4][1] = 0x00;
    scoreString[5][1] = 2;
    scoreString[6][1] = 32;//0x08;
    scoreString[7][1] = E;
    scoreString[8][1] = 0x02;
    scoreString[9][1] = 0x7F;
    
    scoreString[10][1] = 0xFF;
    scoreString[11][1] = 0x00;
    scoreString[12][1] = 2;
    scoreString[13][1] = 32;//0x00;
    scoreString[14][1] = 0x00;
    scoreString[15][1] = 2;
    scoreString[16][1] = 40;//0x08;
    scoreString[17][1] = D;
    scoreString[18][1] = 0x02;
    scoreString[19][1] = 0x7F;
    
    scoreString[20][1] = 0xFF;
    scoreString[21][1] = 0x00;
    scoreString[22][1] = 2;
    scoreString[23][1] = 40;//0x00;
    scoreString[24][1] = 0x00;
    scoreString[25][1] = 2;
    scoreString[26][1] = 48;//0x08;
    scoreString[27][1] = D;
    scoreString[28][1] = 0x02;
    scoreString[29][1] = 0x7F;
    
    scoreString[30][1] = 0xFF;
    scoreString[31][1] = 0x00;
    scoreString[32][1] = 2;
    scoreString[33][1] = 48;//0x00;
    scoreString[34][1] = 0x00;
    scoreString[35][1] = 3;
    scoreString[36][1] = 0;//0x08;
    scoreString[37][1] = C;
    scoreString[38][1] = 0x02;
    scoreString[39][1] = 0x7F;
    
    scoreString[40][1] = 0xFF;
    scoreString[41][1] = 0x00;
    scoreString[42][1] = 3;
    scoreString[43][1] = 16;//0x00;
    scoreString[44][1] = 0x00;
    scoreString[45][1] = 4;
    scoreString[46][1] = 0;//0x08;
    scoreString[47][1] = G;
    scoreString[48][1] = 0x02;
    scoreString[49][1] = 0x7F;
    
    scoreString[50][1] = 0xFF;
    scoreString[51][1] = 0x00;
    scoreString[52][1] = 3;
    scoreString[53][1] = 24;//0x00;
    scoreString[54][1] = 0x00;
    scoreString[55][1] = 4;
    scoreString[56][1] = 0;//0x08;
    scoreString[57][1] = E;
    scoreString[58][1] = 0x02;
    scoreString[59][1] = 0x7F;
    
    scoreString[60][1] = 0xFF;
    scoreString[61][1] = 0x00;
    scoreString[62][1] = 3;
    scoreString[63][1] = 32;//0x00;
    scoreString[64][1] = 0x00;
    scoreString[65][1] = 4;
    scoreString[66][1] = 0;//0x08;
    scoreString[67][1] = C;
    scoreString[68][1] = 0x02;
    scoreString[69][1] = 0x7F;
}

void createOutput(){
    
    output = 20000;
    
    // *************************************************************************************
    // READING OF NEW NOTES GOES HERE
    if(note1.measureStart == measure){
        if((note1.locationStart == tempo64Counter) || 
                ((note1.locationStart == tempo32TripletCounter) && (note1.triplet == 1))){
            note1.noteIsOn = 1;
        }
    }

    if(note2.measureStart == measure){
        if((note2.locationStart == tempo64Counter) || 
                ((note2.locationStart == tempo32TripletCounter) && (note2.triplet == 1))){
            note2.noteIsOn = 1;
        }
    }

    if(note3.measureStart == measure){
        if((note3.locationStart == tempo64Counter) || 
                ((note3.locationStart == tempo32TripletCounter) && (note3.triplet == 1))){
            note3.noteIsOn = 1;
        }
    }

    if(note4.measureStart == measure){
        if((note4.locationStart == tempo64Counter) || 
                ((note4.locationStart == tempo32TripletCounter) && (note4.triplet == 1))){
            note4.noteIsOn = 1;
        }
    }

    if(note5.measureStart == measure){
        if((note5.locationStart == tempo64Counter) || 
                ((note5.locationStart == tempo32TripletCounter) && (note5.triplet == 1))){
            note5.noteIsOn = 1;
        }
    }

    if(note6.measureStart == measure){
        if((note6.locationStart == tempo64Counter) || 
                ((note6.locationStart == tempo32TripletCounter) && (note6.triplet == 1))){
            note6.noteIsOn = 1;
        }
    }

    if(note7.measureStart == measure){
        if((note7.locationStart == tempo64Counter) || 
                ((note7.locationStart == tempo32TripletCounter) && (note7.triplet == 1))){
            note7.noteIsOn = 1;
        }
    }

    if(note8.measureStart == measure){
        if((note8.locationStart == tempo64Counter) || 
                ((note8.locationStart == tempo32TripletCounter) && (note8.triplet == 1))){
            note8.noteIsOn = 1;
        }
    }

    if(note9.measureStart == measure){
        if((note9.locationStart == tempo64Counter) || 
                ((note9.locationStart == tempo32TripletCounter) && (note9.triplet == 1))){
            note9.noteIsOn = 1;
        }
    }

    if(note10.measureStart == measure){
        if((note10.locationStart == tempo64Counter) || 
                ((note10.locationStart == tempo32TripletCounter) && (note10.triplet == 1))){
            note10.noteIsOn = 1;
        }
    }
    // *************************************************************************************

    // *************************************************************************************
    // PLAYING NOTES GOES HERE
    if(note1.noteIsOn == 1){
        output = output + note1.waveform[note1.currentElement];
        note1.currentElement++;

        if(note1.currentElement >= note1.waveformElementMax) note1.currentElement = 0;

        if(note1.measureEnd == measure){
            if((note1.locationEnd == tempo64Counter) || 
                    ((note1.locationEnd == tempo32TripletCounter) && (note1.triplet == 1))){
                note1.noteIsOn = 0;
                note1.available = 1;
            }
        }
    }

    if(note2.noteIsOn == 1){
        output = output + note2.waveform[note2.currentElement];
        note2.currentElement++;

        if(note2.currentElement >= note2.waveformElementMax) note2.currentElement = 0;

        if(note2.measureEnd == measure){
            if((note2.locationEnd == tempo64Counter) || 
                    ((note2.locationEnd == tempo32TripletCounter) && (note2.triplet == 1))){
                note2.noteIsOn = 0;
                note2.available = 1;
            }
        }
    }

    if(note3.noteIsOn == 1){
        output = output + note3.waveform[note3.currentElement];
        note3.currentElement++;

        if(note3.currentElement >= note3.waveformElementMax) note3.currentElement = 0;

        if(note3.measureEnd == measure){
            if((note3.locationEnd == tempo64Counter) || 
                    ((note3.locationEnd == tempo32TripletCounter) && (note3.triplet == 1))){
                note3.noteIsOn = 0;
                note3.available = 1;
            }
        }
    }

    if(note4.noteIsOn == 1){
        output = output + note4.waveform[note4.currentElement];
        note4.currentElement++;

        if(note4.currentElement >= note4.waveformElementMax) note4.currentElement = 0;

        if(note4.measureEnd == measure){
            if((note4.locationEnd == tempo64Counter) || 
                    ((note4.locationEnd == tempo32TripletCounter) && (note4.triplet == 1))){
                note4.noteIsOn = 0;
                note4.available = 1;
            }
        }
    }

    if(note5.noteIsOn == 1){
        output = output + note5.waveform[note5.currentElement];
        note5.currentElement++;

        if(note5.currentElement >= note5.waveformElementMax) note5.currentElement = 0;

        if(note5.measureEnd == measure){
            if((note5.locationEnd == tempo64Counter) || 
                    ((note5.locationEnd == tempo32TripletCounter) && (note5.triplet == 1))){
                note5.noteIsOn = 0;
                note5.available = 1;
            }
        }
    }

    if(note6.noteIsOn == 1){
        output = output + note6.waveform[note6.currentElement];
        note6.currentElement++;

        if(note6.currentElement >= note6.waveformElementMax) note6.currentElement = 0;

        if(note6.measureEnd == measure){
            if((note6.locationEnd == tempo64Counter) || 
                    ((note6.locationEnd == tempo32TripletCounter) && (note6.triplet == 1))){
                note6.noteIsOn = 0;
                note6.available = 1;
            }
        }
    }

    if(note7.noteIsOn == 1){
        output = output + note7.waveform[note7.currentElement];
        note7.currentElement++;

        if(note7.currentElement >= note7.waveformElementMax) note7.currentElement = 0;

        if(note7.measureEnd == measure){
            if((note7.locationEnd == tempo64Counter) || 
                    ((note7.locationEnd == tempo32TripletCounter) && (note7.triplet == 1))){
                note7.noteIsOn = 0;
                note7.available = 1;
            }
        }
    }

    if(note8.noteIsOn == 1){
        output = output + note8.waveform[note8.currentElement];
        note8.currentElement++;

        if(note8.currentElement >= note8.waveformElementMax) note8.currentElement = 0;

        if(note8.measureEnd == measure){
            if((note8.locationEnd == tempo64Counter) || 
                    ((note8.locationEnd == tempo32TripletCounter) && (note8.triplet == 1))){
                note8.noteIsOn = 0;
                note8.available = 1;
            }
        }
    }

    if(note9.noteIsOn == 1){
        output = output + note9.waveform[note9.currentElement];
        note9.currentElement++;

        if(note9.currentElement >= note9.waveformElementMax) note9.currentElement = 0;

        if(note9.measureEnd == measure){
            if((note9.locationEnd == tempo64Counter) || 
                    ((note9.locationEnd == tempo32TripletCounter) && (note9.triplet == 1))){
                note9.noteIsOn = 0;
                note9.available = 1;
            }
        }
    }
    if(note10.noteIsOn == 1){
        output = output + note10.waveform[note10.currentElement];
        note10.currentElement++;

        if(note10.currentElement >= note10.waveformElementMax) note10.currentElement = 0;

        if(note10.measureEnd == measure){
            if((note10.locationEnd == tempo64Counter) || 
                    ((note10.locationEnd == tempo32TripletCounter) && (note10.triplet == 1))){
                note10.noteIsOn = 0;
                note10.available = 1;
            }
        }
    }
    
    update = 0;
}