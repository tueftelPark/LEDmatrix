/* red = 0x00,
    orange = 0x12,
    yellow = 0x18,
    green = 0x52,
    cyan = 0x7f,
    blue = 0xaa,
    purple = 0xc3,
    pink = 0xdc,
    white = 0xfe,
    black = 0xff,
*/

#include "grove_two_rgb_led_matrix.h"
#include <avr/pgmspace.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

#define DISPLAY_COLOR 0x7f

GroveTwoRGBLedMatrixClass matrix;

// ---------------- Button-Pins ----------------
const int BUTTON_P1 = 2;  // Spieler 1
const int BUTTON_P2 = 4;  // Spieler 2

// ---------------- Animationsdaten im Flash ----------------
const uint64_t example[] PROGMEM = {

  0xffff1d000000ffff,
  0xff0000000000ffff,
  0xff80d38010d310ff,
  0xffd31010d3d310ff,
  0xffffd3d3d312ffff,
  0xffff29982c0101ff,
  0xff1898989807ff18,
  0xffff12ffff12ffff,
 
  0xff1d000000ffffff,
  0x0000000000ffffff,
  0x80d38010d310ffff,
  0xd31010d3d310ffff,
  0xffd3d3d312ffffff,
  0xff29982c0101ffff,
  0x1898989807ff18ff,
  0xffff1affff1affff,
 
  0x1d000000ffffffff,
  0x00000000ffffffff,
  0xd38010d310ffffff,
  0x1010d3d310ffffff,
  0xd3d3d312ffffffff,
  0x29982c0101ffffff,
  0x98989807ff18ffff,
  0x12ffff12ffffffff,
 
  0x000000ffffffffff,
  0x000000ffffffffff,
  0x8010d310ffffffff,
  0x10d3d310ffffffff,
  0xd3d312ffffffffff,
  0x982c0101ffffffff,
  0x989807ff18ffffff,
  0x1affff1affffffff,
 
  0x0000ffffffffffff,
  0x0000ffffffffffff,
  0x10d310ffffffffff,
  0xd3d310ffffffffff,
  0xd312ffffffffffff,
  0x2c0101ffffffffff,
  0x9807ff18ffffffff,
  0xff12ffffffffffff,
 
  0x2c2c2c2c2c2c2c2c,
  0x2c2c2c2c2c2c2c2c,
  0x2c2c2c2c2c2c2c2c,
  0x2c2c2c2c2c2c2c2c,
  0x2c2c2c2c2c2c2c2c,
  0x2c2c2c2c2c2c2c2c,
  0x2c2c2c2c2c2c2c2c,
  0x2c2c2c2c2c2c2c2c,
 
  0x4848484848484848,
  0x4848484848484848,
  0x4848484848484848,
  0x4848484848484848,
  0x4848484848484848,
  0x4848484848484848,
  0x4848484848484848,
  0x4848484848484848,
 
  0xffffffffffffff27,
  0xffffffffffff8227,
  0xffffffffffffff27,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x55ffffffffffffff,
  0x55ffffffffffffff,
  0x55ffffffffffffff,
 
  0xffffffffffffff27,
  0xffffffffff82ff27,
  0xffffffffffffff27,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x55ffffffffffffff,
  0x55ffffffffffffff,
  0x55ffffffffffffff,
 
  0xffffffffffffff27,
  0xffffffff82ffff27,
  0xffffffffffffff27,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0x55ffffffffffffff,
  0x55ffffffffffffff,
  0x55ffffffffffffff,
 
  0xffffffffffffff27,
  0xffffffff82ffff27,
  0xffffffffffffff27,
  0xffffffffffffffff,
  0x52ffffffffffffff,
  0x55ffffffffffffff,
  0x55ffffffffffffff,
  0xffffffffffffffff,
 
  0xffffffffffffff27,
  0xffffff7affffff27,
  0xffffffffffffff27,
  0x52ffffffffffffff,
  0x52ffffffffffffff,
  0x55ffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0xffffffffffffff27,
  0xffff7affffffff27,
  0x4fffffffffffff27,
  0x52ffffffffffffff,
  0x52ffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0x50ffffffffffff27,
  0x507fffffffffff27,
  0x4fffffffffffff27,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0x50ffffffffffffff,
  0x50ffffffffffff27,
  0x4fff7fffffffff27,
  0xffffffffffffff2b,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0x50ffffffffffffff,
  0x50ffffffffffffff,
  0x4fffffffffffff27,
  0xffffff7fffffff2b,
  0xffffffffffffff2c,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0x50ffffffffffffff,
  0x50ffffffffffffff,
  0x4fffffffffffff27,
  0xffffffffffffff2b,
  0xffffffff7bffff2c,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0x50ffffffffffffff,
  0x50ffffffffffffff,
  0x4fffffffffffff27,
  0xffffffffffffff2b,
  0xffffffffffffff2c,
  0xffffffffff7bffff,
  0xffffffffffffffff,
  0xffffffffffffffff,
 
  0x50ffffffffffffff,
  0x50ffffffffffffff,
  0x4fffffffffffff27,
  0xffffffffffffff2b,
  0xffffffffffffff2c,
  0xffffffffffffffff,
  0xffffffffffff7bff,
  0xffffffffffffffff,
 
  0x50ffffffffffffff,
  0x50ffffffffffffff,
  0x4fffffffffffff27,
  0xffffffffffffff2b,
  0xffffffffffffff2c,
  0xffffffffffffffff,
  0xffffffffffffffff,
  0xffffffffffffff7b,
 
  0x0202020202020202,
  0x0202020202020202,
  0x0202020202020202,
  0x0202020202020202,
  0x0202020202020202,
  0x0202020202020202,
  0x0202020202020202,
  0x0202020202020202,
 
  0x4d4d4d4d4d4d4d4d,
  0x4d4d4d4d4d4d4d4d,
  0x4d4d4d4d4d4d4d4d,
  0x4d4d4d4d4d4d4d4d,
  0x4d4d4d4d4d4d4d4d,
  0x4d4d4d4d4d4d4d4d,
  0x4d4d4d4d4d4d4d4d,
  0x4d4d4d4d4d4d4d4d
};

const int FRAME_COUNT = 22;

// ---- 64-Bit aus PROGMEM lesen ----
uint64_t read_qword_from_progmem(const uint64_t *ptr) {
  uint64_t value = 0;
  for (uint8_t i = 0; i < 8; i++) {
    value |= ((uint64_t)pgm_read_byte((const uint8_t*)ptr + i)) << (8 * i);
  }
  return value;
}

void waitForMatrixReady() { delay(1000); }

// Idle-Animation: einen Frame anzeigen
void showAnimationFrame(int frameIndex) {
  uint64_t buffer[8];
  for (int row = 0; row < 8; row++) {
    buffer[row] = read_qword_from_progmem(&example[frameIndex * 8 + row]);
  }
  matrix.displayFrames(buffer, 0, true, 1);
}

// Matrix „löschen“ (alles weiss)
void clearMatrix() {
  uint64_t clearFrame[8];
  for (int i = 0; i < 8; i++) {
    clearFrame[i] = 0xffffffffffffffff;
  }
  matrix.displayFrames(clearFrame, 0, true, 1);
}

// kleine Smiley-Animation mit „Augen hin und her“ (als Text)
void showSmileyAnimation() {
  // 3x Augen links/rechts wechseln
  for (int i = 0; i < 3; i++) {
    matrix.displayString(":)", 1000, true, 0x18);
    delay(2000);
    
  }
}

// Hilfsfunktion: ein kompletter Reaktions-Spiel-Loop
void runReactionGame() {
  while (true) {
    // -------- READY --------
    clearMatrix();
    matrix.displayString("READY", 2500, true, DISPLAY_COLOR);
    delay(1000);

    // -------- Zufallswartezeit mit FRUEHSTART-Erkennung --------
    unsigned long waitTime  = random(1000, 4000);  // 1–4 s
    unsigned long startWait = millis();

    bool lastP1 = digitalRead(BUTTON_P1);
    bool lastP2 = digitalRead(BUTTON_P2);
    bool falseStart = false;
    int falsePlayer = 0;

    while (millis() - startWait < waitTime && !falseStart) {
      bool p1 = digitalRead(BUTTON_P1);
      bool p2 = digitalRead(BUTTON_P2);

      bool p1Pressed = (lastP1 == HIGH && p1 == LOW);
      bool p2Pressed = (lastP2 == HIGH && p2 == LOW);

      lastP1 = p1;
      lastP2 = p2;

      if (p1Pressed && !p2Pressed) {
        falseStart = true;
        falsePlayer = 1;
      } else if (p2Pressed && !p1Pressed) {
        falseStart = true;
        falsePlayer = 2;
      }

      delay(1);
    }

    if (falseStart) {
      clearMatrix();
      if (falsePlayer == 1) {
        matrix.displayString("P1 FEHLSTART!VERLOREN!", 7500, true, 0x00);
      } else {
        matrix.displayString("P2 FEHLSTART!VERLOREN!", 7500, true, 0x00);
      }
      delay(7900);
      clearMatrix();
      delay(2000);
      return;  // zurueck zur Idle-Animation
    }

    // -------- GO!! anzeigen --------
    clearMatrix();
    matrix.displayString("GO!!", 0, true, 0xdc);
    unsigned long goTime = millis();

    // -------- Reaktionsphase (max 3 Sekunden) --------
    int winner = 0;
    unsigned long reactionTime = 0;

    lastP1 = digitalRead(BUTTON_P1);
    lastP2 = digitalRead(BUTTON_P2);

    while ((millis() - goTime < 3000) && winner == 0) {
      bool p1 = digitalRead(BUTTON_P1);
      bool p2 = digitalRead(BUTTON_P2);

      bool p1Pressed = (lastP1 == HIGH && p1 == LOW);
      bool p2Pressed = (lastP2 == HIGH && p2 == LOW);

      lastP1 = p1;
      lastP2 = p2;

      if (p1Pressed && !p2Pressed) {
        winner = 1;
        reactionTime = millis() - goTime;
      } else if (p2Pressed && !p1Pressed) {
        winner = 2;
        reactionTime = millis() - goTime;
      }

      delay(1);
    }

    // -------- Niemand hat innerhalb von 3s gedrückt --------
    if (winner == 0) {
      clearMatrix();
      showSmileyAnimation();   // Augen schauen links/rechts (als Text)
      // danach: automatische neue Runde → while(true) läuft weiter
      continue;
    }

    // -------- Gewinner anzeigen --------
    char buf[32];
    if (winner == 1) {
      snprintf(buf, sizeof(buf), "P1 %lums", reactionTime);
    } else {
      snprintf(buf, sizeof(buf), "P2 %lums", reactionTime);
    }

    clearMatrix();
    matrix.displayString(buf, 3000, true, DISPLAY_COLOR);
    delay(3000);
    clearMatrix();

    // Eine Runde gespielt → zur Idle-Animation zurück
    return;
  }
}

void setup() {
  Wire.begin();
  SERIAL.begin(115200);
  waitForMatrixReady();

  pinMode(BUTTON_P1, INPUT_PULLUP);
  pinMode(BUTTON_P2, INPUT_PULLUP);

  uint16_t VID = matrix.getDeviceVID();
  if (VID != 0x2886) {
    SERIAL.println(F("Can not detect led matrix!!!"));
    while (1);
  }
  SERIAL.println(F("Matrix init success!!!"));

  randomSeed(analogRead(0));
}

void loop() {
  // --- Idle-Animation: Frames in Schleife ---
  for (int frame = 0; frame < FRAME_COUNT; frame++) {
    showAnimationFrame(frame);

    // Frame ca. 600 ms anzeigen, dabei immer wieder Buttons prüfen
    for (int i = 0; i < 60; i++) { // 60 * 10 ms = 600 ms
      bool p1 = (digitalRead(BUTTON_P1) == LOW);
      bool p2 = (digitalRead(BUTTON_P2) == LOW);

      // Spielstart: wenn beide Buttons gedrückt sind
      if (p1 && p2) {
        runReactionGame();
        // nach dem Spiel: zurück in loop(), Animation beginnt vorne
        return;
      }
      delay(10);
    }
  }
}