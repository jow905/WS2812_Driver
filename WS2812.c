//
// Created by think on 2/24/2024.
//
#include "WS2812.h"
#include "tim.h"

/**
 *定义二维数组存放PWM输出数组，行表示LED位置，每行24个bit表示颜色值
 * 最后一行24个0代表帖结束
 */
static uint8_t pixel_Buf[LED_NUM + 1][24] = {0};

void WS2812_Init()
 {
   uint16_t i;
   for(i = 0; i < LED_NUM; i++){
     RGB_SetColor(i, BLACK);
   }
 }


void RGB_SetColor(uint8_t ledPos, RGB_Color_TypeDef color)
{
  uint8_t i;
  if(ledPos > LED_NUM) return; //avoid overflow 防止写入ID大于LED总数

  for(i=0;i<8;i++){
    pixel_Buf[ledPos][i]   = ((color.G & (1 << (7 - i))) ? (CODE_1) : CODE_0 );//数组某一行0~7转化存放G
  }
  for(i=8;i<16;i++){
    pixel_Buf[ledPos][i]  = ((color.R & (1 << (15 - i))) ? (CODE_1) : CODE_0 );//数组某一行8~15转化存放R
  }
  for(i=16;i<24;i++){
    pixel_Buf[ledPos][i] = ((color.B & (1 << (23 - i))) ? (CODE_1) : CODE_0 );//数组某一行16~23转化存放B
  }
}

void RGB_EOF(void)
{
  uint8_t i;
  for(i=0; i<24; i++){
    pixel_Buf[LED_NUM][i] = 0;
  }
}

void RGB_SendArray(void)
{
  HAL_TIM_PWM_Start_DMA(TIMER_HANDLE_ADDRESS, TIM_CHANNEL_1, (uint32_t *)pixel_Buf, (LED_NUM + 1) * 24);
  //延迟5ms，并停止生成PWM波，防止两帖之间干扰导致颜色误差
  HAL_Delay(5);
  HAL_TIM_PWM_Stop(TIMER_HANDLE_ADDRESS, TIM_CHANNEL_1);
}


void WS2812_SetRed(uint16_t len)
{
  uint16_t i;
  //给对应个数LED写入红色
  for(i=0; i < len; i++){
    RGB_SetColor(i, RED);
  }
  RGB_EOF();
  RGB_SendArray();
}


void WS2812_SetGreen(uint16_t len)
{
  uint16_t i;
  //给对应个数LED写入绿色
  for(i=0; i < len; i++){
    RGB_SetColor(i, GREEN);
  }
  RGB_EOF();
  RGB_SendArray();
}


void WS2812_SetBlue(uint16_t len)
{
  uint16_t i;
  //给对应个数LED写入蓝色
  for(i=0;i<len;i++){
    RGB_SetColor(i, BLUE);
  }
  RGB_EOF();
  RGB_SendArray();
}


void WS2812_SetWhite(uint16_t len)
{
  uint16_t i;
  //给对应个数LED写入白色
  for(i=0;i<len;i++){
    RGB_SetColor(i, WHITE);
  }
  RGB_EOF();
  RGB_SendArray();
}


void WS2812_Clear()
{
  uint16_t i;
  for(i=0; i<LED_NUM; i++){
    RGB_SetColor(i, BLACK);
  }
  RGB_EOF();
  RGB_SendArray();
}


void WS2812_SetRangeColor(uint16_t posStart, uint16_t posEnd, RGB_Color_TypeDef color)
{
  uint16_t i;
  if(posStart <=0 || posEnd <=0 ){
    return;
  }

  if(posStart > LED_NUM || posEnd > LED_NUM){
    return;
  }

  if(posStart > posEnd){
    return;
  }

  posStart = posStart -1;
  posEnd = posEnd - 1;

  for(i = posStart; i <= posEnd; i++) {
    RGB_SetColor(i, color);
  }
  RGB_EOF();
  RGB_SendArray();
}


void WS2812_SetPosColor(uint16_t pos, RGB_Color_TypeDef color)
{
  if(pos <= 0 || pos > LED_NUM ){
    return;
  }

  pos = pos -1;
  RGB_SetColor(pos, color);
  RGB_EOF();
  RGB_SendArray();
}
