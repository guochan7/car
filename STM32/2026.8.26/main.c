#include "stm32f10x.h"
#include "sys.h"
#include "motor.h"

int main(void)
{
    RCC->CSR |= 1 << 24;              // 清除复位标志
    Stm32_Clock_Init(9);              // 外部时钟 8 MHz，9 倍频，系统时钟 72 MHz
    MY_NVIC_PriorityGroupConfig(2);   // 中断优先级分组
    uart_init(115200);                // 串口初始化为 115200
    JTAG_Set(JTAG_SWD_DISABLE);       // 关闭 JTAG 接口
    JTAG_Set(SWD_ENABLE);             // 打开 SWD 接口，可用 SWD 调试

    PWM_Init(7199, 9);                // 定时器 PWM 初始化，频率约 1000 Hz
    colorful_led_Init();              // 炫彩灯初始化（由工程其他文件提供）
    printf("QST青软\\r\\n");

    /* 主要程序 */
    while (1) {
        Set_Pwm(2500, 2500);          // 设置左右轮速度
        delay_ms(100);
    }
}
