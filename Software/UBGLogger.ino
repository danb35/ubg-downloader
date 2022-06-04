/*
 * Data Logger
 * Reads data from EI UBG-16, reformats date, and saves to microSD Card
 * Uses Serial read line code from Nick Gammon, see
 * http://gammon.com.au/serial
 *
 * Copyright 2012 Daniel C. Brown
 * Released under the GNU General Public License, v3
 * See the file COPYING for license agreement or
 * http://www.gnu.org/licenses/gpl.html
 *
 */

#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <SdFat.h>
#include <LiquidCrystal_I2C.h>

const unsigned int MAX_INPUT = 160;
const int SdChipSelect = 0;
unsigned long muxTime = 0;
unsigned long timeSkew = 0;
unsigned long dataTime = 0;
byte ledPin = 11;
byte i;
boolean ready = false; 
char logFile[13];
char dataTimeString[20];
int line = 0;
time_t t;
int lastMinute;

HardwareSerial Uart = HardwareSerial();
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
SdFat sd;
SdFile myFile;

void setup()
{
  Serial.begin(38400);
  Uart.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  setSyncProvider(RTC.get); // the function to get the time from the RTC
  pinMode(ledPin, OUTPUT);
  delay(1000);
  if (!sd.init(SPI_FULL_SPEED, SdChipSelect))
  {
    lcd.print("SD Init failed!");
  }
  else
  {
    lcd.setCursor(1,0);
    lcd.print("Begin download");
    t = now();
    sprintf(dataTimeString,"%.2d/%.2d/%.4d %.2d:%.2d",
    month(t), day(t), year(t), hour(t), minute(t)); 
    lcd.setCursor(0,1);
    lcd.print(dataTimeString);
    lastMinute = minute(t);
  }
}


void process_data (char *data)
{
  // Test for content - READY, OBC, Down, or other
  if ((strncmp(data, "OBC", 3) == 0) && (ready == true)) { // Process OBC and set timeSkew
    char muxTimeString [12];
    if (data[3] == ',') { // Read the timestamp from the MUX
      i = 4;
      do {
        muxTimeString [i - 4] = data [i];
        muxTimeString [i - 3] = 0;
        i++;
      } 
      while ((data [i] != 0) && (i < 15));
    }
    muxTime = atol(muxTimeString);
    timeSkew = now() - muxTime; // Compute difference between MUX timestamp and actual time
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Downloading data");
    lcd.setCursor(0,1);
    lcd.print("Line");  
  } 
  else if ((strncmp(data, "READY", 5) == 0) && (ready == false)) { // Create logfile, annunciate Ready
    if (createLogFile())
    {
      digitalWrite(ledPin, HIGH);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Ready to download");
      line = 0;
      ready = true;
    }
  }
  else if ((strncmp(data, "READY", 5) == 0) && (ready == true)) { // Ignore the line
  }
  else if ((strncmp(data, "Down", 4) == 0) && (ready == true)) { // Download complete - close file
    if (data[10] == 'C') {
      myFile.print("Download Completed ");
    }
    else if (data[10] == 'I') {
      myFile.print("Download Interrupted ");
    }
    else {
      myFile.print(data);
      myFile.print(" ");
    }
    t = now();
    sprintf(dataTimeString,"%.2d/%.2d/%.4d %.2d:%.2d:%.2d",
    month(t), day(t), year(t), hour(t), minute(t), second(t));
    myFile.println(dataTimeString);
    myFile.timestamp(T_WRITE, year(t), month(t), day(t), hour(t), minute(t), second(t));
    myFile.close();
    digitalWrite(ledPin, LOW);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Download");
    lcd.setCursor(4,1);
    lcd.print("Complete");
    ready = false;
  }
  else if (ready == true) // process and log data
  { 
    i = 0;
    while (data [i] != ',') // first field is the time stamp
    {
      dataTimeString[i] = data[i];
      i++;
    }
    dataTimeString[i] = 0;
    dataTime = atol(dataTimeString) + timeSkew; // Adjust dataTime for time skew
    time_t t = dataTime;
    sprintf(dataTimeString,"%.2d/%.2d/%.4d %.2d:%.2d:%.2d",
    month(t), day(t), year(t), hour(t), minute(t), second(t));
    myFile.print(dataTimeString);
    myFile.println((char *)&data[i]); // print out the rest of the input line, from the first comma on
    line++;
    lcd.setCursor(5,1);
    lcd.print(line);
  } 
}  // end of process_data

boolean createLogFile()

/* Creates /logs/ directory, if not already present, and log file
 * file name is yymmddnn.csv
 * yy = 2-digit year
 * mm = 2-digit month
 * dd = 2-digit day
 * nn = first unused number from 00-99
 * Requires system clock to be set to year >= 2000
 */

{
  if (!sd.exists("/logs/"))
  {
    if (!sd.mkdir("/logs"))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Couldn't mkdir!");
      return false;
    }
  } // create logs directory, if not already present
  if  (!sd.chdir("/logs"))
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Couldn't chdir!");
    return false;
  }
  time_t t = now();
  int yr = year(t) - 2000;
  sprintf(logFile,"%.2d%.2d%.2d00.csv",
  yr, month(t), day(t));
  for (i = 0; i < 100; i++)
  {
    logFile[6] = i/10 + '0';
    logFile[7] = i%10 + '0';
    if (sd.exists(logFile))
    {
      continue;
    }
    if(myFile.open(logFile,O_RDWR | O_CREAT | O_AT_END)) {
      t = now();
      myFile.timestamp(T_CREATE, year(t), month(t), day(t), hour(t), minute(t), second(t));
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Downloading to:");
      lcd.setCursor(0,1);
      lcd.print(logFile);
      delay(300);
      return true;
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print(logFile);
      lcd.setCursor(0,1);
      lcd.print("Not Created!");
      return false;
    }
    break;
  }
}

void loop()
{
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  if (ready == false)
  {
    t = now();
    if (minute(t) != lastMinute)
    {
      sprintf(dataTimeString,"%.2d/%.2d/%.4d %.2d:%.2d",
      month(t), day(t), year(t), hour(t), minute(t)); 
      lcd.setCursor(0,1);
      lcd.print(dataTimeString);
      lastMinute = minute(t);
    }
  }

  if (Uart.available () > 0) 
  {
    char inByte = Uart.read ();
    switch (inByte)
    {
    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte
      // terminator reached! process input_line here ...
      process_data (input_line);
      // reset buffer for next time
      input_pos = 0;  
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;

    }  // end of switch

  }  // end of incoming data

}  // end of loop



