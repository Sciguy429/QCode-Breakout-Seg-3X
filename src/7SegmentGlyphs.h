#pragma once

#include <Arduino.h>

//Glyph sizes
#define GLYPH_SIZE 7

//Hex numbers for display
const uint8_t hexNumberGlyphs[] =
{
  B01000000, //0
  B01111001, //1
  B00100100, //2
  B00110000, //3
  B00011001, //4
  B00010010, //5
  B00000010, //6
  B01111000, //7
  B00000000, //8
  B00010000, //9
  B00001000, //A
  B00000011, //B
  B01000110, //C
  B00100001, //D
  B00000110, //E
  B00001110, //F
};

//Special glyphs
const uint8_t specialGlyphs[] =
{
  B01111111, //' '
  B00111111, //-
  B01110111, //_
  B00110111, //=
};

//Alpha glyphs
const uint8_t alphaGlyphs[] =
{
  B00001000, //A
  B00000011, //B
  B01000110, //C
  B00100001, //D
  B00000110, //E
  B00001110, //F
  B01000010, //G
  B00001011, //H
  B01001111, //I
  B01100001, //J
  B00001010, //K
  B01000111, //L
  B01101010, //M
  B01001000, //N
  B01000000, //O
  B00001100, //P
  B00011000, //Q
  B01001100, //R
  B00010010, //S
  B00000111, //T
  B01000001, //U
  B01010001, //V
  B01010101, //W
  B00001001, //X
  B00010001, //Y
  B00110100, //Z
};
