/*
Beispiele für möglichkeiten mit der LED Matrix
1. Statisches Smiley
2. Zahlen
3. Text
4. Bild-Animation (Motion Picture)
5. Geometrische Farb-Animation
*/

#include "grove_two_rgb_led_matrix.h"

#ifdef SEEED_XIAO_M0
    #define SERIAL Serial
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif



#define DISPLAY_COLOR    0X11


GroveTwoRGBLedMatrixClass matrix;
void setup() {
    Wire.begin();
    SERIAL.begin(115200);
    delay(1000);
    matrix.scanGroveTwoRGBLedMatrixI2CAddress();
    uint16_t VID = 0;
    VID = matrix.getDeviceVID();
    if (VID != 0x2886) {
        SERIAL.println("Can not detect led matrix!!!");
        while (1);
    }
    SERIAL.println("Matrix init success!!!");

}

//Smiley
uint64_t pic[1] = {0x2345654354345676};

uint8_t pic8[] = {
  0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75,
  0x75, 0x75, 0x95, 0x75, 0x75, 0x95, 0x75, 0x75,
  0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75,
  0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75,
  0x75, 0x95, 0x75, 0x75, 0x75, 0x75, 0x95, 0x75, 
  0x75, 0x75, 0x95, 0x75, 0x75, 0x95, 0x75, 0x75, 
  0x75, 0x75, 0x75, 0x95, 0x95, 0x75, 0x75, 0x75,
  0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75, 0x75,
};

void loop() {
    //Statrisches Bild (Smiley von oben)
    matrix.displayFrames(pic8, 4000, true, 1);
    delay(5000);
    //Zahlen
    matrix.displayNumber(12345, 4000, false, DISPLAY_COLOR);
    delay(5000);
    //Text
    matrix.displayString("Jetzt kommen Animationen:", 8000, true, DISPLAY_COLOR);
    delay(8000);
    //Motion picture
    matrix.displayColorAnimation(4, 4000, true);
    delay(5000);
    //Geometrische Farb-Animation
    matrix.displayColorAnimation(2, 4000, true);
    delay(5000);
}