//______________________________________________________________
// Ranger rendering
// Modified: 11/03/2003
//
//______________________________________________________________
// Include headers:
#include "soarxdrawable.h"
#include "soarx_camera.h"

#include <iostream>

using namespace dtSOARX;

using namespace std;

//______________________________________________________________
void SOARXDrawable::Render0()
// Full quality rendering using two passes:
// The 1st pass renders combined gradient maps onto the surface.
// The result of the 1st pass is copied to a screen sized texture.
// The 2nd pass projects generated texture onto surface and
// uses dependent texture lookup to read irradiance values.
// Ambient map and cloud shadows are also applied in this pass.
{
	// constant vector parameters
	const f32 bt = 1.0f / (base_horizontal_resolution * base_size);
	const f32 dt = 1.0f / (detail_horizontal_resolution * detail_size);
	const f32 dt2 = 0.03f;

	static f32 s1 = 0;
	static f32 t1 = 0;
	static f32 s2 = 0;
	static f32 t2 = 0;
	f32 c1 = 0.000015f;
	f32 c2 = 0.00001f;

	float s = m_time * 100.0f;
	s1 = 0.0001f * s;
	t1 = 0.00005f * s;
	s2 = 0.00005f * s;
	t2 = 0.00009f * s;

	v4f* texgen_matrix = (v4f*)m_view_camera->get_projector(window_size.x, window_size.y);

	v4f s_plane_delta_base(bt, 0, 0, 0);
	v4f t_plane_delta_base(0, 0, bt, 0);
	v4f s_plane_delta_detail(dt, 0, 0, 0);
	v4f t_plane_delta_detail(0, 0, dt, 0);
	v4f s_plane_delta_detail_2(dt2, 0, 0, 0);
	v4f t_plane_delta_detail_2(0, 0, dt2, 0);
	v4f s_plane_cloud_1(c1, 0, 0, s1);
	v4f t_plane_cloud_1(0, 0, c1, t1);
	v4f s_plane_cloud_2(c2, 0, 0, s2);
	v4f t_plane_cloud_2(0, 0, c2, t2);
	v4f constant_0_5(0.5f, 0.5f, 0.5f, 0.5f);

	// setup first pass:

	// setup texture 0: detail texture
	// using replace texture environment
	// result = detail
	// output0 = texture0
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[DETAIL_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_detail);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_detail);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// setup texture 1: detail scale texture
	// using interpolate texture environment
	// result = (detail - 0.5) * scale + 0.5
	// result = detail * (scale) + 0.5 * (1 - scale)
	// output1 = output0 * (texture1) + constant * (1 - texture1)
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[SCALE_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_base);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_base);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constant_0_5);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

	// setup texture 2: base texture
	// using signed addition texture environment
	// result = ((detail - 0.5) * scale + 0.5) + base - 0.5
	// output2 = output1 + texture2 - 0.5 
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[BASE_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_base);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_base);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

	// setup texture 3: detail bump map
	// using signed addition texture environment
	// result = ((detail - 0.5) * scale + 0.5) + (base - 0.5) + (bump - 0.5)
	// output3 = output2 + texture3 - 0.5 
	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[BUMP_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_detail_2);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_detail_2);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

	// final fragment color = ((detail - 0.5) * scale + 0.5) + (base - 0.5)

	// render first pass:
	glDisable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, va_index, ia_index, GL_UNSIGNED_INT, 0);

	//____________________
	// setup second pass:
	glEnable(GL_TEXTURE_SHADER_NV);

	// setup texture 0: copy image to texture rectangle
	glActiveTexture(GL_TEXTURE0);
	glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texture[SCREEN_TEX]);
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, window_size.x, window_size.y);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, texgen_matrix[0]);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, texgen_matrix[1]);
	glTexGenfv(GL_Q, GL_OBJECT_PLANE, texgen_matrix[3]);

	// setup texture 1: normal texture
	// using dependent texture read:
	// n(G, B) = normal vector encoded in RGB
	// using dot3 texture environment
	// result = light_direction dprod surface_normal
	// output1 = constant dprod texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture[NORMAL_TEX]);
	glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_DEPENDENT_GB_TEXTURE_2D_NV);
	glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, GL_TEXTURE0);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_texture[COLOR_TEX]);
	glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_base);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_base);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_texture[CLOUD_TEX]);
	glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_cloud_2);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_cloud_2);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_ALPHA);

	//glActiveTexture(GL_TEXTURE2);
	//glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
	//glActiveTexture(GL_TEXTURE3);
	//glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);

	// render second pass:
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	v4f fog_color(0.6f, 0.7f, 0.8f, 0);
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.000010f);
	glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
	glFogfv(GL_FOG_COLOR, fog_color);
	glEnable(GL_FOG);

	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, va_index, ia_index, GL_UNSIGNED_INT, 0);

	glDisable(GL_TEXTURE_SHADER_NV);
	glDisable(GL_FOG);

	// glResetTextureUnits();

   glMatrixMode(GL_TEXTURE);

	for (int i=0; i<4; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glLoadIdentity();
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_3D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_RECTANGLE_NV);
	}

	glActiveTexture(GL_TEXTURE0);

	glMatrixMode(GL_MODELVIEW);
	
	Render_Sky();

	glDepthMask(GL_TRUE);
}

//______________________________________________________________
void SOARXDrawable::Render1()
// One pass delta rendering, very fast, but no real lighting.
// (Basically it's the first pass of the full quality)
{
	// constant vector parameters
	const f32 bt = 1.0f / (base_horizontal_resolution * base_size);
	const f32 dt = 1.0f / (detail_horizontal_resolution * detail_size);
	const f32 dt2 = 0.07f;

	v4f s_plane_delta_base(bt, 0, 0, 0);
	v4f t_plane_delta_base(0, 0, bt, 0);
	v4f s_plane_delta_detail(dt, 0, 0, 0);
	v4f t_plane_delta_detail(0, 0, dt, 0);
	v4f s_plane_delta_detail_2(dt2, 0, 0, 0);
	v4f t_plane_delta_detail_2(0, 0, dt2, 0);
	v4f constant_0_5(0.5f, 0.5f, 0.5f, 0.5f);

	// setup first pass:

	// setup texture 0: detail texture
	// using replace texture environment
	// result = detail
	// output0 = texture0
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[DETAIL_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_detail);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_detail);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// setup texture 1: detail scale texture
	// using interpolate texture environment
	// result = (detail - 0.5) * scale + 0.5
	// result = detail * (scale) + 0.5 * (1 - scale)
	// output1 = output0 * (texture1) + constant * (1 - texture1)
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[SCALE_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_base);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_base);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constant_0_5);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

	// setup texture 2: base texture
	// using signed addition texture environment
	// result = ((detail - 0.5) * scale + 0.5) + base - 0.5
	// output2 = output1 + texture2 - 0.5 
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[BASE_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_base);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_base);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE2);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[BUMP_TEX]);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_delta_detail_2);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_delta_detail_2);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE3);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

	// final fragment color = ((detail - 0.5) * scale + 0.5) + (base - 0.5)

	// render first pass:
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_DEPTH_CLAMP_NV);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, va_index, ia_index, GL_UNSIGNED_INT, 0);
	
	// glResetTextureUnits();
	
	glMatrixMode(GL_TEXTURE);

	for (int i=0; i<4; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glLoadIdentity();
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_3D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_RECTANGLE_NV);
	}

	glActiveTexture(GL_TEXTURE0);

	glMatrixMode(GL_MODELVIEW);
}

//______________________________________________________________
void SOARXDrawable::Render2()
// Solid-wireframe rendering of the geometry:
// 1st pass renders solid triangles (using polygon offset).
// 2nd pass renders the wireframe.
{
	v4f solid_color(0.0f, 0.7f, 0.6f, 1.0f);
	v4f wire_color(1.0f, 1.0f, 1.0f, 1.0f);

   glDisable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
// glEnable(GL_DEPTH_CLAMP_NV);
	glPolygonOffset(1.0f, 2.0f);

	glColor4fv(solid_color);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, va_index, ia_index, GL_UNSIGNED_INT, 0);
	glDisable(GL_POLYGON_OFFSET_FILL);
   
	glColor4fv(wire_color);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, va_index, ia_index, GL_UNSIGNED_INT, 0);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//______________________________________________________________
void SOARXDrawable::Render_Sky()
// Renders cloud cover:
// Two alpha textures are animated and combined for the good
// looking morphing clouds effect.
{
	f32 cx = base_horizontal_resolution * base_size;

	static f32 s1 = 0;
	static f32 t1 = 0;
	static f32 s2 = 0;
	static f32 t2 = 0;
	f32 c1 = 0.000015f;
	f32 c2 = 0.00001f;

	v4f s_plane_cloud_1(c1, 0, 0, s1);
	v4f t_plane_cloud_1(0, 0, c1, t1);
	v4f s_plane_cloud_2(c2, 0, 0, s2);
	v4f t_plane_cloud_2(0, 0, c2, t2);

	float s = m_time * 80.0f;
	s1 = 0.0001f * s;
	t1 = 0.00005f * s;
	s2 = 0.00005f * s;
	t2 = 0.00009f * s;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture[CLOUD_TEX]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_cloud_1);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_cloud_1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture[CLOUD_TEX]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_plane_cloud_2);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_plane_cloud_2);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_SUBTRACT);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_ALPHA_SCALE, 4);

	v4f fog_color(0.6f, 0.7f, 0.8f, 0);
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.000010f);
	glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
	glFogfv(GL_FOG_COLOR, fog_color);
	glEnable(GL_FOG);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord4f(0, 1, 0, 1);
	glVertex4f(0, 10000, 0, 1);
	glTexCoord4f(0, 0, 0, 1);
	glVertex4f(0, 10000, cx, 1);
	glTexCoord4f(1, 0, 0, 1);
	glVertex4f(cx, 10000, cx, 1);
	glTexCoord4f(1, 1, 0, 1);
	glVertex4f(cx, 10000, 0, 1);
	glEnd();

	// glResetTextureUnits();

   glMatrixMode(GL_TEXTURE);

	for (int i=0; i<4; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glLoadIdentity();
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_3D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_RECTANGLE_NV);
	}
   
	glActiveTexture(GL_TEXTURE0);

	glMatrixMode(GL_MODELVIEW);
	
	glDisable(GL_FOG);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

//______________________________________________________________
