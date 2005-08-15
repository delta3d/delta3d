/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005, BMH Associates, Inc. 
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
* @author Matthew W. Campbell
*/
#ifndef __plugin_export__h
#define __plugin_export__h

/**
 * This is modeled from the DT_EXPORT macro found in dtCore/export.h.  
 * We define another due to conflicts with using the DT_EXPORT while 
 * trying to import Delta3D symbols.  The DT_PLUGIN macro should be used
 * in front of any classes that are to be exported from an ActorRegistryPlugin
 * library or any other Delta3D plugin.
 */
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef DT_PLUGIN
#    define DT_PLUGIN_EXPORT __declspec(dllexport)
#  else
#	  define DT_PLUGIN_EXPORT __declspec(dllimport)
#	endif
#else
#  define DT_PLUGIN_EXPORT
#endif

#endif
