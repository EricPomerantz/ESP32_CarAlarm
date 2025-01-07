#include <ESP32_Servo.h>
#include <Keypad.h>

Servo myservo;

// Inputs
int readA = 39;
int readB = 36;
int motorControlButton = 15; 
// Outputs
int redLed = 33;
int greenLed = 32;
int yellowLed = 25;
int motorControlPin = 13; // pin to control the motor with transistor
const int servoPin = 23;
int buzzerPin = 22; //  passive buzzer

// Setup PWM properties
const int freq = 2000; 
const int buzzerChannel = 2; // PWM channel. I had to change due to interference with servo and buzzer function
const int resolution = 8; 

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {17, 5, 18, 19};
byte colPins[COLS] = {12, 14, 27, 26};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Status tracking
bool immoStatus = false;
bool lastAState = LOW;
bool lastBState = LOW;
bool startEnable = false;
bool motorState = false; // False = motor off, True = motor on

// Pin code tracking
String enteredPin = "";
const String correctPin = "1234";

void setup() {
  pinMode(readA, INPUT);
  pinMode(readB, INPUT);
  pinMode(motorControlButton, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(motorControlPin, OUTPUT);

  // Initialize PWM for the buzzer
  ledcSetup(buzzerChannel, freq, resolution);
  ledcAttachPin(buzzerPin, buzzerChannel);

  myservo.attach(servoPin);

  // Initialize positions for the servo and LEDs
  myservo.write(180);
  delay(1000);
  myservo.write(0);
  delay(1000);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, HIGH);
  digitalWrite(yellowLed, LOW); // Start with yellow led off
  digitalWrite(motorControlPin, LOW); // Ensure motor is off initially
}

void loop() {
  bool currentAState = digitalRead(readA);
  bool currentBState = digitalRead(readB);
  static bool lastMotorButtonState = HIGH; // Assume active low button
  bool currentMotorButtonState = digitalRead(motorControlButton);

  // Read keypad
  char key = customKeypad.getKey();
  if (key) {
    enteredPin += key;
    if (enteredPin.length() == 4) {
      if (enteredPin == correctPin) {
        startEnable = true; // toggle start enable
        digitalWrite(yellowLed, HIGH);
        playCorrectCodeSound(); 
      } else {
        startEnable = false;
        digitalWrite(yellowLed, LOW); // Indicate wrong pin entry
        soundBuzzer(); // buzzer on incorrect pin entry
      }
      enteredPin = ""; // Reset for next entry
    }
  }

  // Toggle motor with control button
  if (startEnable && currentMotorButtonState == LOW && lastMotorButtonState == HIGH) {
    motorState = !motorState; // Toggle motor state
    digitalWrite(motorControlPin, motorState ? HIGH : LOW); //turn on and off
  }
  lastMotorButtonState = currentMotorButtonState; // save state

  if (!startEnable && currentMotorButtonState == HIGH) {
    alarmBuzzer();
  }

  // Detect button A press
  if (currentAState == HIGH && lastAState == LOW) {
    immoStatus = true;
    startEnable = false; // Disable start when immobilized
    digitalWrite(yellowLed, LOW); 
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    myservo.write(180); // Move servo to indicate system is locked
  }

  // Detect button B press
  if (currentBState == HIGH && lastBState == LOW && immoStatus) {
    immoStatus = false;
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    myservo.write(0); // Move servo to indicate system is not locked
  }

  // Update the last button states for next loop
  lastAState = currentAState;
  lastBState = currentBState;

  delay(20); // Debounce delay for start button
}

void soundBuzzer() {
  ledcWriteTone(buzzerChannel, 200);
  ledcWrite(buzzerChannel, 204);
  delay(100);
  ledcWrite(buzzerChannel, 0); 
  delay(25);
  ledcWrite(buzzerChannel, 204);
  delay(200); 
  ledcWrite(buzzerChannel, 0); 
  
}

void alarmBuzzer() {
  ledcWriteTone(buzzerChannel, 500);
  while (digitalRead(readB) == LOW) {  // Continue as long as the B button is not pressed
    ledcWrite(buzzerChannel, 204);
    delay(300); 
    ledcWrite(buzzerChannel, 0);  
    delay(300); 
  }
  // Automatically stops when the B button is pressed (back to main loop)
}

void playCorrectCodeSound() {
  int toneDuration = 100; 
  for (int freq = 2000; freq <= 3000; freq += 250) { // Create a rising tone sequence
    ledcWriteTone(buzzerChannel, freq); // Play tone at increasing frequencies
    delay(toneDuration);
  }
  ledcWrite(buzzerChannel, 0);
}
