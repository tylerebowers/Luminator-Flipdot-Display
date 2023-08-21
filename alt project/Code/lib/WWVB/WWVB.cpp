/*
  WWVB.h - WWVB C++ library - implementation
  Shoehorning code from Capt. Tagon and Vin Marshall into a C++ Libarary
  So it is more portable.
  Copyright (c) 2016 Brian Varela.  All right reserved.
*/

// include core Wiring API
//#include "WProgram.h"

// include this library's description file
#include "WWVB.h"

// include description files for other libraries used (if any)
#include "HardwareSerial.h"

#include "stdio.h"

#include "Arduino.h"

//Initializations
uint8_t wwvbInPin = 2;
bool debug = true;

// Month abbreviations
char *months[12] = {
"Jan",
"Feb",
"Mar",
"Apr",
"May",
"Jun",
"Jul",
"Aug",
"Sep",
"Oct",
"Nov",
"Dec"
};

// Day of Year to month translation (thanks to Capt.Tagon)
// End of Month - to calculate Month and Day from Day of Year
uint8_t eomYear[14][2] = {
  {0,0},      // Begin
  {31,31},    // Jan
  {59,60},    // Feb
  {90,91},    // Mar
  {120,121},  // Apr
  {151,152},  // May
  {181,182},  // Jun
  {212,213},  // Jul
  {243,244},  // Aug
  {273,274},  // Sep
  {304,305},  // Oct
  {334,335},  // Nov
  {365,366},  // Dec
  {366,367}   // overflow
};


//Globals

// Timing and error recording
unsigned long pulseStartTime = 0;
unsigned long pulseEndTime = 0;
unsigned long frameEndTime = 0;
bool wasMark = false;
uint8_t framePosition = 0;
uint8_t bitPosition = 1;
char lastBit = ' ';
uint8_t errors[10] = { 1,1,1,1,1,1,1,1,1,1 };
uint8_t errIdx = 0;
uint8_t bitError = 0;
bool frameError = false;

// WWVB time variables
uint8_t wwvb_hour = 0;
uint8_t wwvb_minute = 0;
uint8_t wwvb_day = 0;
uint8_t wwvb_year = 0;

/* WWVB time format struct - acts as an overlay on wwvbRxBuffer to extract time/date data.
 * This points to a 64 bit buffer wwvbRxBuffer that the bits get inserted into as the
 * incoming data stream is received.  (Thanks to Capt.Tagon @ duinolab.blogspot.com)
 */
struct wwvbBuffer {
  unsigned long long U12       :4;  // no value, empty four bits only 60 of 64 bits used
  unsigned long long Frame     :2;  // framing
  unsigned long long Dst       :2;  // dst flags
  unsigned long long Leapsec   :1;  // leapsecond
  unsigned long long Leapyear  :1;  // leapyear
  unsigned long long U11       :1;  // no value
  unsigned long long YearOne   :4;  // year (5 -> 2005)
  unsigned long long U10       :1;  // no value
  unsigned long long YearTen   :4;  // year (5 -> 2005)
  unsigned long long U09       :1;  // no value
  unsigned long long OffVal    :4;  // offset value
  unsigned long long U08       :1;  // no value
  unsigned long long OffSign   :3;  // offset sign
  unsigned long long U07       :2;  // no value
  unsigned long long DayOne    :4;  // day ones
  unsigned long long U06       :1;  // no value
  unsigned long long DayTen    :4;  // day tens
  unsigned long long U05       :1;  // no value
  unsigned long long DayHun    :2;  // day hundreds
  unsigned long long U04       :3;  // no value
  unsigned long long HourOne   :4;  // hours ones
  unsigned long long U03       :1;  // no value
  unsigned long long HourTen   :2;  // hours tens
  unsigned long long U02       :3;  // no value
  unsigned long long MinOne    :4;  // minutes ones
  unsigned long long U01       :1;  // no value
  unsigned long long MinTen    :3;  // minutes tens
};

struct wwvbBuffer * wwvbFrame;
unsigned long long receiveBuffer;
unsigned long long lastFrameBuffer;

WWVB::WWVB(uint_t pin)
{
  wwvbInPin = pin;
}
void WWVB::begin()
{
  // Setup the WWVB Signal In Handling
  pinMode(wwvbInPin, INPUT);
  attachInterrupt(0, wwvbChange, CHANGE);

  // Setup the WWVB Buffer
  lastFrameBuffer = 0;
  receiveBuffer = 0;
  wwvbFrame = (struct wwvbBuffer *) &lastFrameBuffer;
}

void WWVB::debug(bool on)
{
  if(on)
  {
    debug = true;
  }
  else
  {
    debug = false;
  }
}

// String WWVB::datetimeString()
// {
//   String rtime = "No Time";
//   if(frameEndTime != 0)
//   {
//     char time[255];
//     uint8_t minTen = (uint8_t) wwvbFrame->MinTen;
//     uint8_t minOne = (uint8_t) wwvbFrame->MinOne;
//     uint8_t hourTen = (uint8_t) wwvbFrame->HourTen;
//     uint8_t hourOne = (uint8_t) wwvbFrame->HourOne;
//     uint8_t dayHun = (uint8_t) wwvbFrame->DayHun;
//     uint8_t dayTen = (uint8_t) wwvbFrame->DayTen;
//     uint8_t dayOne = (uint8_t) wwvbFrame->DayOne;
//     uint8_t yearOne = (uint8_t) wwvbFrame->YearOne;
//     uint8_t yearTen = (uint8_t) wwvbFrame->YearTen;
//
//     uint8_t wwvb_minute = (10 * minTen) + minOne;
//     uint8_t wwvb_hour = (10 * hourTen) + hourOne;
//     uint8_t wwvb_day = (100 * dayHun) + (10 * dayTen) + dayOne;
//     uint8_t wwvb_year = (10 * yearTen) + yearOne;
//
//     sprintf(time, "%0.2i:%0.2i  %0.3i  20%0.2i",
//             wwvb_hour, wwvb_minute, wwvb_day, wwvb_year);
//     rtime = time;
//
//   }
//
//   return rtime;
// }

// String WWVB::dateString()
// {
//   String rtime = "No Time";
//   if(frameEndTime != 0)
//   {
//     char time[255];
//     uint8_t dayHun = (uint8_t) wwvbFrame->DayHun;
//     uint8_t dayTen = (uint8_t) wwvbFrame->DayTen;
//     uint8_t dayOne = (uint8_t) wwvbFrame->DayOne;
//     uint8_t yearOne = (uint8_t) wwvbFrame->YearOne;
//     uint8_t yearTen = (uint8_t) wwvbFrame->YearTen;
//
//     uint8_t wwvb_day = (100 * dayHun) + (10 * dayTen) + dayOne;
//     uint8_t wwvb_year = (10 * yearTen) + yearOne;
//
//     sprintf(time, "%0.3i  20%0.2i",
//             wwvb_day, wwvb_year);
//     rtime = time;
//
//   }
//
//   return rtime;
// }

// String WWVB::timeString()
// {
//   String rtime = "No Time";
//   if(frameEndTime != 0)
//   {
//     char time[255];
//     uint8_t minTen = (uint8_t) wwvbFrame->MinTen;
//     uint8_t minOne = (uint8_t) wwvbFrame->MinOne;
//     uint8_t hourTen = (uint8_t) wwvbFrame->HourTen;
//     uint8_t hourOne = (uint8_t) wwvbFrame->HourOne;
//
//     uint8_t wwvb_minute = (10 * minTen) + minOne;
//     uint8_t wwvb_hour = (10 * hourTen) + hourOne;
//
//     sprintf(time, "%0.2i:%0.2i",
//             wwvb_hour, wwvb_minute);
//     rtime = time;
//
//   }
//
//   return rtime;
// }

uint8_t WWVB::getMinute()
{
  uint8_t min = -1;

  if(frameEndTime != 0)
  {
    uint8_t minTen = (uint8_t) wwvbFrame->MinTen;
    uint8_t minOne = (uint8_t) wwvbFrame->MinOne;

    uint8_t wwvb_minute = (10 * minTen) + minOne;
    min = (uint8_t)wwvb_minute;
  }

  return min;
}

uint8_t WWVB::getHour()
{
  uint8_t hour = -1;

  if(frameEndTime != 0)
  {
    uint8_t hourTen = (uint8_t) wwvbFrame->HourTen;
    uint8_t hourOne = (uint8_t) wwvbFrame->HourOne;

    uint8_t wwvb_hour = (10 * hourTen) + hourOne;
    hour = (uint8_t)wwvb_hour;
  }

  return hour;
}

uint8_t WWVB::getDay()
{
  uint8_t day = -1;

  if frameEndTime != 0)
  {
    uint8_t dayHun = (uint8_t) wwvbFrame->DayHun;
    uint8_t dayTen = (uint8_t) wwvbFrame->DayTen;
    uint8_t dayOne = (uint8_t) wwvbFrame->DayOne;

    uint8_t wwvb_day = (100 * dayHun) + (10 * dayTen) + dayOne;
    day = (uint8_t)wwvb_day;
  }

  return day;
}

uint8_t WWVB::getYear()
{
  uint8_t year = -1;

  if(frameEndTime != 0)
  {
    uint8_t yearOne = (uint8_t) wwvbFrame->YearOne;
    uint8_t yearTen = (uint8_t) wwvbFrame->YearTen;

    uint8_t wwvb_year = (10 * yearTen) + yearOne;
    year = (uint8_t)wwvb_year + 2000;
  }

  return year;
}

void WWVB::processBit() {

  // Clear the bitReceived flag, as we're processing that bit.
  bitReceived = false;

  // determine the width of the received pulse
  unsigned uint8_t pulseWidth = pulseEndTime - pulseStartTime;

  // Attempt to decode the pulse into an Unweighted bit (0),
  // a Weighted bit (1), or a Frame marker.

  // Pulses < 0.2 sec are an error in reception.
  if (pulseWidth < 100) {
  buffer(-2);
  bitError++;
  wasMark = false;

  // 0.2 sec pulses are an Unweighted bit (0)
  } else if (pulseWidth < 400) {
    buffer(0);
    wasMark = false;

  // 0.5 sec pulses are a Weighted bit (1)
  } else if (pulseWidth < 700) {
    buffer(1);
    wasMark = false;

  // 0.8 sec pulses are a Frame Marker
  } else if (pulseWidth < 900) {

    // Two Frame Position markers in a row indicate an
    // end/beginning of frame marker
    if (wasMark) {

	 // For the display update
	 lastBit = '*';
	 if (debug) { Serial.print("*"); }

	 // Verify that our position data jives with this being
	 // a Frame start/end marker
	 if ( (framePosition == 6) &&
	      (bitPosition == 60)  &&
              (bitError == 0)) {

           // Process a received frame
	   frameEndTime = pulseStartTime;
           lastFrameBuffer = receiveBuffer;
           logFrameError(false);

	   if (debug) {
             debugPrintFrame();
           }

	 } else {

           frameError = true;
	 }

	 // Reset the position counters
	 framePosition = 0;
	 bitPosition = 1;
	 wasMark = false;
         bitError = 0;
	 receiveBuffer = 0;

    // Otherwise, this was just a regular frame position marker
    } else {

	 buffer(-1);
	 wasMark = true;

    }

  // Pulses > 0.8 sec are an error in reception
  } else {
    buffer(-2);
    bitError++;
    wasMark = false;
  }

  // Reset everything if we have more than 60 bits in the frame.  This means
  // the frame markers went missing somewhere along the line
  if (frameError == true || bitPosition > 60) {

        // Debugging
        if (debug) { Serial.print("  Frame Error\n"); }

        // Reset all of the frame pointers and flags
        frameError = false;
        logFrameError(true);
        framePosition = 0;
        bitPosition = 1;
        wasMark = false;
        bitError = 0;
        receiveBuffer = 0;
  }

}


/*
 * logFrameError
 *
 * Log the error in the buffer that is a window on the past 10 frames.
 */

void WWVB::logFrameError(bool err) {

  // Add a 1 to log errors to the buffer
  uint8_t add = err?1:0;
  errors[errIdx] = add;

  // and move the buffer loop-around pointer
  if (++errIdx >= 10) {
    errIdx = 0;
  }
}


/*
 * sumFrameErrors
 *
 * Turn the errors in the frame error buffer into a number useful to display
 * the quality of reception of late in the status messages.  Sums the errors
 * in the frame error buffer.
 */

uint8_t WWVB::sumFrameErrors() {

  // Sum all of the values in our error buffer
  uint8_t i, rv;
  for (i=0; i< 10; i++) {
    rv += errors[i];
  }

  return rv;
}


/*
 * debugPrintFrame
 *
 * Print the decoded frame over the seriail port
 */

void WWVB::debugPrintFrame() {

  char time[255];
  uint8_t minTen = (uint8_t) wwvbFrame->MinTen;
  uint8_t minOne = (uint8_t) wwvbFrame->MinOne;
  uint8_t hourTen = (uint8_t) wwvbFrame->HourTen;
  uint8_t hourOne = (uint8_t) wwvbFrame->HourOne;
  uint8_t dayHun = (uint8_t) wwvbFrame->DayHun;
  uint8_t dayTen = (uint8_t) wwvbFrame->DayTen;
  uint8_t dayOne = (uint8_t) wwvbFrame->DayOne;
  uint8_t yearOne = (uint8_t) wwvbFrame->YearOne;
  uint8_t yearTen = (uint8_t) wwvbFrame->YearTen;

  uint8_t wwvb_minute = (10 * minTen) + minOne;
  uint8_t wwvb_hour = (10 * hourTen) + hourOne;
  uint8_t wwvb_day = (100 * dayHun) + (10 * dayTen) + dayOne;
  uint8_t wwvb_year = (10 * yearTen) + yearOne;

  sprintf(time, "\nFrame Decoded: %0.2i:%0.2i  %0.3i  20%0.2i\n",
          wwvb_hour, wwvb_minute, wwvb_day, wwvb_year);
  Serial.print(time);

}


/*
 * buffer
 *
 * Places the received bits in the receive buffer in the order they
 * were recived.  The first received bit goes in the highest order
 * bit of the receive buffer.
 */

void WWVB::buffer(uint8_t bit) {

  // Process our bits
  if (bit == 0) {
    lastBit = '0';
    if (debug) { Serial.print("0"); }

  } else if (bit == 1) {
    lastBit = '1';
    if (debug) { Serial.print("1"); }

  } else if (bit == -1) {
    lastBit = 'M';
    if (debug) { Serial.print("M"); }
    framePosition++;

  } else if (bit == -2) {
    lastBit = 'X';
    if (debug) { Serial.print("X"); }
  }

  // Push the bit into the buffer.  The 0s and 1s are the only
  // ones we care about.
  if (bit < 0) { bit = 0; }
  receiveBuffer = receiveBuffer | ( (unsigned long long) bit << (64 - bitPosition) );

  // And increment the counters that keep track of where we are
  // in the frame.
  bitPosition++;
}


/*
 * incrementWwvbMinute
 *
 * The frame On Time Marker occurs at the beginning of the frame.  This means
 * that the time in the frame is one minute old by the time it has been fully
 * received.  Adding one to the minute can be somewhat complicated, in as much
 * as it can roll over the successive hours, days, and years in just the right
 * circumstances.  This handles that.
 */

void WWVB::incrementWwvbMinute() {

  // Increment the Time and Date
  if (++(wwvbFrame->MinOne) == 10) {
	  wwvbFrame->MinOne = 0;
	  wwvbFrame->MinTen++;
  }

  if (wwvbFrame->MinTen == 6) {
	  wwvbFrame->MinTen = 0;
	  wwvbFrame->HourOne++;
  }

  if (wwvbFrame->HourOne == 10) {
	  wwvbFrame->HourOne = 0;
	  wwvbFrame->HourTen++;
  }

  if ( (wwvbFrame->HourTen == 2) && (wwvbFrame->HourOne == 4) ) {
	  wwvbFrame->HourTen = 0;
	  wwvbFrame->HourOne = 0;
	  wwvbFrame->DayOne++;
  }

  if (wwvbFrame->DayOne == 10) {
	  wwvbFrame->DayOne = 0;
	  wwvbFrame->DayTen++;
  }

  if (wwvbFrame->DayTen == 10) {
	  wwvbFrame->DayTen = 0;
	  wwvbFrame->DayHun++;
  }

  if ( (wwvbFrame->DayHun == 3) &&
       (wwvbFrame->DayTen == 6) &&
       (wwvbFrame->DayOne == (6 + (uint8_t) wwvbFrame->Leapyear)) ) {
	   // Happy New Year.
	   wwvbFrame->DayHun = 0;
	   wwvbFrame->DayTen = 0;
	   wwvbFrame->DayOne = 1;
           wwvbFrame->YearOne++;
  }

  if (wwvbFrame->YearOne == 10) {
    wwvbFrame->YearOne = 0;
    wwvbFrame->YearTen++;
  }

  if (wwvbFrame->YearTen == 10) {
    wwvbFrame->YearTen = 0;
  }

}


/*
 * wwvbChange
 *
 * This is the interrupt servicing routine.  Changes in the level of the
 * received WWVB pulse are recorded here to be processed in processBit().
 */

void WWVB::wwvbChange() {

  uint8_t signalLevel = digitalRead(wwvbInPin);

  // Determine if this was triggered by a rising or a falling edge
  // and record the pulse low period start and stop times
  if (signalLevel == LOW) {
    pulseStartTime = millis();
  } else {
    pulseEndTime = millis();
    processBit();
  }

}
