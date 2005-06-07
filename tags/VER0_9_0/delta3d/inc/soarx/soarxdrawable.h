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

//______________________________________________________________

#ifndef DELTA_SOARXDRAWABLE
#define DELTA_SOARXDRAWABLE

#define DATA_LAYOUT_EMBEDDED_QUADTREE

#include "soarx/soarx_framework.h"
#include "soarx/soarx_generic.h"
#include "soarx/soarx_terrain.h"
#include "soarx/soarx_tbuilder.h"

#include <osg/Drawable>
#include <osg/Image>
#include <osg/Shape>
#include <osg/State>

#include <vector>

namespace dtSOARX
{
   //______________________________________________________________
   // class declarations:

   class Camera;
   class SOARX;
   typedef void* HANDLE;

   /**
    * An osg::Drawable implementation of the SOARX algorithm.
    */
   class SOARXDrawable : public osg::Drawable
   {
   private:
	   // textures
	   enum Textures {BASE_TEX, DETAIL_TEX, SCALE_TEX, SCREEN_TEX, NORMAL_TEX, CLOUD_TEX, COLOR_TEX, BUMP_TEX};
	   GLuint m_texture[8];
	   GLuint vertex_buffer[2];
	   GLuint index_buffer[2];

	   // vertex buffers
	   v4f* vertex_array;
	   u32* index_array;
	   u32 va_index;
	   u32 ia_index;
	   int page;

      // skirt
      float skirtHeight;
      std::vector<v4f> skirtVertices;
      std::vector<u32> skirtIndices;
      
	   // rendering
	   typedef void (SOARXDrawable::*Renderer)();
	   Renderer renderer[4];
	   static int m_max_renderer;
	   static int m_current_renderer;
	   int m_compatibility_level;

	   // misc
	   v4i window_size;
	   float m_time;

	   // camera
	   Camera* m_refinement_camera;
	   Camera* m_view_camera;

	   // refinement
	   const bool side;
	   const bool bottom;
	   bool left_only;
	   bool first;
	   float magic;
	   f32 radii[64];
	   f32* detail;
	   Data* base;
	   Data* baseq;
	   Vertex base_vertices[13];

	   // file map handles
	   HANDLE base_file;
	   HANDLE base_file_map;
	   HANDLE baseq_file;
	   HANDLE baseq_file_map;
	   HANDLE detail_file;
	   HANDLE detail_file_map;

	   int level;
	   int map_levels;
	   int detail_levels;
	   int base_levels;
	   f32 temp;

	   int map_bits;
	   int base_bits;
	   int detail_bits;
	   int embedded_bits;

	   int map_size;
	   int base_size;
	   int detail_size;
	   int embedded_size;

	   float base_horizontal_resolution;
	   float base_vertical_resolution;
	   float base_vertical_bias;

	   float detail_horizontal_resolution;
	   float detail_vertical_resolution;
	   float detail_vertical_bias;

	   float m_threshold;
	   float m_detail_multiplier;

      std::string mPath, mPrefix;
      
      Extensions* extensions;
      
      typedef void (APIENTRY *glDrawRangeElementsFunc)(GLenum mode, GLuint start, GLuint end,
                                             GLsizei count, GLenum type, const GLvoid *indices);
      typedef GLboolean (APIENTRY *glIsBufferFunc)(GLuint buffer);
      typedef GLvoid (APIENTRY *glGetBufferSubDataFunc)(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
      typedef GLvoid* (APIENTRY *glMapBufferFunc)(GLenum target, GLenum access);
      typedef GLboolean (APIENTRY *glUnmapBufferFunc)(GLenum target);
      typedef GLvoid (APIENTRY *glGetBufferParameterivFunc)(GLenum target, GLenum pname, GLint *params);
      typedef GLvoid (APIENTRY *glGetBufferPointervFunc)(GLenum target, GLenum pname, GLvoid **params);
      typedef GLvoid (APIENTRY *glActiveTextureFunc)(GLenum texture);
      
      glDrawRangeElementsFunc glDrawRangeElements;
      glIsBufferFunc glIsBuffer;
      glGetBufferSubDataFunc glGetBufferSubData;
      glMapBufferFunc glMapBuffer;
      glUnmapBufferFunc glUnmapBuffer;
      glGetBufferParameterivFunc glGetBufferParameteriv;
      glGetBufferPointervFunc glGetBufferPointerv;
      glActiveTextureFunc glActiveTexture;
      
   public:
	   SOARXDrawable();
   	
	   virtual osg::Object* cloneType() const;
	   virtual osg::Object* clone(const osg::CopyOp& copyop) const;
   	
	   virtual void drawImplementation(osg::State& state) const;
   	
      virtual bool supports(osg::PrimitiveFunctor&) const { return true; }

      virtual void accept(osg::PrimitiveFunctor& pf) const { 
         pf.setVertexArray(va_index, (osg::Vec4*)vertex_array);
         pf.drawElements(GL_TRIANGLE_STRIP, ia_index, (GLuint*)index_array);
      }
      
	   int OnRenderContextCreated();
	   int OnRenderContextDestroy();
	   int OnWindowResized(int width, int height);

	   int Init();
	   void CalculateRadii(float f);
	   int CycleRendrer();
	   static int SetRendrer(int i_renderer);
	   float SetThreshold(float i_threshold);
	   float SetDetailMultiplier(float i_detail_multiplier);
	   void Render(osg::State& state);
	   
	   void Render0();
	   void Render1();
	   void Render2();
	   void Render3();
	   void Render_Sky();
	   void ViewTexture(u8 tex);
	   void LoadTextures();
	   void Setup();
	   
	   /**
	    * Loads terrain data.
	    *
	    * @param path the terrain path
	    * @param prefix the terrain prefix
	    * @param hf the osg::HeightField
	    * @param baseBits the base size in bits
	    * @param baseHorizontalResolution the base horizontal resolution
	    */
	   int Load(const char* path,
	            const char* prefix,
	            osg::HeightField* hf,
	            int baseBits,
	            float baseHorizontalResolution);
	   
	   // void SetCamera(Camera* i_refinement_camera, Camera* i_view_camera);
	   // float Attach(Camera* i_camera, int attach_mode);

      float GetHeight(float x, float y);
      
   protected:
	   virtual ~SOARXDrawable();
	   virtual osg::BoundingBox computeBound() const;
      
   private:
	   Vertex GetVertex(i32 x, i32 y);
	   void GetVertex(Vertex& v);
	   bool Active(Vertex& v, u32& planes);
	   void Append(Vertex& v);
	   void TurnCorner();
	   void RefineTop();
	   void Refine(Vertex& i, Vertex& j, bool in, bool out, u32 planes);
	   void RefineL(Vertex& i, Vertex& j, bool in, u32 planes);
	   void RefineR(Vertex i, Vertex& j, bool out, u32 planes);
   };
};

#endif // DELTA_SOARXDRAWABLE
