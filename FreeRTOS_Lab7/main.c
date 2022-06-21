
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
#include "timers.h"


#define STACKSIZE 128

TimerHandle_t TimerFunction;

void Timer1(TimerHandle_t TimerFunction)
{
    static u8 state = DIO_LOW;
    DIO_PortWrite(PortF, YELLOW, state);
    state ^= 0xFF;
}

void change_Fading_frequency(void *pvParameters)
{
    char in;
    uint32 period = 100;
    while(1)
    {
        in = UART0_ReadChr();
        UART0_SendChr(in);
        UART0_Println();
        if(in == 's')
        {
            UART0_SendString("State: ");
            UART0_SendString("Stop");
            xTimerStop(TimerFunction, 1000);
        }
        else if (in == 'r')
        {
            UART0_SendString("State: ");
            UART0_SendString("Reset");
            xTimerReset(TimerFunction, 1000);
        }
        else if(in == 'i')
        {
            period /= 2;
            UART0_SendString("State: ");
            UART0_SendString("change period-> ");
            UART0_SendInt(period);
            xTimerChangePeriod(TimerFunction, period, 1000);
        }
        else if(in == 'd')
        {
            period *= 2;
            UART0_SendString("State: ");
            UART0_SendString("change period-> ");
            UART0_SendInt(period);
            xTimerChangePeriod(TimerFunction, period, 1000);
        }
    }

}
int main(void)
{
    PLL_Set80();
    UART0_Init(115200, 80000000);
    DIO_PortInit(PortF, Pin1|Pin2|Pin3, 0);
    DIO_PortDirection(PortF, Pin1|Pin2|Pin3, DIO_OUTPUT);

    TaskHandle_t TaskHandle;
    xTaskCreate(change_Fading_frequency, "Freq_speed", STACKSIZE, NULL,1, &TaskHandle);

    TimerFunction = xTimerCreate("MyTimer",100, pdTRUE,( void * ) 0, Timer1);
	xTimerStart(TimerFunction, 0);
	vTaskStartScheduler();
	return 0;
}

