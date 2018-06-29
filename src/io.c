/**
 * PLATOTerm64 - A PLATO Terminal for the Commodore 64
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * io.c - Input/output functions (serial/ethernet)
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <serial.h>
#include "ip65.h"
#include <stdint.h>
#include <peekpoke.h>
#include "io.h"
#include "protocol.h"
#include "prefs.h"
#include "config.h"

#define NULL 0

#define XOFF_THRESHOLD 160
#define XON_THRESHOLD  16

uint8_t xoff_enabled;

static uint8_t ch=0;
static uint8_t lastch=0;
static uint8_t io_res;
static uint8_t recv_buffer_size=0;
extern char temp_ip_address[64];
extern ConfigInfo config;

unsigned char buf[1500];
int len;

static struct ser_params params = {
  SER_BAUD_38400,
  SER_BITS_8,
  SER_STOP_1,
  SER_PAR_NONE,
  SER_HS_HW
};

extern uint8_t running;
extern uint8_t restart;

/**
 * io_init() - Set-up the I/O
 */
void io_init(void)
{
  io_res=ser_load_driver(config.driver_ser);
  xoff_enabled=false;
  
  if (io_res!=SER_ERR_OK)
    {
      POKE(0xD020,2);
      return;
    }

  io_open();

}

/**
 * io_open() - Open the device
 */
void io_open(void)
{
  if (config.io_mode == IO_MODE_SERIAL)
    {
      params.baudrate = config.baud;
      
      io_res=ser_open(&params);
      
      if (io_res!=SER_ERR_OK)
	{
	  POKE(0xD020,2);
	  return;
	}
      
      // Needed to enable up2400. Ignored with swlink.
      ser_ioctl(1, NULL);
    }
  else if (config.io_mode == IO_MODE_ETHERNET)
    {
      io_open_ethernet();
    }
}

/**
 * io_send_byte(b) - Send specified byte out
 */
void io_send_byte(uint8_t b)
{
  if (config.io_mode == IO_MODE_SERIAL)
    {
      if ((xoff_enabled==false) || (b==0x11))
	ser_put(b);
    }
  else if (config.io_mode == IO_MODE_ETHERNET)
    {
      tcp_send(&b,1);
    }
    
}

/**
 * io_recv_serial() - Receive and interpret serial data.
 */
void io_recv_serial(void)
{
  recv_buffer_size=PEEK(0x29B)-PEEK(0x29C)&0xff;
  if (recv_buffer_size>XOFF_THRESHOLD && xoff_enabled==false)
    {
      POKE(0xD020,0);
      ser_put(0x13);
      xoff_enabled=true;
    }
  else if (recv_buffer_size<XON_THRESHOLD && xoff_enabled==true)
    {
      POKE(0xD020,14);
      ser_put(0x11);
      xoff_enabled=false;
    }

  if (ser_get(&ch)==SER_ERR_OK)
    {
      // Detect and strip IAC escapes (two consecutive bytes of 0xFF)
      if (ch==0xFF && lastch == 0xFF)
	{
	  lastch=0x00;
	}
      else
	{
	  lastch=ch;
	  ShowPLATO(&ch,1);
	}
    }
}

/**
 * io_recv_ethernet() - Receive and interpret serial data.
 */
void io_recv_ethernet(void)
{
  uint16_t bufindex;
  ip65_process();
  if (len==-1)
    {
      // Disconnected. Restart.
      running=false;
      restart=true;
      prefs_display("disconnected. press return to restart");
      prefs_get_address();
    }
  else if (len)
    {
      for (bufindex=0;bufindex<len;bufindex++)
	{
	  if ((bufindex % 11) == 0)
	    ip65_process();
	  
	  ch=buf[bufindex];
	  if (ch==0xff && lastch == 0xFF)
	    {
	      lastch=0x00;
	    }
	  else
	    {
	      lastch=ch;
	      ShowPLATO(&ch,1);
	    }
	}
      len=0;
    }
}

/**
 * io_open_ethernet() - Open ethernet device and set up network connection.
 */
void io_open_ethernet(void)
{
  uint32_t address;
  uint8_t resolved=false;
  
  // I am re-using the prefs code to provide messages and prompts for ethernet.
  
  prefs_clear();
  prefs_display("initializing ip65...");
  if (ip65_init(DRV_INIT_DEFAULT))
    {
      prefs_select("failed. opening prefs.");
      prefs_clear();
      prefs_run();
      restart=true;
      running=false;
      return;
    }
  prefs_select("ok");
  
  if (config.use_dhcp==true)
    {
      prefs_display("dhcp...");
      if (dhcp_init())
	{
	  prefs_select("failed. opening prefs");
	  prefs_clear();
	  prefs_run();
	  restart=true;
	  running=false;
	  return;
	}
      prefs_select("ok");
    }

  while (resolved==false)
    {
      prefs_display("host (return for last): ");
      prefs_get_hostname();
      
      if (temp_ip_address[0]==0x0d) // RETURN was pressed.
	{
	  strcpy(temp_ip_address,config.hostname);
	}
      
      prefs_select("ok");
      prefs_clear();
      
      prefs_display("resolving host...");
      address=dns_resolve(temp_ip_address);
      
      if (address==0)
	{
	  prefs_select("failed.");
	  prefs_clear();
	}
      else
	{
	  prefs_select(dotted_quad(address));
	  prefs_clear();
	  prefs_save(); // Go ahead and save host for later.
	  resolved=true;
	}

      prefs_display("connecting...");

      if (tcp_connect(address,8005,tcp_recv))
	{
	  prefs_select("failed.");
	  prefs_clear();
	}
      else
	{
	  prefs_select("connected.");
	  resolved=true;
	  prefs_clear();
	}
    }
}

/**
 * io_done() - Called to close I/O
 */
void io_done(void)
{
  if (config.io_mode==IO_MODE_SERIAL)
    {
      ser_close();
      ser_uninstall();
    }
  else if (config.io_mode==IO_MODE_ETHERNET)
    {
      tcp_close();
    }
}

/**
 * tcp_recv(tcp_buf, tcp_len) - ip65 callback to fill buffer.
 */
void tcp_recv(const uint8_t* tcp_buf, int16_t tcp_len)
{
  if (len)
    return;

  len = tcp_len;
  
  if (len != 1)
    memcpy(buf,tcp_buf,len);
}
