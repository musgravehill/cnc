void OLED_init() {
  //u8g.setFont(u8g_font_unifont_0_8); //u8g_font_9x15r
  //u8g.setFontPosTop();
}

void OLED_render() {
  OLED_mainScreen();
}


void OLED_setOn() {  
  u8g.sleepOff();
}
/*
  А  \xb0
  Б   \xb1
  В \xb2
  Г \xb3
  Д \xb4
  Е \xb5
  Ж \xb6
  З \xb7
  И \xb8
  Й \xb9
  К \xba
  Л \xbb
  М \xbc
  Н \xbd
  О \xbe
  П \xbf
  Р \xc0
  С \xc1
  Т \xc2
  У \xc3
  Ф \xc4
  Х \xc5
  Ц \xc6
  Ч \xc7
  Ш \xc8
  Щ \xc9
  Ъ \xca
  Ы \xcb
  Ь \xcc
  Э \xcd
  Ю \xce
  Я \xcf

*/




void OLED_mainScreen() {
  u8g.firstPage();
  do {

    //u8g.setFont(u8g_font_unifont_86); //u8g_font_9x15r
    u8g.setFont(u8g_font_unifont_0_8); //u8g_font_9x15r
    u8g.setFontPosTop();

    //base battery
    u8g.drawFrame(0, 0, BATT_logo_box_w, 12);
    u8g.drawBox(0, 0, BATT_logo_energy_w, 12);
    u8g.drawStr(80, 0, F("V"));    
    u8g.setPrintPos(40, 0);
    u8g.print(BATT_voltage_points/244.0, 2);

    u8g.setFont(u8g_font_fub35n); //u8g_font_9x15r u8g_font_helvB24
    u8g.setFontPosTop();
    //u8g.drawStr(100, 32, F("C"));
    u8g.setPrintPos(0, 20);
    u8g.print(round(ThermoSensor_temperature));


  } while ( u8g.nextPage() );
}




