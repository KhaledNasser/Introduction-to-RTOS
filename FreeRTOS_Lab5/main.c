// include ARM libraries
#include "DIO.h"
#include "Reg.h"
#include "PLL.h"
#include "Types.h"
#include "UART.h"

// include FreeRTOS libraries
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define STACKSIZE 128

SemaphoreHandle_t Client_Sem;

volatile u8 data = 0;
void Button(void *pvParameters)
{
    u8 flow_flag = Pin0;
    while(1)
    {
        DIO_PortRead(PortF, Pin0, &data);
        if ((data == DIO_LOW) && (flow_flag == Pin0))
        {
            if(xSemaphoreGive(Client_Sem) != pdTRUE)
            {
            }
        }
        flow_flag = data;
    }
}

void Server(void *pvParameters)
{
    Client_Sem = xSemaphoreCreateBinary();
    xSemaphoreTake(Client_Sem, (TickType_t)0);
    uint32 i = 0;

    while(1)
    {
        xSemaphoreTake(Client_Sem, (TickType_t)portMAX_DELAY);
        UART0_SendString("Client Number: ");
        UART0_SendInt(++i);
        UART0_SendString(" Served!");
        UART0_Println();
    }
}


int main(void)
{
    PLL_Set80();
    UART0_Init(115200, 80000000);
    DIO_PortInit(PortF, Pin0|Pin1|Pin2|Pin3|Pin4, Pin0|Pin4);
    DIO_PortDirection(PortF, Pin1|Pin2|Pin3, DIO_OUTPUT);
    DIO_PortDirection(PortF, Pin0|Pin4, DIO_INPUT);

    TaskHandle_t ButtonHandle, ServerHandle;
    xTaskCreate(Button, "Button", STACKSIZE, NULL, 1, &ButtonHandle);
    xTaskCreate(Server, "Server", STACKSIZE, NULL, 2, &ServerHandle);

    vTaskStartScheduler();


	return 0;
}
