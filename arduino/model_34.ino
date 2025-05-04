// Model 34 tape & card punch controller software
// Vesa-Pekka Palmu 2025
// Lisenced under the MIT lisence.

// Punch control pins
#define P_START 2
#define P_SKIP 3
#define P_BUSY 4
#define P_BACKSPACE 5

// Pins for data tracks
#define T_8 A0
#define T_7 A1
#define T_6 A2
#define T_5 A3
#define T_4 A4
#define T_3 A5
#define T_2 12
#define T_1 11

#define BUF_HIGH 32
#define S_IDLE 0
#define S_PUNCH 1
#define S_BUSY 2

#define XON 0x11
#define XOFF 0x13

uint8_t tracks[] = {T_1, T_2, T_3, T_4, T_5, T_6, T_7, T_8};

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);

  // Initialize the punch control signals
  pinMode(P_START, OUTPUT);
  digitalWrite(P_START, LOW);
  pinMode(P_SKIP, OUTPUT);
  digitalWrite(P_SKIP, HIGH);
  pinMode(P_BACKSPACE, OUTPUT);
  digitalWrite(P_BACKSPACE, LOW);
  
  pinMode(P_BUSY, INPUT);
  
  // Led
  pinMode(13, OUTPUT);

  // Data track outputs
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(tracks[i], OUTPUT);
    digitalWrite(tracks[i], HIGH);
  }
  Serial.write("S");
}

// Punch a character and set state to busy
void startPunch(uint8_t data) {
  // Punch data bits
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(tracks[i], (data >> i) & 1);
  }

  // Start the punch
  delay(1);
  digitalWrite(P_START, HIGH);
  delay(1);
  digitalWrite(P_START, LOW);
  delay(1);
}

void loop() {
  if (!Serial.available()) {
    // No bytes, send XON to request input
    Serial.write(XON);
  }

  if (Serial.available() > BUF_HIGH) {
    // Buffer is almost full, send XOFF until we have punched everything
    Serial.write(XOFF);
  }

  if (digitalRead(P_BUSY) == HIGH) {
      if (Serial.available()) {
        // Punch is idle and we have data, punch it
        startPunch(Serial.read());
      }
  }
}
