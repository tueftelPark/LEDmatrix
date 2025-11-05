#include "grove_two_rgb_led_matrix.h"

#ifdef SEEED_XIAO_M0
    #define SERIAL Serial
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

uint64_t example[] = {

  0x0000000000000000,
  0x0113136868131301,
  0x0113682768681301,
  0x0168686868686801,
  0x6868686868682768,
  0x011313b7b7131301,
  0x011313b7b7131301,
  0x010101b7b7010101,
 
  0x0000000000000000,
  0x011313682b131301,
  0x0113685d68681301,
  0x0168682b68682b01,
  0x682b686868685d68,
  0x011313b7b7131301,
  0x011313b7b7131301,
  0x010101b7b7010101,
 
  0x0000000000000000,
  0x0113132561131301,
  0x0113256168251301,
  0x0168686168686101,
  0x2561256825686125,
  0x011313b7b7131301,
  0x011313b7b7131301,
  0x010101b7b7010101
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
 for (int i=0;i<3;i++) {
        matrix.displayFrames(example+i*8, 1000, false, 1);
        delay(800);
    }
}
