#include <Keypad.h>
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>

// Define the SD card CS pin
const int SD_CS_PIN = 53;

// Define the speaker pin
const int speakerPin = 5;

// Initialize the SD card and audio libraries
TMRpcm tmrpcm;

// Define the connections for the 4x4 Keypad
const byte ROWS_4x4 = 4;
const byte COLS_4x4 = 4;
char keys_4x4[ROWS_4x4][COLS_4x4] = {
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'},
  {'9', 'A', 'B', 'C'},
  {'D', 'E', 'F', 'G'}
};
byte rowPins_4x4[ROWS_4x4] = {9, 8, 7, 6};
byte colPins_4x4[COLS_4x4] = {13, 12, 11, 10};
Keypad keypad_4x4 = Keypad(makeKeymap(keys_4x4), rowPins_4x4, colPins_4x4, ROWS_4x4, COLS_4x4);

// Define the connections for the 4x5 Keypad
const byte ROWS_4x5 = 5;
const byte COLS_4x5 = 4;
char keys_4x5[ROWS_4x5][COLS_4x5] = {
  {'H', 'I', 'J', 'K'},
  {'L', 'M', 'N', 'O'},
  {'P', 'Q', 'R', 'S'},
  {'T', 'U', 'V', 'W'},
  {'X', 'Y', 'Z', '0'}
};
byte rowPins_4x5[ROWS_4x5] = {A4, A5, A6, A7, A8};
byte colPins_4x5[COLS_4x5] = {A0, A1, A2, A3};
Keypad keypad_4x5 = Keypad(makeKeymap(keys_4x5), rowPins_4x5, colPins_4x5, ROWS_4x5, COLS_4x5);

// Define the LED matrix pins for 4x4
const int ledRowPins_4x4[4] = {26, 30, 34, 38}; // Rows
const int ledColPins_4x4[4] = {4, 3, 49 , 22}; // Columns

// Define the LED matrix pins for 4x5
const int ledRowPins_4x5[5] = {A13, A14, A15, 42, 46}; // Rows
const int ledColPins_4x5[4] = {A9, A10, A11, A12}; // Columns

// Song files associated with each button
const char* songs_4x4[16] = {
  "AN_U1.wav", "MZ1.wav", "TR1.wav", "ML1.wav", "AR1.wav", "NL1.wav",
  "MN1.wav", "AS1.wav", "JH1.wav", "WB1.wav", "OD1.wav", "CG1.wav",
  "MP1.wav", "UP1.wav", "SK1.wav", "BR1.wav"
};

const char* songs_4x5[20] = {
  "LA_U1.wav", "KL1.wav", "TN1.wav", "PY_U1.wav", "GA1.wav", "KA1.wav",
  "AP1.wav", "TG1.wav", "RJ1.wav", "GJ1.wav", "DD_U1.wav", "MH1.wav",
  "UK1.wav", "HR1.wav", "DL_U1.wav", "PB1.wav", "LD_U1.wav", "JK_U1.wav",
  "HP1.wav", "CH_U1.wav"
};

// Variables to keep track of currently active LEDs
int currentLedIndex_4x4 = -1;
int currentLedIndex_4x5 = -1;

void setup() {
  // Initialize the LED row and column pins for 4x4
  for (int i = 0; i < 4; i++) {
    pinMode(ledRowPins_4x4[i], OUTPUT);
    pinMode(ledColPins_4x4[i], OUTPUT);
    digitalWrite(ledRowPins_4x4[i], LOW);
    digitalWrite(ledColPins_4x4[i], HIGH);
  }

  // Initialize the LED row and column pins for 4x5
  for (int i = 0; i < 5; i++) {
    pinMode(ledRowPins_4x5[i], OUTPUT);
    digitalWrite(ledRowPins_4x5[i], LOW);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(ledColPins_4x5[i], OUTPUT);
    digitalWrite(ledColPins_4x5[i], HIGH);
  }

  pinMode(speakerPin, OUTPUT);
  Serial.begin(9600); // For debugging purposes

  // Initialize the SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Initialize the TMRpcm library
  tmrpcm.speakerPin = speakerPin;
}

void loop() {
  char key_4x4 = keypad_4x4.getKey();
  char key_4x5 = keypad_4x5.getKey();

  if (key_4x4) {
    Serial.print("4x4 Keypad pressed: ");
    Serial.println(key_4x4);
    int index = getIndexFromKey_4x4(key_4x4);
    if (index != -1) {
      handleKeyPress(index, true);
    }
  }

  if (key_4x5) {
    Serial.print("4x5 Keypad pressed: ");
    Serial.println(key_4x5);
    int index = getIndexFromKey_4x5(key_4x5);
    if (index != -1) {
      handleKeyPress(index, false);
    }
  }

  // Check if the LED duration has passed
  if (currentLedIndex_4x4 != -1 && !tmrpcm.isPlaying()) {
    // Turn off the current LED
    turnOffLed(currentLedIndex_4x4, true);
    currentLedIndex_4x4 = -1;
  }

  if (currentLedIndex_4x5 != -1 && !tmrpcm.isPlaying()) {
    // Turn off the current LED
    turnOffLed(currentLedIndex_4x5, false);
    currentLedIndex_4x5 = -1;
  }
}

// Function to handle key press and toggle the LED and sound
void handleKeyPress(int index, bool is4x4) {
  if (is4x4) {
    if (currentLedIndex_4x4 == index) {
      // If the same key is pressed again, turn off the LED and stop the sound
      turnOffLed(currentLedIndex_4x4, true);
      tmrpcm.stopPlayback();
      currentLedIndex_4x4 = -1;
    } else {
      // Turn off any active LED from 4x5 keypad
      if (currentLedIndex_4x5 != -1) {
        turnOffLed(currentLedIndex_4x5, false);
        tmrpcm.stopPlayback();
        currentLedIndex_4x5 = -1;
      }
      turnOffLed(currentLedIndex_4x4, true);
      currentLedIndex_4x4 = index;
      turnOnLed(currentLedIndex_4x4, true);
      tmrpcm.play(songs_4x4[currentLedIndex_4x4]);
    }
  } else {
    if (currentLedIndex_4x5 == index) {
      // If the same key is pressed again, turn off the LED and stop the sound
      turnOffLed(currentLedIndex_4x5, false);
      tmrpcm.stopPlayback();
      currentLedIndex_4x5 = -1;
    } else {
      // Turn off any active LED from 4x4 keypad
      if (currentLedIndex_4x4 != -1) {
        turnOffLed(currentLedIndex_4x4, true);
        tmrpcm.stopPlayback();
        currentLedIndex_4x4 = -1;
      }
      turnOffLed(currentLedIndex_4x5, false);
      currentLedIndex_4x5 = index;
      turnOnLed(currentLedIndex_4x5, false);
      tmrpcm.play(songs_4x5[currentLedIndex_4x5]);
    }
  }
}

// Function to turn on the specified LED
void turnOnLed(int index, bool is4x4) {
  if (is4x4) {
    int row = index / 4;
    int col = index % 4;
    digitalWrite(ledRowPins_4x4[row], HIGH);
    digitalWrite(ledColPins_4x4[col], LOW);
  } else {
    int row = index / 4;
    int col = index % 4;
    digitalWrite(ledRowPins_4x5[row], HIGH);
    digitalWrite(ledColPins_4x5[col], LOW);
  }
}

// Function to turn off the specified LED
void turnOffLed(int index, bool is4x4) {
  if (index == -1) return;
  if (is4x4) {
    int row = index / 4;
    int col = index % 4;
    digitalWrite(ledRowPins_4x4[row], LOW);
    digitalWrite(ledColPins_4x4[col], HIGH);
  } else {
    int row = index / 4;
    int col = index % 4;
    digitalWrite(ledRowPins_4x5[row], LOW);
    digitalWrite(ledColPins_4x5[col], HIGH);
  }
}

// Function to get the index from the key pressed for 4x4 keypad
int getIndexFromKey_4x4(char key) {
  for (int row = 0; row < ROWS_4x4; row++) {
    for (int col = 0; col < COLS_4x4; col++) {
      if (keys_4x4[row][col] == key) {
        return row * COLS_4x4 + col;
      }
    }
  }
  return -1; // Key not found
}

// Function to get the index from the key pressed for 4x5 keypad
int getIndexFromKey_4x5(char key) {
  for (int row = 0; row < ROWS_4x5; row++) {
    for (int col = 0; col < COLS_4x5; col++) {
      if (keys_4x5[row][col] == key) {
        return row * COLS_4x5 + col;
      }
    }
  }
  return -1; // Key not found 
}
