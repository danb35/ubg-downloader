# Software
This device uses a [Teensy](https://www.pjrc.com/teensy/index.html) microcontroller, an Arduino-compatible device in a smaller physical package.  To use the software, you'll need to compile and flash it to the device, and to do that, you'll need some software:

* The latest version of the [Arduino development environment](https://www.arduino.cc/en/software)
* [Teensyduino](https://www.pjrc.com/teensy/td_download.html), a software add-on for the Arduino software that allows it to compile for the Teensy board.
* The [Teensy Loader](https://www.pjrc.com/teensy/loader.html), the program that actually loads compiled code onto the Teensy board.

With these packages installed, you’ll be able to compile software and load it onto the Teensy board.  You’ll then need to install a couple of libraries to compile and use my code.  Specifically, you’ll need:

* The Time library, which has functions for keeping time, communicating with a Real-Time Clock, and doing math with dates and times

* The SdFat library, which allows the program to create files on SD cards and save data there.

* The [LiquidCrystal_I2C library](http://www.dfrobot.com/image/data/DFR0154/LiquidCrystal_I2Cv1-1.rar), which provides support for the serial LCD.  Please note, there are at least two different libraries with this name.  If you use the LCD module I used, you must use this library.

The first two can be installed directly through the Arduino IDE, but the third must be downloaded and comes as a .zip file.  Unzip it and move the contents into the Arduino/Libraries directory under your home directory, then restart the Arduino software.

The current version of the source code is available in the Software directory.  It appears to work on my prototype hardware, but has not been extensively tested.

# Limitations
The software's in a fairly rough state--I developed it until it did what I needed it to do, then stopped.  Probably the biggest limitation is that there's simply no way, using my software, to set the real-time clock; you'll need to use a different sketch to do that.  Once you've set the clock, it's persistent, and the clock IC used on the PCB holds its accuracy quite well.  But that feature really should have been coded into the software, perhaps using the SW1 and SW2 switches the PCB provides for.
