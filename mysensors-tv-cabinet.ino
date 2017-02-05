// https://www.domoticz.com/forum/viewtopic.php?t=8039#p57521

#define MY_RADIO_NRF24
#define SKETCH_NAME "Tv Cabinet"
#define SKETCH_VERSION "1.0"

#define CHILD_ID 0
//#define CHILD_ID_LED_COLOR 0
//#define CHILD_ID_LED_MODE 1
//#define CHILD_ID_LED_LEVEL 2

#include <FastLED.h>
#include <MySensors.h>
// S_LIGHT_LEVEL with V_LIGHT_LEVEL (uncalibrated percentage)
// S_RGB_LIGHT with V_RGB, V_WATT "ff0000" 
//MyMessage levelMsg(NODE_ID, V_TRIPPED);

const int stripPin = 7 ;                  // pin where 2812 LED strip is connected
const int numPixel = 12 ;                  // set to number of pixels

CRGB leds[numPixel];
char actRGBvalue[] = "000000";               // Current RGB value
uint16_t actRGBbrightness = 0xFF ;         // Controller Brightness 
int actRGBonoff=0;                        // OnOff flag

MyMessage lastColorStatusMsg(CHILD_ID,V_VAR1);


void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, stripPin>(leds, numPixel); // initialize led strip

  // Flash the "hello" color sequence: R, G, B, black. 
  colorBars();
}

void presenstation() {
  sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void receive(MyMessage &message) {
  if (message.type == V_RGB) {            // check for RGB type
    actRGBonoff=1;
    strcpy(actRGBvalue, message.getString());    // get the payload
    sendColor2AllLEDs(strtol(actRGBvalue, NULL, 16));
    sendLastColorStatus();
  }
  else if (message.type == V_DIMMER) {           // if DIMMER type, adjust brightness
    actRGBonoff=1;
    actRGBbrightness = map(message.getLong(), 0, 100, 0, 255);
    FastLED.setBrightness( actRGBbrightness );
    sendLastColorStatus();
  }
  else if (message.type == V_STATUS) {           // if on/off type, toggle brightness
    actRGBonoff = message.getInt();
    FastLED.setBrightness((actRGBonoff == 1)?actRGBbrightness:0);
    sendLastColorStatus();
  }
  else if (message.type==V_VAR1) {            // color status
    String szMessage=message.getString();
    strcpy(actRGBvalue, getValue(szMessage,'&',0).c_str());
    actRGBbrightness=atoi(getValue(szMessage,'&',1).c_str());
    actRGBonoff=atoi(getValue(szMessage,'&',2).c_str());
    sendColor2AllLEDs(strtol(actRGBvalue, NULL, 16));
    FastLED.setBrightness((actRGBonoff == 1)?actRGBbrightness:0);
  }
}

void colorBars()
{
  //sendColor2AllLEDs( CRGB::Red );   FastLED.show(); delay(1000);
  //sendColor2AllLEDs( CRGB::Green ); FastLED.show(); delay(1000);
  //sendColor2AllLEDs( CRGB::Blue );  FastLED.show(); delay(1000);
  FastLED.setTemperature(Candle);
  FastLED.setBrightness(0xA0); 
  sendColor2AllLEDs( CRGB::White ); FastLED.show(); delay(1000);
} 

void sendLastColorStatus() {
  String cStatus=actRGBvalue+String("&")+String(actRGBbrightness)+String("&")+String(actRGBonoff);
  send(lastColorStatusMsg.set(cStatus.c_str()));
}

String getValue(String data, char separator, int index) {
 int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void sendColor2AllLEDs(const CRGB lcolor)
{
  for(int i = 0 ; i < numPixel ; i++) {
    leds[i] = lcolor;
  }
}

