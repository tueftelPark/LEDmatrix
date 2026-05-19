// ===========================================================
//  DEFINITION Platzhalter die vor Kompilation im Code ersetzt werden (Alle mit #define)
// ===========================================================


#include "grove_two_rgb_led_matrix.h"
#include <avr/pgmspace.h> // Für PROGMEM (große Arrays im Flash speichern)

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

#define IDLE 0
#define BOTH 1
#define LEFT 2
#define RIGHT 3

#define SIZE_DEAD 0
#define SIZE_SMALL 1
#define SIZE_MEDIUM 2
#define SIZE_LARGE 3


// ===========================================================
//  DEFINITION Variablen die später im Code verwendet werden
// ===========================================================

GroveTwoRGBLedMatrixClass matrix;

int currentSize = SIZE_SMALL; // Das Tamagochi kann 4 Grössen haben (SIZE_DEAD, SIZE_SMALL, SIZE_MEDIUM und SIZE_LARGE)... Hier wird definiert mit welcher grösse die Animation anfängt
int cyclesToShrink = 10; // Hier wird gesteuert wie viele Cycles (Durchläufe der jeweiligen Animation der entsprechenden Grösse) es braucht, bis das Tamagochi eine Grösse kleiner wird.
bool lastActionWasGrow = false; // Hilfsvariable, damit beim gedrückt halten des linken Buttons nicht ausversehen zweimal nacheinander gewachsen wird.

const int buttonPinOne = 2;  // Die Pin-Nummer des linken Buttons
const int buttonPinTwo = 4;  // Die Pin-Nummer des rechten Buttons

int buttonStateOne = LOW; // Hier werden die Buttons initialisert
int buttonStateTwo = LOW; // Hier werden die Buttons initialisert

int potentiometer = A0; // Die Pin-Nummer des Potentiometers
int displayTimePerFrame = 300; // Standard-Zeit wie lange ein Frame gezeigt wird, wenn kein Potentiometer angeschlossen ist.


// ===========================================================
//  DEFINITION Arrays mit den verschiedenen Animationen für idle (untätig) in 3 verschiedenen Grössen / Evolutionsstufen, wachsen, schrumpfen, essen (für längere Lebenszeit) und sterben. 
//  Zusätzlich könnte eine weitere Animation für beide Buttons gleichzeitig implementiert werden 
// ===========================================================

const int idleAnimationSmallFrameCount = 8;
const uint64_t idleAnimationSmall[] PROGMEM = {

  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babffffffffff,
  0xabababababffffff,
  0xffffababababffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2bababffffffff,
  0xabababababffffff,
  0xffffababababffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babababffffff,
  0xabababababffffff,
  0xffffababababffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babababffffff,
  0xababababababffff,
  0xffffababababffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babababffffff,
  0xababababababffff,
  0xffffabababababff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babababffffff,
  0xababababababffff,
  0xffffababababffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babababffffff,
  0xabababababffffff,
  0xffffababababffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2bababffffffff,
  0xabababababffffff,
  0xffffababababffff
};

const int idleAnimationMediumFrameCount = 5;
const uint64_t idleAnimationMedium[] PROGMEM = {

  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffababffff,
  0xababffababababff,
  0xab2babababababab,
  0xabababababababab,
  0xffffabffffffabff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffababffff,
  0xababffababd0abff,
  0xab2bababd0ababab,
  0xabababababababab,
  0xffffabffffffabff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffababffff,
  0xababffabd0ababff,
  0xab2babababd0abab,
  0xabababababababab,
  0xffffabffffffabff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffababffff,
  0xababffabd0ababff,
  0xabababababd0abab,
  0xabababababababab,
  0xffffabffffffabff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffababffff,
  0xababffabd0ababff,
  0xab2cabababd0abab,
  0xabababababababab,
  0xffffabffffffabff
};

const int idleAnimationLargeFrameCount = 10;
const uint64_t idleAnimationLarge[] PROGMEM = {

  0xadadadffffffffff,
  0xad2badadffffffff,
  0xadadadadadffffff,
  0xffffadadababffff,
  0xffadadadabababff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadadffffff,
  0xffffadadababffff,
  0xffadadadabababff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadfdffffff,
  0xffffadadababffff,
  0xffadadadabababff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadfdffffff,
  0xffffadadabfdffff,
  0xffadadadabababff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadfdffffff,
  0xffffadadabfdffff,
  0xffadadadababfdff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadfdffffff,
  0xffffadadabfdffff,
  0xffadadadababfdff,
  0xffffffadabababfd,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadfdffffff,
  0xffffadadabfdffff,
  0xffadadadababfdff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadfdffffff,
  0xffffadadabfdffff,
  0xffadadadabababff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadfdffffff,
  0xffffadadababffff,
  0xffadadadabababff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff,
 
  0xadadadffffffffff,
  0xad2badfdffffffff,
  0xadadadadabffffff,
  0xffffadadababffff,
  0xffadadadabababff,
  0xffffffadabababab,
  0xffffffadadababab,
  0xffffffadffffabff
};

const int growAnimationFrameCount = 8;
const uint64_t growAnimation[] PROGMEM = {

  0xfffed5d5d5d5feff,
  0xfffffec5c5feffff,
  0xfffec5c5c5c5feff,
  0xfec5c5c5d5c5c5fe,
  0xfec5d5c5c5c5c5fe,
  0xfec5c5c5c5d5c5fe,
  0xfffec5c5c5c5feff,
  0xfffffefefefeffff,
 
  0xfffefffffffffeff,
  0xfffffed5d5feffff,
  0xfffec5c5c5c5feff,
  0xfec5c5c5d5c5c5fe,
  0xfec5d5c5c5c5c5fe,
  0xfec5c5c5c5d5c5fe,
  0xfffec5c5c5c5feff,
  0xfffffefefefeffff,
 
  0xfffefffffffffeff,
  0xfffffefffffeffff,
  0xfffed5d5d5d5feff,
  0xfec5c5c5d5c5c5fe,
  0xfec5d5c5c5c5c5fe,
  0xfec5c5c5c5d5c5fe,
  0xfffec5c5c5c5feff,
  0xfffffefefefeffff,
 
  0xfffefffffffffeff,
  0xfffffefffffeffff,
  0xfffefffffffffeff,
  0xfed5d5d5d5d5d5fe,
  0xfec5d5c5c5c5c5fe,
  0xfec5c5c5c5d5c5fe,
  0xfffec5c5c5c5feff,
  0xfffffefefefeffff,
 
  0xfffefffffffffeff,
  0xfffffefffffeffff,
  0xfffefffffffffeff,
  0xfefffffffffffffe,
  0xfed5d5d5d5d5d5fe,
  0xfec5c5c5c5d5c5fe,
  0xfffec5c5c5c5feff,
  0xfffffefefefeffff,
 
  0xfffefffffffffeff,
  0xfffffefffffeffff,
  0xfffefffffffffeff,
  0xfefffffffffffffe,
  0xfefffffffffffffe,
  0xfed5d5d5d5d5d5fe,
  0xfffec6c5c5c5feff,
  0xfffffefefefeffff,
 
  0xfffefffffffffeff,
  0xfffffefffffeffff,
  0xfffefffffffffeff,
  0xfefffffffffffffe,
  0xfefffffffffffffe,
  0xfefffffffffffffe,
  0xfffed5d5d5d5feff,
  0xfffffefefefeffff,
 
  0xfffefffffffffeff,
  0xfffffefffffeffff,
  0xfffefffffffffeff,
  0xfefffffffffffffe,
  0xfefffffffffffffe,
  0xfefffffffffffffe,
  0xfffefffffffffeff,
  0xfffffefefefeffff
};

const int shrinkAnimationFrameCount = 4;
const uint64_t shrinkAnimation[] PROGMEM = {

  0xfffffefefefeffff,
  0xfffefffffffffeff,
  0xfefffefffffffffe,
  0xfefffffefefffffe,
  0xfefffffefefffffe,
  0xfefffffffffffffe,
  0xfffefffffffffeff,
  0xfffffefefefeffff,
 
  0xfffffefefefeffff,
  0xfffefffffffffeff,
  0xfefffffffffffffe,
  0xfefffffefefffffe,
  0xfefffffefefffffe,
  0xfefffefffffffffe,
  0xfffefffffffffeff,
  0xfffffefefefeffff,
 
  0xfffffefefefeffff,
  0xfffefffffffffeff,
  0xfefffffffffffffe,
  0xfefffffefefffffe,
  0xfefffffefefffffe,
  0xfefffffffffefffe,
  0xfffefffffffffeff,
  0xfffffefefefeffff,
 
  0xfffffefefefeffff,
  0xfffefffffffffeff,
  0xfefffffffffefffe,
  0xfefffffefefffffe,
  0xfefffffefefffffe,
  0xfefffffffffffffe,
  0xfffefffffffffeff,
  0xfffffefefefeffff
};

const int dieAnimationFrameCount = 16;
const uint64_t dieAnimation[] PROGMEM = {
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babffffffffff,
  0xabababababffffff,
  0xffffababababffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babffffffffff,
  0xabababababffffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
  0xab2babffffffffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xabababffffffffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xfffffffefefeffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfeff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfeff,
  0xfffffefefffefefe,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfeff,
  0xfffffefefffefefe,
  0xfffefefefefefefe,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfeff,
  0xfffffefefffefefe,
  0xfffefefefefefefe,
  0xfefefefefefefeff,
 
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfeff,
  0xfffffefefffefefe,
  0xfffefefefefefefe,
  0xfefefefefefefeff,
  0xfefefefefefeffff,
 
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfefe,
  0xfffffefefffefefe,
  0xfffefefefefefefe,
  0xfefefefefefefeff,
  0xfefefefefefeffff,
 
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfeff,
  0xfffffefefffefefe,
  0xfffefefefefefefe,
  0xfefefefefefefeff,
  0xfefefefefefeffff,
 
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfefe,
  0xfffffefefffefefe,
  0xfffefefefefefefe,
  0xfefefefefefefeff,
  0xfefefefefefeffff,
 
  0xffffffffffffffff,
  0xfffffffefefeffff,
  0xfffffefefefefeff,
  0xfffffefdfefdfeff,
  0xfffffefefffefefe,
  0xfffefefefefefefe,
  0xfefefefefefefeff,
  0xfefefefefefeffff
};

const int eatAnimationFrameCount = 9;
const uint64_t eatAnimation[] PROGMEM = {

  0x1414141414141414,
  0x2424242424242424,
  0xff242801280124ff,
  0xff242828012824ff,
  0xffff24012824ffff,
  0xffff24280124ffff,
  0xffffff0124ffffff,
  0xffffff2424ffffff,
 
  0xffffffffffffffff,
  0x1414141414141414,
  0x2424242424242424,
  0xff242801280124ff,
  0xff242828012824ff,
  0xffff24012824ffff,
  0xffff24280124ffff,
  0xffffff0124ffffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x1414141414141414,
  0x2424242424242424,
  0xff242801280124ff,
  0xff242828012824ff,
  0xffff24012824ffff,
  0xffff24280124ffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x1414141414141414,
  0x2424242424242424,
  0xff242801280124ff,
  0xff242828012824ff,
  0xffff24012824ffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x1414141414141414,
  0x2424242424242424,
  0xff242801280124ff,
  0xff242828012824ff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x1414141414141414,
  0x2424242424242424,
  0xff242801280124ff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x1414141414141414,
  0x2424242424242424,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x1414141414141414,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff
};

const int bothButtonsAnimationFrameCount = 9;
const uint64_t bothButtonsAnimation[] PROGMEM = {

  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffff00ff00ff00ff,
  0xffffff000000ffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffffffffffffffff,
  0xffff00ff00ff00ff,
  0xffffff000000ffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffff00ff00ff00ff,
  0xffffff000000ffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffff00ff00ff00ff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffffff000000ffff,
  0xffff00ff00ff00ff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffffffff00ffffff,
  0xffffff000000ffff,
  0xffff00ff00ff00ff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffffffffffffffff,
  0xffffffff00ffffff,
  0xffffff000000ffff,
  0xffff00ff00ff00ff,
  0xffffffff00ffffff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff,
 
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffff00ffffff,
  0xffffff000000ffff,
  0xffff00ff00ff00ff,
  0xffffffff00ffffff,
  0xffffff00ff00ffff,
  0xffffff00ff00ffff
};





// ======================================================
//  SETUP – wird beim Start einmal ausgeführt
// ======================================================
void setup() {
  Wire.begin();
  SERIAL.begin(9600);
  delay(1000);

  uint16_t VID = matrix.getDeviceVID();
  if (VID != 0x2886) {
    SERIAL.println(F("LED-Matrix nicht erkannt!"));
    while (1);
  }
  SERIAL.println(F("Matrix erfolgreich verbunden!"));

  //Initialisiert die Buttons und das Potentiometer
  pinMode(buttonPinOne, INPUT);
  pinMode(buttonPinTwo, INPUT);
  pinMode(potentiometer, INPUT); //Sets the pinmode to input
}

// ======================================================
//  SETUP – wird beim Start einmal ausgeführt
// ======================================================

void loop() {
    int potentiometerValue = analogRead(potentiometer); 
    if(potentiometerValue > 0) {
      displayTimePerFrame = map(potentiometerValue, 0, 1023, 100, 1000); //Map the value from 0, 1023 to 100, 1000
    }
    handleLifeCycle();
}

void handleLifeCycle() {
  // read the state of the pushbutton value:
  buttonStateOne = digitalRead(buttonPinOne);
  buttonStateTwo = digitalRead(buttonPinTwo);

  int action = IDLE;

  // Herausfinden welcher Button gedrückt wurde und wenn ja welcher.
  if (buttonStateOne == HIGH && buttonStateTwo == HIGH) {
    action = BOTH;
  } else if (buttonStateOne == HIGH) {
    action = LEFT;
  } else if (buttonStateTwo == HIGH) {
    action = RIGHT;
  }

  switch (action) {
    case IDLE:
      switch (currentSize) {
        case SIZE_DEAD:
          die();
          break;
        case SIZE_SMALL:
          displayFramesViaBuffer(idleAnimationSmall, displayTimePerFrame, idleAnimationSmallFrameCount);
          break;
        case SIZE_MEDIUM:
          displayFramesViaBuffer(idleAnimationMedium, displayTimePerFrame, idleAnimationMediumFrameCount);
          break;
        case SIZE_LARGE:        
          displayFramesViaBuffer(idleAnimationLarge, displayTimePerFrame, idleAnimationLargeFrameCount);
          break;
      }
      shrink();
      lastActionWasGrow = false;
      break;
      
    case LEFT:
      grow();
      lastActionWasGrow = true;
      break;
    case RIGHT:
      eat();
      lastActionWasGrow = false;
      break;
    case BOTH:
      displayFramesViaBuffer(bothButtonsAnimation, displayTimePerFrame, bothButtonsAnimationFrameCount);
      lastActionWasGrow = false;
      break;
  }

  SERIAL.print("CURRENT SIZE: ");
  SERIAL.println(currentSize);

  SERIAL.print("Remaining Cycles until next shrinking: ");
  SERIAL.println(cyclesToShrink);
}

void grow() {
  if (!lastActionWasGrow && currentSize < 3) {
    SERIAL.println("GROWING");
    currentSize++;
    cyclesToShrink = 10;
    displayFramesViaBuffer(growAnimation, 100, growAnimationFrameCount);
  }
  else {
    SERIAL.println("Could not grow, already on largest size");
  }
}

void shrink() {
  if (cyclesToShrink < 1 && currentSize > 0) {
    currentSize--;
    cyclesToShrink = 10;
    if (currentSize != SIZE_DEAD) {
      SERIAL.println("SHRINKING");
      displayFramesViaBuffer(shrinkAnimation, 100, shrinkAnimationFrameCount);
    }
  }
  cyclesToShrink--;
}

void eat() {
  SERIAL.println("EATING");
  displayFramesViaBuffer(eatAnimation, displayTimePerFrame, eatAnimationFrameCount);
  cyclesToShrink+=10;
}

void die() {
  SERIAL.println("DYING");
  displayFramesViaBuffer(dieAnimation, 300, dieAnimationFrameCount);
  currentSize = SIZE_SMALL;
  cyclesToShrink = 10;
  delay(1000);
}

void displayFramesViaBuffer(uint64_t animation[], int displayTimePerFrame, int frameCount) {
  // --- Animation abspielen ---
  uint64_t buffer[8];

  for (int frame = 0; frame < frameCount; frame++) {
    for (int row = 0; row < 8; row++) {
      buffer[row] = read_qword_from_progmem(&animation[frame * 8 + row]);
    }

    matrix.displayFrames(buffer, displayTimePerFrame, false, 1); // Zahl nach buffer verändern
    delay(displayTimePerFrame);
  }
}

// ===========================================================
//  HILFSFUNKTION: Warten bis Matrix bereit ist.
// ===========================================================
void waitForMatrixReady() {
    delay(1000);
}

// ===========================================================
//  HILFSFUNKTION: Arbeitsspeicher sparen
// ===========================================================
uint64_t read_qword_from_progmem(const uint64_t *ptr) {
  uint64_t value = 0;
  for (uint8_t i = 0; i < 8; i++) {
    value |= ((uint64_t)pgm_read_byte((const uint8_t*)ptr + i)) << (8 * i);
  }
  return value;
}