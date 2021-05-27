//Raymond

#include <SPI.h>
#define MOSI 11
#define MISO 12
#define SCK 13
#define CS0 10


int ledPin = 5;
int buttonApin = 2; //on uno, pin 2 and 3 allow interrupts
bool on = false;


const byte motorControl = 9;
const byte motorPower = 4;
const byte wiper = A0;
const byte main = 5;
//ISR function declaration
void ISR_LED();

void setup() {
  // put your setup code here, to run once:
  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(CS0, OUTPUT); //pot0 chipselect

  //Initialise SPI interface
  //SPI clock speed:10MHz, Data Shift:MSB First, Data Clock Idle: SPI_MODE0 (MODE =00)
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); //comment if not MCP4161
  SPI.begin();


  //set up interrupts
  attachInterrupt(digitalPinToInterrupt(buttonApin), ISR_LED, FALLING);
  //Note: FALLING is for when the pin goes from high to low.

  serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!on) {
    digitalWrite(ledPin, LOW);  
    digitalWrite(main, HIGH);
    return; 
  }
  
  digitalWrite(ledPin, HIGH); 
  digitalWrite(main, LOW);
  
  //read from POT
  int wiperValue = analogRead(wiper);  //note analogRead can return 0 to 1023
  
  
  //map the wipeValue to the PWM control for the motor
  int motorControlValue = map(wiperValue, 5, 1023, 0, 180); //note: servo motor rotates from 0 to 180
  
  //write to control servoMotor
  servoObj.write(motorControlValue);
  //delay(100);
  Serial.println(motorControlValue);

}

void ISR_LED() {
  //ledState = !ledState;
  on = !on;
  
  Serial.print("Button Pressed\n");
}

void digitalPotWrite(int cs, int, address, int value) {
  digitalWrite(CS, LOW);
  SPI.transfer(address); //if MCP4161
  //SPI.transfer(B00010001); //if MCP41010
  SPI.transfer(value);
  digitalWrite(CS, HIGH);
}
