//______________________________________________________________
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // _WIN32

#include <cstdlib>
#include <string>
#include "soarx/soarx_framework.h"
#include "soarx/soarx_generic.h"
#include "soarx/soarxdrawable.h"
#include "soarx/soarx_camera.h"

#include <iostream>

#include "dtCore/notify.h"

#include <osg/GL>
#include <osg/GLExtensions>

#include <osgDB/FileUtils>

#include <ul.h>

using namespace std;
using namespace dtSOARX;

int SOARXDrawable::m_max_renderer = 3;
int SOARXDrawable::m_current_renderer = 0;

//______________________________________________________________
SOARXDrawable::SOARXDrawable():
side(true),
bottom(false),
left_only(true),
level(0),
va_index(0),
ia_index(0),
page(0),
skirtHeight(200.0f),
m_threshold(2.0f),
m_detail_multiplier(7.0f),
m_compatibility_level(0),
extensions(NULL)
{
   setSupportsDisplayList(false);
   
   renderer[0] = Render0;
   renderer[1] = Render1;
   renderer[2] = Render2;

   m_view_camera = m_refinement_camera = new Camera;
   
   window_size.x = 640;
	window_size.y = 480;
}

osg::Object* SOARXDrawable::cloneType() const
{
   return new SOARXDrawable;
}

osg::Object* SOARXDrawable::clone(const osg::CopyOp& copyop) const
{
   return NULL; // NYI
}

void SOARXDrawable::drawImplementation(osg::State& state) const
{
   SOARXDrawable* mutable_this = (SOARXDrawable*)this;
   
   mutable_this->Render(state);
}

osg::BoundingBox SOARXDrawable::computeBound() const
{
   float horizontal_size = base_horizontal_resolution * base_size;

   osg::BoundingBox bb = osg::BoundingBox(
      0, -horizontal_size, -10000,
      horizontal_size, 0, 10000
      );
   
   setBound( bb );
   
   return bb;
}

//______________________________________________________________
SOARXDrawable::~SOARXDrawable()
{
	// sys->Unsubscribe("Event.WindowResized", Callback(this, OnWindowResized));
	// sys->Unsubscribe("Event.RenderContextCreated", Callback(this, OnRenderContextCreated));
	// sys->Unsubscribe("Event.RenderContextDestroy", Callback(this, OnRenderContextDestroy));

   if(extensions != NULL)
   {
      OnRenderContextDestroy();
   
	   ::UnmapViewOfFile(detail);
	   ::CloseHandle(detail_file_map);
	   ::CloseHandle(detail_file);

	   ::UnmapViewOfFile(base);
	   ::CloseHandle(base_file_map);
	   ::CloseHandle(base_file);

	   ::UnmapViewOfFile(baseq);
	   ::CloseHandle(baseq_file_map);
	   ::CloseHandle(baseq_file);
   }
}

//______________________________________________________________
int SOARXDrawable::OnRenderContextCreated()
{
	int vertex_buffer_size = 512*1024*4*4 + 1024*4*4;
	int index_buffer_size = 512*1024*4 + 1024*4;

	extensions->glGenBuffers(2, vertex_buffer);
	extensions->glGenBuffers(2, index_buffer);

	extensions->glBindBuffer(GL_ARRAY_BUFFER_ARB, vertex_buffer[0]);
	extensions->glBufferData(GL_ARRAY_BUFFER_ARB, vertex_buffer_size, 0, GL_STREAM_DRAW_ARB);
	extensions->glBindBuffer(GL_ARRAY_BUFFER_ARB, vertex_buffer[1]);
	extensions->glBufferData(GL_ARRAY_BUFFER_ARB, vertex_buffer_size, 0, GL_STREAM_DRAW_ARB);

	extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buffer[0]);
	extensions->glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buffer_size, 0, GL_STREAM_DRAW_ARB);
	extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buffer[1]);
	extensions->glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buffer_size, 0, GL_STREAM_DRAW_ARB);

	return 0;
}

//______________________________________________________________
int SOARXDrawable::OnRenderContextDestroy()
{
	extensions->glDeleteBuffers(2, vertex_buffer);
	extensions->glDeleteBuffers(2, index_buffer);
	return 0;
}

//______________________________________________________________
int SOARXDrawable::OnWindowResized(int width, int height)
{
	window_size.x = width;
	window_size.y = height;
	magic = static_cast<float>(window_size.y) / (deg2rad(m_refinement_camera->m_fovy) * m_threshold);
	return 0;
}

//______________________________________________________________
int SOARXDrawable::Init()
{
   extensions = getExtensions(0, true);
   
   glDrawRangeElements = 
      (glDrawRangeElementsFunc)osg::getGLExtensionFuncPtr("glDrawRangeElements");
      
   glIsBuffer =
      (glIsBufferFunc)osg::getGLExtensionFuncPtr("glIsBufferARB");
   
   glGetBufferSubData =
      (glGetBufferSubDataFunc)osg::getGLExtensionFuncPtr("glGetBufferSubDataARB");
   
   glMapBuffer =
      (glMapBufferFunc)osg::getGLExtensionFuncPtr("glMapBufferARB");
   
   glUnmapBuffer =
      (glUnmapBufferFunc)osg::getGLExtensionFuncPtr("glUnmapBufferARB");
   
   glGetBufferParameteriv =
      (glGetBufferParameterivFunc)osg::getGLExtensionFuncPtr("glGetBufferParameterivARB");
   
   glGetBufferPointerv =
      (glGetBufferPointervFunc)osg::getGLExtensionFuncPtr("glGetBufferPointervARB");
   
   glActiveTexture =
      (glActiveTextureFunc)osg::getGLExtensionFuncPtr("glActiveTexture");
   
   OnRenderContextCreated();
   
	return 0;
}

//______________________________________________________________
void SOARXDrawable::CalculateRadii(float f)
{
	f32 t = (1.0f/sqrtf(2.0f));
	radii[0] = 1.0f / (1.0f - t) * detail_horizontal_resolution * map_size * 0.5f * f;
	for (u32 i=1; i<64; i++) {
		radii[i] = radii[i-1] * t;
	}

	for (u32 i=0; i<64; i++) {
		radii[i] = radii[i] * radii[i];
	}
}

/**
 * Loads terrain data.
 *
 * @param path the terrain path
 * @param prefix the terrain prefix
 * @param hf the osg::HeightField
 * @param baseBits the base size in bits
 * @param baseHorizontalResolution the base horizontal resolution
 */
int SOARXDrawable::Load(const char* path,
	                     const char* prefix,
	                     osg::HeightField* hf,
	                     int baseBits,
	                     float baseHorizontalResolution)
{
   base_bits = baseBits; // static_cast<int>(sys->GetGlobalNumber("Terrain.base_bits"));
   base_horizontal_resolution = baseHorizontalResolution; // static_cast<float>(sys->GetGlobalNumber("Terrain.horizontal_resolution"));
   
   map_bits = 16; // static_cast<int>(sys->GetGlobalNumber("Terrain.map_bits"));
	detail_bits = 10; // static_cast<int>(sys->GetGlobalNumber("Terrain.detail_bits"));
	base_vertical_resolution = 0.1f; // static_cast<float>(sys->GetGlobalNumber("Terrain.vertical_resolution"));
	base_vertical_bias = 0; // static_cast<float>(sys->GetGlobalNumber("Terrain.vertical_bias"));
	m_threshold = 2.0f; //static_cast<float>(sys->GetGlobalNumber("Terrain.error_threshold"));
	embedded_bits = map_bits - base_bits;
	map_size = (1 << map_bits) + 1;
	base_size = (1 << base_bits) + 1;
	detail_size = 1 << detail_bits;
	embedded_size = 1 << embedded_bits;
	detail_horizontal_resolution = base_horizontal_resolution / static_cast<float>(embedded_size);
	detail_vertical_resolution = 0.0012; // static_cast<float>(sys->GetGlobalNumber("Terrain.detail_vertical_resolution"));
	detail_vertical_bias = -32768.0f * detail_vertical_resolution;
	map_levels = map_bits << 1;
	detail_levels = detail_bits << 1;
	base_levels = base_bits << 1;

   dirtyBound();
   
	temp = 1.0f / embedded_size;

	magic = static_cast<float>(window_size.y) / (deg2rad(m_refinement_camera->m_fovy) * m_threshold);
	CalculateRadii(m_detail_multiplier);

	int c4 = (map_size - 1);
	int c2 = c4 >> 1;
	int c1 = c2 >> 1;
	int c3 = c2 + c1;

	base_vertices[0].index = Index(0, 0, c4);
	base_vertices[1].index = Index(1, c4, c4);
	base_vertices[2].index = Index(2, c4, 0);
	base_vertices[3].index = Index(3, 0, 0);
	base_vertices[4].index = Index(4, c2, c2);
	base_vertices[5].index = Index(5, c2, 0);
	base_vertices[6].index = Index(6, c4, c2);
	base_vertices[7].index = Index(7, c2, c4);
	base_vertices[8].index = Index(8, 0, c2);
	base_vertices[9].index = Index(9, c3, c1);
	base_vertices[10].index = Index(14, c1, c1);
	base_vertices[11].index = Index(19, c1, c3);
	base_vertices[12].index = Index(24, c3, c3);

   std::string pathPlusPrefix(path);
   
   if(prefix != NULL)
   {
      pathPlusPrefix.append(prefix);
   }
   
	std::string base_file_path(pathPlusPrefix);
	std::string baseq_file_path(pathPlusPrefix);
	std::string detail_file_path(path);
   
	base_file_path.append("base.t");
	baseq_file_path.append("base.q");
	detail_file_path.append("detail.t");

	base_file = ::CreateFile(base_file_path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	baseq_file = ::CreateFile(baseq_file_path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	detail_file = ::CreateFile(detail_file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

#ifdef DATA_LAYOUT_EMBEDDED_QUADTREE
	if (base_file == INVALID_HANDLE_VALUE || baseq_file == INVALID_HANDLE_VALUE || detail_file == INVALID_HANDLE_VALUE) {
#else
	if (base_file == INVALID_HANDLE_VALUE || detail_file == INVALID_HANDLE_VALUE) {
#endif
      TBuilder tbuilder;
      
		tbuilder.Build(
		   path, 
		   prefix, 
		   map_bits, 
		   base_bits, 
		   detail_bits, 
		   base_horizontal_resolution, 
		   base_vertical_resolution, 
		   base_vertical_bias,
		   detail_vertical_resolution,
		   hf,
		   detail_file == INVALID_HANDLE_VALUE
		);
		
		base_file = ::CreateFile(base_file_path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	   baseq_file = ::CreateFile(baseq_file_path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	   
	   if(detail_file == INVALID_HANDLE_VALUE)
	   {
	      detail_file = ::CreateFile(detail_file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		}
		
		//return -1;
	} else {
		// sys->ExecuteString("Console.Toggle(0)");
	}

	base_file_map = ::CreateFileMapping(base_file, 0, PAGE_READONLY, 0, 0, 0);
	baseq_file_map = ::CreateFileMapping(baseq_file, 0, PAGE_READONLY, 0, 0, 0);
	detail_file_map = ::CreateFileMapping(detail_file, 0, PAGE_READONLY, 0, 0, 0);
	
	base = reinterpret_cast<Data*>(::MapViewOfFile(base_file_map, FILE_MAP_READ, 0, 0, 0));
	baseq = reinterpret_cast<Data*>(::MapViewOfFile(baseq_file_map, FILE_MAP_READ, 0, 0, 0));
	detail = reinterpret_cast<f32*>(::MapViewOfFile(detail_file_map, FILE_MAP_READ, 0, 0, 0));

	for (int i=0; i<13; i++) {
#ifdef DATA_LAYOUT_EMBEDDED_QUADTREE
		Data data = baseq[base_vertices[i].index.q];
		base_vertices[i].error = data.error;
		base_vertices[i].radius = data.radius;
		base_vertices[i].position.x = detail_horizontal_resolution * base_vertices[i].index.x;
		base_vertices[i].position.z = detail_horizontal_resolution * base_vertices[i].index.y;
		base_vertices[i].position.y = data.height;
#else
		GetVertex(base_vertices[i]);
#endif
	}
   
	return 0;
}


//______________________________________________________________
void SOARXDrawable::Render(osg::State& state)
{
   if(extensions == NULL)
   {
      Init();
   }
   
	m_time = (float)state.getFrameStamp()->getReferenceTime();
   
   if(state.getCurrentViewport() != NULL &&
      (window_size.x != state.getCurrentViewport()->width() ||
       window_size.y != state.getCurrentViewport()->height()))
   {
      OnWindowResized(
         state.getCurrentViewport()->width(),
         state.getCurrentViewport()->height()
      );
	}
	
	osg::Matrix persp = state.getProjectionMatrix();
	
	double fovy, aspect, nearClip, farClip;
	
	persp.getPerspective(fovy, aspect, nearClip, farClip);
	
   m_view_camera->m_aspect = aspect;
   m_view_camera->m_fovy = fovy;
   
   magic = static_cast<float>(window_size.y) / (deg2rad(m_refinement_camera->m_fovy) * m_threshold);
   
	//glMatrixMode(GL_PROJECTION);
	//glLoadTransposeMatrixf(m_view_camera->m_projection);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadTransposeMatrixf(m_view_camera->m_view);

   osg::RefMatrix* mat = new osg::RefMatrix(state.getModelViewMatrix());
   
   osg::Matrix postRot;
   
   postRot.makeRotate(osg::PI_2, 1, 0, 0);
   
   mat->preMult(postRot);
   
   state.applyModelViewMatrix(mat);
   
   osg::Matrix invModelView = osg::Matrix::inverse(*mat);
   
   osg::Vec3 trans = invModelView.getTrans();
   
   m_view_camera->m_position.v[0] = trans[0];
   m_view_camera->m_position.v[1] = trans[1];
   m_view_camera->m_position.v[2] = trans[2];
   
   osg::Quat rot;
   
   invModelView.get(rot);
   
   m_view_camera->m_orientation.v[0] = rot[0];
   m_view_camera->m_orientation.v[1] = rot[1];
   m_view_camera->m_orientation.v[2] = rot[2];
   m_view_camera->m_orientation.v[3] = rot[3];
   
   m_view_camera->update();
   
   state.disableAllVertexArrays();
   
   extensions->glBindBuffer(GL_ARRAY_BUFFER_ARB, vertex_buffer[page]);
   extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buffer[page]);
   
	vertex_array = static_cast<v4f*>(glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB));
	index_array = static_cast<u32*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB));

	if (vertex_array == 0) {
		return;
	}

	if (index_array == 0) {
		return;
	}

	RefineTop();

	if (glUnmapBuffer(GL_ARRAY_BUFFER_ARB) == GL_FALSE) {
		return;
	}
	
	if (glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB) == GL_FALSE) {
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, 0, 0);
	page = 1-page;

   // glClearColor(0, 0.4f, 0.5f, 0);
	// glClearColor(0.6f, 0.7f, 0.8f, 0);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
   
   glDrawRangeElements(GL_TRIANGLE_STRIP, 0, va_index, ia_index, GL_UNSIGNED_INT, 0);
   
   extensions->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
   extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
   
   state.dirtyVertexPointer();
   //state.dirtyAllModes();
   //state.dirtyAllAttributes();
   state.dirtyAllVertexArrays();
      
	// static char* render_str[] = {
	// 	"2 pass full quality",
	// 	"1 pass, pseudo coloring",
	// 	"2 pass solid wireframe"
	// };

	// static float passes[] = {2, 1, 2};

	// float fps = sys->GetFPS();
	// sys->OverlayWrite(7, "Triangles: %luK (%luK)", ia_index >> 10, va_index >> 10);
	// sys->OverlayWrite(8, "Stripping ratio: %.2f", double(ia_index) / double(va_index));
	// sys->OverlayWrite(9, "Throughput: %.2fMtris/sec", ia_index*fps*passes[m_current_renderer]*0.000001);
	// sys->OverlayWrite(10, "Renderer: %s", render_str[m_current_renderer]);
	// sys->OverlayWrite(11, "FPS: %.0f", fps);

//	ViewTexture(m_texture[BUMP]);

}

//______________________________________________________________
void SOARXDrawable::ViewTexture(u8 tex)
{
	if (tex == 0) {
		return;
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
	glTexCoord4f(0, 0, 0, 1);
	glVertex4f(-0.9f, -0.9f, 0, 1);
	glTexCoord4f(2.0f, 0, 0, 1);
	glVertex4f(0.9f, -0.9f, 0, 1);
	glTexCoord4f(2.0f, 2.0f, 0, 1);
	glVertex4f(0.9f, 0.9f, 0, 1);
	glTexCoord4f(0, 2.0f, 0, 1);
	glVertex4f(-0.9f, 0.9f, 0, 1);
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}

/*

void SOARX::SetCamera(Camera* i_refinement_camera, Camera* i_view_camera)
{
	m_refinement_camera = i_refinement_camera;
	m_view_camera = i_view_camera;
}

float SOARX::Attach(Camera* c, int attach_mode)
{
#ifdef DATA_LAYOUT_EMBEDDED_QUADTREE
	return 0;
#else

	if (m_compatibility_level == 2) {
		return 0;
	}

	float x = c->m_position.x;
	float z = c->m_position.z;

	x /= detail_horizontal_resolution;
	z /= detail_horizontal_resolution;
	float xq = floorf(x);
	float zq = floorf(z);
	x -= xq;
	z -= zq;

	Index i0(u32(xq+0), u32(zq+0));
	Index i1(u32(xq+1), u32(zq+0));
	Index i2(u32(xq+0), u32(zq+1));
	Index i3(u32(xq+1), u32(zq+1));

	i0.clamp(map_size);
	i1.clamp(map_size);
	i2.clamp(map_size);
	i3.clamp(map_size);

	Vertex v0(i0);
	Vertex v1(i1);
	Vertex v2(i2);
	Vertex v3(i3);

	GetVertex(v0);
	GetVertex(v1);
	GetVertex(v2);
	GetVertex(v3);

	f32 height = lerp(z, lerp(x, v0.position.y, v1.position.y), lerp(x, v2.position.y, v3.position.y)) + 1.8f;

	if (attach_mode == 0) { // fly
		c->m_position.y = max(c->m_position.y, height);
	} else if (attach_mode == 1) { // walk
		c->m_position.y = height;
	}

	return c->m_position.y - height + 1.8f;

#endif

}

*/

float SOARXDrawable::GetHeight(float x, float y)
{
   x /= detail_horizontal_resolution;
	y = -y/detail_horizontal_resolution;
	float xq = floorf(x);
	float yq = floorf(y);
	x -= xq;
	y -= yq;

	Index i0(u32(xq+0), u32(yq+0));
	Index i1(u32(xq+1), u32(yq+0));
	Index i2(u32(xq+0), u32(yq+1));
	Index i3(u32(xq+1), u32(yq+1));

	i0.clamp(map_size);
	i1.clamp(map_size);
	i2.clamp(map_size);
	i3.clamp(map_size);

	Vertex v0(i0);
	Vertex v1(i1);
	Vertex v2(i2);
	Vertex v3(i3);

	GetVertex(v0);
	GetVertex(v1);
	GetVertex(v2);
	GetVertex(v3);

   return lerp(y, lerp(x, v0.position.y, v1.position.y), lerp(x, v2.position.y, v3.position.y));
}

//______________________________________________________________
int SOARXDrawable::CycleRendrer()
{
	m_current_renderer++;

	if (m_current_renderer == m_max_renderer) {
		m_current_renderer = 0;
	}

	return 0;
}

//______________________________________________________________
int SOARXDrawable::SetRendrer(int i_renderer)
{
	if (i_renderer >=0 && i_renderer < m_max_renderer) {
		m_current_renderer = i_renderer;
	}
	
	return 0;
}

//______________________________________________________________
float SOARXDrawable::SetThreshold(float i_threshold)
{
	m_threshold = i_threshold;
	m_threshold = clamp(m_threshold, 1.0f, 10.0f);
	magic = static_cast<float>(window_size.y) / (deg2rad(m_refinement_camera->m_fovy) * m_threshold);
	// sys->OverlayNote("Terrain error threshold set to %3.1f.\n", m_threshold);
	return m_threshold;
}

//______________________________________________________________
float SOARXDrawable::SetDetailMultiplier(float i_detail_multiplier)
{
	m_detail_multiplier = i_detail_multiplier;
	m_detail_multiplier = clamp(m_detail_multiplier, 1.0f, 20.0f);
	CalculateRadii(m_detail_multiplier);
	// sys->OverlayNote("Terrain detail multiplier set to %3.1f.\n", m_detail_multiplier);
	return m_detail_multiplier;
}

//______________________________________________________________
