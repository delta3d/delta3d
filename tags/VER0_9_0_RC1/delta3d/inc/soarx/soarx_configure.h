/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_SOARX_CONFIGURE
#define DELTA_SOARX_CONFIGURE

#ifdef NDEBUG
#define BUILD_TYPE "Release build"
#else
#define BUILD_TYPE "Debug build"
#endif

#define DATA_LAYOUT_LINEAR
//#define DATA_LAYOUT_EMBEDDED_QUADTREE
//#define DATA_LAYOUT_HIERARCHICAL_PI_ORDER

#endif // DELTA_SOARX_CONFIGURE
