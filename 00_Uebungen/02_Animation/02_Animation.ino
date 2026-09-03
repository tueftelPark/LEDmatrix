// ======================================================
// BEFEHLSÜBERSICHT FÜR DIE GROVE RGB LED MATRIX
// ======================================================
//
// matrix.displayString("TEXT", GESCHWINDIGKEIT, WARTEN, FARBE);
//  → Zeigt Text scrollend auf der LED-Matrix an.
//    "TEXT" = beliebiger Text in Anführungszeichen
//    GESCHWINDIGKEIT = Tempo Textanzeige: 80 -> schnell, 150 -> mittel, 250 -> langsam
//    FARBE = Farbe des Textes
//             0x01 = Rot  0x02 = Grün  0x03 = Gelb
//             0x04 = Blau 0x05 = Magenta 0x06 = Cyan 0x07 = Weiß
//
// matrix.displayFrames(example + FRAME*8, ZEIT_MS, false, 1);
//  → Zeigt ein einzelnes Frame (Bild) an.
//    FRAME = Nummer des Frames (0 = erstes, 1 = zweites, …)
//    ZEIT_MS = Dauer der Anzeige eines Frames
//
//  for (int i = 0; i < 8; i++) clearFrame[i] = 0xffffffffffffffff;
//  matrix.displayFrames(clearFrame, 100, false, 1);
//  → Ein „leeres“ Frame anzeigen
//
//  delay(ZEIT_MS);  
//  → Pause zwischen zwei Aktionen
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

// ==============================================================
//  ANIMATIONS-DATEN -> hier dein Code vom Online Editor einfügen
// ==============================================================

int anzahl_bilder = 2;
int bildrate = 600; //in ms
const uint64_t example[] PROGMEM = {
//-----unter dieser Linie Frames einfügen------------------------------

  







//-----------------------------------
};

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


// ======================================================
//  SETUP – wird beim Start einmal ausgeführt
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
//  LOOP – wiederholt sich endlos
// ======================================================
void loop() {

  // --- Textanzeige ---
  // (Text, Zeit in ms, warten, Farbe)
  //matrix.displayString("Let's goo!!", 2500, true, 0x03);
  //delay(2700);

  // --- Display leeren ---
    for (int i = 0; i < 8; i++) clearFrame[i] = 0xffffffffffffffff;
  matrix.displayFrames(clearFrame, 100, false, 1);

  // --- Animation abspielen ---
  uint64_t buffer[8];
  //AUFGABE: Passe hier die Anzahl Frames an gemäss deiner Animation
  const int frameCount = anzahl_bilder; 

  for (int frame = 0; frame < frameCount; frame++) {
    for (int row = 0; row < 8; row++) {
      buffer[row] = read_qword_from_progmem(&example[frame * 8 + row]);
    }
//AUFGABE: Test - passe die Zahlen in den nächsten zwei Zeilen an und schaue was passiert 
    matrix.displayFrames(buffer, bildrate, false, 1); // Zahl nach buffer verändern
    delay(bildrate);
  }

  // --- Zweiter Text ---
  //matrix.displayString("GOAL 1:0!!", 2500, true, 0x03);
  //delay(2500);

  // ======================================================
  // ✍️ HIER EIGENE BEFEHLE EINFÜGEN (KOPIEREN AUS OBEN)
  // ======================================================
  //
  // Beispiele:
  //
  // matrix.displayString("LEVEL UP!", 3000, true, 0x05);
  // delay(1000);
  //
  // matrix.displayFrames(example + 1*8, 800, false, 1);
  // delay(500);
  //
  // matrix.displayString("GAME OVER", 4000, true, 0x04);
  // delay(2000);
  //
  // → Text ändern: erster Parameter ("TEXT")
  // → Anzeigedauer ändern: zweiter Parameter (z. B. 3000 = 3 Sekunden)
  // → Farbe ändern: letzter Parameter (z. B. 0x05 = Magenta)
  //
  // ======================================================

}
