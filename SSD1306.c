#include "SSD1306.h"
#include <msp430.h>


void begin(int contrast) {

    // Configure Pins for I2C
    P1SEL |= BIT6 + BIT7;                  // Pin init
    P1SEL2 |= BIT6 + BIT7;                  // Pin init

  const unsigned char Init[] = { // https://www.adafruit.com/datasheets/SSD1306.pdf
    0xAE, // Display off
    
    0x81, // Set Contrast Control for BANK0 (81h)
    contrast, // This command sets the Contrast Setting of the display. The chip has 256 contrast steps from 00h to FFh. The
              //   segment output current increases as the contrast step value increases.
    
    0x20, // Set Memory Adress Mode
    0x00, // Horizontal Adressing Mode
    
    0x21, // Set Column Adress
    0x00, // Start Adress 0
    0x7F, // End Adress 127
    
    0x22, // Set Page Adress
    0x00, // Start Adress 0
    0x07, // End Adress 7
    
    0x40, // Set start line adress 0
    
    0xA1, // Set Segment Re-map (A0h/A1h) --set segment re-map 0 to 127
    0xA8, // Set Multiplex Ratio (A8h) (1 to 64)
    0x3F, // This command switches the default 63 multiplex mode to any multiplex ratio, ranging from 16 to 63. 
          //   The output pads COM0~COM63 will be switched to the corresponding COM signal.

    0xC8, // Set COM Output Scan Direction to normal mode
    
    0xD3, // set display offset
    0x00, // 0
    
    0x8D, // Set Charge Pump Setting
    0x14, // ON
    
    0xDA, // Set COM Pins Hardware Configuration (DAh)
    0x12, // alternative + disable remap
    
    0xD5, // Set Display Clock Divide Ratio/ Oscillator Frequency (D5h)
    0x80, // divide by 1 and medium freq
    
    0xD9, // Set Pre-charge Period (D9h)
    0x22, // medium (reset value)
    
    0xDB, // Set Vcomh regulator output
    0x20, // 0.77 x Vcc
    
    0xA6, // Set Normal/Inverse Display (A6h/A7h)
    
    0xA4, // Output follows RAM Content
    
    0xAF  // Display on
  };

  const unsigned char Mod[] = {0xA5};
//  WDTCTL = WDTPW + WDTHOLD;

  printC(Init,31);
  __delay_cycles(5000000);
  printC(Mod,1);
}

void printC(const unsigned char* Array, unsigned int length){

    UCB0CTL1 = UCSWRST;
    UCB0CTL0 = UCMODE_3 + UCMST + UCSYNC;           // I2C master mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST; // Use SMCLK, keep SW reset
    UCB0BR0 = 0x40;                          // < 100 kHz
    UCB0I2CSA = 0x3C;                       // address
    UCB0CTL1 &= ~UCSWRST;
    IE2 |= UCB0TXIE;                // Enable TX ready interrupt
    __disable_interrupt();
    UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start condition


    __bis_SR_register(LPM3_bits + GIE);

    unsigned int c;
    for(c = 0; c < length; c++){
        //__bis_SR_register(LPM3_bits + GIE);
        UCB0TXBUF = 0x80;
        __bis_SR_register(LPM3_bits + GIE);
        UCB0TXBUF = Array[c];
        __bis_SR_register(LPM3_bits + GIE);
    }

    UCB0CTL1 |= UCTXSTP;
    IE2 &= ~UCB0TXIE;

}


void printS(void){
    UCB0CTL1 = UCSWRST;
    UCB0CTL0 = UCMODE_3 + UCMST + UCSYNC;           // I2C master mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST; // Use SMCLK, keep SW reset
    UCB0BR0 = 0x40;                          // < 100 kHz
    UCB0I2CSA = 0x3C;                       // address
    UCB0CTL1 &= ~UCSWRST;
    IE2 |= UCB0TXIE;                // Enable TX ready interrupt
    __disable_interrupt();
    UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start condition

    __bis_SR_register(LPM3_bits + GIE);;
}

void printD(const unsigned char Data){
    UCB0TXBUF = Data;
    __bis_SR_register(LPM3_bits + GIE);
}

void printE(void){
    UCB0CTL1 |= UCTR + UCTXSTP;
}


void text(char s[], int str_len, int font_size) {
  if(str_len > 0) {
    if (font_size==0) {
      if(str_len < 22) {
        for (j=0; j<str_len; j++) {
          for (i=0; i<5; i++) 
            printD(Terminal6x8[s[j]-' '][i]);
          printD(0x00); // put a distance of one pixel between the characters
        } 
        text__continue_cursor_to_end_of_row(str_len,font_size);
      } else
        text("-ERROR:Exceeds row-",17,font_size);
    } else if (font_size==1) { 
      if(str_len < 11) {
        // print first part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<11; i++) 
              printD(Terminal11x16[s[j]-' '][2*i]);
          printD(0x00); // put a distance of one pixel between the characters
        }
        text__continue_cursor_to_end_of_row(str_len,font_size);
    
        // print second part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<11; i++) 
            printD(Terminal11x16[s[j]-' '][2*i+1]);
          printD(0x00); //put a distance of one pixel between the characters
        }  
        text__continue_cursor_to_end_of_row(str_len,font_size);
      } else
        text("-ERR:long-",10,font_size);
    } else if(font_size == 2) {
      if(str_len < 8) {
        // print first part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<16; i++) 
              printD(Arial16x24[s[j]-'0'][i*3]);
          printD(0x00); // put a distance of one pixel between the characters
        }
        text__continue_cursor_to_end_of_row(str_len,font_size);
    
        // print second part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<16; i++) 
            printD(Arial16x24[s[j]-'0'][i*3+1]);
          printD(0x00); //put a distance of one pixel between the characters
        }  
        text__continue_cursor_to_end_of_row(str_len,font_size);
    
        // print third part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<16; i++) 
            printD(Arial16x24[s[j]-'0'][i*3+2]);
          printD(0x00); //put a distance of one pixel between the characters
        }  
        text__continue_cursor_to_end_of_row(str_len,font_size);
      } else
        text("",0,font_size); // since this Arial font size only has numbers included, so instead of displaying an error-text, we display nothing
    } else if(font_size == 3) {
      if(str_len < 6) {
        // print first part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<24; i++) 
              printD(Arial24x40[s[j]-'0'][i*5]);
          printD(0x00); // put a distance of one pixel between the characters
        }
        text__continue_cursor_to_end_of_row(str_len,font_size);
    
        // print second part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<24; i++) 
            printD(Arial24x40[s[j]-'0'][i*5+1]);
          printD(0x00); //put a distance of one pixel between the characters
        }  
        text__continue_cursor_to_end_of_row(str_len,font_size);
    
        // print third part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<24; i++) 
            printD(Arial24x40[s[j]-'0'][i*5+2]);
          printD(0x00); //put a distance of one pixel between the characters
        }  
        text__continue_cursor_to_end_of_row(str_len,font_size);

        // print fourth part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<24; i++) 
            printD(Arial24x40[s[j]-'0'][i*5+3]);
          printD(0x00); //put a distance of one pixel between the characters
        }  
        text__continue_cursor_to_end_of_row(str_len,font_size);

        // print fifth part/row of the character(s)
        for (j=0; j<str_len; j++) {
          for (i=0; i<24; i++) 
            printD(Arial24x40[s[j]-'0'][i*5+4]);
          printD(0x00); //put a distance of one pixel between the characters
        }  
        text__continue_cursor_to_end_of_row(str_len,font_size);
      } else
        text("",0,font_size); // since this Arial font size only has numbers included, so instead of displaying an error-text, we display nothing
    }
  } else 
    text__continue_cursor_to_end_of_row("",font_size);
}


void text__continue_cursor_to_end_of_row(int str_len, int font_size) {
  if(font_size == 3) {
    width = 24;
    rows = 5;
  } else if(font_size == 2) {
    width = 16;
    rows = 3;
  } else if(font_size == 1) {
    width = 11;
    rows = 2;
  } else if(font_size == 0) {
    width = 5;
    rows = 1;
  }
  if(str_len > 0)
    for(i2=0; i2<128 - width * str_len - str_len; ++i2)
      printD(0x00);
  else 
    for(i2=0; i2<128 * rows; ++i2) 
      printD(0x00);
}

// USCI_B0 Data ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void){
    IFG2 &= ~UCB0TXIFG;
    __bic_SR_register_on_exit(LPM3_bits);
}
