#include "contiki.h"

#include <stdio.h>
#include <string.h>

#include "sys/autostart.h"
#include "dev/leds.h"

#include "cfs.h"
#include "cfs-coffee.h"
#include "xmem.h"

#include "core-clocks.h"
#include "uart.h"
#include "udelay.h"
#include "clock.h"
#include "llwu.h"
#include "init-net.h"
#include "power-control.h"
#include "voltage.h"
#include "K60.h"
#include "dbg-uart.h"
#include "devicemap.h"
#include "random.h"
#include "rtc.h"
#include "spi-config.h"
#include "spi-k60.h"
#include "nvram-spi-old.h"
#include "mulle-nvram.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...)     printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifdef MAKE_BOOTLOADER
#include "bootloader.h"
#endif

static nvram_t mulle_nvram_dev;
const nvram_t *mulle_nvram = NULL;
static nvram_spi_params_t nvram_spi_params = {
  .spi = FRAM_SPI_NUM,
  .cs = (1 << FRAM_CHIP_SELECT_PIN),
  .ctas = FRAM_CTAS,
  .address_count = 1,
};

static int mulle_nvram_init(void)
{
    union {
        uint32_t u32;
        uint8_t  u8[sizeof(uint32_t)];
    } rec;
    rec.u32 = 0;

    if (nvram_spi_init(&mulle_nvram_dev, &nvram_spi_params, MULLE_NVRAM_CAPACITY) != 0) {
        return -2;
    }
    mulle_nvram = &mulle_nvram_dev;

    if (mulle_nvram->read(mulle_nvram, &rec.u8[0], MULLE_NVRAM_MAGIC, sizeof(rec.u32)) != sizeof(rec.u32)) {
        return -3;
    }

    if (rec.u32 != MULLE_NVRAM_MAGIC_EXPECTED) {
        int i;
        union {
            uint64_t u64;
            uint8_t  u8[sizeof(uint64_t)];
        } zero;
        zero.u64 = 0;
        for (i = 0; i < MULLE_NVRAM_CAPACITY; i += sizeof(zero)) {
            if (mulle_nvram->write(mulle_nvram, &zero.u8[0], i, sizeof(zero.u64)) != sizeof(zero.u64)) {
                return -4;
            }
        }
        rec.u32 = MULLE_NVRAM_MAGIC_EXPECTED;
        if (mulle_nvram->write(mulle_nvram, &rec.u8[0], MULLE_NVRAM_MAGIC, sizeof(rec.u32)) != sizeof(rec.u32)) {
            return -5;
        }
    }
    return 0;
}

static void increase_boot_count(void)
{
    union {
        uint32_t u32;
        uint8_t  u8[sizeof(uint32_t)];
    } rec;
    rec.u32 = 0;
    if (mulle_nvram->read(mulle_nvram, &rec.u8[0], MULLE_NVRAM_BOOT_COUNT, sizeof(rec.u32)) != sizeof(rec.u32)) {
        return;
    }
    ++rec.u32;
    mulle_nvram->write(mulle_nvram, &rec.u8[0], MULLE_NVRAM_BOOT_COUNT, sizeof(rec.u32));
}

/*---------------------------------------------------------------------------*/
#define COFFEE_AUTO_FORMAT 1
static void
init_cfs(void)
{
  int fd;
  PRINTF("Initialize xmem and coffee...\n");
  xmem_init();
  PRINTF("Xmem initialized.\n");
#ifdef COFFEE_AUTO_FORMAT
  if((fd = cfs_open("formated", CFS_READ)) == -1) {
    /* Storage is not formated */
    PRINTF("Coffee not formated\n");
    if(cfs_coffee_format() == -1) {
      /* Format failed, bail out */
      PRINTF("Failed to format coffee, bail out\n");
      return;
    }
    if((fd = cfs_open("formated", CFS_WRITE)) == -1) {
      /* Failed to open file to indicate formated state. */
      PRINTF("Failed to open file to indicate formated state\n");
      return;
    }
    cfs_write(fd, "DO NOT REMOVE!", strlen("DO NOT REMOVE!"));
  }
  cfs_close(fd);
#endif /* COFFEE_AUTO_FORMAT */
  PRINTF("Coffee initialized.\r\n");
}
/*---------------------------------------------------------------------------*/
LLWU_CONTROL(deep_sleep);

/* C entry point (after startup code has executed) */
int
main(void)
{
  leds_arch_init();

  power_control_init();

  /* Turn off power to the on board peripherals to force a power on reset */
  power_control_vperiph_set(0);

  /* Set up core clocks so that timings will be correct in all modules */
  SystemInit();

  /*
   * There is probably some better place for this
   * Set all int priorities to max/2.
   */
  {
    int i;
    for(i = 0; i < PORTE_IRQn; ++i)
    {
      NVIC_SetPriority(i, 16/2);
    }
  }

  /* Update SystemCoreClock global var */
  SystemCoreClockUpdate();

  /* Set RTC time to 0 in order to start counting seconds. */
  rtc_time_set(0);
  rtc_start();

  dbg_uart_init();
  devicemap_init();

  llwu_init();
  llwu_enable_wakeup_module(LLWU_WAKEUP_MODULE_LPTMR);
  llwu_register(deep_sleep);
  /* Dont allow deep sleep for now because radio cant wake up the mcu from it. */
  /* TODO(Henrik) Fix this when a new revision is made of the hardware. */
#ifndef WITH_SLIP
  llwu_set_allow(deep_sleep, 1);
#else
  llwu_set_allow(deep_sleep, 0);
#endif

  /* Turn on power to the on board peripherals */
  power_control_vperiph_set(1);
  /* Turn on AVDD */
  /* Board errata: Power consumption in fact increases because of leakage
   * currents from floating transistor gates if AVDD is turned off when nothing
   * is connected to AVDD other than AREF. */
  /* voltage_read_vbat, voltage_read_vchr won't give correct values with avdd
   * turned off either. */
  power_control_avdd_set(1);

  udelay_init();

  /* Initialize SPI bus driver */
  board_spi_init();
  /** \todo make SPI0 on-demand clocked. */
  spi_start(0);

#ifndef WITH_SLIP
  PRINTF("Booted\n");
  PRINTF("CPUID: %08x\n", (unsigned int)SCB->CPUID);
  PRINTF("UID: %08x %08x %08x %08x\n", (unsigned int)SIM->UIDH, (unsigned int)SIM->UIDMH, (unsigned int)SIM->UIDML, (unsigned int)SIM->UIDL);
  PRINTF("Clocks:\n F_CPU: %u\n F_SYS: %u\n F_BUS: %u\n F_FLEXBUS: %u\n F_FLASH: %u\n", (unsigned int)SystemCoreClock, (unsigned int)SystemSysClock, (unsigned int)SystemBusClock, (unsigned int)SystemFlexBusClock, (unsigned int)SystemFlashClock);
#endif


  init_cfs();
#ifdef MAKE_BOOTLOADER
  bootloader_startup();
#endif

  /* Initialize NVRAM */
  int status = mulle_nvram_init();
  if (status == 0) {
    /* Increment boot counter */
    increase_boot_count();
  }

  /*
   * Initialize Contiki and our processes.
   */
  random_init((unsigned short)SIM->UIDL);
  rtimer_init();
  clock_init();

  process_init();
  process_start(&etimer_process, NULL);

  ctimer_init();


  init_net();
  voltage_init();

  autostart_start(autostart_processes);

  while(1) {
    while(process_run() > 0);
    llwu_sleep();
  }
}
