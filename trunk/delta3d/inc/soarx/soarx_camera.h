#pragma once
#include "soarx_generic.h"

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
