void ThermoSensor_read() {
  ThermoSensor_temperature = round(10.0 * ThermoSensor.readObjectTempC()) / 10.0;
}
