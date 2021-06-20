//Raymond

#include <SPI.h>
#include <mcp4xxx.h>
using namespace icecave::arduino;

/*Note that in pins_arduino.h (included in Arduino IDE installation):
MOSI = 11  should connect
MISO = 12
SCK = 13 should connect
SS = 10 should connect
*/

const byte CS0 = SS;
const byte CS1 = 6;
const byte ledPin = 3;
const byte buttonApin = 2; //on uno, pin 2 and 3 allow interrupts
const byte mainSig = 9;
const byte wiper = A0;
const byte gearUp = 7;
const byte gearDown = 8;

const byte address = 0x00;

bool on;

int count;

//initialize the digital pot object for MCP4161 
MCP4XXX MCP4146_0(CS0);//note that the constructor calls SPI.begin()
/*this defaults to:
MCP4XXX MCP4146_0(CS0, B00, 255, 1);  
*/
MCP4XXX MCP4146_1(CS1);//note that the constructor calls SPI.begin()


void setup() {
  
  on = false;
  
  pinMode(ledPin, OUTPUT);
  pinMode(mainSig, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(CS0, OUTPUT); //pot0 (accel) chipselect
  pinMode(CS1, OUTPUT); //pot1 (regen) chipselect
  pinMode(gearUp, OUTPUT);
  pinMode(gearDown, OUTPUT);

  //make sure mainSig defaults to high and LedPin is low
  digitalWrite(mainSig, HIGH);
  digitalWrite(ledPin, LOW);

  digitalWrite(gearUp, HIGH);
  digitalWrite(gearDown, HIGH);

  digitalWrite(CS1, HIGH); //need to deselect regen pot
  
  //make sure digital pot outputs 0 from wiper
  MCP4146_0.set(0);

  MCP4146_1.set(0);

  //set up interrupts
  attachInterrupt(digitalPinToInterrupt(buttonApin), ISR_LED, RISING);
  //Note: FALLING is for when the pin goes from high to low.

  Serial.begin(9600); 

}

void loop() {
  //do nothing if the motor is not turned on
  if (!on) {
    //Serial.println("not on in loop");
    MCP4146_0.set(0);
    MCP4146_1.set(0);

    return; 
  }
  MCP4146_1.set(0);

  digitalWrite(ledPin, HIGH); 
  digitalWrite(mainSig, LOW);
  
  //read from mechanical pot turned by hand
  int wiperValue = analogRead(wiper);  //note analogRead can return 0 to 1023
  
  
  //map the wipeValue to the control for the motor
  int motorControlValue = map(wiperValue, 60, 1023, 0, 255); 
  if (motorControlValue < 0) 
    motorControlValue = 0;

  //send signal for digital Pot via SPI
  MCP4146_0.set(motorControlValue);
  if (count % 1000 == 0) {
    Serial.print("Accel Wiper Position: ");
    Serial.println(motorControlValue);
  }
  count++;

  //check and send gear up and down signals if necessary
  if (Serial.available() > 0) {
    String serialInput = Serial.readString();
    if (serialInput == "w") {
      changeGear(gearUp);
      Serial.println("gear up");
    }
    else if (serialInput == "s") {
      changeGear(gearDown);
      Serial.println("gear down");
    }
  }

}

//this function sends a pulse to change gear
void changeGear(byte gearPin) {
  digitalWrite(gearPin, LOW);
  delay(20); //send a low pulse of 20ms
  digitalWrite(gearPin, HIGH);
}

void ISR_LED() {
  //ledState = !ledState;
  Serial.print("Button Pressed\n");

  if (on) {
    on = false;
    Serial.print("turn off\n");
    digitalWrite(ledPin, LOW);  
    digitalWrite(mainSig, HIGH);
    return;
  }
  if (!on) {
    Serial.print("turn on\n");
    on = true;
    return;
  }
}

/* 
//unnecessary here
void digitalPotWrite(int CS, int address, int command) {
  digitalWrite(CS, LOW);
  SPI.transfer(address); //if MCP4161
  //SPI.transfer(B00010001); //if MCP41010
  SPI.transfer(command);
  digitalWrite(CS, HIGH);
} */
