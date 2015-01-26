#include <ExtPins.h>
#include <Wire.h>
#include <interboards_comm.h>
#include <MsTimer2.h>


void setup()
{
		Serial.begin(115200);
		Serial.println("Start Test scatch.");

		ext.PinMode(EXT_PIN_1, ANALOG_IN);
		ext.PinMode(EXT_PIN_2, ANALOG_IN);
		ext.PinMode(EXT_PIN_3, ANALOG_IN);
		ext.PinMode(EXT_PIN_6, OUTPUT_PP);
		ext.DigitalWrite(EXT_PIN_6, 1);

		ext.PowerOutMode(EXT_POWER_OUT_0, LOGICAL);
		ext.PowerOutWrite(EXT_POWER_OUT_0, 1);

		MsTimer2::set(STD_UPDATE_PERIOD, timer_ev);
		MsTimer2::start();
}

void timer_ev()
{
		interrupts();
		ext.process();
}


void loop()
{
		uint8_t Ch1, Ch2, Ch3;
		static uint8_t isClosed = 1;
		Ch1 = ext.AnalogRead(EXT_PIN_1);
		Ch2 = ext.AnalogRead(EXT_PIN_2);
		Ch3 = ext.AnalogRead(EXT_PIN_3);
		Serial.print(" Ch1 = ");
		Serial.print(Ch1, DEC);
		Serial.print(" Ch2 = ");
		Serial.print(Ch2, DEC);
		Serial.print(" Ch3 = ");
		Serial.println(Ch3, DEC);
		if (isClosed) {
				if (Ch1 > 100 && Ch1 < 120) {
						if (Ch2 > 120 && Ch2 < 140) {
								if (Ch3 > 50 && Ch3 < 70) {
										ext.PowerOutWrite(EXT_POWER_OUT_0, 0);
										isClosed = 0;
								}
						}
				}
		}
		delay(1000);
}


