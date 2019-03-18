#include <xc.h>
#include <math.h>
#include "hardwareConfig.h"
#include "waveform.h"

int baseVolume = 1000;

int totalElementsC = (SAMPLING_FREQUENCY / 65.4063) + 0.5;
int totalElementsCSharp = (SAMPLING_FREQUENCY / 69.2956) + 0.5;
int totalElementsD = (SAMPLING_FREQUENCY / 73.4161) + 0.5;
int totalElementsDSharp = (SAMPLING_FREQUENCY / 77.7817) + 0.5;
int totalElementsE = (SAMPLING_FREQUENCY / 82.4068) + 0.5;
int totalElementsF = (SAMPLING_FREQUENCY / 87.3070) + 0.5;
int totalElementsFSharp = (SAMPLING_FREQUENCY / 92.4986) + 0.5;
int totalElementsG = (SAMPLING_FREQUENCY / 97.9988) + 0.5;
int totalElementsGSharp = (SAMPLING_FREQUENCY / 103.826) + 0.5;
int totalElementsA = (SAMPLING_FREQUENCY / 110) + 0.5;
int totalElementsASharp = (SAMPLING_FREQUENCY / 116.5409) + 0.5;
int totalElementsB = (SAMPLING_FREQUENCY / 123.4708) + 0.5;

int waveformC[250];
int waveformCSharp[250];
int waveformD[250];
int waveformDSharp[250];
int waveformE[250];
int waveformF[250];
int waveformFSharp[250];
int waveformG[250];
int waveformGSharp[250];
int waveformA[250];
int waveformASharp[250];
int waveformB[250];

void setVolumes()
{
    LED5 = 1;
    T1CONbits.TON = 0;
    
    uint16_t adc;
    float volume1 = 0.0;
    float volume2 = 0.0;
    float volume3 = 0.0;
    float volume4 = 0.0;
    float volume5 = 0.0;
    
    // Read AN9, pin 12, RV1
    AD1CHS0bits.CH0SA = 9;
    AD1CON1bits.SAMP = 1;
    
    while(AD1CON1bits.DONE != 1);
    
    AD1CON1bits.DONE = 0;
    adc = ADC1BUF0;
    volume1 = adc/4096.0;
    
    
    // Read AN0, pin 13, RV2
    AD1CHS0bits.CH0SA = 0;
    AD1CON1bits.SAMP = 1;
    
    while(AD1CON1bits.DONE != 1);
    
    AD1CON1bits.DONE = 0;
    adc = ADC1BUF0;
    volume2 = adc/4096.0;
    
    
    // Read AN1, pin 14, RV3
    AD1CHS0bits.CH0SA = 1;
    AD1CON1bits.SAMP = 1;
    
    while(AD1CON1bits.DONE != 1);
    
    AD1CON1bits.DONE = 0;
    adc = ADC1BUF0;
    volume3 = adc/4096.0;
    
    
    // Read AN3, pin 16, RV4
    AD1CHS0bits.CH0SA = 3;
    AD1CON1bits.SAMP = 1;
    
    while(AD1CON1bits.DONE != 1);
    
    AD1CON1bits.DONE = 0;
    adc = ADC1BUF0;
    volume4 = adc/4096.0;
    
    
    // Read AN6, pin 21, RV5
    AD1CHS0bits.CH0SA = 6;
    AD1CON1bits.SAMP = 1;
    
    while(AD1CON1bits.DONE != 1);
    
    AD1CON1bits.DONE = 0;
    adc = ADC1BUF0;
    volume5 = adc/4096.0;
    
    generateWaveformC(volume1, volume2, volume3, volume4, volume5);
    generateWaveformCSharp(volume1, volume2, volume3, volume4, volume5);
    generateWaveformD(volume1, volume2, volume3, volume4, volume5);
    generateWaveformDSharp(volume1, volume2, volume3, volume4, volume5);
    generateWaveformE(volume1, volume2, volume3, volume4, volume5);
    generateWaveformF(volume1, volume2, volume3, volume4, volume5);
    generateWaveformFSharp(volume1, volume2, volume3, volume4, volume5);
    generateWaveformG(volume1, volume2, volume3, volume4, volume5);
    generateWaveformGSharp(volume1, volume2, volume3, volume4, volume5);
    generateWaveformA(volume1, volume2, volume3, volume4, volume5);
    generateWaveformASharp(volume1, volume2, volume3, volume4, volume5);
    generateWaveformB(volume1, volume2, volume3, volume4, volume5);
    
    LED5 = 0;
    T1CONbits.TON = 1;
}

void generateWaveformC(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsC; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsC * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsC * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsC * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsC * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsC * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsC * 1.0)));
        
        waveformC[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformCSharp(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsCSharp; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsCSharp * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsCSharp * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsCSharp * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsCSharp * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsCSharp * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsCSharp * 1.0)));
        
        waveformCSharp[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformD(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsD; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsD * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsD * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsD * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsD * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsD * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsD * 1.0)));
        
        waveformD[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformDSharp(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsDSharp; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsDSharp * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsDSharp * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsDSharp * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsDSharp * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsDSharp * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsDSharp * 1.0)));
        
        waveformDSharp[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformE(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsE; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsE * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsE * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsE * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsE * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsE * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsE * 1.0)));
        
        waveformE[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformF(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsF; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsF * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsF * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsF * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsF * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsF * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsF * 1.0)));
        
        waveformF[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformFSharp(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsFSharp; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsFSharp * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsFSharp * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsFSharp * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsFSharp * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsFSharp * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsFSharp * 1.0)));
        
        waveformFSharp[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformG(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsG; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsG * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsG * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsG * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsG * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsG * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsG * 1.0)));
        
        waveformG[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformGSharp(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsGSharp; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsGSharp * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsGSharp * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsGSharp * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsGSharp * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsGSharp * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsGSharp * 1.0)));
        
        waveformGSharp[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformA(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsA; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsA * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsA * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsA * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsA * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsA * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsA * 1.0)));
        
        waveformA[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformASharp(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsASharp; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsASharp * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsASharp * 1.0)));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsASharp * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsASharp * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsASharp * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsASharp * 1.0)));
        
        waveformASharp[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}

void generateWaveformB(float vol1, float vol2, float vol3, float vol4, float vol5)
{
    int i, root, ot1, ot2, ot3, ot4, ot5;
    
    for(i=0; i<totalElementsB; i++)
    {
        root = baseVolume * sin(2.0*PI*(i/(totalElementsB * 1.0)));
        ot1 = vol1 * baseVolume * sin(4.0*PI*(i/(totalElementsB) * 1.0));
        ot2 = vol2 * baseVolume * sin(6.0*PI*(i/(totalElementsB * 1.0)));
        ot3 = vol3 * baseVolume * sin(8.0*PI*(i/(totalElementsB * 1.0)));
        ot4 = vol4 * baseVolume * sin(10.0*PI*(i/(totalElementsB * 1.0)));
        ot5 = vol5 * baseVolume * sin(12.0*PI*(i/(totalElementsB * 1.0)));
        
        waveformB[i] = root + ot1 + ot2 + ot3 + ot4 + ot5;
    }
}