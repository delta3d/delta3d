/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.

     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

     For further information visit http://plib.sourceforge.net

     $Id: jsBSD.cxx 2063 2006-01-05 21:21:55Z fayjf $
*/

/*
 * Inspired by the X-Mame USB HID joystick driver for NetBSD and
 * FreeBSD by Krister Walfridsson <cato@df.lth.se>.
 * Incorporates the original analog joystick driver for BSD by
 * Stephen Montgomery-Smith <stephen@math.missouri.edu>, with
 * NetBSD mods courtesy of Rene Hexel.
 *
 * Bert Driehuis <driehuis@playbeing.org>
 *
 * Notes:
 * Hats are mapped to two axes for now. A cleaner implementation requires
 * an API extension, and to be useful for my devious purposes, FlightGear
 * would need to understand that.
 */

#include "js.h"

#if defined (UL_BSD)

#if defined(__NetBSD__) || defined(__FreeBSD__)
#define HAVE_USB_JS	1
#endif

#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#if defined(__FreeBSD__)
# include <sys/joystick.h>
#else
# include <machine/joystick.h>		// For analog joysticks
#endif
#ifdef HAVE_USB_JS
#if defined(__NetBSD__)
#ifdef HAVE_USBHID_H
#include <usbhid.h>
#else
#include <usb.h>
#endif
#elif defined(__FreeBSD__)
extern "C" {
#  if __FreeBSD_version < 500000
#    include <libusbhid.h>
#  else
#    define HAVE_USBHID_H 1
#    include <usbhid.h>
#  endif
}
#endif
#include <dev/usb/usb.h>
#include <dev/usb/usbhid.h>

/* Compatibility with older usb.h revisions */
#if !defined(USB_MAX_DEVNAMES) && defined(MAXDEVNAMES)
#define USB_MAX_DEVNAMES MAXDEVNAMES
#endif
#endif

static int hatmap_x[9] = { 0, 0, 1, 1, 1, 0, -1, -1, -1 };
static int hatmap_y[9] = { 0, 1, 1, 0, -1, -1, -1, 0, 1 };
struct os_specific_s {
  char             fname [128 ];
  int              fd;
  int              is_analog;
  // The following structure members are specific to analog joysticks
  struct joystick  ajs;
#ifdef HAVE_USB_JS
  // The following structure members are specific to USB joysticks
  struct hid_item *hids;
  int              hid_dlen;
  int              hid_offset;
  char            *hid_data_buf;
  int              axes_usage [ _JS_MAX_AXES ] ;
#endif
  // We keep button and axes state ourselves, as they might not be updated
  // on every read of a USB device
  int              cache_buttons ;
  float            cache_axes [ _JS_MAX_AXES ] ;
};

// Idents lower than USB_IDENT_OFFSET are for analog joysticks.
#define USB_IDENT_OFFSET	2

#define USBDEV "/dev/usb"
#define UHIDDEV "/dev/uhid"
#define AJSDEV "/dev/joy"

#ifdef HAVE_USB_JS
/*
 * findusbdev (and its helper, walkusbdev) try to locate the full name
 * of a USB device. If /dev/usbN isn't readable, we punt and return the
 * uhidN device name. We warn the user of this situation once.
 */
static char *
walkusbdev(int f, char *dev, char *out, int outlen)
{
  struct usb_device_info di;
  int i, a;
  char *cp;

  for (a = 1; a < USB_MAX_DEVICES; a++) {
    di.udi_addr = a;
    if (ioctl(f, USB_DEVICEINFO, &di) != 0)
      return NULL;
    for (i = 0; i < USB_MAX_DEVNAMES; i++)
      if (di.udi_devnames[i][0] &&
          strcmp(di.udi_devnames[i], dev) == 0) {
        cp = new char[strlen(di.udi_vendor) + strlen(di.udi_product) + 2];
        strcpy(cp, di.udi_vendor);
        strcat(cp, " ");
        strcat(cp, di.udi_product);
        strncpy(out, cp, outlen - 1);
	out[outlen - 1] = 0;
	delete cp;
        return out;
      }
  }
  return NULL;
}

static int
findusbdev(char *name, char *out, int outlen)
{
  int i, f;
  char buf[50];
  char *cp;
  static int protection_warned = 0;

  for (i = 0; i < 16; i++) {
    sprintf(buf, "%s%d", USBDEV, i);
    f = open(buf, O_RDONLY);
    if (f >= 0) {
      cp = walkusbdev(f, name, out, outlen);
      close(f);
      if (cp)
        return 1;
    } else if (errno == EACCES) {
      if (!protection_warned) {
        fprintf(stderr, "Can't open %s for read!\n",
          buf);
        protection_warned = 1;
      }
    }
  }
  return 0;
}

static int joy_initialize_hid(struct os_specific_s *os,
	int *num_axes, int *num_buttons)
{
  int size, is_joystick;
#ifdef HAVE_USBHID_H
  int report_id = 0;
#endif
  struct hid_data *d;
  struct hid_item h;
  report_desc_t rd;

  if ((rd = hid_get_report_desc(os->fd)) == 0)
    {
      fprintf(stderr, "error: %s: %s", os->fname, strerror(errno));
      return FALSE;
    }

  os->hids = NULL;

#ifdef HAVE_USBHID_H
  if (ioctl(os->fd, USB_GET_REPORT_ID, &report_id) < 0)
    {
      fprintf(stderr, "error: %s: %s", os->fname, strerror(errno));
      return FALSE;
    }

  size = hid_report_size(rd, hid_input, report_id);
#else
  size = hid_report_size(rd, 0, hid_input);
#endif
  os->hid_data_buf = new char[size];
  os->hid_dlen = size;

  is_joystick = 0;
#ifdef HAVE_USBHID_H
  d = hid_start_parse(rd, 1 << hid_input, report_id);
#else
  d = hid_start_parse(rd, 1 << hid_input);
#endif
  while (hid_get_item(d, &h))
  {
    int usage, page, interesting_hid;

    page = HID_PAGE(h.usage);
    usage = HID_USAGE(h.usage);

    /* This test is somewhat too simplistic, but this is how MicroSoft
     * does, so I guess it works for all joysticks/game pads. */
    is_joystick = is_joystick ||
      (h.kind == hid_collection &&
       page == HUP_GENERIC_DESKTOP &&
       (usage == HUG_JOYSTICK || usage == HUG_GAME_PAD));

    if (h.kind != hid_input)
      continue;

    if (!is_joystick)
      continue;

    interesting_hid = TRUE;
    if (page == HUP_GENERIC_DESKTOP)
    {
       switch(usage) {
         case HUG_X:
         case HUG_RX:
         case HUG_Y:
         case HUG_RY:
         case HUG_Z:
         case HUG_RZ:
         case HUG_SLIDER:
           if (*num_axes < _JS_MAX_AXES)
           {
             os->axes_usage[*num_axes] = usage;
             (*num_axes)++;
           }
           break;
         case HUG_HAT_SWITCH:
           if (*num_axes + 1 < _JS_MAX_AXES)	// Allocate two axes for a hat
           {
             os->axes_usage[*num_axes] = usage;
             (*num_axes)++;
             os->axes_usage[*num_axes] = usage;
             (*num_axes)++;
           }
           break;
         default:
           interesting_hid = FALSE;
      }
    }
    else if (page == HUP_BUTTON)
    {
      interesting_hid = (usage > 0) && (usage <= _JS_MAX_BUTTONS);

      if (interesting_hid && usage - 1 > *num_buttons)
      {
        *num_buttons = usage - 1;
      }
    }

    if (interesting_hid)
      {
        h.next = os->hids;
        os->hids = new struct hid_item;
        *os->hids = h;
      }
  }
  hid_end_parse(d);

  return (os->hids != NULL);
}
#endif

void jsJoystick::open ()
{
  char *cp;

  name [0] = '\0' ;

  for ( int i = 0 ; i < _JS_MAX_AXES ; i++ )
    os->cache_axes [ i ] = 0.0f ;

  os->cache_buttons = 0 ;

  os->fd = ::open ( os->fname, O_RDONLY | O_NONBLOCK) ;

  if (os->fd < 0 && errno == EACCES)
    fprintf(stderr, "%s exists but is not readable by you\n", os->fname);

  error = ( os->fd < 0 ) ;

  if ( error )
    return ;

  num_axes = 0;
  num_buttons = 0;
  if ( os->is_analog )
  {
    num_axes    =  2 ;
    num_buttons = 32 ;
    FILE *joyfile ;
    char joyfname [ 1024 ] ;
    int noargs, in_no_axes ;
  
    float axes  [ _JS_MAX_AXES ] ;
    int buttons [ _JS_MAX_AXES ] ;
  
    rawRead ( buttons, axes ) ;
    error = axes[0] < -1000000000.0f && axes[1] < -1000000000.0f ;
    if ( error )
      return ;
  
    sprintf( joyfname, "%s/.joy%drc", ::getenv ( "HOME" ), id ) ;
  
    joyfile = fopen ( joyfname, "r" ) ;
    error = ( joyfile == NULL ) ;
    if ( error )
    {
      ulSetError ( UL_WARNING, "unable to open calibration file %s (%s), joystick %i disabled (you can generate the calibration file with the plib-jscal utility)",
		   joyfname, strerror ( errno ), id + 1 );
      return ;
    }

    noargs = fscanf ( joyfile, "%d%f%f%f%f%f%f", &in_no_axes,
                      &min [ 0 ], &center [ 0 ], &max [ 0 ],
                      &min [ 1 ], &center [ 1 ], &max [ 1 ] ) ;
    error = noargs != 7 || in_no_axes != _JS_MAX_AXES ;
    fclose ( joyfile ) ;
    if ( error )
      return ;
  
    for ( int i = 0 ; i < _JS_MAX_AXES ; i++ )
    {
      dead_band [ i ] = 0.0f ;
      saturate  [ i ] = 1.0f ;
    }

    return;	// End of analog code
  }

#ifdef HAVE_USB_JS
  if ( !joy_initialize_hid(os, &num_axes, &num_buttons ) )
  {
     ::close(os->fd);
     error = 1;
     return;
  }

  cp = strrchr(os->fname, '/');
  if (cp) {
    if (findusbdev(&cp[1], name, sizeof(name)) == 0)
      strcpy(name, &cp[1]);
  }

  if ( num_axes > _JS_MAX_AXES )
    num_axes = _JS_MAX_AXES ;

  for ( int i = 0 ; i < _JS_MAX_AXES ; i++ )
  {
	// We really should get this from the HID, but that data seems
	// to be quite unreliable for analog-to-USB converters. Punt for
	// now.
    if ( os->axes_usage [ i ] == HUG_HAT_SWITCH )
    {
      max       [ i ] = 1.0f ;
      center    [ i ] = 0.0f ;
      min       [ i ] = -1.0f ;
    }
    else
    {
      max       [ i ] = 255.0f ;
      center    [ i ] = 127.0f ;
      min       [ i ] = 0.0f ;
    }
    dead_band [ i ] = 0.0f ;
    saturate  [ i ] = 1.0f ;
  }
#endif
}



void jsJoystick::close ()
{
  if (os) {
    if ( ! error )
      ::close ( os->fd ) ;
#ifdef HAVE_USB_JS
    if (os->hids)
      delete os->hids;
    if (os->hid_data_buf)
      delete os->hid_data_buf;
#endif
    delete os;
  }
}


jsJoystick::jsJoystick ( int ident )
{
  id = ident ;
  error = 0;

  os = new struct os_specific_s;
  memset(os, 0, sizeof(struct os_specific_s));
  if (ident < USB_IDENT_OFFSET)
    os->is_analog = 1;
  if (os->is_analog)
    sprintf(os->fname, "%s%d", AJSDEV, ident);
  else
    sprintf(os->fname, "%s%d", UHIDDEV, ident - USB_IDENT_OFFSET);
  open () ;
}


void jsJoystick::rawRead ( int *buttons, float *axes )
{
  int len, usage, page, d;
  struct hid_item *h;

  if ( error )
  {
    if ( buttons )
      *buttons = 0 ;

    if ( axes )
      for ( int i = 0 ; i < num_axes ; i++ )
        axes[i] = 1500.0f ;

    return ;
  }

  if ( os->is_analog )
  {
    int status = ::read ( os->fd, &os->ajs, sizeof(os->ajs) );
    if ( status != sizeof(os->ajs) ) {
      perror ( os->fname ) ;
      setError () ;
      return ;
    }
    if ( buttons != NULL )
      *buttons = ( os->ajs.b1 ? 1 : 0 ) | ( os->ajs.b2 ? 2 : 0 ) ;

    if ( axes != NULL )
    {
      if ( os->ajs.x >= -1000000000 )
        os->cache_axes[0] = os->ajs.x;
      if ( os->ajs.y >= -1000000000 )
        os->cache_axes[1] = os->ajs.y;

      axes[0] = os->cache_axes[0];
      axes[1] = os->cache_axes[1];
    }

    return;
  }

#ifdef HAVE_USB_JS
  while ((len = ::read(os->fd, os->hid_data_buf, os->hid_dlen)) == os->hid_dlen)
  {
    for (h = os->hids; h; h = h->next)
    {
      d = hid_get_data(os->hid_data_buf, h);
  
      page = HID_PAGE(h->usage);
      usage = HID_USAGE(h->usage);
  
      if (page == HUP_GENERIC_DESKTOP)
      {
        for (int i = 0; i < num_axes; i++)
          if (os->axes_usage[i] == usage)
          {
            if (usage == HUG_HAT_SWITCH)
            {
              if (d < 0 || d > 8)
                d = 0;	// safety
              os->cache_axes[i] = (float)hatmap_x[d];
              os->cache_axes[i + 1] = (float)hatmap_y[d];
            }
            else
            {
              os->cache_axes[i] = (float)d;
            }
            break;
          }
      }
      else if (page == HUP_BUTTON)
      {
         if (usage > 0 && usage < _JS_MAX_BUTTONS + 1)
         {
           if (d)
             os->cache_buttons |= (1 << usage - 1) ;
           else
             os->cache_buttons &= ~(1 << usage - 1) ;
         }
      }
    }
  }
  if (len < 0 && errno != EAGAIN)
  {
    perror( os->fname ) ;
    setError () ;
    error = 1;
  }
  if ( buttons != NULL ) *buttons = os->cache_buttons ;
  if ( axes    != NULL )
    memcpy ( axes, os->cache_axes, sizeof(float) * num_axes ) ;
#endif
}

void jsInit () {}

#endif
