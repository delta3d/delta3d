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

#ifndef DELTA_SOARX_CAMERA
#define DELTA_SOARX_CAMERA

#include "soarx/soarx_generic.h"

namespace dtSOARX
{
   class Camera;

   class Camera
   {
   public:
	   v4f m_orientation;
	   v4f m_position;
	   f32 m_fovy;
	   f32 m_aspect;
	   f32 m_heading;
	   f32 m_pitch;
	   f32 m_roll;
	   m4f m_view;
	   m4f m_projection;
	   v4f m_frustum_planes[6];
   public:
	   Camera();
	   ~Camera();

	   void reset();
	   void move(f32 x, f32 y, f32 z);
	   void turn(f32 heading, f32 pitch, f32 roll);
	   void update();
	   f32* get_projector(int cx, int cy);
	   void RenderFrustum();
   };
};

#endif // DELTA_SOARX_CAMERA
