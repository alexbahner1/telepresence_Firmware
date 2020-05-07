
#include "elecanisms.h"
#include "usb.h"
#include "ajuart.h"
#include <stdio.h>
#include <math.h>
// #include <unistd.h>


#define FORWARD_R           0
#define STOP_R              1
#define BACKWARD_R          2
#define FORWARD_L           3
#define STOP_L              4
#define BACKWARD_L          5


// DEFINES
#define ENC_MISO            D1
#define ENC_MOSI            D0
#define ENC_SCK             D2
#define ENC_CSn             D3

#define ENC_MISO_DIR        D1_DIR
#define ENC_MOSI_DIR        D0_DIR
#define ENC_SCK_DIR         D2_DIR
#define ENC_CSn_DIR         D3_DIR

#define ENC_MISO_RP         D1_RP
#define ENC_MOSI_RP         D0_RP
#define ENC_SCK_RP          D2_RP

#define SERVO_MIN_WIDTH     900e-6
#define SERVO_MAX_WIDTH     2.1e-3
uint16_t servo_offset, servo_multiplier;
WORD32 servo_temp;
// this is code for the angle sencor

// this is code for the angle sencor END
int chan = 0;

void vendor_requests(void) {
    // WORD temp;
    // uint16_t i;
    // uint16_t angle_reg = 0x3FFF;
    // uint16_t angle;

    switch (USB_setup.bRequest) {
        case FORWARD_R:   // should keep a change
            // LED1 = 1; LED2 = 0; LED3 = 0; MODE = 0; chan = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            forwardR();
            break;
        case STOP_R: // should keep a change
            // LED1 = 0; LED2 = 1; LED3 = 0; MODE = 1; chan = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            stopR();
            break;
        case BACKWARD_R: // should keep a change
            // LED1 = 0; LED2 = 0; LED3 = 1; MODE = 2; chan = 1;
            //D9 = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            backwardR();
            break;              // should addd the 4th button
        case FORWARD_L: // should keep a change
            // LED3 = !LED3;
            // LED1 = 1; LED2 = 0; LED3 = 1; MODE = 3; chan = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            forwardL();
            break;
        case STOP_L:  // could delete
            //BD[EP0IN].address[0] = SW2 ? 1 : 0;
            BD[EP0IN].bytecount = 1;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            stopL();
            break;
        case BACKWARD_L: // could delete
            //BD[EP0IN].address[0] = SW3 ? 1 : 0;
            BD[EP0IN].bytecount = 1;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            backwardL();
            break;

        default:
            USB_error_flags |= REQUEST_ERROR;
    }
}

void forwardL(){
  servo_temp.ul = 0x8000 * (uint32_t)(19200 - 9600);//servo_multiplier ;
  OC1RS = servo_offset + servo_temp.w[1];
}
void forwardR(){
  servo_temp.ul = 0x8000 * (uint32_t)(19200 + 9600) ;
  OC2RS = servo_offset + servo_temp.w[1]; // 15e-4; //servo_temp.w[1];
}
void backwardL(){
  servo_temp.ul = 0x8000 * (uint32_t)(19200 + 9600);//servo_multiplier ;
  OC1RS = servo_offset + servo_temp.w[1];
}
void backwardR(){
  servo_temp.ul = 0x8000 * (uint32_t)(19200 - 9600) ;
  OC2RS = servo_offset + servo_temp.w[1]; // 15e-4; //servo_temp.w[1];
}
void stopL(){
  servo_temp.ul = 0x8000 * (uint32_t)(19200);//servo_multiplier ;
  OC1RS = servo_offset + servo_temp.w[1];
}
void stopR(){
  servo_temp.ul = 0x8000 * (uint32_t)(19200) ;
  OC2RS = servo_offset + servo_temp.w[1]; // 15e-4; //servo_temp.w[1];
}

int16_t main(void) {
    uint8_t *RPOR, *RPINR;

    init_elecanisms();
    init_ajuart();

    servo_offset = (uint16_t)(FCY * SERVO_MIN_WIDTH);
    servo_multiplier = (uint16_t)(FCY * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH));

    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPINR[MISO2_RP] = ENC_MISO_RP;
    RPOR[ENC_MOSI_RP] = MOSI2_RP;
    RPOR[ENC_SCK_RP] = SCK2OUT_RP;
    __builtin_write_OSCCONL(OSCCON | 0x40);

    SPI2CON1 = 0x003B;              // SPI2 mode = 1, SCK freq = 8 MHz
    SPI2CON2 = 0;
    SPI2STAT = 0x8000;

    D8_DIR = OUT;
    D9_DIR = OUT;
    D8 = 0;
    D9 = 0;


    RPOR = (uint8_t *)&RPOR0;
    RPINR = (uint8_t *)&RPINR0;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D8_RP] = OC1_RP;  // connect the OC1 module output to pin D13
    RPOR[D9_RP] = OC2_RP;  // connect the OC2 module output to pin D12
    __builtin_write_OSCCONL(OSCCON | 0x40);

    OC1CON1 = 0x1C0F;   // configure OC1 module to use the peripheral
                        //   clock (i.e., FCY, OCTSEL<2:0> = 0b111),
                        //   TRIGSTAT = 1, and to operate in center-aligned
                        //   PWM mode (OCM<2:0> = 0b111)
    OC1CON2 = 0x008B;   // configure OC1 module to trigger from Timer1
                        //   (OCTRIG = 1 and SYNCSEL<4:0> = 0b01011)

    // set OC1 pulse width to 1.5ms (i.e. halfway between 0.9ms and 2.1ms)
    //servo_temp.ul = 0x8000 * (uint32_t)servo_multiplier;
    servo_temp.ul = 0x8000 * (uint32_t)(19200);//servo_multiplier ;
    OC1RS = servo_offset + servo_temp.w[1];
    OC1R = 1;
    OC1TMR = 0;

    OC2CON1 = 0x1C0F;   // configure OC2 module to use the peripheral
                        //   clock (i.e., FCY, OCTSEL<2:0> = 0b111),
                        //   TRIGSTAT = 1, and to operate in center-aligned
                        //   PWM mode (OCM<2:0> = 0b111)
    OC2CON2 = 0x008B;   // configure OC2 module to trigger from Timer1
                        //   (OCTRIG = 1 and SYNCSEL<4:0> = 0b01011)

    // set OC2 pulse width to 1.5ms (i.e. halfway between 0.9ms and 2.1ms)
    servo_temp.ul = 0x8000 * (uint32_t)(19200) ;
    OC2RS = servo_offset + servo_temp.w[1]; // 15e-4; //servo_temp.w[1];
    OC2R = 1;
    OC2TMR = 0;

    T1CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR1 = 0x9C3F;

    TMR1 = 0;
    T1CONbits.TON = 1;

    // stop();

  // while(1){
  //
  // }
  USB_setup_vendor_callback = vendor_requests;
  init_usb();
  
  while (USB_USWSTAT != CONFIG_STATE) {
#ifndef USB_INTERRUPT
       usb_service();
       modeControl();
#endif
   }
   while (1) {
#ifndef USB_INTERRUPT
       usb_service();
       modeControl();
#endif
   }

}
