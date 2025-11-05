#include "grove_two_rgb_led_matrix.h"

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif


#define DISPLAY_COLOR    0X11


#include "grove_two_rgb_led_matrix.h"

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

uint64_t example[] = {

  0xeeebcfbc86472c00,
  0xebcfbc86472c002e,
  0xcfbc86472c002e47,
  0xbc86472c002e4786,
  0x86472c002e4786bc,
  0x472c002e4786bcd5,
  0x2c002e4786bcd5eb,
  0x002e4786bcd5eb01,
 
  0xebcfbc86472c002e,
  0xcfbc86472c002e47,
  0xbc86472c002e4786,
  0x86472c002e4786bc,
  0x472c002e4786bcd5,
  0x2c002e4786bcd5eb,
  0x002e4786bcd5eb01,
  0x2e4786bcd5eb010d,
 
  0xcfbc86472c002e47,
  0xbc86472c002e4786,
  0x86472c002e4786bc,
  0x472c002e4786bcd5,
  0x2c002e4786bcd5fa,
  0x002e4786bcd5fa01,
  0x2e4786bcd5fa0111,
  0x4786bcd5fa011117,
 
  0xbc86472c002e4786,
  0x86472c002e4786bc,
  0x472c002e4786bcd5,
  0x2c022e4786bcd5fa,
  0x002e4786bcd5fa01,
  0x2e4786bcd5fa0111,
  0x4786bcd5fa011117,
  0x86bcd5fa01111722,
 
  0x86472c002e4786bc,
  0x472c002e4786bcd5,
  0x2c002e4786bcd5fa,
  0x002e4786bcd5fa01,
  0x2e4786bcd5fa0111,
  0x4786bcd5fa011117,
  0x86bcd5fa01111722,
  0xbcd5fa0111172224,
 
  0x472c002e4786bcd5,
  0x2c002e4786bcd5fa,
  0x002e4786bcd5fa01,
  0x2e4786bcd5fa0111,
  0x4786bcd5fa011117,
  0x86bcd5fa01111722,
  0xbcd5fa0111172224,
  0xd5fa011117222428
};

void waitForMatrixReady()
{
    delay(1000);
}

GroveTwoRGBLedMatrixClass matrix;
void setup()
{
    Wire.begin();
    SERIAL.begin(115200);
    waitForMatrixReady();
    uint16_t VID = 0;
    VID = matrix.getDeviceVID();
    if(VID != 0x2886)
    {
        SERIAL.println("Can not detect led matrix!!!");
        while(1);
    }
    SERIAL.println("Matrix init success!!!");
   
}

void loop()
{
 for (int i=0;i<6;i++) {
        matrix.displayFrames(example+i*8, 600, false, 1);
        delay(600);
    }
}
