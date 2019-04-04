void INTERFACE_init() {
  pinMode(LED_pin, OUTPUT);
}

void BATT_check() {
  analogReference(INTERNAL); // 1.1 V
  //When switching from the DEFAULT to internal 3.3V reference you have to throwaway about 50 readings (5.5 ms) before the reference voltage stabilizes.
  analogRead(BATT_analog_pin);
  delay(50);
  analogRead(BATT_analog_pin);

  // 4.02v = 981 points =>  233.5714 points on 1 v    
 
  // 3.3v  = 
  // 4.20 = 981              

  BATT_voltage_points =  analogRead(BATT_analog_pin); //0.5 * BATT_voltage_points + 0.5 *

  BATT_logo_energy_w = map(constrain(BATT_voltage_points, 805, 981), 805, 1023, 0, BATT_logo_box_w);

  analogReference(DEFAULT); // VCC 
   
}









