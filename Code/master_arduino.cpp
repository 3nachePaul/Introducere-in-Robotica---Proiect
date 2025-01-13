#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

// Constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SLAVE_ADDRESS 0x08

const int PIN_A = 3;
const int PIN_B = 4;
const int BUTTON_PIN = 5;
const int SERVO_UNLOCK_ANGLE =  map(50, -60, 60, 0, 180);;
const int SERVO_LOCK_ANGLE =  map(-50, -60, 60, 0, 180);
const unsigned long DEBOUNCE_TIME = 50; // Debounce time in milliseconds

const int CORRECT_NUM_LEDS[] = {13, 11, 9, 6};
const int CORRECT_PLACE_LEDS[] = {12, 10, 8, 7};

// Globals
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo lockServo;

byte code[4] = {0};
byte codeGuess[4] = {0};
byte guessingDigit = 0;
byte numGuesses = 0;
volatile int encoderValue = 0;
volatile int lastAState;


bool isUnlocking = false;
bool isLocking = false;
bool isDisplayingMessage = false;
bool correctGuess = false;
bool oldButtonState = HIGH;
bool isDisplayingCrackedMessage = false;
bool isStartupAnimation = false;
int startupAnimationStep = 0;
unsigned long buttonPressTime = 0;
unsigned long lastActionTime = 0;
unsigned long crackedMessageStartTime = 0;

#define FRAME_DELAY (42)
#define FRAME_WIDTH (32)
#define FRAME_HEIGHT (32)
#define FRAME_COUNT (sizeof(frames) / sizeof(frames[0]))
const byte PROGMEM frames[][128] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,32,25,144,0,32,24,208,0,32,8,112,0,32,12,48,0,32,6,16,0,32,3,0,0,32,1,128,0,32,0,192,0,32,0,96,0,32,0,56,0,96,0,12,0,192,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0,0,0,31,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,32,9,144,0,32,24,208,0,32,8,112,0,32,4,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,7,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,27,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,25,144,0,32,24,208,0,32,8,112,0,32,4,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,7,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,27,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,9,144,0,32,24,208,0,32,12,112,0,32,6,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0,0,0,31,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,9,144,0,32,8,208,0,32,12,112,0,32,6,48,0,32,3,16,0,32,1,128,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,30,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,9,144,0,32,8,208,0,32,12,112,0,32,6,48,0,32,3,16,0,32,1,128,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,64,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,31,0,0,0,31,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,9,144,0,32,8,208,0,32,12,112,0,32,6,48,0,32,3,16,0,32,1,128,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,27,0,0,0,53,128,0,0,63,128,0,0,49,128,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,9,144,0,32,8,208,0,32,12,112,0,32,6,48,0,32,3,16,0,32,1,128,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,63,0,0,0,32,128,0,0,46,128,0,0,59,128,0,0,49,128,0,0,49,128,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,9,144,0,32,24,208,0,32,12,112,0,32,6,48,0,32,3,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,33,128,0,0,96,192,0,0,78,64,0,0,91,64,0,0,81,64,0,0,49,128,0,0,49,128,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,25,144,0,32,24,208,0,32,12,112,0,32,6,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,31,0,0,0,63,128,0,0,96,192,0,0,64,64,0,0,222,64,0,0,211,64,0,0,81,64,0,0,81,64,0,0,49,128,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,96,0,17,19,32,15,16,2,32,25,144,0,32,24,208,0,32,8,112,0,32,4,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,3,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,31,0,0,0,97,192,0,0,64,64,0,0,204,96,0,0,159,32,0,0,145,32,0,0,145,96,0,0,209,64,0,0,113,192,0,0,49,128,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,32,25,144,0,32,24,208,0,32,8,112,0,32,4,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,7,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,14,0,0,0,63,128,0,0,96,192,0,0,192,96,0,0,142,32,0,0,155,32,0,0,145,32,0,0,145,32,0,0,145,96,0,0,209,64,0,0,113,192,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,32,9,144,0,32,24,208,0,32,8,112,0,32,4,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,7,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,31,0,0,0,113,128,0,0,192,64,0,0,128,96,0,1,142,32,0,1,147,48,0,1,17,48,0,1,145,48,0,0,145,32,0,0,209,96,0,0,113,192,0,0,49,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,32,9,144,0,32,24,208,0,32,8,112,0,32,4,48,0,32,2,16,0,32,1,0,0,32,0,128,0,32,0,64,0,32,0,32,0,32,0,24,0,96,0,12,0,192,0,7,255,128,0,0,120,0,0,0,0,0},
  {0,0,0,0,0,63,128,0,0,97,192,0,0,192,96,0,1,128,32,0,1,30,48,0,1,17,16,0,1,17,16,0,1,17,16,0,1,145,48,0,0,145,32,0,0,81,64,0,0,49,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,32,25,144,0,32,24,208,0,32,8,112,0,32,4,48,0,32,2,16,0,32,3,0,0,32,1,128,0,32,0,192,0,32,0,96,0,32,0,56,0,96,0,12,0,192,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,63,128,0,0,96,192,0,0,128,32,0,1,140,48,0,1,31,16,0,1,17,16,0,1,17,16,0,1,17,16,0,1,17,48,0,0,145,32,0,0,209,96,0,0,113,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,32,25,144,0,32,24,208,0,32,8,112,0,32,12,48,0,32,6,16,0,32,3,0,0,32,1,128,0,32,0,192,0,32,0,96,0,32,0,56,0,96,0,12,0,192,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,4,0,0,0,59,128,0,0,64,64,0,0,128,32,0,1,14,16,0,1,27,16,0,1,17,16,0,1,17,16,0,1,17,16,0,1,17,16,0,0,145,32,0,0,209,96,0,0,49,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,56,0,96,0,12,0,192,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,49,128,0,0,64,64,0,0,128,32,0,1,14,16,0,1,27,16,0,1,17,16,0,0,17,16,0,1,17,16,0,1,17,16,0,0,145,32,0,0,145,32,0,0,113,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,56,0,96,0,12,0,192,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,4,0,0,0,49,128,0,0,64,64,0,0,128,32,0,1,14,16,0,1,19,16,0,0,17,16,0,0,17,0,0,1,17,16,0,1,17,16,0,0,145,32,0,0,145,32,0,0,113,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,19,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,0,0,0,17,192,0,0,17,254,0,0,17,51,192,0,17,51,32,0,17,19,32,15,16,3,48,25,144,0,48,24,208,0,48,8,112,0,48,12,48,0,48,6,16,0,48,3,0,0,48,1,128,0,48,0,192,0,32,0,96,0,32,0,48,0,96,0,12,0,64,0,7,255,128,0,0,254,0,0,0,0,0}
};




void setup() {
    Serial.begin(9600);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (true);
    }

    display.clearDisplay();
    lockServo.attach(2);
    Wire.begin();

    // Initialize LEDs
    for (int i = 0; i < 4; i++) {
        pinMode(CORRECT_NUM_LEDS[i], OUTPUT);
        pinMode(CORRECT_PLACE_LEDS[i], OUTPUT);
        digitalWrite(CORRECT_NUM_LEDS[i], LOW);
        digitalWrite(CORRECT_PLACE_LEDS[i], LOW);
    }

    // Initialize encoder
    pinMode(PIN_A, INPUT_PULLUP);
    pinMode(PIN_B, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_A), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_B), updateEncoder, CHANGE);

    // Initialize button
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    randomSeed(analogRead(0));
    display.setTextColor(SSD1306_WHITE);

    lockSafe();
    startupAnimation();
    animateLEDs();
    generateNewCode();
}

void loop() {
    if (correctGuess) {
        unlockSafe();
        return;
    }

    handleCodeInput();
    numGuesses++;
    evaluateGuess();

    resetGuess();
    updateDisplayCode();

    // Handle non-blocking delays
    if (isUnlocking && millis() - lastActionTime >= 500) {
        lockServo.write(SERVO_UNLOCK_ANGLE);
        isUnlocking = false;
        lastActionTime = millis();
    }

    if (isLocking && millis() - lastActionTime >= 500) {
        lockServo.write(SERVO_LOCK_ANGLE);
        isLocking = false;
        lastActionTime = millis();
    }
}

void updateDisplayCode() {
    display.clearDisplay();
    String temp;
    for (int i = 0; i < 4; i++) {
        if (i < guessingDigit) {
            temp += String(codeGuess[i]);
        } else if (i == guessingDigit) {
            temp += String(abs(encoderValue) % 10);
        } else {
            temp += "0";
        }
    }
    display.setTextSize(2);
    display.setCursor(20, 10);
    display.println(temp);
    display.display();
}

void generateNewCode() {
    Serial.print("Code: ");
    for (int i = 0; i < 4; i++) {
        code[i] = random(0, 10);
        Serial.print(code[i]);
    }
    Serial.println();
}

void handleCodeInput() {
    for (int i = 0; i < 4; i++) {
        guessingDigit = i;
        bool confirmed = false;
        while (!confirmed) {

            if (checkStopSignal()) {
                displayAccessGrantedMessage();
                unlockSafe();
                return;
            }

            bool buttonState = digitalRead(BUTTON_PIN);
            if (buttonState != oldButtonState && millis() - buttonPressTime >= DEBOUNCE_TIME) {
                buttonPressTime = millis();
                oldButtonState = buttonState;

                if (buttonState == LOW) {
                    codeGuess[i] = abs(encoderValue) % 10;
                    confirmed = true;
                }
            }
            updateDisplayCode();
        }
    }
}

void displayAccessGrantedMessage() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 10);
    display.println(F("Access Granted!"));
    display.display();
    unsigned long startMillis = millis();
    while (millis() - startMillis < 5000) {
        // Wait for 5000 milliseconds (5 seconds)
    }
    display.clearDisplay();
    display.display();
}

void evaluateGuess() {
    int correctNum = 0;
    int correctPlace = 0;
    bool usedDigits[4] = {false};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (codeGuess[i] == code[j] && !usedDigits[j]) {
                correctNum++;
                usedDigits[j] = true;
                break;
            }
        }
        if (codeGuess[i] == code[i]) {
            correctPlace++;
        }
    }

    updateLEDs(correctNum, correctPlace);
    if (correctPlace == 4) {
        correctGuess = true;
        display.clearDisplay();
        display.setCursor(20, 10);
        display.println(F("Cracked!"));
        display.display();
        isDisplayingCrackedMessage = true;
        crackedMessageStartTime = millis();
    }
}

void updateLEDs(int correctNum, int correctPlace) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(CORRECT_NUM_LEDS[i], i < correctNum ? HIGH : LOW);
        digitalWrite(CORRECT_PLACE_LEDS[i], i < correctPlace ? HIGH : LOW);
    }
}

void unlockSafe() {
    lockServo.write(SERVO_UNLOCK_ANGLE);
    isUnlocking = true;
    lastActionTime = millis();

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(35, 10);
    display.println(F("Unlocked!"));
    display.display();

    unsigned long startMillis = millis();
    while (millis() - startMillis < 3000) {
        // Wait for 3000 milliseconds (3 seconds)
    }

    display.clearDisplay();
    display.display();
    isDisplayingMessage = false;
    animateLEDs();
    displayButtonPressAnimation();
}

void displayButtonPressAnimation() {
    const char* staticText = "Press the button";
    int16_t textX = (SCREEN_WIDTH - strlen(staticText) * 6) / 2; // Center the text horizontally
    int16_t textY = 0;  // Fixed vertical position for static text

    int frame = 0;
    unsigned long lastFrameTime = millis();
    unsigned long lastScrollTime = millis();

    while (true) {
        unsigned long currentMillis = millis();

        if (currentMillis - lastFrameTime >= 42) { // Adjust the speed of the frame animation
            lastFrameTime = currentMillis;

            display.clearDisplay();
            display.setTextSize(1);
            display.setCursor(textX, textY);
            display.println(staticText);

            display.drawBitmap(32, (SCREEN_HEIGHT - FRAME_HEIGHT) / 2, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
            display.display();

            frame = (frame + 1) % FRAME_COUNT;
        }

        // Check for button press to start a new round
        bool buttonState = digitalRead(BUTTON_PIN);
        if (buttonState != oldButtonState && millis() - lastActionTime >= DEBOUNCE_TIME) {
            buttonPressTime = millis();
            oldButtonState = buttonState;

            if (buttonState == LOW) {
                lockSafe();
                break;
            }
        }
    }
}




void waitForLock() {
    bool locked = false;
    while (!locked) {
        bool buttonState = digitalRead(BUTTON_PIN);
        if (buttonState != oldButtonState && millis() - buttonPressTime >= DEBOUNCE_TIME) {
            buttonPressTime = millis();
            oldButtonState = buttonState;

            if (buttonState == LOW) {
                lockSafe();
                locked = true;
            }
        }
    }
}

void startupAnimation() {
    const char* messages[] = {"Crack", "The", "Code"};
    for (int i = 0; i < 3; i++) {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(40, 10);
        display.println(messages[i]);
        display.display();

        unsigned long startMillis = millis();
        while (millis() - startMillis < 500) {
            // Wait for 500 milliseconds
        }
    }
}

void updateEncoder() {
    int currentAState = digitalRead(PIN_A);
    if (currentAState != lastAState) {
        encoderValue += (digitalRead(PIN_B) != currentAState) ? 1 : -1;
        lastAState = currentAState;
    }
}

bool checkStopSignal() {
    Wire.requestFrom(SLAVE_ADDRESS, 1);
    while (Wire.available()) {
        return Wire.read() == 1;
    }
    return false;
}

void lockSafe() {
    lockServo.write(SERVO_LOCK_ANGLE);
    isLocking = true;
    lastActionTime = millis();

    display.clearDisplay();
    display.setCursor(30, 10);
    display.println(F("Locked"));
    display.display();
    isDisplayingMessage = true;
    lastActionTime = millis();
    resetGame();
}

void resetGame() {
    correctGuess = false;
    resetGuess();
    generateNewCode();
    updateLEDs(0, 0);
}

void resetGuess() {
    encoderValue = 0;
    guessingDigit = 0;
    for (int i = 0; i < 4; i++) {
        codeGuess[i] = 0;
    }
}

void animateLEDs() {
  unsigned long startTime = millis();
  unsigned long elapsedTime = 0;

  // Total duration for the animation in milliseconds
  unsigned long animationDuration = 2000; // 2 seconds
  unsigned long interval = 200;  // Delay between each LED action (ms)

  int numLeds = 4; // Number of LEDs in each array

  while (elapsedTime < animationDuration) {
    elapsedTime = millis() - startTime;

    // Step 1: Light up CORRECT_NUM_LEDS[] one by one, ensuring the last one is the 4th LED
    if (elapsedTime < (interval * numLeds)) {
      int ledIndex = elapsedTime / interval;  // Determine which LED to turn on
      if (ledIndex < numLeds - 1) {
        digitalWrite(CORRECT_NUM_LEDS[ledIndex], HIGH);  // Turn on LED in CORRECT_NUM_LEDS[]
      }
      if (ledIndex == numLeds - 1) {
        // Light up the last LED (CORRECT_NUM_LEDS[3]) last
        digitalWrite(CORRECT_NUM_LEDS[ledIndex], HIGH);
      }
    }

    // Step 2: Light up CORRECT_PLACE_LEDS[] after all CORRECT_NUM_LEDS[] are lit
    else if (elapsedTime < (interval * (numLeds + numLeds))) {
      int ledIndex = (elapsedTime - (interval * numLeds)) / interval;  // Determine which LED to turn on from CORRECT_PLACE_LEDS
      if (ledIndex < numLeds) {
        digitalWrite(CORRECT_PLACE_LEDS[ledIndex], HIGH);  // Turn on LED in CORRECT_PLACE_LEDS[]
      }
    }

    // Step 3: Turn off LEDs in reverse order after the animation duration
    else if (elapsedTime < animationDuration + (interval * numLeds)) {
      int ledIndex = (elapsedTime - animationDuration) / interval;  // Reverse turn-off
      if (ledIndex < numLeds) {
        digitalWrite(CORRECT_PLACE_LEDS[numLeds - 1 - ledIndex], LOW);  // Turn off LED in reverse order from CORRECT_PLACE_LEDS[]
      }
    } else {
      int ledIndex = (elapsedTime - animationDuration - (interval * numLeds)) / interval;  // Reverse turn-off
      if (ledIndex < numLeds) {
        digitalWrite(CORRECT_NUM_LEDS[numLeds - 1 - ledIndex], LOW);  // Turn off LED in reverse order from CORRECT_NUM_LEDS[]
      }
    }

    // Non-blocking: Avoid using delay() to keep everything smooth
  }

  // After the full 2 seconds, make sure all LEDs are off
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(CORRECT_NUM_LEDS[i], LOW);
    digitalWrite(CORRECT_PLACE_LEDS[i], LOW);
  }
}
