
基于 STM32F103C8 和 Keil µVision 的双电机基础运动测试程序。程序上电后控制小车低速前进一段时间，短暂停车，再低速后退，最后保持停止。

本项目主要用于验证以下部分是否正常：

- STM32 主控程序烧录；
- 电机驱动板与电池供电；
- 左右电机接线及旋转方向；
- TIM4 PWM 输出；
- 小车基础前进、后退功能。

> 当前版本为开环电机测试，不使用编码器和 PID 闭环控制。

## 运行效果

程序启动后依次执行：

1. 低速前进 1 秒；
2. 停车 0.5 秒；
3. 低速后退 1 秒；
4. 停车并保持。

## 硬件环境

- STM32F103C8 主控板；
- QST 智能小车底盘；
- 双路电机驱动模块；
- 两个直流减速电机；
- 小车电池；
- ST-Link 下载器。

## 软件环境

- Keil µVision / MDK-ARM；
- STM32F10x Standard Peripheral Library；
- ST-Link 驱动程序。

## 主要引脚

| 功能 | STM32 引脚/外设 |
| --- | --- |
| 电机 PWM A | PB6 / TIM4_CH1 |
| 电机 PWM B | PB7 / TIM4_CH2 |
| 电机方向 A | PB14 |
| 电机方向 B | PB13 |
| 串口发送 | PA9 / USART1_TX |
| 串口接收 | PA10 / USART1_RX |

具体方向与电机编号以实际小车接线为准。

## 核心代码

```c
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
    Stm32_Clock_Init(9);
    MY_NVIC_PriorityGroupConfig(2);
    uart_init(115200);
    JTAG_Set(SWD_ENABLE);

    PWM_Init(7199, 9);
    colorful_led_Init();

    printf("motor test start\r\n");

    /* 前进 1 秒。 */
    Set_Pwm(TEST_PWM, -TEST_PWM);
    delay_ms(MOVE_TIME_MS);

    /* 停车 0.5 秒。 */
    Set_Pwm(0, 0);
    delay_ms(STOP_TIME_MS);

    /* 后退 1 秒。 */
    Set_Pwm(-TEST_PWM, TEST_PWM);
    delay_ms(MOVE_TIME_MS);

    /* 停车并保持。 */
    Set_Pwm(0, 0);
    printf("motor test finished\r\n");

    while (1)
    {
    }
}
```

## 参数设置

```c
#define TEST_PWM          1500
#define MOVE_TIME_MS      1000
#define STOP_TIME_MS       500
```

| 参数 | 说明 |
| --- | --- |
| `TEST_PWM` | 电机测试速度，首次运行建议使用较小值 |
| `MOVE_TIME_MS` | 前进和后退的持续时间，单位为毫秒 |
| `STOP_TIME_MS` | 前进与后退之间的停车时间，单位为毫秒 |

PWM 计数周期为 `7199`，请勿让输出值超过 `-7199` 到 `7199`。

## 工程文件要求

µVision 工程至少需要包含：

- `main.c`；
- `motor.c` 和 `motor.h`；
- `sys.c` 和 `sys.h`；
- `delay.c` 和 `delay.h`；
- `usart.c` 和 `usart.h`；
- STM32F10x CMSIS 与所需标准外设库文件。

确保相关头文件目录已经加入：

`Options for Target → C/C++ → Include Paths`

## 编译和烧录

1. 使用 Keil µVision 打开 STM32 工程。
2. 用仓库中的 `main.c` 替换原文件。
3. 确认工程中只有一个 `main()` 函数。
4. 点击 Build，确保显示 `0 Error(s)`。
5. 使用 ST-Link 连接 STM32 主板。
6. 选择 `Flash → Download`，或按 `F8`。
7. 等待输出窗口显示：

```text
Erase Done.
Programming Done.
Verify OK.
```

## 安全试车

1. 首次测试必须将车轮架空。
2. 烧录时暂时不要接通电机电池。
3. 烧录成功后拔掉电脑数据线。
4. 接通小车电池并打开电源。
5. 按下 STM32 主板复位键。
6. 确认两个车轮的方向正确后，再把小车放到地面。

## 常见问题

### 两个车轮都不转

- 检查电池是否有电及电源开关是否打开；
- 检查电机驱动板是否获得电机电源；
- 确认 `motor.c` 已加入 µVision 工程；
- 确认烧录结果包含 `Verify OK`；
- 检查电机插头和驱动板连接。

### 小车原地旋转

某侧电机方向与程序设定相反。检查电机插头方向，或者调整该侧 `Set_Pwm()` 参数的正负号。

### 前进和后退完全相反

交换两次运动指令的正负号：

```c
Set_Pwm(-TEST_PWM, TEST_PWM);
Set_Pwm(TEST_PWM, -TEST_PWM);
```

### 电机转速过快

降低 `TEST_PWM`，例如：

```c
#define TEST_PWM 1000
```

如果电机突然高速旋转或出现剧烈抖动，请立即断开电池。
