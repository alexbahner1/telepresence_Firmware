
#include "elecanisms.h"
#include "usb.h"
#include "ajuart.h"
#include <stdio.h>
#include <math.h>
// #include <unistd.h>

#define SPRING              0
#define DAMPER              1
#define TEXTURE             2
#define WALL            3
#define READ_SW2            4
#define READ_SW3            5
#define ENC_READ_REG        6
#define SET_DUTY_VAL        7
#define GET_DUTY_VAL        8
#define GET_DUTY_MAX        9


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

// USB REQest and mode code here
// void vendor_requests(void) {
//     WORD temp;
//     uint16_t i;
//     uint16_t angle_reg = 0x3FFF;
//     // uint16_t angle;
//
//     switch (USB_setup.bRequest) {
//         case SPRING:   // should keep a change
//             LED1 = 1; LED2 = 0; LED3 = 0; MODE = 0; chan = 1;
//             BD[EP0IN].bytecount = 0;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         case DAMPER: // should keep a change
//             LED1 = 0; LED2 = 1; LED3 = 0; MODE = 1; chan = 1;
//             BD[EP0IN].bytecount = 0;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         case TEXTURE: // should keep a change
//             LED1 = 0; LED2 = 0; LED3 = 1; MODE = 2; chan = 1;
//             //D9 = 1;
//             BD[EP0IN].bytecount = 0;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;              // should addd the 4th button
//         case WALL: // should keep a change
//             // LED3 = !LED3;
//             LED1 = 1; LED2 = 0; LED3 = 1; MODE = 3; chan = 1;
//             BD[EP0IN].bytecount = 0;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         // case READ_SW1:  // should keep a change
//         //     BD[EP0IN].address[0] = SW1 ? 1 : 0;
//         //     BD[EP0IN].bytecount = 1;
//         //     BD[EP0IN].status = UOWN | DTS | DTSEN;
//         //     break;
//         case READ_SW2:  // could delete
//             BD[EP0IN].address[0] = SW2 ? 1 : 0;
//             BD[EP0IN].bytecount = 1;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         case READ_SW3: // could delete
//             BD[EP0IN].address[0] = SW3 ? 1 : 0;
//             BD[EP0IN].bytecount = 1;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         case ENC_READ_REG: // keep butt change to the angle sencor
//             //angle = enc_readReg(angle_reg);
//             //temp.w = read_analog(A0_AN);
//             //temp = enc_readReg(USB_setup.wValue);
//             BD[EP0IN].address[0] = temp.b[0];
//             BD[EP0IN].address[1] = temp.b[1];
//             BD[EP0IN].bytecount = 2;
//             // anglelast = angle;
//             // angle = ((temp.b[0] + (temp.b[1] << 8)) & 0x3FFF)/100;
//             // angle = (temp[0] + 256 * temp[1]) && 0x3FFF;
//             //printf("angles: %d\r\n",angle);
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//
//
//             //postion();
//             //modeControl();
//             break;
//         case SET_DUTY_VAL:
//             OC1R = USB_setup.wValue.w;
//             BD[EP0IN].bytecount = 0;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         case GET_DUTY_VAL:
//             temp.w = OC1R;
//             BD[EP0IN].address[0] = temp.b[0];
//             BD[EP0IN].address[1] = temp.b[1];
//             BD[EP0IN].bytecount = 2;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         case GET_DUTY_MAX:
//             temp.w = OC1RS;
//             BD[EP0IN].address[0] = temp.b[0];
//             BD[EP0IN].address[1] = temp.b[1];
//             BD[EP0IN].bytecount = 2;
//             BD[EP0IN].status = UOWN | DTS | DTSEN;
//             break;
//         default:
//             USB_error_flags |= REQUEST_ERROR;
//     }
// }


int16_t main(void) {
    uint8_t *RPOR, *RPINR;

    init_elecanisms();
    init_ajuart();

    servo_offset = (uint16_t)(FCY * SERVO_MIN_WIDTH);
    servo_multiplier = (uint16_t)(FCY * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH));

    // Configure pin D13 to produce a 1-kHz PWM signal with a 25% duty cycle
    // using the OC1 module.

    // // Configure encoder pins and connect them to SPI2
    // ENC_CSn_DIR = OUT; ENC_CSn = 1;
    // ENC_SCK_DIR = OUT; ENC_SCK = 0;
    // ENC_MOSI_DIR = OUT; ENC_MOSI = 0;
    // ENC_MISO_DIR = IN;

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

    // D13_DIR = OUT;      // configure D13 to be a digital output
    // D13 = 0;            // set D13 low
    //
    // D10_DIR = OUT;
    // D9_DIR = OUT;
    // D9 = 0;
    // D10 = 1;
    // D5_DIR = OUT;
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
    servo_temp.ul = 0x8000 * (uint32_t)(19200 - 9600);//servo_multiplier ;
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
    servo_temp.ul = 0x8000 * (uint32_t)(19200 + 9600) ;
    OC2RS = servo_offset + servo_temp.w[1]; // 15e-4; //servo_temp.w[1];
    OC2R = 1;
    OC2TMR = 0;

    T1CON = 0x0010;     // configure Timer1 to have a period of 20ms
    PR1 = 0x9C3F;

    TMR1 = 0;
    T1CONbits.TON = 1;

    // __builtin_write_OSCCONL(OSCCON & 0xBF);
    // RPOR[D8_RP] = OC1_RP;  // connect the OC1 module output to pin D13
    // RPOR[D9_RP] = OC1_RP;
    // __builtin_write_OSCCONL(OSCCON | 0x40);
    //
    // OC1CON1 = 0x1C06;   // configure OC1 module to use the peripheral
    //                     //   clock (i.e., FCY, OCTSEL<2:0> = 0b111) and
    //                     //   and to operate in edge-aligned PWM mode
    //                     //   (OCM<2:0> = 0b110)
    // OC1CON2 = 0x001F;   // configure OC1 module to syncrhonize to itself
    //                     //   (i.e., OCTRIG = 0 and SYNCSEL<4:0> = 0b11111)
    //
    // OC1RS = (uint16_t)(FCY / 1e3 - 1.);     // configure period register to
    //                                         //   get a frequency of 1kHz
    // OC1R = OC1RS >> 1; //OC1R = OC1RS >> 2;  // configure duty cycle to 25% (i.e., period / 4)
    //
    //
    // OC1TMR = 0;         // set OC1 timer count to 0

    // USB_setup_vendor_callback = vendor_requests;
    // init_usb();
    // while(1){
    //   modeControl();
    //   usb_service();
    // }
//     while (USB_USWSTAT != CONFIG_STATE) {
// #ifndef USB_INTERRUPT
//         usb_service();
//         modeControl();
// #endif
//     }
//     while (1) {
// #ifndef USB_INTERRUPT
//         usb_service();
//         modeControl();
// #endif
//     }

  while(1){

  }

}
