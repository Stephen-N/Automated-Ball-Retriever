/* 
 * // File:   newmain.c
 * // Author: andre
 *
 * Created on November 23, 2022, 11:56 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>


#pragma config FOSC=INTRCIO, WDTE=OFF, PWRTE=OFF, MCLRE=ON, CP=OFF, \
                CPD=OFF, BOREN=OFF, IESO=OFF, FCMEN=OFF
#define _XTAL_FREQ 4000000

#define RPi_Supply PORTBbits.RB4
#define Motor12V PORTBbits.RB5

int result;


/*
 * 
 */
void main(void) {
    
    ANSEL = 0b00000100;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    
    // Set Pin to Output
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
 
    //Setup ADC module
    // ADC channel selection - select AN2/RA2 pin as input
    ADCON0bits.CHS = 0b0010;
    
    // Set V_ref to VDD
    ADCON0bits.VCFG = 0;
    
    // conversion clock
    ADCON1bits.ADCS = 0b001;
    
    // result formatting - right justified
    ADCON0bits.ADFM = 1;
    
    // enable ADC
    ADCON0bits.ADON = 1;
    
    //Begin with GPIO HIGH so Rasp Pi is supplied with power
    RPi_Supply = 1;
    // Begin with GPIO HIGH so Motors are supplied with power
    Motor12V = 1;
    //Tell MCU to power-off system if digital value reaches certain point
    while(1)
    {
        // ADC Sample Time
       __delay_us(5); 
       
       // start ADC cycle, and wait until it is done
        ADCON0bits.GO = 1;
        while (ADCON0bits.GO) {};
        
         result = (ADRESH<<8) + ADRESL;
         float voltage = (float)result/1023f*5.0F;
         
         // WHen a full load is pulled from the power circuitry with a 11.6V power supply this is the minimum read by the ADC coming from the MAX4081
         if((voltage >= 4.2)&&(voltage <= 5.0)){
             RPi_Supply = 1;
             Motor12V = 1;
         }
         else{
             RPi_Supply = 0;
             Motor12V = 0;
         }
    }
    
}

