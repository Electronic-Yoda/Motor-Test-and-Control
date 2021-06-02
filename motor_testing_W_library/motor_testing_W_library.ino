#include <SPI.h>
#include <mcp4xxx.h>
using namespace icecave::arduino;

/*Note that in pins_arduino.h (included in Arduino IDE installation):
MOSI = 11
MISO = 12
SCK = 13 
SS = 10
*/

const byte CS0 = SS;
const byte ledPin = 5;
const byte buttonApin = 2; //on uno, pin 2 and 3 allow interrupts
const byte motorControl = 9;
const byte mainSig = 8;
const byte wiper = A0;

const byte address = 0x00;

bool on;

//initialize the digital pot object for MCP4161 
MCP4XXX MCP4146(CS0);//note that the constructor calls SPI.begin()
/*this defaults to:
MCP4XXX MCP4146(CS0, B00, 255, 1);  
*/
void setup() {
  
  on = false;
  
  pinMode(ledPin, OUTPUT);
  pinMode(mainSig, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(CS0, OUTPUT); //pot0 chipselect

  //make sure mainSig defaults to high and LedPin is low
  digitalWrite(mainSig, HIGH);
  pinMode(ledPin, LOW);



  //make sure digital pot outputs 0 from wiper
  MCP4146.set(0);

  //set up interrupts
  attachInterrupt(digitalPinToInterrupt(buttonApin), ISR_LED, FALLING);
  //Note: FALLING is for when the pin goes from high to low.

  Serial.begin(9600); 
}

void loop() {
  //do nothing if the motor is not turned on
  if (!on) {
    return; 
  }
  
  digitalWrite(ledPin, HIGH); 
  digitalWrite(mainSig, LOW);
  
  //read from mechanical pot turned by hand
  int wiperValue = analogRead(wiper);  //note analogRead can return 0 to 1023
  
  
  //map the wipeValue to the control for the motor
  int motorControlValue = map(wiperValue, 5, 1023, 0, 255); 
  
  MCP4146.set(motorControlValue);

}

void ISR_LED() {
  //ledState = !ledState;
  Serial.print("Button Pressed\n");

  if (on) {
    on = false;
    digitalWrite(ledPin, LOW);  
    digitalPotWrite(CS0, address, 0);
    digitalWrite(mainSig, HIGH);
  }
  if (!on) {
    on = true;
  }
}
