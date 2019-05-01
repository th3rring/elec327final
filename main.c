#include "SSD1306.h"
#include <msp430.h>
#include <stdio.h>
#include <math.h>


// Macros for calculation of speed in MPH
#define T_ACLK 12000            //ACLK default frequency = 12kHz
#define CIRCUMFERENCE 76.96       // pi * 24.5 inches
#define IN_2_MILE 63360         //63360 inches in a mile
#define SECONDS_PER_HOUR 3600       // 3600 seconds per hour

// Global variables for ADC counting
unsigned int ADCconverted;
unsigned int count = 0;
int num;

// Global variables for current values
float RPM;
float time_in_seconds = 0;
float rev_per_hour = 0;
float speed = 0;

int main( void )
{
    WDTCTL = WDTPW | WDTHOLD;

    BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;
       if (CALBC1_16MHZ != 0xFF) {

           __delay_cycles(100000); //To allow recovery of hall effect before going again

           DCOCTL = 0x00;
           BCSCTL1 = CALBC1_16MHZ;  //Set DCO to 16MHz
           DCOCTL = CALDCO_16MHZ;
       }

    BCSCTL1 |= XT2OFF | DIVA_0;

    BCSCTL3 = XT2S_0 | LFXT1S_2 | XCAP_1;

    P1DIR |= BIT3;
    P1OUT &= ~BIT3;
    P1SEL |= BIT3;                          //Enable A0 for P 1.3

    ADC10CTL0 &= ~ENC;      //Set up ADC
    ADC10CTL0 = SREF_1 + REF2_5V + REFON + ADC10ON;
    ADC10CTL1 = ADC10DIV_3 + INCH_0 + SHS_0 + CONSEQ_2 + ADC10SSEL_2;


    TA0CCR0 = 65535; //Set end point of counter
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TAIE;     // Set up TA0 with ACLK @ 12kHZ in up mode
    TA0R = 0;

    //Init screen
    begin(0x50);


    while ( 1 ) {
        ADC10CTL0 |= ENC + ADC10SC;
        ADCconverted = ADC10MEM;

        //Rotation count. Increments one per rotation regardless of clock cycle speed
        if(ADCconverted <= 400){            //Triggered hall effect
            ++num;

            if( num == 1 ) {
                //Reset timer if we're starting again
                TA0R = 55671;
            } else {
                // After starting, actually calculate the speed
                count = TA0R;
                TA0R = 0;
                count -= 55671;
                time_in_seconds = (float)count / T_ACLK;        // count / counts/s = s/rev
                rev_per_hour = SECONDS_PER_HOUR / time_in_seconds;      // change rev/s to rev/hr
                speed = ( rev_per_hour * CIRCUMFERENCE ) / IN_2_MILE;   // RPH to MPH

                printS(); // Start talking to display
                char str[10];
                sprintf(str, "%d",(int)speed ); // Cast speed to int and convert to string
                text(str,12,0); // Display speed
                printE(); // Close channel to display

                //Reset timer
                num = 0;
                TA0R = 55671;
            }

            // Wait for falling edge on ADC
            while(!(ADCconverted >= 550 ))
            {
                ADC10CTL0 &= ~(ADC10SC);
                ADC10CTL0 |= ENC + ADC10SC;
                ADCconverted = ADC10MEM;
            }

            if ( num == 0 ){
                TA0R = 55671;
            }

        }

        ADC10CTL0 &= ~(ADC10SC);
    }
}


// Timer ISR
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR_HOOK(void)
{
    P2OUT ^= BIT3;
    num = 0;
    TA0CTL &= ~TAIFG;
}
