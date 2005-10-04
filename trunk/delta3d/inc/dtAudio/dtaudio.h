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

#ifndef  DELTA_DTAUDIO
#define  DELTA_DTAUDIO

#include <dtAudio/sound.h>
#include <dtAudio/listener.h>
#include <dtAudio/audiomanager.h>
#include <dtAudio/soundeffectbinder.h>

/** The Audio Library contains functionality for controlling the audio playback
*  in applications.
*/
namespace dtAudio
{
}

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

/* You may turn off this include message by defining _NOAUTOLIB */
#undef _AUTOLIBNAME1
#undef _AUTOLIBNAME2
#undef _AUTOLIBNAME3

#if   defined(_DEBUG)
   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtAudiod.lib"
   #endif
#else
   #ifndef  DT_LIBRARY
      #define  _AUTOLIBNAME1  "dtAudio.lib"
   #endif
#endif

#define  _AUTOLIBNAME2  "OpenAL32.lib" // Note: Not Debug
#define  _AUTOLIBNAME3  "ALut.lib"     // Note: Not Debug

#if   !  defined(_NOAUTOLIBMSG)
   #ifndef  DT_LIBRARY
      #pragma message( "Will automatically link with " _AUTOLIBNAME1 )
   #endif

   #pragma message( "Will automatically link with " _AUTOLIBNAME2 )
   #pragma message( "Will automatically link with " _AUTOLIBNAME3 )
#endif

#ifndef  DT_LIBRARY
   #pragma  comment( lib, _AUTOLIBNAME1 )
#endif

#pragma  comment( lib, _AUTOLIBNAME2 )
#pragma  comment( lib, _AUTOLIBNAME3 )

#endif  // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#endif   // DELTA_DTAUDIO
