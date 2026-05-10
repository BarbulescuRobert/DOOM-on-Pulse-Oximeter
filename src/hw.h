#ifndef PULSEOX_HW_H
#define PULSEOX_HW_H

#include <stdint.h>

#define RCC_BASE        0x40021000UL
#define GPIOA_BASE      0x48000000UL
#define GPIOB_BASE      0x48000400UL
#define GPIOC_BASE      0x48000800UL

#define RCC_AHBENR      (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_AHBENR_GPIOAEN (1u << 17)
#define RCC_AHBENR_GPIOBEN (1u << 18)
#define RCC_AHBENR_GPIOCEN (1u << 19)

#define GPIO_MODER(port)    (*(volatile uint32_t *)((port) + 0x00))
#define GPIO_OTYPER(port)   (*(volatile uint32_t *)((port) + 0x04))
#define GPIO_OSPEEDR(port)  (*(volatile uint32_t *)((port) + 0x08))
#define GPIO_PUPDR(port)    (*(volatile uint32_t *)((port) + 0x0C))
#define GPIO_IDR(port)      (*(volatile uint32_t *)((port) + 0x10))
#define GPIO_ODR(port)      (*(volatile uint32_t *)((port) + 0x14))
#define GPIO_BSRR(port)     (*(volatile uint32_t *)((port) + 0x18))

static inline void gpio_output_pushpull(uint32_t port, uint32_t pin)
{
    GPIO_MODER(port) &= ~(3u << (pin * 2u));
    GPIO_MODER(port) |=  (1u << (pin * 2u));

    GPIO_OTYPER(port) &= ~(1u << pin);

    GPIO_OSPEEDR(port) &= ~(3u << (pin * 2u));
    GPIO_OSPEEDR(port) |=  (3u << (pin * 2u));

    GPIO_PUPDR(port) &= ~(3u << (pin * 2u));
}

static inline void gpio_set(uint32_t port, uint32_t pin)
{
    GPIO_BSRR(port) = (1u << pin);
}

static inline void gpio_reset(uint32_t port, uint32_t pin)
{
    GPIO_BSRR(port) = (1u << (pin + 16u));
}

#endif