//-- Asus Qcode Card --
//
// - Firmware by: Sciguy429
//
// - This is the firmware source for the 3X Qcode card.

//7-segment library
#include "7Segment.h"

//Debounce library
#include "Debounce.h"

//Echo flag
// - If enabled, any QCodes received will be echoed back on the serial port. Allowing for
// - another device to receive the data in tandem.
#define ECHO_SERIAL_DATA true

//Board led
const uint8_t boardLED = 13;

//Button pin
const uint8_t buttonPin = 12;

//Button debouncer
Debounce buttonDebounce = Debounce(buttonPin);

//Total number of displays
#define MAX_DSP 6

//Mux pins
const uint8_t muxPins[] = {7, 6, 3, 4, 5, 8, 9, 2}; //A, B, C, D, E, F, G, DP

//Dsp pins
const uint8_t dspPins[] = {11, 10, A1, A0, A2, A3}; //Left to right

//7-segment object
SegmentDSP segmentDSP(MAX_DSP, muxPins, dspPins);

//Past qcodes
#define MAX_PAST_QCODES 256
uint8_t pastQCodes[MAX_PAST_QCODES];
int pastQcodeHead = 0;
int pastQcodeCount = 0;
uint8_t curQCode = 0x00;

//Update the displays
void updateDisplays()
{
  //Make sure we have something in the buffer
  if (pastQcodeCount > 0)
  {
    //Find starting pos
    int curPos = pastQcodeHead - 1;
    if (curPos < 0)
    {
      curPos = curPos + MAX_PAST_QCODES;
    }

    //Grab the most recent 3 things from the buffer
    int codesDisplayed = (pastQcodeCount > 3 ? 3 : pastQcodeCount);
    for (int i = 0; i < codesDisplayed; i++)
    {
      //Display the code
      uint8_t pos = i * 2;
      segmentDSP.setHex(pos, pastQCodes[curPos]);
      segmentDSP.setDP(pos, false);
      segmentDSP.setDP(pos + 1, true);

      //Push the position backward
      curPos--;
      if (curPos < 0)
      {
        curPos = curPos + MAX_PAST_QCODES;
      }
    }

    //Blank any empty displays
    if (codesDisplayed < 3)
    {
      for (int i = 0; i < 3 - codesDisplayed; i++)
      {
        uint8_t pos = (2 - i) * 2;
        segmentDSP.setGlyphSpecial(pos, 0);
        segmentDSP.setGlyphSpecial(pos + 1, 0);
        segmentDSP.setDP(pos, false);
        segmentDSP.setDP(pos + 1, false);
      }
    }
  }
  else
  {
    //If the buffer is empty, blank the displays
    segmentDSP.setGlyphSpecial(0, 1);
    segmentDSP.setGlyphSpecial(1, 1);
    segmentDSP.setGlyphSpecial(2, 0);
    segmentDSP.setGlyphSpecial(3, 0);
    segmentDSP.setGlyphSpecial(4, 0);
    segmentDSP.setGlyphSpecial(5, 0);
    segmentDSP.setAllDP(false);
  }
}

//Called ~300 times per second
SIGNAL(TIMER2_COMPA_vect) 
{
  //Advance The COMPA Register
  OCR2A += 206;

  //Update the 7-segment displays
  segmentDSP.dspUpdate();
}

void setup()
{
  //Board LED
	pinMode(boardLED, OUTPUT);
  digitalWrite(boardLED, LOW);

  //Button pin
  pinMode(buttonPin, INPUT_PULLUP);

  //Setup all mux pins
  for (int i = 0; i < 8; i++)
  {
    pinMode(muxPins[i], OUTPUT);
    digitalWrite(muxPins[i], HIGH); //Active LOW
  }

  //Setup the dsp pins
  for (int i = 0; i < MAX_DSP; i++)
  {
    pinMode(dspPins[i], OUTPUT);
    digitalWrite(dspPins[i], LOW); //Active HIGH
  }

  //Open serial connection to the COM_DEBUG port
  Serial.begin(115200);

  //Clear the list of past qcodes
  for (int i = 0; i < MAX_PAST_QCODES; i++)
  {
    pastQCodes[i] = 0x00;
  }

  //Clear displays
  updateDisplays();

  //Setup interrupt
  // - Timer 2 COMP-A
  // - 300hz fire rate
  // - 50hz flash for each dsp
  cli();
  TCCR2A = 0;           //Init Timer2A
  TCCR2B = 0;           //Init Timer2B
  TCCR2B |= B00000110;  //Prescaler = 256
  OCR2A = 206;          //Timer Compare2A Register
  TIMSK2 |= B00000010;
  sei();
}

void loop()
{
  //Update the button state
  ButtonState buttonState = buttonDebounce.read();

  //Check our button state
  if (buttonState == ButtonState::BS_PRESSED)
  {
    //Print our saved codes
    if (pastQcodeCount > 0)
    {
      //Find starting pos
      int curPos = pastQcodeHead - pastQcodeCount;
      if (curPos < 0)
      {
        curPos = curPos + MAX_PAST_QCODES;
      }

      //Print codes
      // - This is a fairly quick and dirty way of doing this, but it works.
      bool dp = false;
      uint8_t curCode = 0x00;
      uint8_t lastCode1 = 0x00;
      uint8_t lastCode2 = 0x00;
      for (int i = 0; i < pastQcodeCount; i++)
      {
        //Get the current code
        curCode = pastQCodes[curPos];

        //Set the first display pair
        segmentDSP.setHex(0, curCode);
        segmentDSP.setDP(0, dp);
        segmentDSP.setDP(1, !dp);

        //Set the second display pair
        if (i > 0)
        {
          segmentDSP.setHex(2, lastCode1);
          segmentDSP.setDP(2, dp);
          segmentDSP.setDP(3, !dp);
        }
        else
        {
          segmentDSP.setGlyphSpecial(2, 1);
          segmentDSP.setGlyphSpecial(3, 1);
          segmentDSP.setDP(2, false);
          segmentDSP.setDP(3, false);
        }

        //Set the third display pair
        if (i > 1)
        {
          segmentDSP.setHex(4, lastCode2);
          segmentDSP.setDP(4, dp);
          segmentDSP.setDP(5, !dp);
        }
        else
        {
          segmentDSP.setGlyphSpecial(4, 1);
          segmentDSP.setGlyphSpecial(5, 1);
          segmentDSP.setDP(4, false);
          segmentDSP.setDP(5, false);
        }

        //Toggle DP value
        dp = !dp;

        //Push last code vars forward
        lastCode2 = lastCode1;
        lastCode1 = curCode;

        //Push the position forward
        curPos++;
        if (curPos >= MAX_PAST_QCODES)
        {
          curPos = 0;
        }

        //Wait
        delay(250);
      }
    }

    //Reset the display to the last code
    updateDisplays();
  }

  //Wait for serial data
  if (Serial.available())
  {
    //Flash LED
    digitalWrite(boardLED, HIGH);

    //Grab our new code and update the displays
    // - TODO
    curQCode = Serial.read();
    //segmentDSP.setHex(0, curQCode);

    //Echo the serial data if need be
    if (ECHO_SERIAL_DATA)
    {
      Serial.write(curQCode);
    }

    //Save our code into the buffer
    pastQCodes[pastQcodeHead] = curQCode;
    pastQcodeHead++;
    if (pastQcodeHead >= MAX_PAST_QCODES)
    {
      pastQcodeHead = 0;
    }

    //Add up our buffer size
    if (pastQcodeCount < MAX_PAST_QCODES)
    {
      pastQcodeCount++;
    }

    //Update the displays
    updateDisplays();
  }
  else
  {
    //Flash LED
    digitalWrite(boardLED, LOW);
  }
}
