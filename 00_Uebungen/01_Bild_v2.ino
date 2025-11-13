// ======================================================
// Wir gestalten ein Bild
// ======================================================
// Benutzte Befehle:
// matrix.displayFrames(example + FRAME*8, ZEIT_MS, false, 1);
//  → Zeigt ein einzelnes Frame (Bild) aus dem Array an.
//    FRAME = Nummer des Frames (0 = erstes, 1 = zweites, …)
//    ZEIT_MS = Dauer der Anzeige eines Frames
//
// delay(ZEIT_MS);  → Pause zwischen zwei Aktionen
// for (int i = 0; i < ANZAHL; i++) { … } → Wiederholt Befehle (z. B. für Animationen)
//
// ======================================================

#include "grove_two_rgb_led_matrix.h"
#include <avr/pgmspace.h> // Für PROGMEM (große Arrays im Flash speichern)

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

#define DISPLAY_COLOR 0x11 // Standardfarbe der Anzeige (frei anpassbar)

GroveTwoRGBLedMatrixClass matrix;
uint64_t clearFrame[8];

// ==================================================================
// BILD-DATEN -> hier kommt dein Bild als Zahlen und Buchstaben rein
// ==================================================================
// TODO: Füge hier dein Bild als Code aus dem Online Editor ein
uint64_t smile[] = {

  0x8383838383838383,
  0x8383cb8383cb8383,
  0x8383838383838383,
  0x8383838383838383,
  0x83cb83838383cb83,
  0x8383cb8383cb8383,
  0x838383cbcb838383,
  0x8383838383838383
};

// ======================================================
// SETUP – hier werden alle Vorbereitungen getroffen
// ======================================================
void setup() {
  Wire.begin();
  SERIAL.begin(115200);
  delay(1000);

  uint16_t VID = matrix.getDeviceVID();
  if (VID != 0x2886) {
    SERIAL.println(F("LED-Matrix nicht erkannt!"));
    while (1);
  }
  SERIAL.println(F("Matrix erfolgreich verbunden!"));
}

// ======================================================
// LOOP – hier werden die Aktionen ausgeführt
// ======================================================
void loop() {
  // --- Display leeren ---
  matrix.displayFrames(clearFrame, 100, false, 1);

  // --- Bild anzeigen ---
    matrix.displayFrames(smile, 10000, false, 1); // 600 = Zeit pro Frame (langsamer/schneller)
    delay(10000);
}
