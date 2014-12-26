#include <MsTimer2.h>
#include <ExtPins.h>
#include <interboards_comm.h>
#include <Wire.h>

Ext ext;
uint8_t cnt_ext_process, ext_process_flag;

void setup(){
  uint8_t i;
  Serial.begin(115200);
  Serial.println("start Arduino");
  
  for(i = 0; i < 20; i++){
    ext.PinMode((EXT_PIN_NAME_T)i, INPUT_PU);
  }
  ext.PinMode(EXT_PIN_20, OUTPUT_PP);
  ext.DigitalWrite(EXT_PIN_20, 1);
  Serial.println("External pins config completed");

  for(i = 0; i < 4; i++){
    ext.PowerOutMode((EXT_POWER_OUT_NAME_T)i, LOGICAL);
    ext.PowerOutWrite((EXT_POWER_OUT_NAME_T)i, 1);
  }
  Serial.println("External power outs config completed");
  MsTimer2::set(1, timer_ev);
  MsTimer2::start();
}

void timer_ev(){
  if(++cnt_ext_process>10){    
    cnt_ext_process = 0;
    /*
    ext_process_flag = 1;
    */
    interrupts();
    ext.process();
  }
}


void loop(){
  /*
  if(ext_process_flag){
    ext_process_flag = 0;
    ext.process();
  }
  */
  Serial.println("Good.");
  delay(500);

}

