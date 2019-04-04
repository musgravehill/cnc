// 0. upload OPTIBOOT for normal watchdog
// Narrow beam  MLX90614ESF-DCI

#define DBG 1

#include <stdlib.h>
#include <avr/wdt.h> //wathchdog
#include <stdint.h>
#include <Wire.h> //oled and mlx ir-sensor
#include <Adafruit_MLX90614.h>

#define LED_pin A0

#define BATT_analog_pin A6
#define BATT_logo_box_w 24
uint8_t BATT_logo_energy_w = 0;
uint16_t BATT_voltage_points = 0;

//===================== MLX IR SENSOR=============================================================================
Adafruit_MLX90614 ThermoSensor = Adafruit_MLX90614();  


//================================== TIMEMACHINE =================================================================
uint32_t TM_next_15311ms = 0UL; // если чаще, чем 3600, то ключи обновляются, а потом только приходит телеметрия от прошлых ключей. Не валидируется и слом.
uint32_t TM_next_911ms = 0UL;
uint32_t TM_next_301ms = 0UL; 

//======================================= OLED ======================================================================
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);  // I2C / TWI
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC

void setup() {
  wdt_reset();
  wdt_disable();
  delay(100);
#ifdef DBG //#endif
  Serial.begin(9600);
#endif
  Wire.begin();
  delay(50);
  OLED_init();  
  INTERFACE_init();
  ThermoSensor.begin();  
  delay(50);
}

void loop() {
  wdt_enable (WDTO_8S); //try to have time < 8s, else autoreset by watchdog
  TM_loop();
  wdt_reset();
  wdt_disable();
}




