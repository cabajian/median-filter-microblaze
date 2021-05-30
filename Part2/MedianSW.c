#include <stdio.h>
#include <stdlib.h>

#include "xio.h"
#include "xparameters.h"
#include "xuartlite_i.h"
#include "xtmrctr.h"

u8 img[256][256];

void bubbleSort(u8 *array, u8 size)
{
  u8 x, y;
  for(x = 0; x < size; x++)
  {
    for(y = 0; y < size - 1; y++)
    {
      if(array[y] > array[y+1])
      {
        u8 temp = array[y+1];
        array[y+1] = array[y];
        array[y] = temp;
      }
    }
  }
}

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

	  int i, j, k, l;
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

	  // process data
	  for(i = 0; i < rows - 2; i++)
	  {
		for(j = 0; j < cols - 2; j++)
		{
		  u8 values[9];

		  // get 3x3 window values and put them into the array
		  for(k = 0; k < 3; k++)
		  {
			for(l = 0; l < 3; l++)
			{
			  values[k*3+l] = img[i+k][j+l];
			}
		  }

		  // sort the values
		  bubbleSort(&(values[0]), 9);

		  // get result of the median filter
		  img[i][j] = values[4];
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
