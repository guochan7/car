
#include "stm32f10x.h"
#include "sys.h"
#include "motor.h"
#include "colorful_led.h"
#include "usart.h"
#include <stdio.h>

#define TEST_PWM          1500
#define MOVE_TIME_MS      1000
#define STOP_TIME_MS       500

int main(void)
{
    Stm32_Clock_Init(9);              /* 8 MHz x 9 = 72 MHz */
    MY_NVIC_PriorityGroupConfig(2);
    uart_init(115200);

    /* 保留 SWD 下载和调试接口。 */
    JTAG_Set(SWD_ENABLE);

    /* TIM4 PWM：72 MHz / 10 / 7200 = 1 kHz。 */
    PWM_Init(7199, 9);
    colorful_led_Init();

    printf("motor test start\r\n");

    /* 小车低速前进 1 秒。 */
    Set_Pwm(TEST_PWM, -TEST_PWM);
    delay_ms(MOVE_TIME_MS);

    /* 停车 0.5 秒。 */
    Set_Pwm(0, 0);
    delay_ms(STOP_TIME_MS);

    /* 小车低速后退 1 秒。 */
    Set_Pwm(-TEST_PWM, TEST_PWM);
    delay_ms(MOVE_TIME_MS);

    /* 停车并保持。 */
    Set_Pwm(0, 0);
    printf("motor test finished\r\n");

    while (1)
    {
    }
}
