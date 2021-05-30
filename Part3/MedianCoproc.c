#include <stdio.h>
#include <stdlib.h>

#include "xio.h"
#include "xparameters.h"
#include "xuartlite_i.h"
#include "xtmrctr.h"
#include "fsl.h"

u8 img[256][256];

/* Get macro for the stream interface
 * This macro blocks the main code, but allows interrupts to happen
 * We only use interface 0, so that has been hard coded.
 * val is the destination variable (NOT a pointer to)
 */
#define getstream(val) getfsl_interruptible(val, 0)

/* Get macro for the stream interface
 * This macro blocks the main code, but allows interrupts to happen
 * We only use interface 0, so that has been hard coded.
 * val is the input variable (NOT a pointer to, must be a variable not an immediate)
 */
#define putstream(val) putfsl_interruptible(val, 0)

int main()
{

  int status;
  int statusAll = 0;
  XTmrCtr XPS_Timer;
  unsigned int cycles1, cycles2, cycles;
  status = XTmrCtr_Initialize(&XPS_Timer, XPAR_TMRCTR_0_DEVICE_ID);
  if(status != XST_SUCCESS)
  {
	  statusAll |= 1;
  } else
  {
	  XTmrCtr_SetResetValue(&XPS_Timer, 0, 0x00000000);
  }

  if (!statusAll) {

	  XTmrCtr_Reset(&XPS_Timer, 0);
	  XTmrCtr_Start(&XPS_Timer, 0);

	  u16 rows, cols;
	  u8 val;

	  // read cols
	  val = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
	  cols = val;
	  val = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
	  cols = cols + (val << 8);

	  // read rows
	  val = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
	  rows = val;
	  val = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
	  rows = rows + (val << 8);

	  cycles1 = XTmrCtr_GetValue(&XPS_Timer, 0);

	  int i, j;
	  for(i = 0; i < rows; i++)
	  {
		for(j = 0; j < cols; j++)
		{
		  // read value
		  val = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
		  // store in BRAM
		  img[i][j] = val;
		}
	  }

	  cycles2 = XTmrCtr_GetValue(&XPS_Timer, 0);
	  cycles = cycles2-cycles1;
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)cycles);
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 8));
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 16));
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 24));
	  cycles1 = XTmrCtr_GetValue(&XPS_Timer, 0);

	  // process data using coprocessor (AXI stream)
	  for(i = 0; i < rows - 2; i++)
	  {
		for(j = 0; j < cols - 2; j++)
		{
		  int med;
		  // get 1x3 window values and send over stream
		  putstream((int)img[i+0][j+1]);
		  putstream((int)img[i+1][j+1]);
		  putstream((int)img[i+2][j+1]);

		  // get result of the median filter
		  getstream(med);
		  img[i][j] = (u8)med;
		}
	  }

	  cycles2 = XTmrCtr_GetValue(&XPS_Timer, 0);
	  cycles = cycles2-cycles1;
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)cycles);
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 8));
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 16));
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 24));
	  cycles1 = XTmrCtr_GetValue(&XPS_Timer, 0);

	  // send the data back
	  for(i = 0; i < rows; i++)
	  {
		for(j = 0; j < cols; j++)
		{
		  // read value from mem
		  val = img[i][j];
		  // transmit the value
		  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, val);
		}
	  }

	  cycles2 = XTmrCtr_GetValue(&XPS_Timer, 0);
	  cycles = cycles2-cycles1;
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)cycles);
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 8));
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 16));
	  XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, (u8)(cycles >> 24));
  }

  return 0;
}
