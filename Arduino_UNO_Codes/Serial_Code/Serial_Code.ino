//***************** vv DOCUMENTING AND HELPFUL STUFF vv ********************
	//Serial_Code.ino
	
	//Compiled using arduino uno software found at
	//http://arduino.cc/en/Main/Software
	//On Windows 7 64 bits
	
	//IMPORTANT
	//ARDUINO UNO MUST BE 
	//COMPILED USING A DLINE Mode and
	//RUN USING DLINE Mode
	
	//Created by Jorge Saldivar on May 13 2012.
	//using Tom Igoe and Mikal Hart example which is on a public domain 

//***************** vv DOCUMENTING AND HELPFUL STUFF vv ********************

//Libraries used
#include <SoftwareSerial.h>
	
	//PINS used to receive and transfer bytes
	const int pinRX = 2;
	const int pinTX = 3;

	SoftwareSerial XBee(pinRX, pinTX); // Initialized RX and TX

	void setup()  
	{
		  // Set data rates
		  Serial.begin(9600);
		  // set the data rate for the SoftwareSerial port
		  XBee.begin(9600);
	}

	void loop() // run over and over
	{
		  if (XBee.available())			//If read from Serial hardware then writes to serial software
			Serial.write(XBee.read());	
		  if (Serial.available())		//If read from Serial software writes to serial hardware
			XBee.write(Serial.read());
	}