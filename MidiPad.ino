#include <Arduino.h>
#include <BLEMidi.h>
#include "TFT_eSPI.h"
//#include "mesas.h"
unsigned colour = 0xFFFF;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
#define topbutton 0
#define lowerbutton 14
#define PIN_POWER_ON 15  // LCD and battery Power Enable
#define PIN_LCD_BL 38    // BackLight enable pin (see Dimming.txt)
#define TOUCH_MODULES_CST_SELF
#include "TouchLib.h"
#include "Wire.h"
#include "pin_config.h"
#define LCD_MODULE_CMD_1

#warning Please confirm that you have purchased a display screen with a touch chip, otherwise the touch routine cannot be implemented.
#if defined(TOUCH_MODULES_CST_MUTUAL)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS328_SLAVE_ADDRESS, PIN_TOUCH_RES);
#elif defined(TOUCH_MODULES_CST_SELF)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS820_SLAVE_ADDRESS, PIN_TOUCH_RES);
#endif

#define TOUCH_GET_FORM_INT 0
bool connection_notified = false;
bool flags_sleep = false;
#if TOUCH_GET_FORM_INT
bool get_int = false;
#endif
unsigned long lastTouchTime = 0;  // To manage debounce
unsigned long debounceDelay = 10;  // Delay in milliseconds to wait for next valid touch input

int progress = 0;
int x;
int blocks = 0;
int i = 0;
bool ticktock = true;
int lastNoteIndex = -1;  // Global variable to store the last note played

// Define the MIDI notes for each drum sound
const int drumNotes[] = {
  36, 37, 38, 39, 40, 41, 42, 43, 44, 46, 47, 48, 49, 51, 52, 53
};

// Global variables to store colors
uint16_t gridColor = TFT_BLUE;
uint16_t highlightColor = TFT_RED;

void setup() {

  pinMode(PIN_POWER_ON, OUTPUT);  //enables the LCD and to run on battery
  pinMode(PIN_LCD_BL, OUTPUT);    // BackLight enable pin
  pinMode(lowerbutton, INPUT);    //Right button pulled up, push = 0
  pinMode(topbutton, INPUT);      //Left button  pulled up, push = 0
  delay(100);
  digitalWrite(PIN_POWER_ON, HIGH);
  digitalWrite(PIN_LCD_BL, HIGH);
  pinMode(PIN_TOUCH_RES, OUTPUT);
  digitalWrite(PIN_TOUCH_RES, LOW);
  delay(500);
  digitalWrite(PIN_TOUCH_RES, HIGH);
  tft.begin();
  Serial.begin(115200);  // be sure to set USB CDC On Boot: "Enabled"
  //(Serial print slows progres bar Demo)
  delay(100);
  tft.init();
  tft.setRotation(5);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);  //horiz / vert<> position/dimension
  //tft.pushImage(0, 0, 320, 170, mesas);  //push the image to the screen
  tft.setTextSize(1);
  tft.setTextDatum(4);
  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
  delay(100);
  //Serial.println("In setup!");
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin("MidiPad V0.1");
  Serial.println("Waiting for connections...");
  delay(2000);
}

void highlightSquare(int gridX, int gridY, uint16_t color) {
  int squareWidth = 320 / 4;
  int squareHeight = 170 / 4;
  
  int xStart = gridX * squareWidth;
  int yStart = gridY * squareHeight;
  
  tft.fillRect(xStart, yStart, squareWidth, squareHeight, color);
}


void checkTouchAndSendMIDI(int x, int y) {
  static int lastNoteIndex = -1;  // Store the last note played

  // Adjust coordinates
  int adjustedX = y;
  int adjustedY = 170 - x;

  // If a touch is detected
  if(adjustedX >= 0 && adjustedY >= 0) {
    // Correct divisions
    int gridX = adjustedX / (320 / 4);
    int gridY = adjustedY / (170 / 4);
    
    int noteIndex = gridY * 4 + gridX;

    // Check if note index is in range of our drumNotes array
    if(noteIndex >= 0 && noteIndex < 16) {
      int midiNote = drumNotes[noteIndex];

      // If a new grid is touched, stop the last note and play the new note
      if (noteIndex != lastNoteIndex) {
        if (lastNoteIndex != -1) {
          //BLEMidiServer.noteOff(0, drumNotes[lastNoteIndex], 127);
          int lastGridX = lastNoteIndex % 4;
          int lastGridY = lastNoteIndex / 4;
          highlightSquare(lastGridX, lastGridY, TFT_BLACK);  // Un-highlight the previous square
        }
        BLEMidiServer.noteOn(0, midiNote, 127);
        lastNoteIndex = noteIndex;
        highlightSquare(gridX, gridY, TFT_RED); // Highlight the square
      }
    }
  } 
  // If no touch detected
  else {
    // If the touch is released, stop the last note and reset lastNoteIndex
    if (lastNoteIndex != -1) {
      BLEMidiServer.noteOff(0, drumNotes[lastNoteIndex], 127);
      
      int lastGridX = lastNoteIndex % 4;
      int lastGridY = lastNoteIndex / 4;
      highlightSquare(lastGridX, lastGridY, TFT_BLACK);  // Un-highlight the square

      lastNoteIndex = -1;
    }
  }
}



void drawGrid() {
  //tft.fillScreen(TFT_BLACK);
  int gridSizeX = 320 / 4; // Adjust grid size
  int gridSizeY = 170 / 4; 

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      tft.drawRect(i*gridSizeX, j*gridSizeY, gridSizeX, gridSizeY, TFT_BLUE);
    }
  }
}



void DrumTest() {
   // loop to play all the drum notes

      for (int i = 0; i < sizeof(drumNotes) / sizeof(drumNotes[0]); i++) {
    int note = drumNotes[i];
        Serial.print("Playing note: ");
        Serial.println(note);
    // Note On
    BLEMidiServer.noteOn(1, note, 127); // Channel 1, full velocity

    delay(500); // Note duration (adjust as needed)

    // Note Off
    BLEMidiServer.noteOff(1, note, 0); // Channel 1, release the note

    delay(500); // Delay between notes (adjust as needed)
      }
}


void loop() {
 

  drawGrid();
  //delay(100);
  if (BLEMidiServer.isConnected()) {
    if (not connection_notified) {   //so we only get notified the one time
      Serial.println("BLE CONNECTED!");
      connection_notified = !connection_notified;
    }

    if (touch.read()) {
      unsigned long currentTouchTime = millis();
      if (currentTouchTime - lastTouchTime > debounceDelay) {
        lastTouchTime = currentTouchTime;
        Serial.print("touch");
        TP_Point t = touch.getPoint(0);
        Serial.print(t.x);
        Serial.print(", ");
        Serial.println(t.y);
        checkTouchAndSendMIDI(t.x, t.y);
      }
    } else {
      checkTouchAndSendMIDI(-1, -1);  // When no touch detected, to handle note off
    }
  }


}

