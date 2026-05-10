#include <stdint.h>
#include "hw.h"
#include "oled.h"
#include "doom_frames.h"

extern void delay_ms(uint32_t ms);

int main(void)
{
    RCC_AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIO_BSRR(GPIOB_BASE) = (1u << OLED_PIN_PWR);
    gpio_output_pushpull(GPIOB_BASE, OLED_PIN_PWR);

    delay_ms(1000);

    OLED_Init();

    uint32_t frame = 0u;
    while (1) {
        DoomFrames_Blit(frame);
        OLED_Update();
        delay_ms(1000u / DOOM_FPS);
        if (++frame >= DOOM_FRAME_COUNT) {
            frame = 0u;
        }
    }
}
