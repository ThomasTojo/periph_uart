
#include <string.h>
#include "chip.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#if (defined(BOARD_HITEX_EVA_1850) || defined(BOARD_HITEX_EVA_4350))
#define UARTNum 0

#elif (defined(BOARD_KEIL_MCB_1857) || defined(BOARD_KEIL_MCB_4357))
#define UARTNum 3

#elif (defined(BOARD_NGX_XPLORER_1830) || defined (BOARD_NGX_XPLORER_4330))
#define UARTNum 0

#elif defined(BOARD_NXP_LPCLINK2_4370)
#define UARTNum 2

#elif defined(BOARD_NXP_LPCXPRESSO_4337)
#define UARTNum 0

#else
#error No UART selected for undefined board
#endif

#if (UARTNum == 0)
#define LPC_UART LPC_USART0
#define UARTx_IRQn  USART0_IRQn
#define UARTx_IRQHandler UART0_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART0_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART0_Rx
#elif (UARTNum == 1)
#define LPC_UART LPC_UART1
#define UARTx_IRQn  UART1_IRQn
#define UARTx_IRQHandler UART1_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART1_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART1_Rx
#elif (UARTNum == 2)
#define LPC_UART LPC_USART2
#define UARTx_IRQn  USART2_IRQn
#define UARTx_IRQHandler UART2_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART2_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART2_Rx
#elif (UARTNum == 3)
#define LPC_UART LPC_USART3
#define UARTx_IRQn  USART3_IRQn
#define UARTx_IRQHandler UART3_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART3_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART3_Rx
#endif

uint8_t buffer[10];
uint8_t count = 0;

SemaphoreHandle_t xSemaphore = NULL;

static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

/*  freeRTOS Tasks	*/

// vUART_ReadTask1 function
static void vUART_ReadTask1(void *pvParameters) {
	bool LedState = false;
	char InputByte;
    if ( (xSemaphore = xSemaphoreCreateMutex())==NULL )
    	DEBUGOUT("FAiled to create the semaphoreMutex");
	while(1)
	{
			if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE  )
			{
				if(count<10)
						{
							if( Chip_UART_Read(LPC_UART, &InputByte, 1)>0 )
							{
								buffer[count] = InputByte;
								count++;
								Board_LED_Set(6, LedState);			//led6 off
								LedState = (bool) !LedState;
							}
						}
				xSemaphoreGive(xSemaphore);
				Board_LED_Set(4, LedState);			//led6 off
				LedState = (bool) !LedState;
			}



		/* About a 50Hz delay */
		vTaskDelay(configTICK_RATE_HZ /1000);
	}
}

// vUART_WriteTask2 function
static void vUART_WriteTask2(void *pvParameters) {
	bool LedState = false;
	char str[] = "\r\nYou typed:";

	while(xSemaphore == NULL)
		vTaskDelay(100);
	while(1)
	{
			if(  xSemaphoreTake( xSemaphore, ( TickType_t )10 ) == pdTRUE  )
			{
				if(count>0)
				{
					Chip_UART_SendBlocking(LPC_UART, str, strlen(str));
					Chip_UART_SendBlocking(LPC_UART, buffer, count);
					Chip_UART_SendBlocking(LPC_UART, "\r\n", strlen("\r\n"));
					count=0;
				}
				xSemaphoreGive(xSemaphore);
			}
		Board_LED_Set(1, LedState);			//led1 off
		LedState = (bool) !LedState;

		/* About a 0.5Hz delay */
		vTaskDelay(configTICK_RATE_HZ /1);
	}
}



/**
 * @brief	Main UART program body
 * @return	Always returns -1
 */
int main(void)
{
	SystemCoreClockUpdate();
	Board_Init();
	Board_UART_Init(LPC_UART);

#if !((defined(CHIP_LPC43XX) && defined(BOARD_KEIL_MCB_18574357) && UARTNum==3) || ((!(defined(CHIP_LPC43XX) && defined(BOARD_KEIL_MCB_18574357))) && UARTNum==0))
	Chip_UART_Init(LPC_UART);
	Chip_UART_SetBaud(LPC_UART, 115200);
	Chip_UART_ConfigData(LPC_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT); /* Default 8-N-1 */

	/* Enable UART Transmit */
	Chip_UART_TXEnable(LPC_UART);
#endif
	Chip_UART_SetupFIFOS(LPC_UART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
							UART_FCR_TX_RS | UART_FCR_TRG_LEV0));

	//free RTOS hardware setup and task creation
	prvSetupHardware();


	/* uart read task */
	xTaskCreate(vUART_ReadTask1, "vUART_ReadTask1", configMINIMAL_STACK_SIZE,
				NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* uart write task */
	xTaskCreate(vUART_WriteTask2, "vUART_WriteTask2", configMINIMAL_STACK_SIZE,
			NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	return 0;
}
