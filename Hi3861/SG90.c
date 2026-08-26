#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "hi_io.h"
#include "hi_time.h"

#define GPIO2 2

static osMutexId_t servo_mutex;
static osSemaphoreId_t task3_start_sem;
static osSemaphoreId_t task2_start_sem;

static void thread1(void);
static void thread2(void);
static void thread3(void);

/* 输出一个周期为 20 ms 的 SG90 控制脉冲。 */
static void set_angle(unsigned int duty)
{
    GpioSetOutputVal(GPIO2, WIFI_IOT_GPIO_VALUE1);
    hi_udelay(duty);
    GpioSetOutputVal(GPIO2, WIFI_IOT_GPIO_VALUE0);
    hi_udelay(20000 - duty);
}

static void engine_run_45(void) /* 左转 45 度 */
{
    for (int i = 0; i < 10; i++) {
        set_angle(1000);
    }
}

static void engine_run_90(void) /* 舵机居中 */
{
    for (int i = 0; i < 10; i++) {
        set_angle(1500);
    }
}

static void engine_run_135(void) /* 右转 45 度 */
{
    for (int i = 0; i < 10; i++) {
        set_angle(2000);
    }
}

/* 任务 1：最先运行，左转 45 度；保持互斥锁 3 秒。 */
static void thread1(void)
{
    osMutexAcquire(servo_mutex, osWaitForever);
    printf("任务1开始运行\r\n");
    engine_run_45();

    osDelay(3000); /* 任务 3 必须在任务 1 运行 3 秒后才可开始 */
    osMutexRelease(servo_mutex);
    osSemaphoreRelease(task3_start_sem);
}

/* 任务 3：收到任务 1 的信号后运行，右转 45 度。 */
static void thread3(void)
{
    osSemaphoreAcquire(task3_start_sem, osWaitForever);
    osMutexAcquire(servo_mutex, osWaitForever);
    printf("任务3开始运行\r\n");
    printf("任务3开始运行\r\n");
    engine_run_135();
    osMutexRelease(servo_mutex);

    osSemaphoreRelease(task2_start_sem);
}

/* 任务 2：任务 3 完成后立即运行，舵机回到中间。 */
static void thread2(void)
{
    osSemaphoreAcquire(task2_start_sem, osWaitForever);
    osMutexAcquire(servo_mutex, osWaitForever);
    printf("任务2开始运行\r\n");
    printf("任务2开始运行\r\n");
    printf("任务2开始运行\r\n");
    engine_run_90();
    osMutexRelease(servo_mutex);
}

static void SG90(void)
{
    GpioInit();
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_IO_FUNC_GPIO_2_GPIO);
    GpioSetDir(GPIO2, WIFI_IOT_GPIO_DIR_OUT);

    servo_mutex = osMutexNew(NULL);
    task3_start_sem = osSemaphoreNew(1, 0, NULL);
    task2_start_sem = osSemaphoreNew(1, 0, NULL);
    if (servo_mutex == NULL || task3_start_sem == NULL || task2_start_sem == NULL) {
        printf("创建同步对象失败\r\n");
        return;
    }

    osThreadAttr_t attr = {
        .attr_bits = 0U,
        .cb_mem = NULL,
        .cb_size = 0U,
        .stack_mem = NULL,
        .stack_size = 1024 * 4,
        .priority = 25, /* 三个任务使用同一优先级 */
    };

    attr.name = "thread1";
    if (osThreadNew((osThreadFunc_t)thread1, NULL, &attr) == NULL) {
        printf("创建任务1失败\r\n");
    }
    attr.name = "thread3";
    if (osThreadNew((osThreadFunc_t)thread3, NULL, &attr) == NULL) {
        printf("创建任务3失败\r\n");
    }
    attr.name = "thread2";
    if (osThreadNew((osThreadFunc_t)thread2, NULL, &attr) == NULL) {
        printf("创建任务2失败\r\n");
    }
}

APP_FEATURE_INIT(SG90);
