// include Arm libraries
#include "DIO.h"
#include "Reg.h"
#include "PLL.h"
#include "Types.h"

// include FreeRTOS libraries
#include "FreeRTOS.h"
#include "task.h"
#define LED_1_2_STACK 128

// first task blink led1
void LED1(void* pvParameters)
{
    while(1)
    {
        DIO_PortWrite(PortF, GREEN, DIO_HIGH);
        vTaskDelay(100);
        DIO_PortWrite(PortF, GREEN, DIO_LOW);
        vTaskDelay(100);
    }
}

void LED2(void* pvParameters)
{
    u8 state = DIO_LOW;
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        DIO_PortWrite(PortB,CYAN,state);
        state ^= 0xFF;
        vTaskDelayUntil( &xLastWakeTime, 1000);

    }
}

int main(void)
{
    PLL_Set80();
    DIO_PortInit(PortF, Pin1|Pin2|Pin3, 0);
    DIO_PortDirection(PortF, Pin1|Pin2|Pin3, DIO_OUTPUT);

    DIO_PortInit(PortB, Pin0, 0);
    DIO_PortDirection(PortB, Pin0, DIO_OUTPUT);

    TaskHandle_t LED1_Handle, LED2_Handle;
    xTaskCreate(LED1, "LED1_blink", LED_1_2_STACK, NULL, 1, &LED1_Handle);
    xTaskCreate(LED2, "Led2_blink", LED_1_2_STACK, NULL, 1, &LED2_Handle);

    vTaskStartScheduler();
    while(1);

	return 0;
}
