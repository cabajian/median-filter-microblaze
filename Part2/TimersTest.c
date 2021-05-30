#include "xparameters.h"
#include "stdio.h"
#include "xtmrctr.h"

/* Xilinx version of printf - smaller footprint */
#define printf xil_printf

//==============================================================================
int main(void)
{
    printf("-- Entering main() --\r\n");

    int status;
    int statusAll = 0;

    int i;
    XTmrCtr XPS_Timer;
    unsigned int cycles1, cycles2, cycles3;
    unsigned int loop1, loop2;
    int whole, frac;
    float xx;

    status = XTmrCtr_Initialize(&XPS_Timer, XPAR_TMRCTR_0_DEVICE_ID);
    if(status != XST_SUCCESS)
    {
        statusAll |= 1;
        printf("Timer initialization failure!\r\n");
    } else
    {
        XTmrCtr_SetResetValue(&XPS_Timer, 0, 0x00000000);
        printf("Timer initialization success!\r\n");
    }

    if(!statusAll)
    {
        XTmrCtr_Reset(&XPS_Timer, 0);
        XTmrCtr_Start(&XPS_Timer, 0);

        printf("-----------------------------------------------------------\r\n");
        cycles1 = XTmrCtr_GetValue(&XPS_Timer, 0);
        printf("Cycle count 1 - %10d\r\n", cycles1);

        /* Loop 1 */
        for(i = 0; i < 999999; i++);

        cycles2 = XTmrCtr_GetValue(&XPS_Timer, 0);
        printf("Cycle count 2 - %10d\r\n", cycles2);

        /* Loop 2 */
        for(i = 0; i < 4999999; i++);

        cycles3 = XTmrCtr_GetValue(&XPS_Timer, 0);
        printf("Cycle count 3 - %10d\r\n", cycles3);

        printf("-----------------------------------------------------------\r\n");
        printf("Profiling\r\n");

        loop1 = cycles2-cycles1;
        printf("Loop 1 timing = %d\r\n", loop1);

        loop2 = cycles3-cycles2;
        printf("Loop 2 timing = %d\r\n", loop2);

        xx = (float)loop1/loop2;
        whole = xx;
        frac = (xx-whole)*1000;
        printf("Ratio Loop 1 to Loop 2 = %d.%3d\r\n", whole, frac);

        printf("***********************************************************\r\n");
    }
    printf("-- Exiting main() --\r\n");
    return statusAll;
}
