

#ifndef SSD1306_h
#define SSD1306_h

#include "Terminal6.h"
#include "Terminal12.h"
#include "Arial_16x24.h"
#include "Arial_24x40.h"


    void begin(int contrast);
    void printS();
    void printE();
    void text(char string[], int str_len, int font_size);
    void printC(const unsigned char* Array, unsigned int length);
    void printD(const unsigned char Data);
    int i, j; // used in: text()
    void text__continue_cursor_to_end_of_row(int str_len, int font_size);
      int width, rows;
      int i2;

#endif

