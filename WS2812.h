/**
 * File created by thinkjow on 03/27/2024.
 * version 1.1
 *
 * WS2812 编码要求：
 *      0 码， 高电平时间0.22μs~0.38μs
 *      0 码， 低电平时间0.58μs~1μs
 *      1 码， 高电平时间0.580μs~1μs
 *      1 码， 低电平时间0.580μs~1μs
 *      帧单位，低电平时间280μs以上
 *
 * 使用STM32F103C8T6芯片控制WS2812使LED显示指定颜色
 * 系统设置：
 * 1. 设置APB2时钟为72MHz;
 * 2. 打开TIM1并设置Channel1为PWM输出模式
 *      Prescaler: 0
 *      Counter Mode: Up
 *      Counter Period: (90 - 1), 这样使TIM1计数周期为1.25us(800KHz)，符合WS2812要求
 *          { 计数周期时间 = (Counter Period + 1) * (Prescaler + 1) / (APB1或APB2 Timer clocks时钟频率) }
 *      auto-reload preload: FALSE
 * 3. 设置TIM1的DMA模式，
 *      DMA Request: TIM_CH1,
 *      Channel: DMA1_Channel2,
 *      Direction: Memory To Peripheral,
 *      Increment Address: Memory
 *      DataWidth: Perihperal: Word, Memory: Byte(Byte对应uint8_t的buffer, 此项需要与buffer的类型对应)
 *
 * 函数使用方法:
 * 1. 修改宏LED_NUM的值, 表示总的LED数量
 * 2. 修改TIMER_HANDLE_ADDRESS为实际使用的定时器句柄指针
 * 2. 调用WS2812_Inial()初始始化数据结构
 * 3. WS2812_SetPosColor(uint16_t pos, RGB_Color_TypeDef color)设置指定位置LED的颜色
 * 4. WS2812_SetRangeColor(uint16_t posStart, uint16_t posEnd, RGB_Color_TypeDef color)设置指定位置范围内LED的颜色
 *
 * 当使用其它MCU时：
 * 1. 将Timer分频到100MHz内，72MHz左右
 * 2. 计算Counter Period的值并设置
 *     Counter Period = {(1.25 * APB1或APB2 Timer clocks) / (Prescaler + 1)} + 1
 * 3. Timer其它设置与STM32F103C8T6相同
 * 4. DMA设置与STM32F103CT6相同
 */

#ifndef WS2812B
#define WS2812B

/**
 * 定义定时器Handle地址(cubemx默认生成htim1, htim2,...等句柄名，需修改成需要使用的timer句柄)
 */
#define TIMER_HANDLE_ADDRESS &htim2

/**
 * 定义LED总数量
 */
#define LED_NUM  10  //LED数量

/*这里是上文计算所得CCR的宏定义*/
/**
 * CCR的值，对应PWM输出高低电平
 * 设置STM32F103C8T6, TIM1, 总线频率72MHz, 计数值为90时计算得到TIM1时钟周期为1.25us
 * WS2812高低电平占空比要求：
 *      0高电平范围：0.22us ~ 0.38us
 *      1高电平范围：0.58us ~ 1us
 * 计算CCR值范围
 *      0高电平: 15.84 ~ 27.36 { 即： (0.22us / 1.25us) * 90 ~ (0.38us / 1.25us) * 90 }
 *      1高电平: 41.76 ~ 72 { 即： (0.58us / 1.25us) * 90 ~ (1us / 1.25us) * 90 }
 * 取中间值进行设置
 */
#define CODE_1       (57)       //1码定时器计数次数
#define CODE_0       (23)       //0码定时器计数次数

#include "stdint.h"
/*建立一个定义单个LED三原色值大小的结构体*/
typedef struct _RGB_COLOR_
{
  uint8_t R;
  uint8_t G;
  uint8_t B;
}RGB_Color_TypeDef;

/**
 * 预定义颜色
 */
static RGB_Color_TypeDef RED      = {255,0,0};   //显示红色RGB数据
static RGB_Color_TypeDef GREEN    = {0,255,0};
static RGB_Color_TypeDef BLUE     = {0,0,255};
static RGB_Color_TypeDef BLUE1     = {0,50,255};
static RGB_Color_TypeDef SKY      = {0,255,255};
static RGB_Color_TypeDef MAGENTA  = {255,0,220};
static RGB_Color_TypeDef ORANGE= {255,165,0};
static RGB_Color_TypeDef YELLOW   = {0xFF,0xFF,0x00};
static RGB_Color_TypeDef OEANGE   = {127,106,0};
static RGB_Color_TypeDef BLACK    = {0,0,0};
static RGB_Color_TypeDef WHITE    = {255,255,255};


//functions
/**
 * 初始化WS2812
 */
void WS2812_Init();

/**
 * 设置指定位置LED的颜色数据, 但不发送数据
 * @param ledPos LED的位置，编号从0开始
 * @param color 颜色
 * @return void
 */
void RGB_SetColor(uint8_t ledPos, RGB_Color_TypeDef color);

/**
 * 在指令结尾添加帧结束标志,
 * 当使用STM32F103C8T6，TIM1， 时钟设置为72MHz，计数值设置为90时，
 * 设置24个周期占空比为0的PWM波，作为最后reset延时，总时长为24*1.2=30us > 24us(要求大于24us)
 */
void RGB_EOF(void);

/**
 * 将设置好的LED颜色通过DMA传送到Timer中生成PWM波
 * HAL_TIM_PWM_Start_DMA中传入参数，定时器：&tim1, 通道：TIM_CHANNEL_1, 数据：pixel_buf, 大小: (LED_NUM+1) * 24
 * pixel_Buf被强转成uint32_t只是适应函数传参需要，实际指针移动间距是按DMA中的参数设置来移动
 */
void RGB_SendArray(void);

/**
 * 使第0到len位置的LED显示红色
 * @param len
 */
void WS2812_SetRed(uint16_t len);

/**
 * 使第0到len位置的LED显示绿色
 * @param len
 */
void WS2812_SetGreen(uint16_t len);

/**
 * 使第0到len位置的LED显示蓝色
 * @param len
 */
void WS2812_SetBlue(uint16_t len);

/**
 * 使第0到len位置的LED显示白色
 * @param len
 */
void WS2812_SetWhite(uint16_t len);

/**
 * 清除所有颜色
 * @param pixelLen
 */
void WS2812_Clear();

/**
 * 使指定位置范围的LED显示指定的颜色
 * @param color
 * @param posStart 开始位置, 计数从1开始
 * @param posEnd 结束位置, 计数从1开如
 */
void WS2812_SetRangeColor(uint16_t posStart, uint16_t posEnd, RGB_Color_TypeDef color);

/**
 * 使指定位置的LED显示指定的颜色
 * @param pos LED位置，范围从1开始到LED_NUM定义的数量
 * @param color
 */
void WS2812_SetPosColor(uint16_t pos, RGB_Color_TypeDef color);
#endif //WS2812B
