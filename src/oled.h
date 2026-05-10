#ifndef PULSEOX_OLED_H
#define PULSEOX_OLED_H

#include <stdint.h>

#define OLED_PIN_PWR 14u   /* PB14: active-HIGH power enable, set by main() before OLED_Init() */

#define OLED_W 128u
#define OLED_H 64u
#define OLED_BUF_SIZE ((OLED_W * OLED_H) / 8u)

void OLED_Init(void);
void OLED_Update(void);
void OLED_Clear(void);
void OLED_Fill(uint8_t value);
void OLED_FillTestPattern(void);
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t on);
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on);
void OLED_AllOn(void);
void OLED_AllOff(void);
uint8_t *OLED_Framebuffer(void);
void OLED_DebugSignature(void);

#endif