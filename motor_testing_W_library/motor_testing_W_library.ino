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
  pinMode(gearUp, OUTPUT);
  pinMode(gearDown, OUTPUT);

  //make sure mainSig defaults to high and LedPin is low
  digitalWrite(mainSig, HIGH);
  digitalWrite(ledPin, LOW);

  digitalWrite(gearUp, HIGH);
  digitalWrite(gearDown, HIGH);


  //make sure digital pot outputs 0 from wiper
  MCP4146.set(0);

  //set up interrupts
  attachInterrupt(digitalPinToInterrupt(buttonApin), ISR_LED, RISING);
  //Note: FALLING is for when the pin goes from high to low.

  Serial.begin(9600); 

}

void loop() {
  //do nothing if the motor is not turned on
  if (!on) {
    //Serial.println("not on in loop");
    MCP4146.set(0);

    return; 
  }
  
  digitalWrite(ledPin, HIGH); 
  digitalWrite(mainSig, LOW);
  
  //read from mechanical pot turned by hand
  int wiperValue = analogRead(wiper);  //note analogRead can return 0 to 1023
  
  
  //map the wipeValue to the control for the motor
  int motorControlValue = map(wiperValue, 60, 1023, 0, 255); 
  if (motorControlValue < 0) 
    motorControlValue = 0;

  //send signal for digital Pot via SPI
  MCP4146.set(motorControlValue);
  if (count % 1000 == 0) {
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
  delay(500); //send a low pulse of 0.5 seconds
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
