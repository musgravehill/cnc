//Configuration Sketch - Version 1.01 - www.cheap-thermocam.tk

#include <i2cmaster.h>

void setup()
{
  Serial.begin(9600);

  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);

  int dev = 0x00;
  unsigned int data_l = 0;
  unsigned int data_h = 0;
  int pec = 0;
  unsigned int data_t = 0;
  boolean check = true;



  Serial.println("Checking filter settings..");
  i2c_start_wait(dev + I2C_WRITE);
  i2c_write(0x25);
  i2c_rep_start(dev + I2C_READ);
  data_l = i2c_readAck();
  data_h = i2c_readAck();
  pec = i2c_readNak();
  i2c_stop();
  data_t = (((data_h) << 8) + data_l);
  Serial.print(data_t, BIN);


}
void loop() {
}



