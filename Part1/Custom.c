#include "xparameters.h"

#include "stdio.h"
#include "xgpio.h"

/* Xilinx version of printf - smaller footprint */
#define printf xil_printf

#define GPIO_DIP_SWITCHES_CHANNEL 1
#define GPIO_LEDS_CHANNEL 2

//==============================================================================
int main(void) {
    int status;
    int statusAll = 0;
    XGpio gpio;
    int i, dip_check;

    printf("-- Entering main() --\r\n");

    /* Initialize the GPIO and check the return value */
    status = XGpio_Initialize(&gpio, XPAR_GPIO_0_DEVICE_ID);

    if(status != XST_SUCCESS) { /* If init failed */
        /* Set the overall status to 1 (failure) */
        statusAll |= 1;
        printf("GPIO initialization failure! \r\n");
    } else { /* If init succeeded */
        /* Set the data direction (in) of the switches) */
        XGpio_SetDataDirection(&gpio, GPIO_DIP_SWITCHES_CHANNEL, 0xFFFF);
        /* Set the data direction (out) of the LEDs) */
		XGpio_SetDataDirection(&gpio, GPIO_LEDS_CHANNEL, 0x0000);
        printf("GPIO initialization success! \r\n");
    }

    if(!statusAll) { /*If everything was okay */
        while(1) {
            /* Read the switches */
            dip_check = XGpio_DiscreteRead(&gpio, GPIO_DIP_SWITCHES_CHANNEL);
            /* Write the LEDS */
			XGpio_DiscreteWrite(&gpio, GPIO_LEDS_CHANNEL, dip_check);

            /* Wait a bit */
            for(i = 0; i < 999999; i++);
        }
    }

    printf("-- Exiting main() --\r\n");
    return statusAll;
}
