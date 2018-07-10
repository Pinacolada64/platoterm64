/**
 * PLATOTerm64 - A PLATO Terminal for the Commodore 64
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * config.h - Configuration routines
 */

#ifndef CONFIG_H
#define CONFIG_H

#define IO_MODE_SERIAL        0
#define IO_MODE_ETHERNET      1

/* C64/128 Driver defines */
#define CONFIG_MOUSE_DRIVER_1351 "mou-1351"
#define CONFIG_MOUSE_DRIVER_JOY "mou-joy"
#define CONFIG_MOUSE_DRIVER_INKWELL "mou-inkwell"
#define CONFIG_MOUSE_DRIVER_POT "mou-pot"
#define CONFIG_SERIAL_DRIVER_SWIFTLINK "ser-swlink"
#define CONFIG_SERIAL_DRIVER_UP2400 "ser-up2400"

/* Atari Driver defines */
#define CONFIG_SERIAL_DRIVER_ATRRDEV "atrrdev.ser"
#define CONFIG_MOUSE_DRIVER_ATRAMI "atrami.mou"
#define CONFIG_MOUSE_DRIVER_ATRJOY "atrjoy.mou"
#define CONFIG_MOUSE_DRIVER_ATRST "atrst.mou"
#define CONFIG_MOUSE_DRIVER_ATRTRK "atrtrk.mou"
#define CONFIG_MOUSE_DRIVER_ATRTT "atrtt.mou"

/* Apple2 Driver Defines */
#define CONFIG_SERIAL_DRIVER_SSC "a2.ssc.ser"
#define CONFIG_MOUSE_DRIVER_STDMOU "a2.stdmou.mou"

#define CONFIG_DEFAULT_HOSTNAME "irata.online"

typedef struct configInfo
{
  unsigned char io_mode;
  unsigned char baud;
  unsigned char use_dhcp;
  unsigned long ip_address;
  unsigned long netmask;
  unsigned long gateway;
  unsigned long dns;
  unsigned char driver_ser[16];
  unsigned char driver_mou[16];
  unsigned char color_foreground;
  unsigned char color_background;
  unsigned char color_border;
  unsigned char hostname[64];
} ConfigInfo;

/**
 * config_init()
 * Initialize configuration and load either config or defaults.
 */
void config_init(void);

/**
 * config_load()
 * Load the configuration file, or if not found, set some defaults and save.
 */
void config_load(void);

/**
 * config_save()
 * Save the configuration file.
 */
void config_save(void);

/**
 * config_set_defaults()
 * Set default values for config file
 */
void config_set_defaults(void);

#endif /* CONFIG_H */
