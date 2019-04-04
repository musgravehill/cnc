void TM_loop() {
  uint32_t  TM_currMillis = millis();

  if (TM_currMillis > TM_next_301ms) {
    TM_301ms();
    TM_next_301ms = TM_currMillis + 301L;
  }
  if (TM_currMillis > TM_next_911ms ) {
    TM_911ms();
    TM_next_911ms = TM_currMillis + 911L;
  }
  if (TM_currMillis > TM_next_15311ms ) {
    TM_15311ms();
    TM_next_15311ms = TM_currMillis + 15311L;
  }
}

void TM_911ms() {
  Serial.print("Ambient = "); Serial.print(ThermoSensor.readAmbientTempC());
  Serial.print("*C\tObject = "); Serial.print(ThermoSensor.readObjectTempC()); Serial.println("*C");
}

void TM_301ms() {
  OLED_render();
}

void TM_15311ms() {
  BATT_check();
}

