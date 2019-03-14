#define _BV(bit) (1<< (bit))

#define EN_pin 11
#define DIR_pin 12

uint32_t i = 0;

void setup() {
  pinMode(13, OUTPUT);//step
  pinMode(DIR_pin, OUTPUT);//dir
  pinMode(EN_pin, OUTPUT);//^en

  
}

void loop() {
  //EN
  digitalWrite(EN_pin, 0); //ENable
  delay(500);
  //DIR
  digitalWrite(DIR_pin, 1);
  delay(100);
  //STEP
  
  for (i = 0; i < 12800; i++) {
    PORTB |= _BV(PB5); // HIGH
    delayMicroseconds(10);
    //delay(30);
    PORTB &= ~ _BV(PB5); // LOW
    delayMicroseconds(10);
    //delay(30);
  }
  delay(500);
  digitalWrite(EN_pin, 1); //NOT ENable
  delay(1000);

  //EN
  digitalWrite(EN_pin, 0); //ENable
  delay(500);
  //DIR
  digitalWrite(DIR_pin, 0);
  delay(100);
  //STEP
  
  for (i = 0; i < 12800; i++) {
    PORTB |= _BV(PB5); // HIGH
    delayMicroseconds(10);
    //delay(30);
    PORTB &= ~ _BV(PB5); // LOW
    delayMicroseconds(10);
    //delay(30);
  }
  delay(500);
  digitalWrite(EN_pin, 1); //NOT ENable
  delay(1000);



}
