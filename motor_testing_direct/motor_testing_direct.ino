//Raymond

#include <SPI.h>

/*Note that in pins_arduino.h (included in Arduino IDE installation):
MOSI = 11
MISO = 12
SCK = 13 
SS = 10
*/

const byte CS0 = SS;
const byte CS1 = 6;

const byte ledPin = 3;
const byte buttonApin = 2; //on uno, pin 2 and 3 allow interrupts
const byte mainSig = 9;
const byte wiper = A0;


bool on;

void setup() {
  on = false;
  
  pinMode(ledPin, OUTPUT);
  pinMode(mainSig, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(CS0, OUTPUT); //pot0 chipselect
    pinMode(CS1, OUTPUT); //pot1 (regen) chipselect


  //make suire mainSig defaults to high and LedPin is low
  digitalWrite(mainSig, HIGH);
  digitalWrite(ledPin, LOW);


    digitalWrite(CS1, HIGH); //need to deselect regen pot

  //Initialise SPI interface
  //SPI clock speed:10MHz, Data Shift:MSB First, Data Clock Idle: SPI_MODE0 (MODE =00)
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); //comment if not MCP4161
  SPI.begin();

  //make sure digital pot outputs 0 from wiper
  digitalPotWrite(CS0, 0);

  digitalPotWrite(CS1, 0);

  //set up interrupts
  attachInterrupt(digitalPinToInterrupt(buttonApin), ISR_LED, FALLING);
  //Note: FALLING is for when the pin goes from high to low.

  Serial.begin(9600); 
  

  
}

void loop() {
  //do nothing if the motor is not turned on
  if (!on) {
      digitalPotWrite(CS0, address, 0);
      digitalPotWrite(CS1, address, 0);

    return; 
  }
  digitalPotWrite(CS1, address, 0);

  digitalWrite(ledPin, HIGH); 
  digitalWrite(mainSig, LOW);
  
  //read from mechanical pot turned by hand
  int wiperValue = analogRead(wiper);  //note analogRead can return 0 to 1023
  
  
  //map the wipeValue to the control for the motor
  int motorControlValue = map(wiperValue, 60, 1023, 0, 255); 
  if (motorControlValue < 0) motorControlValue = 0;
  //digitalPotWrite(CS0, address, motorControlValue);
  int constantMotorControlValue = 200;
  digitalPotWrite(CS0, address, constantMotorControlValue);
  Serial.println(constantMotorControlValue);

  
  //Serial.println(motorControlValue);
  delay(500);

}

void ISR_LED() {
  //ledState = !ledState;
  Serial.print("Button Pressed\n");

  if (on) {
    Serial.print("turn off\n");
    on = false;
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

void digitalPotWrite(int CS, int wiperValue) {
  byte ninethDataBit = (wiperValue >> 8) & 0b1;

	byte potAddress = 0b0000;
	byte writeCommand = 0b00;
	byte commandByte  = (potAddress << 4) | (writeCommand << 2) | ninethDataBit;
	byte dataByte = wiperValue & 0xFF;
  
  digitalWrite(CS, LOW);
  SPI.transfer(commandByte); //if MCP4161
  //SPI.transfer(B00010001); //if MCP41010
  SPI.transfer(dataByte);
  digitalWrite(CS, HIGH);
}
