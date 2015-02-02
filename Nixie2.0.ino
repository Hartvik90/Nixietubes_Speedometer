
//**************************************************************//
//  Name			: NixieSpeed                              
//  Author			: Hartvik Line
//  Date			: 17 Aug, 2014    
//  Modified		: 17 AUG 2014                                 
//  Version			: 1.0                                            
//  Notes			: Code for calculating speed and show in nixie
//					: via shiftreg 74HC595 and K155 nixiedrivers
//****************************************************************


//**************************************************************//
//  Errorcodes		:		Will blink 2 * 500 mS per error.
//					: 300 = Voltage error
//					: 500 = km not increased
//					:	
//****************************************************************

#include <Encoder.h>
#define BUFFER_SIZE 100;
#include <EEPROM.h>
Encoder myEnc(2, 3);

//**************************************************************\\

//Pin layout for 74HC595
int latchPin = 8;
int clockPin = 12;
int sdPin = 4;
int dataPin = 11;

//Pin for powerloss
int shutDownAnalogPin = 0;

//**************************************************************//

int pos1;
int pos2;
long diff = 0;
long diffold = 0;
long oldSpeed = 0;
long newSpeed = 0;
int kCount;
int kmt;
int tenMeterOld;
int tenMeter;

byte byteArray[100];
byte byteArrayFlip[100];


void setup() {
	//set pins to output so you can control the shift register
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

	// Loading tenMeters

	// Creates an array formatting from 8 bit format to 2*4 bit format.
	int count;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			byteArray[count] = (j << 4) | i;
			count++;
		}
	}
	//überpro 1337 K0-D
	//N1x13flUsH 2k4r34L
	for (int i = 1; i < 1000; i *= 10){
		for (int j = 0; j < 10; j++){
			nixieOut(i*j);
			delay(66);
		}
	}

	//skambad dødsteit dustekode kommenter vekk !
	/*	for (long y = 0; y < 30; y++){			// Flushes through all numbers
	nixieOut(y % 10 * pow(10, y / 10));
	delay(166);
	}*/
}
//	Serial.begin(9600);					// Open serialcommunication


void count(byte data, byte data2){
	// Shift out the bits:
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, data);
	shiftOut(dataPin, clockPin, MSBFIRST, data2);
	digitalWrite(latchPin, HIGH);
}

int calculateNewSpeed(){
	myEnc.write(0);
	pos1 = myEnc.read();
	delay(100);
	pos2 = myEnc.read();
	diff = abs(pos1 - pos2);
	kmt = ((diff + diffold) / 60);
	kCount += kmt;
	if (kCount >= 360){
		tenMeter += 1;
		kCount -= 360;
	}

	diffold = diff;
	return kmt;

}

void nixieOut(int treeDig){
	//Calculates from decimalnvalue to binary sh-reg outputnumeric value out to Nixies. 
	//In = 3 digit int/long.
	count(byteArray[treeDig % 100], byteArray[treeDig / 100]);
}

void loop() {
	EEPROM.write(321, 4);
	tenMeter = 123;
	save();
	nixieOut(tenMeter); //Tripcounter (Show 1000 km)
	delay(3000);

	//For debugging memory
	nixieOut(EEPROM.read(4)); //Show EEPROM1
	delay(3000);
	nixieOut(EEPROM.read(5)); //Show EEPROM2
	delay(3000);

	while (true){
		//Save tripcounter if voltage drops under 7 volt.
		if (analogRead(shutDownAnalogPin) < 600){
			noInterrupts();
			shutDown();
		}
		newSpeed = calculateNewSpeed();
		//For debugging:
		//nixieOut(myEnc.read());

		
		if (newSpeed != oldSpeed){
			nixieOut(newSpeed);
			oldSpeed = newSpeed;
		}
	}
}


void load() {
	unsigned int tenMeterBufferLoad;
	tenMeterBufferLoad |= EEPROM.read(5);
	tenMeterBufferLoad << 8;
	tenMeterBufferLoad |= EEPROM.read(4);
	tenMeter = tenMeterBufferLoad;


}

void save(){
	int tenMeterBuffer1 = tenMeter & 0x11;
	int saveBuf1 = tenMeterBuffer1;
	EEPROM.write(saveBuf1, 4);
	int tenMeterBuffer2 = tenMeter & 0x1100;
	int saveBuf2 = tenMeterBuffer1;
	EEPROM.write(saveBuf2, 5);

}

void shutDown(){

	if (tenMeterOld != tenMeter){
		save();
		for (int i = 0; i <= 10; i++){
			nixieOut(500 * (i % 2));						// Errorcode 500, km saved
			delay(500);
		}
	}
	else{
		for (int i = 0; i <= 10; i++){
			nixieOut(300 * (i % 2)); 				// Errorcode 300, voltage problem (Tries to save old value)
			delay(500);
		}
	}

}