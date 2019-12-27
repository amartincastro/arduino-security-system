#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Servo.h>
#include <Keypad.h>

#define TAG_LENGTH              4
#define ALPHANUM_ADDRESS        0x70
#define SERVO_PIN               11
#define KEYPAD_ROWS             4
#define KEYPAD_COLS             4

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Servo myservo;  // create servo object to control a servo

char hexaKeys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[KEYPAD_ROWS] = {6, 7, 8, 9};
byte colPins[KEYPAD_COLS] = {2, 3, 4, 5};
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS); 

byte arielCard[]={ 0x3B, 0x2F, 0x33, 0x00 };
byte sethCard[]={ 0xE6, 0xD1, 0x19, 0xF9 };

char arielCode[]={ '4', '2', '6', '9' };

void setup(void) {
  Serial.begin(9600);
  Serial.println("NDEF Reader");
  nfc.begin();
  alpha4.begin(ALPHANUM_ADDRESS);  // pass in the address
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  lockClose();

  alpha4.clear();
  printReady();
}

void loop(void) {
//  Serial.println("\nScan a NFC tag\n");
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    tag.print();

    byte tagUid[TAG_LENGTH];
    tag.getUid(tagUid, TAG_LENGTH);

    if (byteArrayCompare(tagUid, arielCard, TAG_LENGTH)) {
      alpha4.clear();
      alpha4.writeDigitAscii(0, 'A');
      alpha4.writeDigitAscii(1, 'R');
      alpha4.writeDigitAscii(2, 'E');
      alpha4.writeDigitAscii(3, 'L');
      alpha4.writeDisplay();
      delay(1000);

      // Check for keys
      printDashes();
      char passcode[4];
      int inputPos = 0;
      while (inputPos < 4) {
        char customKey = customKeypad.getKey();
        if (customKey) {
          Serial.println(customKey);
          passcode[inputPos] = customKey;
          alpha4.writeDigitAscii(inputPos, '*');
          alpha4.writeDisplay();
          inputPos++;
        }
      }
      
      if (charArrayCompare(arielCode, passcode, 4)) {
        lockOpen();
        printOk();
      } else {
        printNope();
      }
    } else {
      printNope();
    }
    delay(5000);
    lockClose();
    printReady();
  } else {
    delay(1000);
  }
}

boolean byteArrayCompare(byte a[], byte b[], int array_size) {
 for (int i = 0; i < array_size; ++i)
   if (a[i] != b[i])
     return(false);
 return(true);
}

boolean charArrayCompare(char a[], char b[], int array_size) {
 for (int i = 0; i < array_size; ++i)
   if (a[i] != b[i])
     return(false);
 return(true);
}

void printReady() {
  alpha4.writeDigitAscii(0, 'R');
  alpha4.writeDigitAscii(1, 'E');
  alpha4.writeDigitAscii(2, 'D');
  alpha4.writeDigitAscii(3, 'Y');
  alpha4.writeDisplay();
}

void printNope() {
  alpha4.clear();
  alpha4.writeDigitAscii(0, 'N');
  alpha4.writeDigitAscii(1, 'O');
  alpha4.writeDigitAscii(2, 'P');
  alpha4.writeDigitAscii(3, 'E');
  alpha4.writeDisplay();
}

void printOk() {
  alpha4.clear();
  alpha4.writeDigitAscii(0, ' ');
  alpha4.writeDigitAscii(1, 'O');
  alpha4.writeDigitAscii(2, 'K');
  alpha4.writeDigitAscii(3, ' ');
  alpha4.writeDisplay();
}

void printDashes() {
  alpha4.clear();
  alpha4.writeDigitAscii(0, '-');
  alpha4.writeDigitAscii(1, '-');
  alpha4.writeDigitAscii(2, '-');
  alpha4.writeDigitAscii(3, '-');
  alpha4.writeDisplay();
}

void lockOpen() {
  myservo.write(180);
}

void lockClose() {
  myservo.write(90);
}
