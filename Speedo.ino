// The Suzuki Vitara has a crap dash. The analog speedometer is low contrast and hard to read in 
// bright sun at certain angles when competing with reflections. There are a number of displays for the
// fuel miser and G forces for the wanker, even a blank one but no digital speed readout. Why???
// This connects a basic GPS sensor to an Arduino Uno and drives an LCD display. I use TinyGPSPlus to 
// parse the RMC message and display it on the LCD. I'm using a 16 x 1 line and deriving a direction too.
// When I'm happy with it I may look at a better display, either an 8 x 1 or perhaps a 128 x 64 pixel OLED 

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <LiquidCrystal.h>        // include the library code:

// initialize the library by associating any needed LCD interface pin with the arduino pin number it is connected to
const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

static const int RXPin = 10, TXPin = 9;       // The serial connection to the GPS device
static const uint32_t GPSBaud = 9600;         // my GPS is outputting at 9600 baud

SoftwareSerial ss(RXPin, TXPin);              // I'm sure I could use the other serial pins. later perhaps...

TinyGPSPlus gps;                              // The TinyGPSPlus object

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
 // lcd.init();                     // initialize the lcd 
 // lcd.backlight();
  lcd.begin(8, 2);        // set up the LCD's number of columns and rows: 16 x 1 is 
  lcd.display();          // internally 8 x 2 so we have to split the display in two

  lcd.print("Hello");
  smartDelay(1000);
}

void loop()
{
  char _buffer[82];            // for the speed
  char dir[3] = "--";                  // direction
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
  sprintf(_buffer, "%s kph ", _buffer);         // create the string for the lcd & add direction

  Serial.println(gps.altitude.meters());
  displayOnLCD(_buffer, dir, 100);

  if (millis() > 5000 && gps.charsProcessed() < 10) displayOnLCD(" No GPS", "Found! ", 1000);
  
  smartDelay(10);              // needs this to read all the data properly!
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



void displayOnLCD(String charArray, int dDelay)
{
 // lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print(charArray);
  smartDelay(dDelay);
}


void displayOnLCD(String charArray, String char_Array, int dDelay)
{
 // lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print(charArray);
  lcd.setCursor(0, 1); 
  lcd.print(char_Array);
  delay(dDelay);
}
