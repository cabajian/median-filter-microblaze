#include "xtmrctr.h"
#include "xintc.h"
#include "xparameters.h"

#define printf xil_printf
/* Xilinx version of printf - smaller footprint */

// Interrupt frequency
unsigned int timer_count0 = 1;
unsigned int timer_count1 = 4;

unsigned int count0 = 0;
unsigned int count1 = 0;

// Interrupt service routines for the timers

void timer0_int_handler(void * baseaddr_p)
{
    unsigned int csr;

    printf("\n\n\rInterrupt 0 - Count0 = %d", count0);
    count0++;
    printf("\n\rGoing back to main 0 ()");
    csr = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 0);
    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 0, csr);
}

void timer1_int_handler(void * baseaddr_p)
{
    unsigned int csr;

    printf("\n\n\rInterrupt 1 - Count1 = %d", count1);
    count1++;
    printf("\n\rGoing back to main 0 ()");
    csr = XTmrCtr_GetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0);
    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0, csr);
}

int main()
{
    int status0;
    int status1;
    int statusAll = 0;
	XIntc intc;
	XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	XTmrCtr timer0;
	XTmrCtr timer1;
    status0 = XTmrCtr_Initialize(&timer0, XPAR_TMRCTR_0_DEVICE_ID);
    status1 = XTmrCtr_Initialize(&timer1, XPAR_TMRCTR_1_DEVICE_ID);
    if(status0 != XST_SUCCESS || status1 != XST_SUCCESS)
	{
		statusAll |= 1;
		printf("Timer initialization failure!\r\n");
	} else
	{
		XTmrCtr_SetResetValue(&timer0, 0, 0x00000000);
		XTmrCtr_SetResetValue(&timer1, 0, 0x00000000);
		printf("Timer initialization success!\r\n");
	}

    /* Register the Timer interrupt handler in the vector table */
    XIntc_Connect(&intc, XPAR_INTC_0_TMRCTR_0_VEC_ID,
            (XInterruptHandler)timer0_int_handler, (void*)timer0.CallBackRef);

    XIntc_Connect(&intc, XPAR_INTC_0_TMRCTR_1_VEC_ID,
                (XInterruptHandler)timer1_int_handler, (void*)timer1.CallBackRef);

    /* Start the interrupt controller */
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);
    XIntc_EnableIntr(XPAR_INTC_0_BASEADDR, 0x3); // 3 = 0011 to enable both interrupts

    /* Set the number of cycles the timer counts before interrupting */
    XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_0_BASEADDR, 0, timer_count0 * 125000000);
    XTmrCtr_SetLoadReg(XPAR_AXI_TIMER_1_BASEADDR, 0, timer_count1 * 125000000);

    /* Reset the timers and clear interrupts */
    XIntc_EnableIntr(XPAR_AXI_TIMER_0_BASEADDR, XPAR_AXI_TIMER_0_INTERRUPT_MASK);
    XIntc_EnableIntr(XPAR_AXI_TIMER_1_BASEADDR, XPAR_AXI_TIMER_1_INTERRUPT_MASK);

    /* Start the timers */
    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK |
            XTC_CSR_ENABLE_INT_MASK | XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

    XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0, XTC_CSR_ENABLE_TMR_MASK |
            XTC_CSR_ENABLE_INT_MASK | XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK);

    microblaze_enable_interrupts();

    /* Wait for interrupts to occur */

    int i = 0;

    while(1)
    {
        printf(".");
        for(i = 0; i < 99999; i++);
    }
}
