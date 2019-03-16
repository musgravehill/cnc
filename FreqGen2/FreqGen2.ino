#define _BV(bit) (1<< (bit))

#define EN_pin 11
#define DIR_pin 12

uint32_t i = 0;

void setup() {
  pinMode(13, OUTPUT);//step  
}

void loop() {  
    PORTB |= _BV(PB5); // HIGH
    delayMicroseconds(3);
    //delay(30);
    PORTB &= ~ _BV(PB5); // LOW
    delayMicroseconds(3);
    //delay(30);
}
