
#define _BV(bit) (1 << (bit))

void setup() {
  pinMode(13, OUTPUT);
}


void loop() {
  PORTB |= _BV(PB5); // HIGH
  delayMicroseconds(10);
  PORTB &= ~ _BV(PB5); // LOW  
  delayMicroseconds(10);
}
