#ifndef WAVEFORM_H
#define	WAVEFORM_H

#define SAMPLING_FREQUENCY 16000
#define TEMPO 120
#define PI 3.14159265
#define NOTE_BUFFER_SIZE 20

// Defines for assigning notes value to note stucts
#define A 0x00
#define AS 0x01
#define B 0x02
#define C 0x03
#define CS 0X04
#define D 0x05
#define DS 0x06
#define E 0x07
#define F 0x08
#define FS 0x09
#define G 0x0A
#define GS 0x0B

// Defines for assigning note type to note structs
#define WHOLE_NOTE 0x00
#define HALF_NOTE 0x01
#define QUARTER_NOTE 0x02
#define EIGHTH_NOTE 0x03
#define SIXTEENTH_NOTE 0x04
#define THIRTY_SECOND_NOTE 0x05
#define SIXTY_FOURTH_NOTE 0x06
#define HALF_NOTE_TRIPLET 0x07
#define QUARTER_NOTE_TRIPLET 0x08
#define EIGHTH_NOTE_TRIPLET 0x09
#define SIXTEENTH_NOTE_TRIPLET 0x0A
#define THIRTY_SECOND_NOTE_TRIPLE 0x0B
#define SIXTY_FOURTH_NOTE_TRIPLET 0x0C

typedef struct{
    uint8_t available;
    uint8_t noteIsOn;
    uint8_t pitch;
    uint16_t measureStart;
    uint8_t locationStart;
    uint16_t measureEnd;
    uint8_t locationEnd;
    uint8_t triplet;
    int *waveform;
    uint16_t currentElement;
    uint16_t waveformElementMax;
} NOTE;



extern int totalElementsC;
extern int totalElementsCSharp;
extern int totalElementsD;
extern int totalElementsDSharp;
extern int totalElementsE;
extern int totalElementsF;
extern int totalElementsFSharp;
extern int totalElementsG;
extern int totalElementsGSharp;
extern int totalElementsA;
extern int totalElementsASharp;
extern int totalElementsB;

extern int waveformC[250];
extern int waveformCSharp[250];
extern int waveformD[250];
extern int waveformDSharp[250];
extern int waveformE[250];
extern int waveformF[250];
extern int waveformFSharp[250];
extern int waveformG[250];
extern int waveformGSharp[250];
extern int waveformA[250];
extern int waveformASharp[250];
extern int waveformB[250];

// Function Prototypes
void setVolumes();
void generateWaveformC(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformCSharp(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformD(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformDSharp(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformE(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformF(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformFSharp(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformG(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformGSharp(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformA(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformASharp(float vol1, float vol2, float vol3, float vol4, float vol5);
void generateWaveformB(float vol1, float vol2, float vol3, float vol4, float vol5);


#endif	/* WAVEFORM_H */

