// The Suzuki Vitara has a crap dash. It's low contraast and hard to read in bright sun at certain angles. 
// There are a number of displays for the fuel miser and even a blank one but no digital speed readout.
// This connects a basic GPS sensor to a Arduino Uno and drives a I2C LCD display. I use TinyGPSPlus to 
// parse the RMC message and display it on the LCD. I'm using a 16 x 2 line and deriving a direction too.
// When I'm happy with it I may look at a better display, either an 8 x 1 or perhaps a 128 x 64 pixel OLED 

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
/*
   This sample code demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 10, TXPin = 9;
static const uint32_t GPSBaud = 9600;        // my GPS is outputting at 9600 baud

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);              // I'm sure I could use the other serial pins. later perhaps...

// initialize the library by associating any needed LCD interface pins with the arduino pin number it is connected to
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  ss.begin(GPSBaud);
  lcd.init();                     // initialize the lcd 
  lcd.backlight();
  lcd.begin(16, 2);               // set up the LCD's number of columns and rows:
  lcd.display();

  lcd.print("Hello");
}

void loop()
{
  char _buffer[10];            // for the speed
  char dir[3];                  // direction
  float course = gps.course.deg();

  if ((course >= 337.5) && (course < 22.5)) strcpy(dir, "N");
  if ((course >= 22.5) && (course < 67.5)) strcpy(dir, "NE");
  if ((course >= 67.5) && (course < 112.5)) strcpy(dir, "E");
  if ((course >= 112.5) && (course < 157.5)) strcpy(dir, "SE");
  if ((course >= 157.5) && (course < 202.5)) strcpy(dir, "S");
  if ((course >= 202.5) && (course < 247.5)) strcpy(dir, "SW");
  if ((course >= 247.5) && (course < 292.5)) strcpy(dir, "W");
  if ((course >= 292.5) && (course < 337.5)) strcpy(dir, "NW");
  if (gps.speed.kmph() < 0.5) strcpy(dir, "--");

  dtostrf(gps.speed.kmph(),3, 0, _buffer);      // convert float to a string -> _buffer
  sprintf(_buffer, "%s kph %s", _buffer, dir);  // create the string for the lcd & add direction

  displayOnLCD(0, _buffer, 1000);

  if (millis() > 5000 && gps.charsProcessed() < 10) displayOnLCD(0, "No GPS found", 1000);
}



// This custom version of delay() ensures that the gps object is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}



void displayOnLCD(int row, String charArray, int dDelay)
{
  lcd.clear();
  lcd.setCursor(0, row); 
  lcd.print(charArray);
  smartDelay(dDelay);
}


void displayOnLCD(String charArray, String char_Array, int dDelay)
{
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print(charArray);
  lcd.setCursor(0, 1); 
  lcd.print(char_Array);
  smartDelay(dDelay);
}

