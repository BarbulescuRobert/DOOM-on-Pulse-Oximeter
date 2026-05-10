#include <stdint.h>

/*
 * Raw register addresses (APM32F030, no SDK dependency).
 *
 * RCM_BASE  = AHBPERIPH_BASE + 0x1000 = 0x40021000
 * FMC_BASE  = AHBPERIPH_BASE + 0x2000 = 0x40022000
 */
#define FMC_CTRL1   (*(volatile uint32_t *)0x40022000UL)  /* WS in bits [2:0] */
#define RCM_CTRL1   (*(volatile uint32_t *)0x40021000UL)  /* PLLEN=24, PLLRDYFLG=25 */
#define RCM_CFG1    (*(volatile uint32_t *)0x40021004UL)  /* PLLMULCFG=[21:18], PLLSRCSEL=16, SCLKSEL=[1:0] */

void SystemInit(void)
{
    /*
     * Target: 48 MHz from HSI via PLL.
     *   HSI = 8 MHz → HSI/2 = 4 MHz (PLL input, fixed when PLLSRCSEL=0)
     *   PLL ×12 = 48 MHz  (PLLMULCFG = 10 → ×12)
     *
     * Step 1: flash wait state — required before raising SYSCLK above 24 MHz.
     */
    FMC_CTRL1 = (FMC_CTRL1 & ~0x7UL) | 0x1UL;

    /*
     * Step 2: configure PLL — source HSI/2 (PLLSRCSEL=0), multiplier ×12
     * (PLLMULCFG=10). Clear SCLKSEL first (keep HSI as system clock for now).
     */
    RCM_CFG1 = (RCM_CFG1 & ~((0xFUL << 18) | (0x3UL << 16) | 0x3UL))
             | (10UL << 18);   /* PLLMULCFG = 10 → ×12 */

    /* Step 3: enable PLL. */
    RCM_CTRL1 |= (1UL << 24);

    /* Step 4: wait for PLL lock. */
    while (!(RCM_CTRL1 & (1UL << 25))) {}

    /* Step 5: switch system clock to PLL (SCLKSEL = 2). */
    RCM_CFG1 = (RCM_CFG1 & ~0x3UL) | 0x2UL;

    /* Step 6: wait until PLL is the active system clock (SCLKSWSTS = 2). */
    while ((RCM_CFG1 & (0x3UL << 2)) != (0x2UL << 2)) {}
}

static void delay_cycles(volatile uint32_t n)
{
    while (n--) {
        __asm volatile ("nop");
    }
}

void delay_ms(uint32_t ms)
{
    /*
     * ~12000 loop iterations ≈ 1 ms at 48 MHz (Cortex-M0 loop ≈ 4–5 cycles).
     * Recalibrate with a scope or SysTick if precision matters for DOOM timing.
     */
    while (ms--) {
        delay_cycles(4000u);
    }
}
