#include <xparameters.h>
#include <xgpio.h>
#include <xstatus.h>
#include <xil_printf.h>

/*
Deliverable 2 rules:

- BTN0 held  -> COUNT++
- BTN1 held  -> COUNT--
- BTN2 held  -> show SWITCH value on LEDs
- BTN3 press -> show COUNT on LEDs

Input bus (8 bits):
  [3:0]  switches
  [7:4]  buttons (BTN0..BTN3)
*/


#define LED_GPIO_DEVICE_ID   XPAR_LED_DEVICE_ID
#define IN_GPIO_DEVICE_ID    XPAR_IN8_DEVICE_ID

#define WAIT_VAL 10000000   // used for ~1 second delay

static void delay_1s(void)
{
    volatile int i = 0;
    while (i < WAIT_VAL) { i++; }
}

int main(void)
{
    XGpio ledGpio;
    XGpio inGpio;
    int status;

    // Init LED GPIO as OUTPUT
    status = XGpio_Initialize(&ledGpio, LED_GPIO_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("LED GPIO init failed\r\n");
        return 1;
    }
    XGpio_SetDataDirection(&ledGpio, 1, 0x00);  // channel 1 output

    // Init input GPIO as INPUT (8 bits)
    status = XGpio_Initialize(&inGpio, IN_GPIO_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("Input GPIO init failed\r\n");
        return 1;
    }
    XGpio_SetDataDirection(&inGpio, 1, 0xFF);  // channel 1 input

    int count = 0;
    int lastLed = -1;
    int lastAction = -1;

    // For BTN3 press detect
    int prevBtn3 = 0;

    xil_printf("Deliverable 2 started\r\n");

    while (1)
    {
        // Read 8-bit input value
        u32 v = XGpio_DiscreteRead(&inGpio, 1);

        int sw   = (int)(v & 0x0F);       // bits 0..3
        int btn0 = (v >> 4) & 1;          // bit 4
        int btn1 = (v >> 5) & 1;          // bit 5
        int btn2 = (v >> 6) & 1;          // bit 6
        int btn3 = (v >> 7) & 1;          // bit 7

        int action = 0;  // 0 idle, 1 inc, 2 dec, 3 show sw, 4 show count press
        int ledVal = lastLed;

        // Priority: BTN2 > BTN0 > BTN1 > BTN3 press
        if (btn2) {
            action = 3;
            ledVal = sw;                 // show switches on LEDs
        }
        else if (btn0) {
            action = 1;
            count = (count + 1) & 0xF;   // keep only 4 bits
            ledVal = count;
            delay_1s();                 // about 1 Hz
        }
        else if (btn1) {
            action = 2;
            count = (count - 1) & 0xF;   // wrap around 0..15
            ledVal = count;
            delay_1s();                 // about 1 Hz
        }
        else {
            // BTN3 edge detect (press)
            if (btn3 && !prevBtn3) {
                action = 4;
                ledVal = count;
            }
        }

        prevBtn3 = btn3;

        // Write LED only if changed (and print)
        if (ledVal != lastLed && ledVal >= 0) {
            XGpio_DiscreteWrite(&ledGpio, 1, (u32)ledVal);
            lastLed = ledVal;
            xil_printf("LED = 0x%x\r\n", lastLed);
        }

        // Print action only if action changed
        if (action != lastAction) {
            lastAction = action;
            if (action == 0) xil_printf("Action: IDLE\r\n");
            if (action == 1) xil_printf("Action: BTN0 INC\r\n");
            if (action == 2) xil_printf("Action: BTN1 DEC\r\n");
            if (action == 3) xil_printf("Action: BTN2 SHOW SWITCHES\r\n");
            if (action == 4) xil_printf("Action: BTN3 SHOW COUNT\r\n");
        }
    }

    return 0;
}
