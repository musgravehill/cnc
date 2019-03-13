#define _BV(bit) (1<< (bit))

void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  PORTB |= _BV(PB5); // HIGH
  delayMicroseconds(8);
  //delay(30);
  PORTB &= ~ _BV(PB5); // LOW
  delayMicroseconds(8);
  //delay(30);
}
