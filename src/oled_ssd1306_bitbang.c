#include "oled.h"   /* also defines OLED_PIN_PWR */
#include "hw.h"

/*
 * Reverse-engineered pulse oximeter OLED wiring:
 *
 * OLED controller: marked/assumed SSD1306BZ-compatible, 128x64 monochrome
 *
 * CS#        -> tied to GND, always selected
 * RES#       -> floating / not MCU-controlled on this PCB
 * D/C#       -> PB15 through 100 ohm
 * SCLK       -> PB13
 * MOSI       -> PB12
 * PWR_ENABLE -> PB14, active HIGH — must be driven HIGH before init
 */

static uint8_t fb[OLED_BUF_SIZE];

#define OLED_PORT       GPIOB_BASE
#define OLED_PIN_MOSI   12u
#define OLED_PIN_SCLK   13u
/* OLED_PIN_PWR = 14u defined in oled.h */
#define OLED_PIN_DC     15u

/*
 * Try 0 first.
 * If this turns out to be SH1106-like or shifted horizontally, try 2.
 */
#define OLED_COL_OFFSET 0u

static inline void dc_low(void)    { gpio_reset(OLED_PORT, OLED_PIN_DC); }
static inline void dc_high(void)   { gpio_set(OLED_PORT, OLED_PIN_DC); }
static inline void clk_low(void)   { gpio_reset(OLED_PORT, OLED_PIN_SCLK); }
static inline void clk_high(void)  { gpio_set(OLED_PORT, OLED_PIN_SCLK); }
static inline void mosi_low(void)  { gpio_reset(OLED_PORT, OLED_PIN_MOSI); }
static inline void mosi_high(void) { gpio_set(OLED_PORT, OLED_PIN_MOSI); }

static void local_delay_ms(uint32_t ms)
{
    /*
     * Rough delay. Good enough for OLED bring-up.
     * Adjust if your system clock is much faster/slower.
     */
    while (ms--) {
        for (volatile uint32_t i = 0; i < 8000u; i++) {
            __asm volatile ("nop");
        }
    }
}

static inline void spi_delay(void)
{
    __asm volatile ("nop");
    __asm volatile ("nop");
}
static void spi_write_byte(uint8_t byte)
{
    clk_high();

    for (uint8_t mask = 0x80u; mask != 0u; mask >>= 1u) {
        if (byte & mask) {
            mosi_high();
        } else {
            mosi_low();
        }

        spi_delay();

        clk_low();
        spi_delay();

        clk_high();
        spi_delay();
    }

    // idle state
    mosi_low();
    clk_high();
}

static void oled_cmd(uint8_t cmd)
{
    dc_low();
    spi_delay();

    spi_write_byte(cmd);

    spi_delay();
    dc_high();

    mosi_low();
    clk_high();
}

static void oled_data(const uint8_t *data, uint16_t len)
{
    dc_high();
    spi_delay();

    while (len--) {
        spi_write_byte(*data++);
    }

    dc_high();
    mosi_low();
    clk_high();
}

static void oled_gpio_init(void)
{
    RCC_AHBENR |= RCC_AHBENR_GPIOBEN;

    /*
     * Pre-load SCLK and DC HIGH in ODR before switching to output mode.
     * CS# is permanently tied to GND so the controller's SPI state machine
     * is always active — a spurious SCLK edge would shift all subsequent
     * bytes by one bit. Writing BSRR sets ODR while pins are still inputs;
     * gpio_output_pushpull then enables the output driver at the correct level.
     * PB14 (PWR_ENABLE) is driven HIGH by main() before this function runs.
     */
    GPIO_BSRR(OLED_PORT) = (1u << OLED_PIN_SCLK);  /* ODR[13] = 1, SCLK idles HIGH */
    GPIO_BSRR(OLED_PORT) = (1u << OLED_PIN_DC);    /* ODR[15] = 1, DC idles HIGH   */

    gpio_output_pushpull(OLED_PORT, OLED_PIN_MOSI); /* starts LOW  (ODR[12] = 0) */
    gpio_output_pushpull(OLED_PORT, OLED_PIN_SCLK); /* starts HIGH (ODR[13] = 1) */
    gpio_output_pushpull(OLED_PORT, OLED_PIN_DC);   /* starts HIGH (ODR[15] = 1) */
}

void OLED_Init(void)
{
    oled_gpio_init();

    /*
     * RES# is not MCU-controlled on your board, so wait for power-on reset.
     * If needed, add a physical 10k pull-up from RES# to 3.3 V.
     */
    local_delay_ms(300);

    oled_cmd(0xAE);              /* display off */

    oled_cmd(0xD5); oled_cmd(0x80); /* display clock divide */
    oled_cmd(0xA8); oled_cmd(0x3F); /* multiplex ratio: 1/64 */
    oled_cmd(0xD3); oled_cmd(0x00); /* display offset */
    oled_cmd(0x40);                 /* display start line = 0 */

    /*
     * I am intentionally not relying on horizontal addressing here.
     * We update using old page-address commands in OLED_Update().
     *
     * If you want to explicitly set SSD1306 page mode, you can try:
     *
     * oled_cmd(0x20); oled_cmd(0x02);
     *
     * But leave it out for now, because page commands B0/00/10
     * are compatible with SSD1306-style and SH1106-style controllers.
     */

    oled_cmd(0xA1);              /* segment remap */
    oled_cmd(0xC8);              /* COM scan direction remap */
    oled_cmd(0xDA); oled_cmd(0x12); /* COM pins config */
    oled_cmd(0x81); oled_cmd(0xFF); /* contrast */

    /*
     * Try 0x14 first. If still blank, try 0x10 again.
     */
    oled_cmd(0x8D); oled_cmd(0x14); /* charge pump enable */

    oled_cmd(0xD9); oled_cmd(0xF1); /* precharge */
    oled_cmd(0xDB); oled_cmd(0x40); /* VCOMH deselect level */

    oled_cmd(0xA4);              /* display follows RAM */
    oled_cmd(0xA6);              /* normal display */
    oled_cmd(0x2E);              /* deactivate scroll */
    oled_cmd(0xAF);              /* display on */

    OLED_Clear();
    OLED_Update();
}

uint8_t *OLED_Framebuffer(void)
{
    return fb;
}

void OLED_Clear(void)
{
    OLED_Fill(0x00u);
}

void OLED_Fill(uint8_t value)
{
    for (uint16_t i = 0; i < OLED_BUF_SIZE; i++) {
        fb[i] = value;
    }
}

void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t on)
{
    if (x >= OLED_W || y >= OLED_H) {
        return;
    }

    uint16_t index = (uint16_t)x + ((uint16_t)(y >> 3u) * OLED_W);
    uint8_t bit = (uint8_t)(1u << (y & 7u));

    if (on) {
        fb[index] |= bit;
    } else {
        fb[index] &= (uint8_t)~bit;
    }
}

void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on)
{
    for (uint8_t yy = y; yy < (uint8_t)(y + h); yy++) {
        for (uint8_t xx = x; xx < (uint8_t)(x + w); xx++) {
            OLED_DrawPixel(xx, yy, on);
        }
    }
}

void OLED_Update(void)
{
    /*
     * Page-addressed update.
     *
     * This should resemble the working oximeter capture more closely:
     * command, command, command, then 128 data bytes.
     */
    for (uint8_t page = 0; page < 8u; page++) {
        oled_cmd((uint8_t)(0xB0u | page));                         /* page address */
        oled_cmd((uint8_t)(0x00u | (OLED_COL_OFFSET & 0x0Fu)));     /* lower column */
        oled_cmd((uint8_t)(0x10u | ((OLED_COL_OFFSET >> 4u) & 0x0Fu))); /* upper column */

        oled_data(&fb[(uint16_t)page * OLED_W], OLED_W);
    }

    dc_high();
}

void OLED_FillTestPattern(void)
{
    OLED_Clear();

    for (uint8_t y = 0; y < OLED_H; y++) {
        for (uint8_t x = 0; x < OLED_W; x++) {
            uint8_t on = (uint8_t)((((x >> 3u) + (y >> 3u)) & 1u) != 0u);
            OLED_DrawPixel(x, y, on);
        }
    }
}

void OLED_AllOn(void)
{
    oled_cmd(0xA5);  /* entire display ON, ignores RAM */
    oled_cmd(0xAF);  /* display ON */
}

void OLED_AllOff(void)
{
    oled_cmd(0xA4);  /* resume RAM display */
    OLED_Clear();
    OLED_Update();
}


void OLED_DebugSignature(void)
{
    oled_gpio_init();

    while (1) {
        // Command-mode signature
        oled_cmd(0xAA);
        oled_cmd(0x55);
        oled_cmd(0xF0);
        oled_cmd(0x0F);
        oled_cmd(0xA5);
        oled_cmd(0x5A);

        // Data-mode signature
        static const uint8_t data[] = {
            0x12, 0x34, 0x56, 0x78,
            0xDE, 0xAD, 0xBE, 0xEF
        };

        oled_data(data, sizeof(data));

        for (volatile uint32_t i = 0; i < 200000; i++) {
            __asm volatile ("nop");
        }
    }
}