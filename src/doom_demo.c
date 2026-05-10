#include "doom_demo.h"
#include "oled.h"

static void hline(uint8_t x0, uint8_t x1, uint8_t y)
{
    for (uint8_t x = x0; x <= x1; x++) {
        OLED_DrawPixel(x, y, 1);
    }
}

static void vline(uint8_t x, uint8_t y0, uint8_t y1)
{
    for (uint8_t y = y0; y <= y1; y++) {
        OLED_DrawPixel(x, y, 1);
    }
}

static void block_letter_d(uint8_t x, uint8_t y)
{
    OLED_DrawRect(x, y, 4, 14, 1);
    OLED_DrawRect(x + 4, y, 6, 3, 1);
    OLED_DrawRect(x + 4, y + 11, 6, 3, 1);
    OLED_DrawRect(x + 9, y + 3, 3, 8, 1);
}

static void block_letter_o(uint8_t x, uint8_t y)
{
    OLED_DrawRect(x, y, 12, 3, 1);
    OLED_DrawRect(x, y + 11, 12, 3, 1);
    OLED_DrawRect(x, y + 3, 3, 8, 1);
    OLED_DrawRect(x + 9, y + 3, 3, 8, 1);
}

static void block_letter_m(uint8_t x, uint8_t y)
{
    OLED_DrawRect(x, y, 3, 14, 1);
    OLED_DrawRect(x + 9, y, 3, 14, 1);
    OLED_DrawRect(x + 3, y + 2, 3, 5, 1);
    OLED_DrawRect(x + 6, y + 2, 3, 5, 1);
}

void DoomDemo_DrawFrame(uint32_t frame)
{
    OLED_Clear();

    /* Logo */
    block_letter_d(28, 3);
    block_letter_o(43, 3);
    block_letter_o(58, 3);
    block_letter_m(73, 3);

    /* Horizon and corridor */
    hline(0, 127, 23);
    hline(0, 127, 49);

    for (uint8_t i = 0; i < 14; i++) {
        uint8_t dx = (uint8_t)(i * 4);
        uint8_t dy = i;
        OLED_DrawPixel((uint8_t)(64 - dx), (uint8_t)(36 - dy), 1);
        OLED_DrawPixel((uint8_t)(64 + dx), (uint8_t)(36 - dy), 1);
        OLED_DrawPixel((uint8_t)(64 - dx), (uint8_t)(36 + dy), 1);
        OLED_DrawPixel((uint8_t)(64 + dx), (uint8_t)(36 + dy), 1);
    }

    /* Animated enemy/imp-ish blob */
    uint8_t ex = (uint8_t)(54 + ((frame >> 2) & 0x0F));
    OLED_DrawRect(ex, 31, 20, 12, 1);
    OLED_DrawRect((uint8_t)(ex + 3), 34, 4, 3, 0);
    OLED_DrawRect((uint8_t)(ex + 13), 34, 4, 3, 0);
    OLED_DrawRect((uint8_t)(ex + 7), 39, 6, 2, 0);

    /* Status bar */
    OLED_DrawRect(0, 53, 128, 11, 1);
    OLED_DrawRect(2, 55, 25, 7, 0);
    OLED_DrawRect(51, 55, 26, 7, 0);
    OLED_DrawRect(101, 55, 25, 7, 0);

    /* Tiny health/ammo bar details */
    vline(31, 55, 61);
    vline(96, 55, 61);

    if (frame & 8u) {
        OLED_DrawRect(58, 56, 4, 4, 1);
        OLED_DrawRect(66, 56, 4, 4, 1);
    } else {
        OLED_DrawRect(57, 56, 4, 4, 1);
        OLED_DrawRect(67, 56, 4, 4, 1);
    }
}
