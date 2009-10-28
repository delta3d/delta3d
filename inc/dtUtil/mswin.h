/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology
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
 * @author David Guthrie
*/

#ifndef DELTA_MSWIN
#define DELTA_MSWIN

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   // Identifier was truncated to '255' characters in the debug information
   #pragma warning( disable:4786 )
   // disable warning for stl classes "needs to have dll-interface to be used by clients of class"
   #pragma warning(disable : 4251)

   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
   #undef GetClassName
   #undef SendMessage
   #undef CreateFont
   #undef GetTimeFormat
   #undef FindResource //due to some windows.h include which conflicts with dtTerrain::FindResource()

#endif


#endif /* DELTA_MSWIN */
