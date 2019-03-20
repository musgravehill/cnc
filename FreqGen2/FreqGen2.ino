#define _BV(bit) (1<< (bit))

#define EN_pin 11
#define DIR_pin 12

uint32_t i = 0;

void setup() {
  pinMode(10, OUTPUT);//step  
}

void loop() {  
    PORTB |= _BV(PB2); // HIGH
    delayMicroseconds(10);
    //delay(30);
    PORTB &= ~ _BV(PB2); // LOW
    delayMicroseconds(10);
    //delay(30);
}
