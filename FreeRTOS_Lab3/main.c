// THIS CODE IS EXAMPLE OF SHARED DATA PROBLEM IN INCREMENTAL TASK IF WE UNCOMMENT CRITCAL FUNCTION IT WILL BE SOLVED BUT
// THIS SOLUTION IS DUMMY THAT WE DISABLE INTERRUPTS TILL THE TASK IS COMPLETE

// include ARM libraries
#include "DIO.h"
#include "Reg.h"
#include "PLL.h"
#include "Types.h"
#include "UART.h"

// include FreeRTOS libraries
#include "FreeRTOS.h"
#include "task.h"

#define STACK_TASK 128

uint32 x=0;
uint32 y=0;

 void incremental(void *pvParameters)
 {
     while(1)
     {
         taskENTER_CRITICAL();
         x++;
         y++;
         taskEXIT_CRITICAL();
     }
 }

 void checker(void *pvParameters)
 {
     while(1)
     {
         if(x != y){
             UART0_SendString("X and Y not equal");
             UART0_Println();
         }
         vTaskDelay(47);
     }
 }

int main(void)
{
    PLL_Set80();
    UART0_Init(115200, 80000000);
    DIO_PortInit(PortF, Pin0|Pin1|Pin2|Pin3|Pin4 , Pin0|Pin4);
    DIO_PortDirection(PortF , Pin1|Pin2|Pin3, DIO_OUTPUT);
    DIO_PortDirection(PortF , Pin0|Pin4, DIO_INPUT);
    TaskHandle_t incremental1, checker1;
    xTaskCreate(incremental, "Task1:increment", STACK_TASK, NULL, 1, &incremental1);
    xTaskCreate(checker, "Task2:checker", STACK_TASK, NULL, 2, &checker1);

    vTaskStartScheduler();

	return 0;
}

