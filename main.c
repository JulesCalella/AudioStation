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

NOTE noteBufferArray[NOTE_BUFFER_SIZE];

/*******************************************************************************
 * Timer 1 Interrupt
 * 
 * Sampling Frequency = 16kHz
 ******************************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{    
    // Send output to DAC
    dacOutput(output);
    
    // Tells the main program if can update the output
    // This controls the index counter of the waveforms
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
    
    // Set all structs to available
    int i;
    for(i=0; i<NOTE_BUFFER_SIZE; i++){
        noteBufferArray[i].available = 1;
    }
    
    // Write the song to the string
    fillString();
    
    // Prefill all available structs
    for(i=0; i<NOTE_BUFFER_SIZE; i++){
        parseScoreString();
    }
    
    // Write the output values for measure 0
    createOutput();
    
    // Set up the device
    adcInit();
    timerInit();
    
    // Set the volumes for the overtones
    setVolumes();
    
    // Main while loop
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

    int i;
    for(i=0; i<NOTE_BUFFER_SIZE; i++){
        if(noteBufferArray[i].available == 1){
            noteBufferArray[i].available = 0;
            noteBufferArray[i].measureStart = measureStartValue;
            noteBufferArray[i].measureEnd = measureEndValue;
            noteBufferArray[i].locationStart = locationStartValue;
            noteBufferArray[i].locationEnd = locationEndValue;
            noteBufferArray[i].pitch = pitchValue;
            noteBufferArray[i].triplet = isTriplet;
            noteBufferArray[i].waveform = waveform;
            noteBufferArray[i].waveformElementMax = waveformMax;
            noteBufferArray[i].currentElement = 0;
            
            scoreElementY += 10;
    
            // Bounds Check
            if(scoreElementY >= SCORE_SIZE_Y){
                scoreElementY = 0;
                scoreElementX++;

                if((scoreElementX == SCORE_SIZE_X) && (scoreElementY == SCORE_SIZE_Y)) return -1;
            }
            
            return 0;
        }
    }
    
    // No available structs to write to
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

/* 
 * Test Song
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
    scoreString[8][0] = 0x01;
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
    scoreString[18][0] = 0x01;
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
    scoreString[28][0] = 0x01;
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
    scoreString[38][0] = 0x01;
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
    scoreString[48][0] = 0x01;
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
    scoreString[58][0] = 0x01;
    scoreString[59][0] = 0x7F;
    
    scoreString[60][0] = 0xFF;
    scoreString[61][0] = 0x00;
    scoreString[62][0] = 1;
    scoreString[63][0] = 48;//0x00;
    scoreString[64][0] = 0x00;
    scoreString[65][0] = 2;
    scoreString[66][0] = 0;//0x08;
    scoreString[67][0] = G;
    scoreString[68][0] = 0x01;
    scoreString[69][0] = 0x7F;
    
    scoreString[70][0] = 0xFF;
    scoreString[71][0] = 0x00;
    scoreString[72][0] = 2;
    scoreString[73][0] = 0;//0x00;
    scoreString[74][0] = 0x00;
    scoreString[75][0] = 2;
    scoreString[76][0] = 8;//0x08;
    scoreString[77][0] = F;
    scoreString[78][0] = 0x01;
    scoreString[79][0] = 0x7F;
    
    scoreString[80][0] = 0xFF;
    scoreString[81][0] = 0x00;
    scoreString[82][0] = 2;
    scoreString[83][0] = 8;//0x00;
    scoreString[84][0] = 0x00;
    scoreString[85][0] = 0x02;
    scoreString[86][0] = 16;//0x08;
    scoreString[87][0] = F;
    scoreString[88][0] = 0x01;
    scoreString[89][0] = 0x7F;
    
    scoreString[90][0] = 0xFF;
    scoreString[91][0] = 0x00;
    scoreString[92][0] = 2;
    scoreString[93][0] = 16;//0x00;
    scoreString[94][0] = 0x00;
    scoreString[95][0] = 2;
    scoreString[96][0] = 24;//0x08;
    scoreString[97][0] = E;
    scoreString[98][0] = 0x01;
    scoreString[99][0] = 0x7F;
    
    scoreString[0][1] = 0xFF;
    scoreString[1][1] = 0x00;
    scoreString[2][1] = 2;
    scoreString[3][1] = 24;//0x00;
    scoreString[4][1] = 0x00;
    scoreString[5][1] = 2;
    scoreString[6][1] = 32;//0x08;
    scoreString[7][1] = E;
    scoreString[8][1] = 0x01;
    scoreString[9][1] = 0x7F;
    
    scoreString[10][1] = 0xFF;
    scoreString[11][1] = 0x00;
    scoreString[12][1] = 2;
    scoreString[13][1] = 32;//0x00;
    scoreString[14][1] = 0x00;
    scoreString[15][1] = 2;
    scoreString[16][1] = 40;//0x08;
    scoreString[17][1] = D;
    scoreString[18][1] = 0x01;
    scoreString[19][1] = 0x7F;
    
    scoreString[20][1] = 0xFF;
    scoreString[21][1] = 0x00;
    scoreString[22][1] = 2;
    scoreString[23][1] = 40;//0x00;
    scoreString[24][1] = 0x00;
    scoreString[25][1] = 2;
    scoreString[26][1] = 48;//0x08;
    scoreString[27][1] = D;
    scoreString[28][1] = 0x01;
    scoreString[29][1] = 0x7F;
    
    scoreString[30][1] = 0xFF;
    scoreString[31][1] = 0x00;
    scoreString[32][1] = 2;
    scoreString[33][1] = 48;//0x00;
    scoreString[34][1] = 0x00;
    scoreString[35][1] = 3;
    scoreString[36][1] = 0;//0x08;
    scoreString[37][1] = C;
    scoreString[38][1] = 0x01;
    scoreString[39][1] = 0x7F;
    
    scoreString[40][1] = 0xFF;
    scoreString[41][1] = 0x00;
    scoreString[42][1] = 3;
    scoreString[43][1] = 16;//0x00;
    scoreString[44][1] = 0x00;
    scoreString[45][1] = 4;
    scoreString[46][1] = 0;//0x08;
    scoreString[47][1] = G;
    scoreString[48][1] = 0x01;
    scoreString[49][1] = 0x7F;
    
    scoreString[50][1] = 0xFF;
    scoreString[51][1] = 0x00;
    scoreString[52][1] = 3;
    scoreString[53][1] = 24;//0x00;
    scoreString[54][1] = 0x00;
    scoreString[55][1] = 4;
    scoreString[56][1] = 0;//0x08;
    scoreString[57][1] = E;
    scoreString[58][1] = 0x01;
    scoreString[59][1] = 0x7F;
    
    scoreString[60][1] = 0xFF;
    scoreString[61][1] = 0x00;
    scoreString[62][1] = 3;
    scoreString[63][1] = 32;//0x00;
    scoreString[64][1] = 0x00;
    scoreString[65][1] = 4;
    scoreString[66][1] = 0;//0x08;
    scoreString[67][1] = C;
    scoreString[68][1] = 0x01;
    scoreString[69][1] = 0x7F;
}

void createOutput(){
    
    output = 32000;
    
    // *************************************************************************************
    // READING OF NEW NOTES GOES HERE
    int i;
    for(i=0; i<NOTE_BUFFER_SIZE; i++){
        if((noteBufferArray[i].measureStart == measure) && (noteBufferArray[i].available == 0)){
            if((noteBufferArray[i].locationStart == tempo64Counter) || 
                    ((noteBufferArray[i].locationStart == tempo32TripletCounter) && noteBufferArray[i].triplet == 1)){
                noteBufferArray[i].noteIsOn = 1;
            }
        }
    }
    
    // *************************************************************************************

    // *************************************************************************************
    // PLAYING NOTES GOES HERE
    int increment;
    for(i=0; i<NOTE_BUFFER_SIZE; i++){
        // Determine if the note is currently on
        if(noteBufferArray[i].noteIsOn == 1){
            // If it is on, add its current value to the output
            output = output + noteBufferArray[i].waveform[noteBufferArray[i].currentElement];
            
            // Increment the index for the next read
            increment = 1 + noteBufferArray[i].pitch;
            noteBufferArray[i].currentElement = noteBufferArray[i].currentElement + increment;
            
            // If the index goes out of bounds, reset it
            if(noteBufferArray[i].currentElement >= noteBufferArray[i].waveformElementMax){
                noteBufferArray[i].currentElement = noteBufferArray[i].currentElement - noteBufferArray[i].waveformElementMax;
            }
            
            // Determine if the note has come to an end
            if(noteBufferArray[i].measureEnd == measure){
                if((noteBufferArray[i].locationEnd == tempo64Counter) ||
                        ((noteBufferArray[i].locationEnd == tempo32TripletCounter) && noteBufferArray[i].triplet == 1)){
                    // Turn note off
                    noteBufferArray[i].noteIsOn = 0;
                    
                    // Mark it available for incoming data
                    noteBufferArray[i].available = 1;
                }
            }
        }
    }
    
    // Reset update so the timer interrupt can call this function
    update = 0;
}