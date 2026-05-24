#include <SPI.h>
#include <LoRa.h>
#include "pitches.h"

#define Buzzer 7
#define NSS   10
#define RST    8
#define DIO0   6

int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

void jouerBuzzer() {
  for (int i = 0; i < 8; i++) {
    int dur = 1000 / noteDurations[i];
    tone(Buzzer, melody[i], dur);
    delay(dur * 1.30);
    noTone(Buzzer);
  }
}

void setup() {
  Serial.begin(115200);
  LoRa.setPins(NSS, RST, DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa KO");
    while (1);
  }
  LoRa.setSpreadingFactor(7);
  Serial.println("Récepteur prêt");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String msg = "";
    while (LoRa.available()) {
      msg += (char)LoRa.read();
    }
    Serial.print("Reçu : ");
    Serial.println(msg);

    if (msg == "CHUTE") {
      Serial.println("ALERTE CHUTE !");
      jouerBuzzer();
    }
  }
}