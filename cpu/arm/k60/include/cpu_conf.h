/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k60 Freescale Kinetis K60
 * @ingroup         cpu
 * @brief           CPU specific implementations for the Freescale Kinetis K60
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CPU_CONF_H_
#define CPU_CONF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "config-board.h"

#if K60_CPU_REV == 2

/* Rev. 2.x silicon */
#define K60_CPU_REV 2
#include "MK60D10.h"

/** The expected CPUID value, can be used to implement a check that we are
 * running on the right hardware */
#define K60_EXPECTED_CPUID 0x410fc241u

/* K60 rev 2.x replaced the RNG module in 1.x by the RNGA PRNG module */
#define KINETIS_RNGA            (RNG)

#elif K60_CPU_REV == 1

/* Rev. 1.x silicon */
#define K60_CPU_REV 1
#include "MK60DZ10.h"

/** The expected CPUID value, can be used to implement a check that we are
 * running on the right hardware */
#define K60_EXPECTED_CPUID 0x410fc240u

/* K60 rev 1.x has the cryptographically strong RNGB module */
#define KINETIS_RNGB            (RNG)

#else
#error Unknown CPU model. Update Makefile.include in the board directory.
#endif

/* Compatibility definitions between the two different Freescale headers */
#include "MK60-comp.h"

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (104U)
#define CPU_FLASH_BASE                  (0x00000000)
/** @} */

/**
 * @name Length and address for reading CPU_ID (named UID in Freescale documents)
 * @{
 */
#define CPUID_ID_LEN                    (16)
#define CPUID_ID_PTR                    ((void *)(&(SIM->UIDH)))
/** @} */

/**
 * @name GPIO pin mux function numbers
 */
/** @{ */
#define PIN_MUX_FUNCTION_ANALOG 0
#define PIN_MUX_FUNCTION_GPIO 1
/** @} */
/**
 * @name GPIO interrupt flank settings
 */
/** @{ */
#define PIN_INTERRUPT_RISING 0b1001
#define PIN_INTERRUPT_FALLING 0b1010
#define PIN_INTERRUPT_EDGE 0b1011
/** @} */

/** @name PORT module clock gates */
/** @{ */
#define PORTA_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTA_SHIFT))
#define PORTB_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT))
#define PORTC_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT))
#define PORTD_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTD_SHIFT))
#define PORTE_CLOCK_GATE (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTE_SHIFT))
/** @} */

/** @brief DMA module clock gate */
#define DMA_CLOCK_GATE (BITBAND_REG32(SIM->SCGC7, SIM_SCGC7_DMA_SHIFT))
/** @brief DMA multiplexer clock gate */
#define DMAMUX_CLOCK_GATE (BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_DMAMUX_SHIFT))

/** @brief DMA multiplexer source numbers */
typedef enum {
    DMA_SOURCE_DISABLED        =  0,
    DMA_SOURCE_RESERVED1       =  1,
    DMA_SOURCE_UART0_RX        =  2,
    DMA_SOURCE_UART0_TX        =  3,
    DMA_SOURCE_UART1_RX        =  4,
    DMA_SOURCE_UART1_TX        =  5,
    DMA_SOURCE_UART2_RX        =  6,
    DMA_SOURCE_UART2_TX        =  7,
    DMA_SOURCE_UART3_RX        =  8,
    DMA_SOURCE_UART3_TX        =  9,
    DMA_SOURCE_UART4_RX        = 10,
    DMA_SOURCE_UART4_TX        = 11,
    DMA_SOURCE_RESERVED12      = 12,
    DMA_SOURCE_RESERVED13      = 13,
    DMA_SOURCE_I2S0_RX         = 14,
    DMA_SOURCE_I2S0_TX         = 15,
    DMA_SOURCE_SPI0_RX         = 16,
    DMA_SOURCE_SPI0_TX         = 17,
    DMA_SOURCE_SPI1_RX         = 18,
    DMA_SOURCE_SPI1_TX         = 19,
    DMA_SOURCE_SPI2_RX         = 20,
    DMA_SOURCE_SPI2_TX         = 21,
    DMA_SOURCE_I2C0            = 22,
    DMA_SOURCE_I2C1            = 23,
    DMA_SOURCE_FTM0CH0         = 24,
    DMA_SOURCE_FTM0CH1         = 25,
    DMA_SOURCE_FTM0CH2         = 26,
    DMA_SOURCE_FTM0CH3         = 27,
    DMA_SOURCE_FTM0CH4         = 28,
    DMA_SOURCE_FTM0CH5         = 29,
    DMA_SOURCE_FTM0CH6         = 30,
    DMA_SOURCE_FTM0CH7         = 31,
    DMA_SOURCE_FTM1CH0         = 32,
    DMA_SOURCE_FTM1CH1         = 33,
    DMA_SOURCE_FTM2CH0         = 34,
    DMA_SOURCE_FTM2CH1         = 35,
    DMA_SOURCE_IEEE1588_TIMER0 = 36,
    DMA_SOURCE_IEEE1588_TIMER1 = 37,
    DMA_SOURCE_IEEE1588_TIMER2 = 38,
    DMA_SOURCE_IEEE1588_TIMER3 = 39,
    DMA_SOURCE_ADC0            = 40,
    DMA_SOURCE_ADC1            = 41,
    DMA_SOURCE_CMP0            = 42,
    DMA_SOURCE_CMP1            = 43,
    DMA_SOURCE_CMP2            = 44,
    DMA_SOURCE_DAC0            = 45,
    DMA_SOURCE_RESERVED46      = 46,
    DMA_SOURCE_CMT             = 47,
    DMA_SOURCE_PDB             = 48,
    DMA_SOURCE_PORT_A          = 49,
    DMA_SOURCE_PORT_B          = 50,
    DMA_SOURCE_PORT_C          = 51,
    DMA_SOURCE_PORT_D          = 52,
    DMA_SOURCE_PORT_E          = 53,
    DMA_SOURCE_DMAMUX_ALWAYS0  = 54,
    DMA_SOURCE_DMAMUX_ALWAYS1  = 55,
    DMA_SOURCE_DMAMUX_ALWAYS2  = 56,
    DMA_SOURCE_DMAMUX_ALWAYS3  = 57,
    DMA_SOURCE_DMAMUX_ALWAYS4  = 58,
    DMA_SOURCE_DMAMUX_ALWAYS5  = 59,
    DMA_SOURCE_DMAMUX_ALWAYS6  = 60,
    DMA_SOURCE_DMAMUX_ALWAYS7  = 61,
    DMA_SOURCE_DMAMUX_ALWAYS8  = 62,
    DMA_SOURCE_DMAMUX_ALWAYS9  = 63,
} dma_source_t;

/**
 * @brief Number of DMA channels available in hardware
 */
#define DMA_NUMOF 16

/**
 * @name UART driver settings
 */
/** @{ */
/** UART typedef from CPU header. */
#define KINETIS_UART                    UART_Type
/** @} */

/**
 * @name Clock settings for the LPTMR0 timer
 * @{
 */
#define LPTIMER_DEV                      (LPTMR0) /**< LPTIMER hardware module */
#define LPTIMER_CLKEN()                  (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_LPTIMER_SHIFT) = 1)    /**< Enable LPTMR0 clock gate */
#define LPTIMER_CLKDIS()                 (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_LPTIMER_SHIFT) = 0)    /**< Disable LPTMR0 clock gate */
#define LPTIMER_CLKSRC_MCGIRCLK          0    /**< internal reference clock (4MHz) */
#define LPTIMER_CLKSRC_LPO               1    /**< PMC 1kHz output */
#define LPTIMER_CLKSRC_ERCLK32K          2    /**< RTC clock 32768Hz */
#define LPTIMER_CLKSRC_OSCERCLK          3    /**< system oscillator output, clock from RF-Part */

#ifndef LPTIMER_CLKSRC
#define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_ERCLK32K    /**< default clock source */
#endif

#if (LPTIMER_CLKSRC == LPTIMER_CLKSRC_MCGIRCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_OSCERCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_ERCLK32K)
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           32768
#else
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           1000
#endif

/** IRQ priority for hwtimer interrupts */
#define LPTIMER_IRQ_PRIO          1
/** IRQ channel for hwtimer interrupts */
#define LPTIMER_IRQ_CHAN          LPTMR0_IRQn

#if K60_CPU_REV == 1
/*
 * The CNR register latching in LPTMR0 was added in silicon rev 2.x. With
 * rev 1.x we do not need to do anything in order to read the current timer counter
 * value
 */
#define LPTIMER_CNR_NEEDS_LATCHING 0

#elif K60_CPU_REV == 2

#define LPTIMER_CNR_NEEDS_LATCHING 1

#endif
/** @} */

/**
 * @name Bit band macros
 * @{
 */
/* Generic bitband conversion routine */
/** @brief Convert bit-band region address and bit number to bit-band alias address
 *
 * @param[in] addr base address in non-bit-banded memory
 * @param[in] bit  bit number within the word
 *
 * @return Address of the bit within the bit-band memory region
 */
#define BITBAND_ADDR(addr, bit) ((((uint32_t) (addr)) & 0xF0000000u) + 0x2000000 + ((((uint32_t) (addr)) & 0xFFFFF) << 5) + ((bit) << 2))

/**
 * @brief Bitband 32 bit access to variable stored in SRAM_U
 *
 * @note SRAM_L is not bit band aliased on the K60, only SRAM_U (0x20000000 and up)
 * @note var must be declared 'volatile'
 */
#define BITBAND_VAR32(var, bit) (*((uint32_t volatile*) BITBAND_ADDR(&(var), (bit))))

/**
 * @brief Bitband 16 bit access to variable stored in SRAM_U
 *
 * @note SRAM_L is not bit band aliased on the K60, only SRAM_U (0x20000000 and up)
 * @note var must be declared 'volatile'
 */
#define BITBAND_VAR16(var, bit) (*((uint16_t volatile*) BITBAND_ADDR(&(var), (bit))))

/**
 * @brief Bitband 8 bit access to variable stored in SRAM_U
 *
 * @note SRAM_L is not bit band aliased on the K60, only SRAM_U (0x20000000 and up)
 * @note var must be declared 'volatile'
 */
#define BITBAND_VAR8(var, bit) (*((uint8_t volatile*) BITBAND_ADDR(&(var), (bit))))

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H_ */
/** @} */
