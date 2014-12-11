#include <ExtPins.h>
#include <Wire.h>

Ext ext;
void setup(){
  ext.PinMode(EXT_PIN_0,OUTPUT_PP);  
}

void loop(){
  ext.loop();
  
  ext.DigitalWrite(EXT_PIN_0, 1);
  
  bool pin0;
  pin0 = ext.DigitalRead(EXT_PIN_0);
  
  ext.AnalogWrite(EXT_PIN_0, 250);
  
  uint8_t val0;
  val0 = ext.AnalogRead(EXT_PIN_0);
    
  ext.PowerOutMode(EXT_POWER_OUT_0, PWM);
  
  ext.PowerOutWrite(EXT_POWER_OUT_0, 200);
    
}


