#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

enum ErrorType {
  UNDERVOLTAGE,
  OVERVOLTAGE,
  NONE
};

// PINS
#define LED_UNDER_VOLTAGE  2
#define LED_OVER_VOLTAGE  3
#define BUTTON_EMERGENCY 18

// LDR Characteristics
const float GAMMA = 0.7;
const float RL10 = 50;

volatile int lux = 0;
volatile float sys_voltage = 0;

volatile bool isRunning = true;

void printOnLcd();
void printOnSerial(ErrorType);
void switchLedState(ErrorType);
void emergencyButtonPressed();

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(LED_UNDER_VOLTAGE, OUTPUT);
  pinMode(LED_OVER_VOLTAGE, OUTPUT);
  pinMode(BUTTON_EMERGENCY, INPUT);
  digitalWrite(LED_UNDER_VOLTAGE, HIGH);
  digitalWrite(LED_OVER_VOLTAGE, HIGH);

  attachInterrupt(digitalPinToInterrupt(BUTTON_EMERGENCY), emergencyButtonPressed, FALLING);

  TCCR1A = 0;           // Init Timer1A
  TCCR1B = 0;           // Init Timer1B
  TCCR1B |= B00000011;  // Prescaler = 64
  TCNT1 = 15535;        // Timer Preloading
  TIMSK1 |= B00000001;  // Enable Timer Overflow Interrupt

  TCCR3A = 0;           // Init Timer3A
  TCCR3B = 0;           // Init Timer3B
  TCCR3B |= B00000100;  // Prescaler = 256
  TCNT3 = 43660;        // Timer Preloading
  TIMSK3 |= B00000001;  // Enable Timer Overflow Interrupt
}

void loop() {
  if (isRunning) {
    checkSysVoltage();
    printOnLcd();
  } else {
    Serial.println("Board in waiting state - please press the emergency button");
    delay(500);
  }
}

void emergencyButtonPressed() {
  isRunning = !isRunning;
}

void checkSysVoltage() {
  if (sys_voltage < 1.8) {
    switchLedState(ErrorType::UNDERVOLTAGE);
    //printOnSerial(ErrorType::UNDERVOLTAGE);
  } else if (sys_voltage > 2.7) {
    switchLedState(ErrorType::OVERVOLTAGE);
    //printOnSerial(ErrorType::OVERVOLTAGE);
  } else {
    switchLedState(ErrorType::NONE);
    //printOnSerial(ErrorType::NONE);
  }
}

void printOnSerial(ErrorType error) {
  switch (error) {
    case ErrorType::UNDERVOLTAGE:
      Serial.println("Under voltage");
      break;
    case ErrorType::OVERVOLTAGE:
      Serial.println("Over voltage");
      break;
    case ErrorType::NONE:
      Serial.println("Normal voltage");
      break;
  }
}

void switchLedState(ErrorType error) {
  switch (error) {
    case ErrorType::UNDERVOLTAGE:
      digitalWrite(LED_UNDER_VOLTAGE, LOW);
      digitalWrite(LED_OVER_VOLTAGE, HIGH);
      break;
    case ErrorType::OVERVOLTAGE:
      digitalWrite(LED_OVER_VOLTAGE, LOW);
      digitalWrite(LED_UNDER_VOLTAGE, HIGH);
      break;
    case ErrorType::NONE:
      digitalWrite(LED_UNDER_VOLTAGE, HIGH);
      digitalWrite(LED_OVER_VOLTAGE, HIGH);
      break;
  }
}

void printOnLcd() {
  lcd.setCursor(0, 0);
  lcd.print("Condition: ");

  lcd.setCursor(2, 1);
  lcd.print("");
  if (lux >= 0.0 && lux <= 0.1) {
    lcd.print("Full moon!");
  } else if (lux > 0.1 && lux <= 1) {
    lcd.print("Deep twilight!");
  } else if (lux > 1 && lux <= 10) {
    lcd.print("Twilight!");
  } else if (lux > 10 && lux <= 50) {
    lcd.print("Computer monitor!");
  } else if (lux > 50 && lux <= 100) {
    lcd.print("Stairway lighting!");
  } else if (lux > 100 && lux <= 400) {
    lcd.print("Office lighting!");
  } else if (lux > 400 && lux <= 1000) {
    lcd.print("Overcast day!");
  } else if (lux > 1000 && lux <= 10000) {
    lcd.print("Full daylight!");
  } else if (lux > 10000 && lux <= 100000) {
    lcd.print("Direct sunlight!");
  } else {
    lcd.print("Unknown condition");
  }
}


ISR(TIMER1_OVF_vect) {
  TCNT1 = 15535;
  float analogValue = analogRead(A0);
  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
}


ISR(TIMER3_OVF_vect) {
  TCNT3 = 43660;
  float systemRead = analogRead(A1);
  sys_voltage = systemRead * (5.0 / 1023.0);
}
