/*
  WWVB.h - WWVB library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef WWVB_h
#define WWVB_h

// include types & constants of Wiring core API
//#include "WConstants.h"

// library interface description
class WWVB
{
  // user-accessible "public" interface
  public:
    WWVB(uint8_t pin);
    void begin();
    void debug(bool on);
    //String datetimeString();
    //String dateString();
    //String timeString();
    uint8_t getMinute();
    uint8_t getHour();
    uint8_t getDay();
    uint8_t getYear();

  // library-accessible "private" interface
  private:
    uint8_t wwvbInPin;
    void wwvbChange();
    void processBit();
    void logFrameError(bool err);
    void buffer(uint8_t bit);
    void incrementWwvbMinute();
};

#endif
