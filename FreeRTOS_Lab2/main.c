// include ARM libraries
#include "DIO.h"
#include "Reg.h"
#include "PLL.h"
#include "Types.h"
#include "UART.h"   // for Serial communication

// include FreeRTOS libraries
#include "FreeRTOS.h"
#include "task.h"
#define SWITCHTASKSTACKSIZE        128

u8 speed = 1; // global variable, shared data problem

volatile uint32 data = 0;

struct Two_Handle{
    TaskHandle_t first_task;
    TaskHandle_t second_task;
}handles;

// first task toggle led
void LED (void *pvParameters)
{
    u8 state = DIO_LOW;
    TickType_t xLastWackTime;
    xLastWackTime = xTaskGetTickCount();
    while(1)
    {
        DIO_PortWrite(PortF, CYAN, state);
        state ^= 0xFF;
        vTaskDelayUntil(&xLastWackTime, 100*speed);
    }
}
// second task get data from Push button
void Button (void * pvParameters)
{
    u8 flag_low = Pin0;
    while(1)
    {
        DIO_PortRead(PortF, Pin0, &data);
        if((flag_low == Pin0) && (data == DIO_LOW)){
            speed += 2;
            speed = (speed >10) ? 1:speed;
        }
        flag_low = data;
        vTaskDelay(25);// this task will block for 25 tick as 25 ms
    }
}
// third task print data on serial
void TaskState (void * pvParameters)
{
    TaskHandle_t state1 = ((struct Two_Handle*) pvParameters) -> first_task;
    TaskHandle_t state2 = ((struct Two_Handle*) pvParameters) -> second_task;
    TaskStatus_t Details1, Details2;
    TaskStatus_t *pvTaskStatusArray;
    volatile UBaseType_t number, x;
    unsigned long ulTotalRunTime;
    number = uxTaskGetNumberOfTasks();
    pvTaskStatusArray = pvPortMalloc(number * sizeof(TaskStatus_t));
    uxTaskGetSystemState(pvTaskStatusArray, number, &ulTotalRunTime);
    UART0_SendString("Number of Tasks = ");
    UART0_SendInt(number);
    UART0_Println();
    for(x=0; x < number; x++)
    {
        UART0_SendString(pvTaskStatusArray[x].pcTaskName);
        UART0_Println();

    }
    while(1)
    {
        vTaskGetInfo(state1, &Details1, pdTRUE, eInvalid);
        vTaskGetInfo(state2, &Details2, pdTRUE, eInvalid);
        UART0_SendString(Details1.pcTaskName);
        UART0_Println();
        UART0_SendInt(Details1.eCurrentState);
        UART0_Println();
        UART0_SendString(Details2.pcTaskName);
        UART0_Println();
        UART0_SendInt(Details2.eCurrentState);
        UART0_Println();
        vTaskDelay(1000);
    }

}
int main(void)
{
    PLL_Set80();
    UART0_Init(115200, 80000000);
    DIO_PortInit(PortF, Pin0|Pin1|Pin2|Pin3|Pin4 , Pin0|Pin4);
    DIO_PortDirection(PortF , Pin1|Pin2|Pin3, DIO_OUTPUT);
    DIO_PortDirection(PortF , Pin0|Pin4, DIO_INPUT);

    TaskHandle_t FirstTask, SecondTask, ThirdTask;
    xTaskCreate(LED, "LED TAsk",  SWITCHTASKSTACKSIZE, NULL, 1, &FirstTask);
    xTaskCreate(Button, "Button Task", SWITCHTASKSTACKSIZE, NULL, 2, &SecondTask);
    handles.first_task = FirstTask;
    handles.second_task = SecondTask;
    xTaskCreate(TaskState, "Statistics", SWITCHTASKSTACKSIZE, (void *)(&handles), 3, &ThirdTask);

    vTaskStartScheduler();
	return 0;
}

