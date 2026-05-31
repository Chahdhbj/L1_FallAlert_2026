#include <SPI.h>
#include <LoRa.h>
#include "kxtj3-1057.h"
#include "Wire.h"
#include <math.h>
#include "pitches.h"

#define Buzzer 7
#define NSS   10
#define RST    8
#define DIO0   6

#define SEUIL_CHUTE    2.5
#define DUREE_IMMOBILITE 3000

float sampleRate = 12.5;
uint8_t accelRange = 4;
KXTJ3 myIMU(0x0E);

bool enSurveillance = false;
unsigned long tempsPic = 0;

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
  Serial.println("LoRa OK");

  myIMU.begin(sampleRate, accelRange);
  myIMU.intConf(123, 1, 10, HIGH);
  jouerBuzzer(); // bip de démarrage
}

void loop() {
  myIMU.standby(false);
  int16_t raw = 0;
  float x, y, z;

  myIMU.readRegisterInt16(&raw, KXTJ3_XOUT_L);
  x = (float)accelRange * raw / 32768;
  myIMU.readRegisterInt16(&raw, KXTJ3_YOUT_L);
  y = (float)accelRange * raw / 32768;
  myIMU.readRegisterInt16(&raw, KXTJ3_ZOUT_L);
  z = (float)accelRange * raw / 32768;

  float acc = sqrt(x*x + y*y + z*z);

  if (acc > SEUIL_CHUTE && !enSurveillance) {
    enSurveillance = true;
    tempsPic = millis();
    Serial.println("Impact détecté !");
  }

  if (enSurveillance && millis() - tempsPic > DUREE_IMMOBILITE) {
    bool immobile = (acc > 0.8 && acc < 1.2);
   bool orientationAnormale = (z > 0.5 || z < -0.5);

    if (immobile && orientationAnormale) {
      Serial.println("CHUTE CONFIRMÉE - envoi LoRa");
      jouerBuzzer();
      LoRa.beginPacket();
      LoRa.print("CHUTE");
      LoRa.endPacket();
    } else {
      Serial.println("Fausse alerte");
    }
    enSurveillance = false;
  }

  delay(10);
}
