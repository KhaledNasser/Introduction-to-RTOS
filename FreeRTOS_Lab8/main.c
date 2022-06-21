// include Arm libraries
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
#include "event_groups.h"

#define STACKSIZE 256

EventGroupHandle_t Group;
volatile uint32 data1,data2;

void Task1(void *pvParameters)
{
    u8 state = DIO_HIGH;
    while(1)
    {
        xEventGroupWaitBits(Group,Pin0|Pin4 ,pdTRUE,pdTRUE,(TickType_t)portMAX_DELAY );
        DIO_PortWrite(PortF,YELLOW,state);
        state ^= 0xFF;
    }
}
void Button1(void *pvParameters)
{
    u8 flag_low =  Pin4;
    while(1)
    {
        DIO_PortRead(PortF,Pin4,&data2);

        if((data2 == DIO_LOW) && (flag_low == Pin4)) //active low , if button is pressed , and last time was high
        {
            xEventGroupSetBits(Group,Pin4 );
        }
        flag_low = data2;
        vTaskDelay(10);

    }

}
void Button2(void *pvParameters)
{
    u8 flag_low =  Pin0;
    while(1)
    {
        DIO_PortRead(PortF,Pin0,&data1);
        if((data1 == DIO_LOW) && (flag_low == Pin0)) //active low , if button is pressed , and last time was high
        {
             xEventGroupSetBits(Group,Pin0 );
        }
        flag_low = data1;
        vTaskDelay(10);
    }
}
int main(void)
{
    PLL_Set80();
    UART0_Init(115200,80000000);
    DIO_PortInit(PortF, Pin0|Pin1|Pin2|Pin3|Pin4 , Pin0|Pin4);
    DIO_PortDirection(PortF , Pin1|Pin2|Pin3, DIO_OUTPUT);
    DIO_PortDirection(PortF , Pin0|Pin4, DIO_INPUT);
    TaskHandle_t  First_handle,Second_handle,Third_handle;
    Group = xEventGroupCreate();
    xTaskCreate(Button1, "Button1",STACKSIZE, NULL,1, &First_handle);
    xTaskCreate(Button2, "Button2",STACKSIZE, NULL,2, &Second_handle);
    xTaskCreate(Task1, "Task1",STACKSIZE, NULL,3, &Third_handle);

    vTaskStartScheduler();
    return 0;
}
