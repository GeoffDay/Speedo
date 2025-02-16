// The Suzuki Vitara has a crap dash. It's low contraast and hard to read in bright sun at certain angles. 
// There are a number of displays for the fuel miser and even a blank one but no digital speed readout.
// This connects a basic GPS sensor to a Arduino Uno and drives a I2C 128 x 64 pixel OLED display. 
// I use TinyGPSPlus to parse the RMC message and display it. 


// // initialize the library by associating any needed LCD interface pin
// // with the arduino pin number it is connected to
// #include <LiquidCrystal.h>        // include the library code:
// const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


/* Arduino UNO with SSD1306 128x64 I2C OLED display
 * Demonstrates graphics and text elements. Tony Goodhew  26 May 2020  */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin  
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Initialise variables
int cw = SSD1306_WHITE;
int cb = SSD1306_BLACK;

int x = 20;
int y = 10;


#include <SoftwareSerial.h>
static const int RXPin = 10, TXPin = 9;
static const uint32_t GPSBaud = 9600;         // my GPS is outputting at 9600 baud

#include <TinyGPSPlus.h>
TinyGPSPlus gps;                              // The TinyGPSPlus object

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);              // I'm sure I could use the other serial pins. later perhaps...
char _buffer[82];            // for the speed
char dir[3] = "..";          // direction
float speed = 0.0;
int oldSpeed = 100;

int ledPin = 13;      // select the pin for the LED

void setup()
{  
  pinMode(ledPin, OUTPUT);    // declare the ledPin as an OUTPUT:
  ss.begin(GPSBaud);
  delay(1000);

  digitalWrite(ledPin, LOW); // turn the ledPin off to show error

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {  // Old Address 0x3D for 128x64
    digitalWrite(ledPin, HIGH); // turn the ledPin on to show error
  } 

  display.display();
  delay(1000); // Pause for 2 seconds

  display.clearDisplay();
  display.setTextSize(3);

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
  sprintf(_buffer, "%s %s ", _buffer, dir);  // create the string for the lcd & add direction

  int intSpeed = (int)speed * 10;

  if (intSpeed != oldSpeed) {
    display.fillRect(x,y, 127, 63, cb); // Overwrite with black rectangle
    display.display();
    display.setCursor(x, y);
    display.setTextColor(cw);
    display.println(_buffer);
    display.display();
  } 
  
  oldSpeed = intSpeed;       // get a copy of the current speed

  smartDelay(10);

  // if (millis() > 5000 && gps.charsProcessed() < 10) {
  //   display.fillRect(x,y, 127, 63, cb); // Overwrite with black rectangle
  //   display.display();
  //   display.setCursor(x, y);
  //   display.setTextColor(cw);
  //   display.println("no GPS");
  //   display.display();
  // } 
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


