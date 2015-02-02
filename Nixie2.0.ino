
//**************************************************************//
//  Name    : NixieSpeed                              
//  Author  : Hartvik Line
//  Date    : 17 Aug, 2014    
//  Modified: 02 feb 2015                                 
//  Version : 2.0                                            
//  Notes   : Code for calculating speed and show in nixie via shiftreg 74HC595 and K155 nixiedrivers
//          :                          
//**************************************************************//

#include <Encoder.h>
#define BUFFER_SIZE 100
Encoder myEnc(2, 3);

int oldSpeed = 111;
int newSpeed = 222;
int cSpeed;

byte byteArray[100];
byte byteArrayFlip[100];
//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;
long pos1;
long pos2;
long diff = 0;
long diffold = 0;
long kmt;
void setup() {
	//Set pins to output so you can control the shift register
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	Serial.begin(9600);
	int count = 0;

	//Making an byteArray to represent the decimalvalue from the shiftreg to the nixiedrivers
	for (int i = 0; i<10; i++) {
		for (int j = 0; j<10; j++) {
			byteArray[count] = (j << 4) | i;
			count++;
		}
	}
	}


void count(byte data, byte data2){


	digitalWrite(latchPin, LOW);
	// shift out the bits:
	shiftOut(dataPin, clockPin, MSBFIRST, data);
	shiftOut(dataPin, clockPin, MSBFIRST, data2);
	
	digitalWrite(latchPin, HIGH);
}

int calculateNewSpeed(){

	pos1 = myEnc.read();
	delay(150);
	pos2 = myEnc.read();
	diff = abs((pos1 - pos2));
	myEnc.write(0);



	kmt = ((diff + diffold) / 24) / 2;
	diffold = diff;
	Serial.println(kmt);
	return kmt;

}

void calSpeed(int speed){
	count(byteArray[speed % 100], byteArray[speed / 100]);
}

void loop() {
	newSpeed = calculateNewSpeed();
	if (oldSpeed != newSpeed){
		calSpeed(newSpeed);
		newSpeed = oldSpeed;
	}
}