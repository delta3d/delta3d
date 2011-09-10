/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 */

#ifndef DTGUI_H
#define DTGUI_H

#include <dtGUI/ceuidrawable.h>
#include <dtGUI/scriptmodule.h>

/** The dtGUI namespace contains the functionality to render and control OpenGL
 * graphical user interfaces.  dtGUI uses <A HREF="http://www.cegui.org.uk/">CEGUI</A>
 * for high-level control of the widgets.
 */
namespace dtGUI
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME

#if defined(_DEBUG)
   #ifndef DT_GUI_LIBRARY  
      #define  _AUTOLIBNAME "dtGUId.lib"
   #endif
#else
   #ifndef DT_GUI_LIBRARY
      #define _AUTOLIBNAME "dtGUI.lib"
   #endif
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
# ifndef DT_GUI_LIBRARY
   #pragma message("Will automatically link with " _AUTOLIBNAME)
# endif
#endif

#ifndef DT_GUI_LIBRARY
#   pragma comment(lib, _AUTOLIBNAME)
#endif

#endif //win32

#endif //DTGUI_H
