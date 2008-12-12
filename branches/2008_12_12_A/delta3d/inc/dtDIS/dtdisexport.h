/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_EXPORT_H__
#define __DELTA_DTDIS_EXPORT_H__

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef DT_DIS_LIBRARY
#    define DT_DIS_EXPORT __declspec(dllexport)
#  else
#    define DT_DIS_EXPORT __declspec(dllimport)
#  endif
#else
#   ifdef DT_DIS_LIBRARY
#      define DT_DIS_EXPORT __attribute__ ((visibility("default")))
#   else
#      define DT_DIS_EXPORT
#   endif 
#endif

/// the Delta3D support framework for the DIS network protocol.
namespace dtDIS
{
}

#endif // __DELTA_DTDIS_EXPORT_H__
