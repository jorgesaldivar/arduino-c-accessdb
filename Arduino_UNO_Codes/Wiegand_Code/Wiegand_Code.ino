//***************** vv DOCUMENTING AND HELPFUL STUFF vv ********************
	//Wiegand_Code.ino
	
	//Compiled using arduino uno software found at
	//http://arduino.cc/en/Main/Software
	//On Windows 7 64 bits
	
	//IMPORTANT
	//ARDUINO UNO MUST BE 
	//COMPILED USING A DLINE Mode and
	//RUN USING UART Mode
	
	//Created by Jorge Saldivar on May 13 2012.
	//using Luis Ricardo Salgado example for academic purposes

//***************** vv DOCUMENTING AND HELPFUL STUFF vv ********************

//Libraries used
#include "pins_arduino.h"

	//Declaration for the PINS that the Arduino UNO used for the card reader wiegand
	const int WiegandData1 = 4;
	const int WiegandData0 = 5;

	//Declaration for the PINS the ARDUINO UNO used to tell if a door opened or closed
	const int greenLed = 8;
	const int redLed = 9;
	const int tunerBip = 10;
	
	//Used for recieving a complete string from the serial communication
	String inputString = "";	
	boolean stringComplete = false;
	
	//Variables used for the wiegand so it can read the card bits
	volatile long readerBits = 0;
	volatile int readerBitCount = 0;

	//Used for getting the user ID from the card. 
	//userIdB * 256 + userIdA = userId as we know it
	char userIdA;
	char userIdB;

	//***************** vv Code for managing interruptions vv ********************
	/*
		All terminals can generate interrupts on ATmega168 transition.
		The bit corresponding to the terminal to be the source of the event should
		be enabled in the registry correspond PCInt and a service routine (ISR).
		Because the registration PCInt operates port, not terminal, the routine ISR
		must use some algorithm to implement an interrupt service routine
		by terminal

		Correspondenicas terminal and to interrupt masking registers:
		D0-D7 => 16-23 = PCIR2 PCInt = PD = PCIE2 = pcmsk2
		D8-D13 => 0-5 = PCIR0 PCInt = PB = PCIE0 = pcmsk0
		A0-A5 (D14-D19) => 8-13 = PCIR1 PCInt = PC = PCIE1 = pcmsk1
	 */
	 
	volatile uint8_t *port_to_pcmask[] = {
	  &PCMSK0,
	  &PCMSK1,
	  &PCMSK2
	};

	typedef void (*voidFuncPtr)(void);

	volatile static voidFuncPtr PCintFunc[24] = { 
	  NULL };

	volatile static uint8_t PCintLast[3];

	void PCattachInterrupt(uint8_t pin, void (*userFunc)(void), int mode) {
	
		  uint8_t bit = digitalPinToBitMask(pin);
		  uint8_t port = digitalPinToPort(pin);
		  uint8_t slot;
		  volatile uint8_t *pcmask;

		  if (mode != CHANGE) {
			return;
		  }
		  // Modify the registry ("Interrupt Control Register") ICR
		  // As requested terminal, validating that is between 0 and 13
		  
		 
		  if (port == NOT_A_PORT) {
			return;
		  } 
		  else {
			port -= 2;
			pcmask = port_to_pcmask[port];
		  }
		  
		  slot = port * 8 + (pin % 8);
		  PCintFunc[slot] = userFunc;
		  
		  // Set the interrupt mask
		  *pcmask |= bit;
		  
		  // Interrupt Enable
		  PCICR |= 0x01 << port;
	}

	static void PCint(uint8_t port) {
	
		  uint8_t bit;
		  uint8_t curr;
		  uint8_t mask;
		  uint8_t pin;

		  // get the pin states for the indicated port.
		  curr = *portInputRegister(port+2);
		  mask = curr ^ PCintLast[port];
		  PCintLast[port] = curr;
		  // mask is pins that have changed. screen out non pcint pins.
		  if ((mask &= *port_to_pcmask[port]) == 0) {
			return;
		  }
		  // mask is pcint pins that have changed.
		  for (uint8_t i=0; i < 8; i++) {
			bit = 0x01 << i;
			if (bit & mask) {
			  pin = port * 8 + i;
			  if (PCintFunc[pin] != NULL) {
				PCintFunc[pin]();
			  }
			}
		  }
	}

	SIGNAL(PCINT0_vect) {
	  PCint(0);
	}
	SIGNAL(PCINT1_vect) {
	  PCint(1);
	}
	SIGNAL(PCINT2_vect) {
	  PCint(2);
	}

	//***************** ^^ Code for managing interruptions ^^ ********************
	//*********** vv Code for counting and storing bits vv **********
	void readerOne(void) {
	  if(digitalRead(4) == LOW){
	   readerBitCount++;
	   readerBits = readerBits << 1;  //Move the bits ...
	   readerBits |= 1;  // ... add a bit to '1 'in the least significant bit
	  }
	}

	void readerZero(void) {
	  if(digitalRead(5) == LOW){
	   readerBitCount++;
	   readerBits = readerBits << 1;  //Move the bits ...
				  
	  }
	}
	//*********** ^^ Code for counting and storing bits ^^ **********

	void setup()
	{
		  //Serial transfer speed
		  Serial.begin(9600);
		  
		  // Attach pin change interrupt service routines from the Wiegand RFID readers
		  PCattachInterrupt(WiegandData1, readerOne, CHANGE);
		  PCattachInterrupt(WiegandData0, readerZero, CHANGE);
		  delay(10);
		  
		  // the interrupt in the Atmel processor misses out the first negative pulse as the inputs are already high,
		  // so this gives a pulse to each reader input line to get the interrupts working properly.
		  // Then clear out the reader variables.
		  // The readers are open collector sitting normally at a one so this is OK
		  for(int i = WiegandData1; i<=WiegandData0; i++){
			   pinMode(i, OUTPUT);
			   digitalWrite(i, HIGH); // enable internal pull up causing a one
			   digitalWrite(i, LOW);  // disable internal pull up causing zero and thus an interrupt
			   pinMode(i, INPUT);
			   digitalWrite(i, HIGH); // enable internal pull up
		  }
		  delay(10);
		  
		  // put the reader input variables to zero
		  readerBits=0;
		  readerBitCount = 0;
		  
		  // enable pins of the LEDs
		  pinMode(greenLed, OUTPUT);
		  pinMode(redLed, OUTPUT);
		  
		  digitalWrite(13, HIGH);  // show Arduino has finished initilisation
	}

	void loop() {
	  
		  //If the card is slided in the card reader enters the condition	
		  if(readerBitCount >= 26){
			   
			   //Protocol to write on serial
			   Serial.write("3");
			   Serial.write(":");
			   Serial.write("1");
			   Serial.write("-");
			   
			   //Gets the user id
			   int userID = readerBits >> 1 & 0xffff;
			   int a,b;
			   a = userID & 0xff;
			   b = userID >> 8 & 0xff;
			   
			   userIdA = (char)a;
			   userIdB = (char)b;
			   
			   //Sends the user id in 1 Byte. 
			   //Serial.write can only send 1 byte at a time
			   Serial.write(userIdB);
			   Serial.write("+");
			   Serial.write(userIdA);
			   
			   readerBits = 0;
			   readerBitCount = 0;
		  }
			 //String complete received from serial communication
			 if(stringComplete){
			   
			   //Protocol to turn on led on on green or red and making a sound or not
			   if(inputString[0] == '3' && inputString[1] == ':' && inputString[2] == '1' && inputString[3] == '-' && inputString[4] == 's'){
					 digitalWrite(greenLed, HIGH);
					 tone(tunerBip, 2000);
					 delay(1000);
					 digitalWrite(greenLed, LOW);
					 noTone(tunerBip);
			   } else if(inputString[0] == '3' && inputString[1] == ':' && inputString[2] == '1' && inputString[3] == '-' && inputString[4] == 'n'){
					 digitalWrite(redLed, HIGH);
					 delay(1000);
					 digitalWrite(redLed, LOW);
			   }
			   
				inputString="";	//inputString reset to null
				stringComplete=false;
			  }
	}

	//Sets serial communication, always ready to receive
	void serialEvent(){
		 while(Serial.available()){
			  char inChar = (char)Serial.read();	//read char from serial
			  inputString += inChar;	//Creating string
			  
			  if(inChar == '\n'){		//if string is an "enter" the string is complete
					stringComplete = true;
			  } 
		 } 
	}