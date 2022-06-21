// include ARM libraries
#include "DIO.h"
#include "Reg.h"
#include "PLL.h"
#include "Types.h"

// include freeRTOS libraries
#include "FreeRTOS.h"
#include "task.h"
#define FIRST_STACK 128  // define stack size in words

// create delay function
void Delay(void)      // just for prototype we will use it, but it is not good practice
{
    int i =1000000;
    while (i > 0)
    {
        i--;
    }

}

// create blink task
void First_Task(void *pvParameters)
{
    while(1)
    {
        DIO_PortWrite(PortF, CYAN, DIO_HIGH);   // Turn on built in led
        Delay();
        DIO_PortWrite(PortF, CYAN, DIO_LOW);    // Turn off built in led
        Delay();
    }
}

int main(void)
{
    PLL_Set80();  // set frequency of MCU to 80MHZ
    DIO_PortInit(PortF, Pin1|Pin2|Pin3, 0); // Init pins from 1 to 3 in PortF, as Pull up
    DIO_PortDirection(PortF, Pin1|Pin2|Pin3, DIO_OUTPUT); // set Pin direction to output
    TaskHandle_t First_Handle;             // create handler to create or delete task
    xTaskCreate(First_Task, "Lab0_Blink", FIRST_STACK, NULL, 1, &First_Handle);   // create blinking task
    vTaskStartScheduler();  // start scheduler
    while(1);

	return 0;
}



