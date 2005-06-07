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

#ifndef DELTA_SOARX_TERRAIN
#define DELTA_SOARX_TERRAIN

#include "soarx/soarx_framework.h"
#include "soarx/soarx_generic.h"

namespace dtSOARX
{
   //______________________________________________________________
   // class declarations:

   class Index;
   class Vertex;
   class Data;

   //______________________________________________________________
   // Index datastructure stores the logical position on the grid

   class Index
   {
   public:
	   u32 q;
	   i32 x;
	   i32 y;
	   Index() : q(0), x(0), y(0) {}
	   Index(i32 ix, i32 iy) : x(ix), y(iy) {}
	   Index(u32 iq, i32 ix, i32 iy) : q(iq), x(ix), y(iy) {}
	   void clamp(i32 c) { x = (x < 0 ? 0 : (x < c ? x : c)); y = (y < 0 ? 0 : (y < c ? y : c));}
	   bool operator < (i32 i) { return (x < i) && (y < i); }
	   bool operator > (i32 i) { return (x > i) && (y > i); }
	   void operator >>= (i32 i) { x >>= i; y >>= i; }
	   void operator <<= (i32 i) { x <<= i; y <<= i; }
	   void operator &= (i32 i) { x &= i; y &= i; }
	   void operator += (Index i) { x += i.x; y += i.y; }
	   void operator -= (Index i) { x -= i.x; y -= i.y; }

	   Index(Index i, Index j, bool parity, bool direction)
	   {
		   if (direction) {
			   q = (i.q << 2) + (((i.q << 1) + j.q + 3) & 3) - 11;
		   } else {
			   q = (i.q << 2) + (((i.q << 1) + j.q + 2) & 3) - 11;
		   }
		   u32 t;

		   if (!parity ^ direction) {
			   t = j.x - i.y;
			   x = y = i.x + j.y;
		   } else {
			   t = i.x - j.y;
			   x = y = j.x + i.y;
		   }

		   x += t;
		   y -= t;
		   x >>= 1;
		   y >>= 1;

		   return;
	   }
   };

   //______________________________________________________________
   // this is the processed, ready to use vertex data

   class Vertex
   {
   public:
	   Index index;
	   v4f position;
	   f32 radius;
	   f32 error;
	   Vertex() : index(0, 0, 0) {}
	   Vertex(Index i) : index(i) {}
   };

   //______________________________________________________________
   // This is the raw data on the disk. It might be compressed,
   // encoded, and so on. It has to be converted to vertex data
   // before using.

   class Data
   {
   public:
	   f32 height;
	   f32 scale;
	   f32 error;
	   f32 radius;
   };

   //______________________________________________________________
   class ISystem;
   class TBuilder;
   class SOARX;

   // extern ISystem* sys;
   // extern TBuilder* tbuilder;
   // extern SOARX* terrain;
   // extern IOpenGL* opengl;

   //______________________________________________________________
};

#endif // DELTA_SOARX_TERRAIN
