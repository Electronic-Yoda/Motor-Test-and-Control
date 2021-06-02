//Raymond

#include <SPI.h>

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

void setup() {
  on = false;
  
  pinMode(ledPin, OUTPUT);
  pinMode(mainSig, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(CS0, OUTPUT); //pot0 chipselect

  //make suire mainSig defaults to high and LedPin is low
  digitalWrite(mainSig, HIGH);
  pinMode(ledPin, LOW);

  //Initialise SPI interface
  //SPI clock speed:10MHz, Data Shift:MSB First, Data Clock Idle: SPI_MODE0 (MODE =00)
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); //comment if not MCP4161
  SPI.begin();

  //make sure digital pot outputs 0 from wiper
  digitalPotWrite(CS0, address, 0);


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
  
  digitalPotWrite(CS0, address, motorControlValue);

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

void digitalPotWrite(int CS, int address, int command) {
  digitalWrite(CS, LOW);
  SPI.transfer(address); //if MCP4161
  //SPI.transfer(B00010001); //if MCP41010
  SPI.transfer(command);
  digitalWrite(CS, HIGH);
}
