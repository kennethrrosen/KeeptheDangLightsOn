/*
  KEEP THE DANG LIGHTS ON
  by: Questionable Mechanics
  contact: questionable.mechanics@gmail.com
  last rev: 10/2020
  This sketch is in the public domain

  Note: Developed and tested using Arduino IDE ver 1.8.42.0

*/

#include <Wire.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Bounce2.h>
#include <DS1307RTC.h>

Servo servoLeft;
Servo servoRight;
Bounce debouncer = Bounce();
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const int servoLeftPin = 6; //11
const int servoRightPin = 5; //10
const int switchPin = 2; 
const int DS1307 = 0x68;

int state = 0;
/*
byte verticalLine[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};
byte char2[8] = {
  B00000,
  B00000,
  B00000,
  B11100,
  B00100,
  B00100,
  B00100,
  B00100
};
byte char1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00111,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};
byte char3[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte char4[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
*/
byte DownArrow[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100
};

byte UpArrow[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte Check[8] = {
  B00000,
  B00001,
  B00011,
  B10110,
  B11100,
  B01000,
  B00000,
  B00000
};

byte Skull[8] = {
  B00000,
  B01110,
  B10101,
  B11011,
  B01110,
  B01110,
  B00000,
  B00000
};
/*
void printFrame()
{
  lcd.setCursor(1, 0);
  lcd.print("------------------");
  lcd.setCursor(1, 3);
  lcd.print("------------------");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(0, 2);
  lcd.write(byte(0));
  lcd.setCursor(19, 1);
  lcd.write(byte(0));
  lcd.setCursor(19, 2);
  lcd.write(byte(0));
  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.setCursor(19, 0);
  lcd.write(byte(2));
  lcd.setCursor(0, 3);
  lcd.write(byte(3));
  lcd.setCursor(19, 3);
  lcd.write(byte(4));
}
*/
void createCustomCharacters()
{
  //lcd.createChar(0, verticalLine);
  //lcd.createChar(1, char1);
  //lcd.createChar(2, char2);
  //lcd.createChar(3, char3);
  //lcd.createChar(4, char4);
  lcd.createChar(5, UpArrow);
  lcd.createChar(6, DownArrow);
  //lcd.createChar(20, Skull);
  //lcd.createChar(10, Check);
}

void setup()
{
  debouncer.attach(switchPin);
  debouncer.interval(2);
  lcd.begin(20, 4);
  Wire.begin();
  Serial.begin(9600);
  setSyncProvider(RTC.get);
  pinMode(switchPin, INPUT_PULLUP);
  servoLeft.attach(servoLeftPin);
  servoRight.attach(servoRightPin);
  servoLeft.write(90);
  servoRight.write(90);
  createCustomCharacters();

}

void loop()
{
  static bool hasRun = false;
  static int outtageQty = 0;

  tmElements_t tm;
  debouncer.update();
  //printFrame();
  (RTC.read(tm));

  printSwitchState(1, 1);
  lcd.setCursor(7, 1);
  lcd.print("Out:");
  lcd.print(outtageQty);
  printDate(14, 1, tm);
  printTime(14, 2, tm);

  digitalRead(switchPin);

  if ((hasRun == false) && (outtageQty < 11))
  {
    if (digitalRead(switchPin) == HIGH) { //LOW
      lcd.setCursor(1, 2);
      lcd.print("Power Loss!");
      delay(500);
      servoLeft.write(180);
      servoRight.write(0);
      delay(500);
      servoLeft.write(90);
      servoRight.write(90);
      delay(500);
      hasRun == true;
      outtageQty++;
    }
    while (outtageQty >= 11) {
      servoLeft.write(90);
      servoRight.write(90);
      lcd.setCursor(1, 2);
      lcd.clear(); //BEGIN NEW LINE OF CLEARING THE HANGING EXCLAIMATION POINT
      printSwitchState(1, 1);
      lcd.setCursor(7, 1);
      lcd.print("Out:");
      lcd.print(outtageQty);
      printDate(14, 1, tm);
      printTime(14, 2, tm);
      lcd.setCursor(1, 2); //END NEW LINE OF CLEARING THE HANGING EXCLAIMATION POINT
      lcd.print("Please Reset");
      delay(1500);
    }
  }
  if (digitalRead(switchPin) == LOW) { //HIGH
    lcd.setCursor(1, 2);
    lcd.print("Monitoring");
    servoLeft.write(90);
    servoRight.write(90);
    delay(1500);
  }
}

void printTime(int character, int line, tmElements_t tm)
{
  String minutes;
  lcd.setCursor(character, line);
  lcd.print(tm.Hour);
  lcd.print(":");
  if (tm.Minute < 10)
  {
    minutes = "0" + String(tm.Minute);
    lcd.print(minutes);
  } else
  {
    lcd.print(tm.Minute);
  }
}

void printDate(int character, int line, tmElements_t tm)
{
  lcd.setCursor(character, line);
  lcd.print(tm.Month);
  lcd.print("/");
  lcd.print(tm.Day);
}

void printSwitchState(int character, int line)
{
  lcd.setCursor(character, line);
  lcd.print("SW:");
  digitalRead(switchPin);
  if (digitalRead(switchPin) == LOW) {
    lcd.write(byte(5));
  } else
  { lcd.write(byte(6));
  }
}