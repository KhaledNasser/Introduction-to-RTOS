
// include ARM Libraries
#include "DIO.h"
#include "PLL.h"
#include "Reg.h"
#include "Types.h"
#include "UART.h"


// include FreeRTOS libraries
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define STACKSIZE 128
#define WATERCONDITION  (O_COUNT >= 1) && (H_COUNT >= 2)

SemaphoreHandle_t Mutex1;
volatile u8 data1 = 0, data2 = 0;
static char O_COUNT, H_COUNT;


void O_Task(void *pvParameters)
{
    Mutex1 = xSemaphoreCreateMutex();
    u8 flag_low = Pin0;
    while(1)
    {
        DIO_PortRead(PortF, Pin0, &data1);
        xSemaphoreTake(Mutex1, (TickType_t)portMAX_DELAY);
        if((data1 == DIO_LOW) && (flag_low == Pin0))
        {
            O_COUNT++;
            UART0_SendString("O pressed!");
            UART0_SendInt(O_COUNT);
            UART0_Println();
        }
        while(WATERCONDITION)
        {
            O_COUNT--;
            H_COUNT-=2;
            UART0_SendString("Water Created! Remaining particles ");
            UART0_SendString("O: ");
            UART0_SendInt(O_COUNT);
            UART0_SendString(" H: ");
            UART0_SendInt(H_COUNT);
            UART0_Println();
        }
        flag_low = data1;
        xSemaphoreGive(Mutex1);
        vTaskDelay(1);
    }
}

void H_Task(void *pvParameters)
{
    u8 flag_low = Pin4;
    while(1)
    {
        DIO_PortRead(PortF, Pin4, &data2);
        xSemaphoreTake(Mutex1, (TickType_t)portMAX_DELAY);
        if((data2 == DIO_LOW) && (flag_low == Pin4))
        {
            H_COUNT++;
            UART0_SendString("H is pressed!");
            UART0_SendInt(H_COUNT);
            UART0_Println();
        }
        while(WATERCONDITION)
        {
            H_COUNT-=2;
            O_COUNT--;
            UART0_SendString("Water created! Particles Remaining ");
            UART0_SendString("O: ");
            UART0_SendInt(O_COUNT);
            UART0_SendString(" H: ");
            UART0_SendInt(H_COUNT);
            UART0_Println();
        }
        flag_low = data2;
        xSemaphoreGive(Mutex1);
        vTaskDelay(1);
    }

}

int main(void)
{
    O_COUNT = 0;
    H_COUNT = 0;
    PLL_Set80();
    UART0_Init(115200, 80000000);
    DIO_PortInit(PortF, Pin0|Pin1|Pin2|Pin3|Pin4, Pin0|Pin4);
    DIO_PortDirection(PortF, Pin1|Pin2|Pin3, DIO_OUTPUT);
    DIO_PortDirection(PortF, Pin0|Pin4, DIO_INPUT);

    TaskHandle_t OHandle, HHandle;
    xTaskCreate(O_Task, "O_Task", STACKSIZE, NULL, 2, &OHandle);
    xTaskCreate(H_Task, "H_Task", STACKSIZE, NULL, 1, &HHandle);

    vTaskStartScheduler();


	return 0;
}
