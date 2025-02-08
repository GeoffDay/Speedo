// The Suzuki Vitara has a crap dash. It's low contraast and hard to read in bright sun at certain angles. 
// There are a number of displays for the fuel miser and even a blank one but no digital speed readout.
// This connects a basic GPS sensor to a Arduino Uno and drives a I2C LCD display. I use TinyGPSPlus to 
// parse the RMC message and display it on the LCD. I'm using a 16 x 2 line and deriving a direction too.
// When I'm happy with it I may look at a better display, either an 8 x 1 or perhaps a 128 x 64 pixel OLED 
/*
   This sample code demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
//
//#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>        // include the library code:

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/* Arduino UNO with SSD1306 128x64 I2C OLED display
 * Demonstrates graphics and text elements. Tony Goodhew  26 May 2020  */
#include <SPI.h>           // Not needed with I2C
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void show() { // Often used sequence - Function to simplify code
  display.display(); delay(200); display.fillScreen(SSD1306_BLACK);
}

static const int RXPin = 10, TXPin = 9;
static const uint32_t GPSBaud = 9600;        // my GPS is outputting at 9600 baud

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);              // I'm sure I could use the other serial pins. later perhaps...
char _buffer[82];            // for the speed
char dir[3] = "..";          // direction
float speed = 0.0;
float oldSpeed = 0.0;
  // Initialise variables
  int cw = SSD1306_WHITE;
  int cb = SSD1306_BLACK;

  int x = 20;
  int y = 10;

void setup()
{
  ss.begin(GPSBaud);

  lcd.begin(8, 2);               // set up the 4 wire LCD's number of columns and rows:
  lcd.display();

  lcd.print("Hello");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Old Address 0x3D for 128x64
    lcd.print(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  

  display.clearDisplay();
  display.setTextSize(6);

  smartDelay(100);
}

void loop()
{
  float course = gps.course.deg();
  speed = gps.speed.kmph();

  if ((course >= 337.5) && (course < 22.5)) strcpy(dir, "N ");
  if ((course >= 22.5) && (course < 67.5)) strcpy(dir, "NE");
  if ((course >= 67.5) && (course < 112.5)) strcpy(dir, "E ");
  if ((course >= 112.5) && (course < 157.5)) strcpy(dir, "SE");
  if ((course >= 157.5) && (course < 202.5)) strcpy(dir, "S ");
  if ((course >= 202.5) && (course < 247.5)) strcpy(dir, "SW");
  if ((course >= 247.5) && (course < 292.5)) strcpy(dir, "W ");
  if ((course >= 292.5) && (course < 337.5)) strcpy(dir, "NW");
  if (speed < 0.5) strcpy(dir, "--");

  dtostrf(speed,3, 0, _buffer);      // convert float to a string -> _buffer

  if (speed != oldSpeed) {
    display.fillRect(x,y, 127, 63, cb); // Overwrite with black rectangle
    display.display();
    display.setCursor(x, y);
    display.setTextColor(cw);
    display.println(_buffer);
    display.display();
  } 
  
  sprintf(_buffer, "%s kph ", _buffer);  // create the string for the lcd & add direction

  displayOnLCD(_buffer, dir, 100);




  oldSpeed = speed;       // get a copy of the current speed

  smartDelay(10);

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
 // lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print(charArray);
  lcd.setCursor(0, 1); 
  lcd.print(char_Array);
  smartDelay(dDelay);
}
