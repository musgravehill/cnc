void INTERFACE_init() {
  pinMode(LED_pin, OUTPUT);
}

void BATT_check() {
  analogReference(INTERNAL); // 1.1 V
  //When switching from the DEFAULT to internal 3.3V reference you have to throwaway about 50 readings (5.5 ms) before the reference voltage stabilizes.
  analogRead(BATT_analog_pin);
  delay(50);
  analogRead(BATT_analog_pin);

  //  points 230.79078809523809523809523809524 to 1V
 
  // 3.3v  = 
  // 4.20 = 981              

  BATT_voltage_points =  analogRead(BATT_analog_pin);  

  BATT_logo_energy_w = map(constrain(BATT_voltage_points, 805, 969), 805, 969, 0, BATT_logo_box_w);

  analogReference(DEFAULT); // VCC 
   
}









