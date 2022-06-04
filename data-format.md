# EI MUX-8A Data Format
 
The [documentation](https://www.iflyei.com/wp-content/uploads/MUX-8A-OIII.pdf) that comes with the MUX-8A data recorder explains quite a bit about the format of the data it downloads to your computer.  However, it leaves a few things unexplained.

First, the connection to your computer is a “dumb” serial connection.  Data only flows one way, from the MUX-8A to your computer, and there’s no way for your computer to control what’s coming or when--the computer just takes, processes, and saves the data dump.

The data dump is initiated using a control switch on your instrument panel.  That switch has three positions: BURST, ON, and DOWNLOAD.  The DOWNLOAD position is momentary--when you release the switch, it returns to the ON position.

When you press (and hold) the switch in the DOWNLOAD position, the MUX-8A sends the word “READY” to your computer.  The software (included with the device) recognizes that, and informs you that it’s ready to download.  You then release the switch, and the data dump begins.

The data is plain ASCII text in CSV format, sent at 9600 bps, 8 data bits, no parity, 1 stop bit (8N1).  I won’t repeat what’s in the manual, but I will describe the difference between the final output of their software and the “raw” download from the device.

This is what the raw download looks like:

```
OBC,157780557
151612429,OFF,1319,0238,0073,0357,1283,0323,1328,0315,0208,0074,0089,0074,0011,0074,0141,0074,GPS,N342065,W0804770,277,132,BUBBA
151612488,OFF,1405,0232,0073,0358,1371,0325,1413,0314,0203,0073,0090,0073,0011,0073,0141,0073,GPS,N342063,W0805043,276,140,BUBBA
151612547,OFF,1410,0236,0074,0362,1376,0333,1418,0321,0199,0074,0089,0073,0009,0073,0142,0073,GPS,N342060,W0805322,277,141,BUBBA
151612606,OFF,1414,0238,0073,0366,1377,0336,1415,0325,0197,0073,0091,0073,0010,0073,0141,0073,GPS,N342058,W0805600,277,139,BUBBA
151612665,OFF,1414,0239,0073,0367,1382,0339,1418,0327,0195,0073,0092,0073,0011,0073,0141,0072,GPS,N342055,W0805878,277,139,BUBBA
[lots snipped]
151987363,OFF,1241,0255,1208,0386,1217,0348,1228,0339,0214,0084,0134,0083,0009,0084,0141,0083,GPS,N372947,W0853511,136,132,BUBBA
Down Load Complete
```

This is identical to the finished download, with a few exceptions:

* The first line, starting with OBC, doesn’t make it into the downloaded file.
* In place of the date/time indications at the beginning of each line, there’s a single large number.
* The downloaded file has the date and time immediately after the “Down Load Complete” indication.

Also note that the downloaded data contains only whole numbers, even if the actual value is fractional.  For example, the 17th field in the data above is the master bus voltage, which is actually 14.1 or 14.2 volts, but is represented as 0141/0142--i.e., it's multiplied by 10.  You'll see something similar with the GPS data.

So here’s the deal: the number in the “OBC” line, and the numbers beginning the rest of the rows, are timestamps.  The EI software converts those, mathematically, to the actual date and time of each of the data sets.  This has to be done in software because the MUX-8A has no idea what the date and time actually are.

So what do the numbers mean?  They’re simply a count of seconds.  My speculation is that it’s the number of seconds since the MUX-8A was first powered on (probably for testing at EI), but EI won’t give further detail on that subject.

The OBC line contains the timestamp for “right now”--the time you begin the download.  The rest of the lines have the timestamp for when they were actually recorded.

Given that information, converting the MUX-8A timestamps to human-readable date and time is fairly straightforward.  UNIX and C treat dates and times as the number of seconds since midnight UTC on 1 Jan 1970.  To convert the MUX-8A values, then, just

* Subtract the number in the OBC line from the current time value (right now, it’s 1343170328).  Call the difference timeSkew.
* Add timeSkew to the timestamps on each line of the downloaded data.
* Using existing time functions, convert that sum back to human-readable date and time.

I hope this information is helpful.
