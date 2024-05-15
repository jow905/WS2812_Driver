# WS2812_Driver
  WS2812 编码要求：
       0 码， 高电平时间0.22μs~0.38μs
       0 码， 低电平时间0.58μs~1μs
       1 码， 高电平时间0.580μs~1μs
       1 码， 低电平时间0.580μs~1μs
       帧单位，低电平时间280μs以上
 
  使用STM32F103C8T6芯片控制WS2812使LED显示指定颜色
  系统设置：
  1. 设置APB2时钟为72MHz;
  2. 打开TIM1并设置Channel1为PWM输出模式
       Prescaler: 0
       Counter Mode: Up
       Counter Period: (90 - 1), 这样使TIM1计数周期为1.25us(800KHz)，符合WS2812要求
           { 计数周期时间 = (Counter Period + 1)  (Prescaler + 1) / (APB1或APB2 Timer clocks时钟频率) }
       auto-reload preload: FALSE
  3. 设置TIM1的DMA模式，
       DMA Request: TIM_CH1,
       Channel: DMA1_Channel2,
       Direction: Memory To Peripheral,
       Increment Address: Memory
       DataWidth: Perihperal: Word, Memory: Byte(Byte对应uint8_t的buffer, 此项需要与buffer的类型对应)
 
  函数使用方法:
  1. 修改宏LED_NUM的值, 表示总的LED数量
  2. 修改TIMER_HANDLE_ADDRESS为实际使用的定时器句柄指针
  2. 调用WS2812_Inial()初始始化数据结构
  3. WS2812_SetPosColor(uint16_t pos, RGB_Color_TypeDef color)设置指定位置LED的颜色
  4. WS2812_SetRangeColor(uint16_t posStart, uint16_t posEnd, RGB_Color_TypeDef color)设置指定位置范围内LED的颜色
 
  当使用其它MCU时：
  1. 将Timer分频到100MHz内，72MHz左右
  2. 计算Counter Period的值并设置
      Counter Period = {(1.25  APB1或APB2 Timer clocks) / (Prescaler + 1)} + 1
  3. Timer其它设置与STM32F103C8T6相同
  4. DMA设置与STM32F103CT6相同
