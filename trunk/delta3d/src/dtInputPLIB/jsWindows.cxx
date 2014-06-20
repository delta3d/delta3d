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

     $Id: jsWindows.cxx 2114 2006-12-21 20:53:13Z fayjf $
*/

#include "js.h"

#if defined (UL_WIN32)

#define _JS_MAX_AXES_WIN 8  /* X,Y,Z,R,U,V,POV_X,POV_Y */

struct os_specific_s {
  JOYCAPS      jsCaps ;
  JOYINFOEX    js     ;
  UINT         js_id  ;
  static bool getOEMProductName ( jsJoystick* joy, char *buf, int buf_sz ) ;
};



// Inspired by
// http://msdn.microsoft.com/archive/en-us/dnargame/html/msdn_sidewind3d.asp

bool os_specific_s::getOEMProductName ( jsJoystick* joy, char *buf, int buf_sz )
{
  if ( joy->error )  return false ;

  union
  {
    char key   [ 256 ] ;
    char value [ 256 ] ;
  } ;
  char OEMKey [ 256 ] ;

  HKEY  hKey ;
  DWORD dwcb ;
  LONG  lr ;

  // Open .. MediaResources\CurrentJoystickSettings
  sprintf ( key, "%s\\%s\\%s",
            REGSTR_PATH_JOYCONFIG, joy->os->jsCaps.szRegKey,
            REGSTR_KEY_JOYCURR ) ;

  lr = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &hKey) ;

  if ( lr != ERROR_SUCCESS ) return false ;

  // Get OEM Key name
  dwcb = sizeof(OEMKey) ;

  // JOYSTICKID1-16 is zero-based; registry entries for VJOYD are 1-based.
  sprintf ( value, "Joystick%d%s", joy->os->js_id + 1, REGSTR_VAL_JOYOEMNAME ) ;

  lr = RegQueryValueEx ( hKey, value, 0, 0, (LPBYTE) OEMKey, &dwcb);
  RegCloseKey ( hKey ) ;

  if ( lr != ERROR_SUCCESS ) return false ;

  // Open OEM Key from ...MediaProperties
  sprintf ( key, "%s\\%s", REGSTR_PATH_JOYOEM, OEMKey ) ;

  lr = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &hKey ) ;

  if ( lr != ERROR_SUCCESS ) return false ;

  // Get OEM Name
  dwcb = buf_sz ;

  lr = RegQueryValueEx ( hKey, REGSTR_VAL_JOYOEMNAME, 0, 0, (LPBYTE) buf,
                           &dwcb ) ;
  RegCloseKey ( hKey ) ;

  if ( lr != ERROR_SUCCESS ) return false ;

  return true ;
}


void jsJoystick::open ()
{
  name [0] = '\0' ;

  os->js . dwFlags = JOY_RETURNALL ;
  os->js . dwSize  = sizeof ( os->js ) ;

  memset ( &(os->jsCaps), 0, sizeof(os->jsCaps) ) ;

  error = ( joyGetDevCaps( os->js_id, &(os->jsCaps), sizeof(os->jsCaps) )
                 != JOYERR_NOERROR ) ;
  num_buttons = os->jsCaps.wNumButtons ;
  if ( os->jsCaps.wNumAxes == 0 )
  {
    num_axes = 0 ;
    setError () ;
  }
  else
  {
    // Device name from jsCaps is often "Microsoft PC-joystick driver",
    // at least for USB.  Try to get the real name from the registry.
    if ( ! os->getOEMProductName ( this, name, sizeof(name) ) )
    {
      ulSetError ( UL_WARNING,
                   "JS: Failed to read joystick name from registry" ) ;

      strncpy ( name, os->jsCaps.szPname, sizeof(name) ) ;
    }

    // Windows joystick drivers may provide any combination of
    // X,Y,Z,R,U,V,POV - not necessarily the first n of these.
    if ( os->jsCaps.wCaps & JOYCAPS_HASPOV )
    {
      num_axes = _JS_MAX_AXES_WIN ;
      min [ 7 ] = -1.0 ; max [ 7 ] = 1.0 ;  // POV Y
      min [ 6 ] = -1.0 ; max [ 6 ] = 1.0 ;  // POV X
    }
    else
      num_axes = 6 ;

    min [ 5 ] = (float) os->jsCaps.wVmin ; max [ 5 ] = (float) os->jsCaps.wVmax ;
    min [ 4 ] = (float) os->jsCaps.wUmin ; max [ 4 ] = (float) os->jsCaps.wUmax ;
    min [ 3 ] = (float) os->jsCaps.wRmin ; max [ 3 ] = (float) os->jsCaps.wRmax ;
    min [ 2 ] = (float) os->jsCaps.wZmin ; max [ 2 ] = (float) os->jsCaps.wZmax ;
    min [ 1 ] = (float) os->jsCaps.wYmin ; max [ 1 ] = (float) os->jsCaps.wYmax ;
    min [ 0 ] = (float) os->jsCaps.wXmin ; max [ 0 ] = (float) os->jsCaps.wXmax ;
  }

  for ( int i = 0 ; i < num_axes ; i++ )
  {
    center    [ i ] = ( max[i] + min[i] ) / 2.0f ;
    dead_band [ i ] = 0.0f ;
    saturate  [ i ] = 1.0f ;
  }
}


void jsJoystick::close ()
{
	delete os;
}



jsJoystick::jsJoystick ( int ident )
{
  id = ident ;
  os = new struct os_specific_s;

  if (ident >= 0 && ident < (int)joyGetNumDevs()) {
        os->js_id = JOYSTICKID1 + ident;
        open();
  }
  else {
        num_axes = 0;
        setError();
  }
}



void jsJoystick::rawRead ( int *buttons, float *axes )
{
  if ( error )
  {
    if ( buttons )
      *buttons = 0 ;

    if ( axes )
      for ( int i = 0 ; i < num_axes ; i++ )
        axes[i] = 1500.0f ;

    return ;
  }

  MMRESULT status = joyGetPosEx ( os->js_id, &(os->js) ) ;

  if ( status != JOYERR_NOERROR )
  {
    setError() ;
    return ;
  }

  if ( buttons != NULL )
    *buttons = (int) os->js.dwButtons ;

  if ( axes != NULL )
  {
    /* WARNING - Fall through case clauses!! */

    switch ( num_axes )
    {
      case 8:
        // Generate two POV axes from the POV hat angle.
        // Low 16 bits of js.dwPOV gives heading (clockwise from ahead) in
        //   hundredths of a degree, or 0xFFFF when idle.

        if ( ( os->js.dwPOV & 0xFFFF ) == 0xFFFF )
        {
          axes [ 6 ] = 0.0 ;
          axes [ 7 ] = 0.0 ;
        }
        else
        {
          // This is the contentious bit: how to convert angle to X/Y.
          //    wk: I know of no define for PI that we could use here:
          //    SG_PI would pull in sg, M_PI is undefined for MSVC
          // But the accuracy of the value of PI is very unimportant at
          // this point.

          float s = (float) sin ( ( os->js.dwPOV & 0xFFFF ) * ( 0.01 * 3.1415926535f / 180 ) ) ;
          float c = (float) cos ( ( os->js.dwPOV & 0xFFFF ) * ( 0.01 * 3.1415926535f / 180 ) ) ;

          // Convert to coordinates on a square so that North-East
          // is (1,1) not (.7,.7), etc.
          // s and c cannot both be zero so we won't divide by zero.
          if ( fabs ( s ) < fabs ( c ) )
          {
            axes [ 6 ] = ( c < 0.0 ) ? -s/c  : s/c  ;
            axes [ 7 ] = ( c < 0.0 ) ? -1.0f : 1.0f ;
          }
          else
          {
            axes [ 6 ] = ( s < 0.0 ) ? -1.0f : 1.0f ;
            axes [ 7 ] = ( s < 0.0 ) ? -c/s  : c/s  ;
          }
        }

      case 6: axes[5] = (float) os->js . dwVpos ;
      case 5: axes[4] = (float) os->js . dwUpos ;
      case 4: axes[3] = (float) os->js . dwRpos ;
      case 3: axes[2] = (float) os->js . dwZpos ;
      case 2: axes[1] = (float) os->js . dwYpos ;
      case 1: axes[0] = (float) os->js . dwXpos ;
              break;

      default:
        ulSetError ( UL_WARNING, "PLIB_JS: Wrong num_axes. Joystick input is now invalid" ) ;
    }
  }
}

void jsInit() {}

#endif
