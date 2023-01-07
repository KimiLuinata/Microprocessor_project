 #include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

void main(void) {
    
    ADCON1 = 0x0F;

    //Timer2 -> On, prescale -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 µs
    OSCCONbits.IRCF = 0b001;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    CCP2CONbits.CCP2M = 0b1100;
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    
    TRISA = 0;
    LATA = 0;
    
    // button RB0 -> input
    PORTB = 0;
    TRISB = 0b00000111;
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 8µs * 4
     * = 0.019968s ~= 20µs
     */
    PR2 = 0x9b;
    
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x04*4 + 0b00) * 8µs * 4
     * = 0.00512s ~= 512µs
     */
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b00; 
    
    CCPR2L = 0x04;
    CCP2CONbits.DC2B = 0b00; 
    
//    RCONbits.IPEN = 1; //enable priority levels on interrupts
    INTCONbits.GIEH = 1; //enable all high-priority interrupts
    INTCONbits.GIEL = 1; //enable all low-priority interrupts

    INTCONbits.INT0IE = 1; //enable INT0 external interrupt
    INTCON3bits.INT1IE = 1; //enable INT1 external interrupt
    INTCON3bits.INT2IE = 1;
    
    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT2IF = 0;
    
    INTCON3bits.INT1IP = 1;
    INTCON3bits.INT2IP = 0;
    
    INTCON2 = 0;
    IPR1bits.RCIP = 0;
    PIE1bits.RCIE = 1;
    
    while(1);
    return;
}
int i = 0;
int j = 0;
int k = 0;
void __interrupt() ISR(void)
{   
    if (INTCONbits.INT0IF==1){ 
        if(i % 2==0){
            CCPR1L = 0x0b;
            CCP1CONbits.DC1B = 0b01; 
        }
        else{
            CCPR1L = 0x12;
            CCP1CONbits.DC1B = 0x03; 
        }
        i++;
        INTCONbits.TMR0IF = 0;
        INTCONbits.INT0IF = 0;
    }
    
    if (INTCON3bits.INT1IF==1){ 
        if(j % 2==0){
            CCPR2L = 0x0b;
            CCP2CONbits.DC2B = 0b01; 
        }
        else{
            CCPR2L = 0x12;
            CCP2CONbits.DC2B = 0x03; 
        }
        j++;
//        INTCON3bits.TMR2IF = 0;
        INTCON3bits.INT1IF = 0;
    }
    
    if (INTCON3bits.INT2IF == 1){ 
        if(k % 2==0){
            LATA = 3; 
        }
        else{
            LATA = 0; 
        }
        k++;
//        INTCON3bits.TMR2IF = 0;
        INTCON3bits.INT2IF = 0;
    }
    
}
