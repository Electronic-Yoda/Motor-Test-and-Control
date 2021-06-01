#include <SPI.h>
#include <mcp4xxx.h>

using namespace icecave::arduino;
MCP4XXX* pot;


void setup() {
  // put your setup code here, to run once:
  pot = new MCP4XXX(9, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
    

}
