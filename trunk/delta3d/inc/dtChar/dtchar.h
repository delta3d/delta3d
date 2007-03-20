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

#ifndef DELTA_DTCHAR
#define DELTA_DTCHAR

#include "dtChar/character.h"
#include "dtChar/characterfilehandler.h"
#include "dtChar/chardrawable.h"
#include "dtChar/coremodel.h"
#include "dtChar/submesh.h"

/** The Character Library contains classes used to control the rendering of 
* animated characters.  
* dtChar uses <A HREF="http://www.vrlab.umu.se/research/replicantbody/">Replicant Body</A>
* and the <A HREF="http://cal3d.sourceforge.net/">Character Animation Library</A>.
*/
namespace dtChar
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#undef _AUTOLIBNAME
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2

#if defined(_DEBUG)
   #ifndef DT_CHAR_LIBRARY
      #define _AUTOLIBNAME  "dtChard.lib"
   #endif
   #define _AUTOLIBNAME1 "ReplicantBodyd.lib"
   #define _AUTOLIBNAME2 "cal3d_d.lib"
#else
   #ifndef DT_CHAR_LIBRARY
      #define _AUTOLIBNAME  "dtChar.lib"
   #endif
   #define _AUTOLIBNAME1 "ReplicantBody.lib"
   #define _AUTOLIBNAME2 "cal3d.lib"
#endif


#ifndef _NOAUTOLIBMSG

   #ifndef DT_CHAR_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )

#endif

#ifndef DT_CHAR_LIBRARY
   #pragma comment (lib, _AUTOLIBNAME)
#endif

#pragma comment (lib, _AUTOLIBNAME1)
#pragma comment (lib, _AUTOLIBNAME2)


#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif // DELTA_DTCHAR
