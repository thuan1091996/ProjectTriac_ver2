/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Project_Triac_PWM
Version : v1.0
Date    : 7/30/2018
Author  : Tran Minh Thuan
Company : Viet Mold Machine
Comments: 
PC0, PC1 - Input-  control button
PC2,PC3,PC4 -  Output - 74595 operation
PD2 - Exteral interrupt
PB5, PB3 - Input -  Power button
(All buttons are pulled-up)


Chip type               : ATmega8
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*******************************************************/

#include <mega8.h>

#define SIGNAL      PORTB.1
#define SW_EMER     PINB.3
#define SW_ON       PINB.5
#define SW_GIAM     PINC.1
#define SW_TANG     PINC.0
#define STR         PORTC.2
#define CLK         PORTC.3
#define SDI         PORTC.4
#define LED         PORTC.5

/* ------Cac dinh nghia ve chuong trinh va cac bien--------*/
//-----------------------------------------------------------
//Dinh nghia cac bien toan cuc
unsigned int    count=99;                  //Bien dem gia tri delay
unsigned int    T_delay=0;                 //Tang 1 moi khi ngat timer0 (100us)
int             T_ISR_delay=0;             //Tang 1 moi ngat timer1 de phuc vu kich B1
unsigned char   T_shift_delay=0;           //Dich de ve diem 0
unsigned char   Status=1;                  //Vua bat nguon thi he thong se hoat dong
unsigned char   MA_7SEG[10]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
//Dinh nghia cac chuong trinh
void INT0_INIT(void);                   //Khoi tao cho ngat ngoai
void Timer_INIT(void);                  //Khoi tao thong so va ngat cho Timer0
void GPIO_INIT(void);                   //Khoi tao cac input, output
void Delay_100us(unsigned int Time);    //Delay 100us ung voi moi don vi
void Display();                         //Hien thi gia tri ra 7segs                             
void CheckSW(void);                     //Doc gia tri nut nhan
void Power_Check(void);                 //Check nut nhan nguon
void Disable_PWM(void);                 //Tat PWM Timer1
void Enable_PWM(void);                  //Mo  PWM Timer1
int  Wait_Shift(void);                  //Doi de shift toi muc 0
 
/*---------------------Interrupt Service Routine-----------*/
interrupt [TIM1_CAPT] void timer1_capt_isr(void)
{
    //Disable_PWM();
}
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    //T_shift_delay=0;
    //if(Wait_Shift()==1)
    //Enable_PWM();  
};

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    // Reinitialize Timer 0 value
    //T_delay++;
    //TCNT0=0x9C;    //Tinh toan = 0x9C (100), nhung bu sai so nen A0
     
};

/*-------------------Mandatory program------------------------*/
void main(void)
{
// Global enable interrupts
#asm("sei")
GPIO_INIT();    //GPIO Initialization
Timer_INIT();   //Timer 0 delay100us, Timer1 Fast PWM Initialization
//INT0_INIT();    //External Rising Edge interrupt Initialization
while (1)
      {
      }
};
/*--------------Cac chuong trinh con--------------------*/
void INT0_INIT(void)
{
    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Rising Edge
    // INT1: Off
    GICR|=(0<<INT1) | (1<<INT0);
    MCUCR=(0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (1<<ISC00);
    GIFR=(0<<INTF1) | (1<<INTF0);    
};   
//////////////////////////////////////////////////////////
void Timer_INIT(void)
{
//----------------Timer0--------------------------------//
    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: 8000.000 kHz
    // Prescaler: 8
    TCCR0=(0<<CS02) | (1<<CS01) | (0<<CS00);  //Prescaler - 8
//----------------Timer1--------------------------------//
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 8000.000 kHz
    // Mode: Fast PWM top=ICR1
    // OC1A output: Inverted PWM
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer Period: 8 ms
    // Output Pulse(s):
    // OC1A Period: 8 ms Width: 3.9999 ms
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: On
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    TCCR1A=(1<<COM1A1) | (1<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
    TCNT1H=0xE7;
    TCNT1L=0x00;
    ICR1H=0xF9;
    ICR1L=0xFF;
    OCR1AH=0x7D;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;
};
///////////////////////////////////////////////////////////
void GPIO_INIT(void)
{
    // Input/Output Ports initialization
    // Port B initialization
    // Function: Bit7=Out Bit6=Out Bit5=In Bit4=Out Bit3=IN Bit2=Out Bit1=Out Bit0=Out 
    DDRB=(1<<DDB7) | (1<<DDB6) | (0<<DDB5) | (1<<DDB4) | (0<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
    // State: Bit7=0 Bit6=0 Bit5=1 Bit4=0 Bit3=1 Bit2=0 Bit1=0 Bit0=0 
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (1<<PORTB5) | (0<<PORTB4) | (1<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

    // Port C initialization
    // Function: Bit6=In Bit5=In Bit4=Out Bit3=Out Bit2=Out Bit1=In Bit0=In 
    DDRC=(0<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (0<<DDC1) | (0<<DDC0);
    // State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);

    // Port D initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);
};
///////////////////////////////////////////////////////////
void Delay_100us(unsigned int Time) 
{
    TIMSK|=0x01;        //Cho phep ngat tran timer0
    T_delay=0;          //Reset gia tri dem
    TCNT0=0x9C;         //Tinh toan = 0x9C, nhung bu sai so nen A0
    while(T_delay<Time);//Chua du
    TIMSK&=~0x01;       //Du thoi gian, tat ngat tran timer0
};
///////////////////////////////////////////////////////////
void CheckSW(void)
{
    if(!SW_GIAM)
    {
        Delay_100us(200);
        if(count>0) count--;
        else        count=0;
    }
    if(!SW_TANG)
    {
        Delay_100us(200);  
        if(count<99) count++;
        else         count=99;
    }
};                                                      
///////////////////////////////////////////////////////////
void Display()
{
    unsigned char i,Q;
    unsigned char Dvi,Chuc;
    Dvi=MA_7SEG[(99-count)%10];
    Chuc=MA_7SEG[(99-count)/10];
    Q=Chuc; for(i=0;i<8;i++){SDI=Q&0x80;CLK=0;CLK=1;Q<<=1;} 
    Q=Dvi; for(i=0;i<8;i++){SDI=Q&0x80;CLK=0;CLK=1;Q<<=1;} 
    STR=0; STR=1; 
};
///////////////////////////////////////////////////////////
int Wait_Shift(void)
{
    if(T_shift_delay>=5)            //sau 4*90us ve diem 0
    return 1;                       //Ve diem 0
    else                            //Chua ve diem 0  
    return 0; 
};
///////////////////////////////////////////////////////////
void Power_Check(void)
{
    if(SW_EMER==0)                  //LOCK
        {   
            Status=0;
            Delay_100us(3000);
            if(SW_EMER==1)          //UNLOCK
            Status=1;     
        }
    else
        {
            if(SW_ON==0)
                {
                    Delay_100us(5000);
                    Status=(Status+1)%2;     
                }
        }
    if(Status==1) {LED=0; DDRB|=0x02;}
    else          {LED=1; DDRB&=~0x02;}
};
///////////////////////////////////////////////////////////
void Disable_PWM(void)
{
    TCCR1B &= 0xf8;    // turn off timer 1;
    TCCR1A = 0;    // disconnect the output pin
    TCNT1H = 0;
    TCNT1L = 0;
    SIGNAL=0;    
};
///////////////////////////////////////////////////////////
void Enable_PWM(void)
{
    TCCR1A=(1<<COM1A1) | (1<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
    TCNT1H = 0;
    TCNT1L = 0;    
};
/*--------------------------------------------------------*/
