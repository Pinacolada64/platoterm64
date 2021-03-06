/**
 * PLATOTerm64 - A PLATO Terminal for the Commodore 64
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * prefs.h - Preferences menu functions
 */

#ifndef PREFS_H
#define PREFS_H

/**
 * Run the preferences menu
 */
void prefs_run(void);

/**
 * prefs_interface(void)
 * Preferences menu to select interface.
 */
void prefs_interface(void);

/**
 * prefs_serial()
 * Preferences menu to show for serial devices.
 */
void prefs_serial(void);

/**
 * prefs_driver(void)
 * Preferences menu to select serial driver
 */
void prefs_driver(void);

/**
 * prefs_baud(void)
 * Preferences for selecting serial baud rate
 */
void prefs_baud(void);

/**
 * prefs_touch(void)
 * Preferences for selecting touch driver
 */
void prefs_touch(void);

/**
 * prefs_save(void)
 * Save preferences
 */
void prefs_save(void);

/**
 * prefs_get_address()
 * get string with ip address numbers, terminated by return.
 */
void prefs_get_address(void);

/**
 * prefs_ip(void)
 * Preferences menu for IP address
 */
void prefs_ip(void);

/**
 * prefs_netmask(void)
 * Preferences menu for netmask
 */
void prefs_netmask(void);

/**
 * prefs_ip(void)
 * Preferences menu for IP address
 */
void prefs_gateway(void);

/**
 * prefs_ethernet(void)
 * Preferences menu to show for ethernet devices.
 */
void prefs_ethernet(void);

/**
 * prefs_display(text)
 * Display a line of the preferences menu
 */
void prefs_display(const char* text);

/**
 * Wait for a key matching input, return it.
 */
unsigned char prefs_get_key_matching(const char* matches);

/**
 * TEMPORARY: Wait for a key matching input, return it.
 */
unsigned char prefs_get_key_matching1(const char* matches);

/**
 * indicate selection, display it, and wait a bit for visual confirmation.
 */
void prefs_select(const char* text);

/**
 * erase prefs bar
 */
void prefs_clear(void);

/**
 * Update program state with selected preferences
 */
void prefs_update(void);

/**
 * Clear the preferences menu from screen. 
 */
void prefs_done(void);

#endif
