//______________________________________________________________
#include "soarx/soarx_generic.h"
#include "soarx/soarx_terrain.h"
#include "soarx/soarx_camera.h"

#include <osg/GL>

using namespace dtSOARX;

//______________________________________________________________
v4d obox_vertices[] = {
	v4d(-1, -1, -1, 1),
	v4d(-1, -1, +1, 1),
	v4d(-1, +1, -1, 1),
	v4d(-1, +1, +1, 1),
	v4d(+1, -1, -1, 1),
	v4d(+1, -1, +1, 1),
	v4d(+1, +1, -1, 1),
	v4d(+1, +1, +1, 1)
};

unsigned char box_indices[] = {
	1, 5, 7, 3,	// back
	5, 4, 6, 7,	// right
	4, 0, 2, 6,	// front
	0, 1, 3, 2,	// left
	3, 7, 6, 2,	// top
	1, 0, 4, 5	// bottom

};

//______________________________________________________________
Camera::Camera()
{
	reset();
	return;
}

//______________________________________________________________
Camera::~Camera()
{
	return;
}

//______________________________________________________________
void Camera::reset()
{
	m_fovy = 45.0f;
	m_aspect = 4.0f/3.0f;
	m_position(0, 0, 0, 1);
	m_orientation.qidentity();
	m_heading = 0;
	m_pitch = 0;
	m_roll = 0;

	//widescreen?
	//m_aspect = 16.0f/9.0f;
	//int cx = m_window_size.x;
	//int cy = int(m_window_size.y * (9.0f / 16.0f));
	//int gap = (m_window_size.y - cy) >> 1;
	//opengl->Viewport(0, gap, cx, cy);

	return;
}

//______________________________________________________________
void Camera::turn(f32 heading, f32 pitch, f32 roll)
{
	m_heading += heading;
	m_pitch += pitch;
	m_roll += roll;
	m_orientation.rotate(heading, v4f(0, 1, 0, 0));
	m_orientation.rotate_local(pitch, v4f(1, 0, 0, 0));
	m_orientation.rotate_local(roll, v4f(0, 0, 1, 0));
	return;
}

//______________________________________________________________
// Move camera position relative to its orientation
void Camera::move(f32 x, f32 y, f32 z)
{
	m4f m;
	m.identity();
	m.rotate(m_orientation);
	v4f mov(x, y, z, 0);
	mov.transform(m);
	// TODO instead: mov.rotate(m_orientation);

	m_position.translate(mov);
}

//______________________________________________________________
void Camera::update()
{
	m_view.identity();
	m_view.translate(m_position);
	m_view.rotate(m_orientation);
	m_view.fastinvert();
	m_projection.identity();
	m_projection.perspective(m_fovy, m_aspect, 5.0f, 500000.0f);

	m4f combined(m_projection);
	combined.multiply(m_view);
	v4f* row = (v4f*)(f32*)combined;

	m_frustum_planes[0] = row[3] + row[0]; // left
	m_frustum_planes[1] = row[3] - row[0]; // right
	m_frustum_planes[2] = row[3] + row[1]; // bottom
	m_frustum_planes[3] = row[3] - row[1]; // top
	m_frustum_planes[4] = row[3] + row[2]; // near
	m_frustum_planes[5] = row[3] - row[2]; // far

	for(int i=0; i<6; i++) {
		m_frustum_planes[i].normalize();
	}

}

//______________________________________________________________
f32* Camera::get_projector(int cx, int cy)
{
	static m4f p;
	p.identity();
	p.scale(v4f(0.5f*float(cx), 0.5f*float(cy), 1, 1));
	p.translate(v4f(1, 1, 0, 0));
	p.multiply(m_projection);
	p.multiply(m_view);
	return p;
}

//______________________________________________________________
void Camera::RenderFrustum()
{
	v4d box_vertices[8];
	m4d p;
	m4d v;
	p(m_projection);
	v(m_view);
	p.multiply(v);
	p.invert();

	for (int i=0; i<8; i++) {
		box_vertices[i] = obox_vertices[i];
		p.multiply(box_vertices[i]);
	}

	glEnable(GL_DEPTH_TEST);
//	opengl->Enable(GL_DEPTH_CLAMP_NV);
	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);

	glColor4ub(255, 155, 0, 255);
	for (int s=0; s<24;) {
		glBegin(GL_LINE_LOOP);
		glVertex4dv(box_vertices[box_indices[s++]]);
		glVertex4dv(box_vertices[box_indices[s++]]);
		glVertex4dv(box_vertices[box_indices[s++]]);
		glVertex4dv(box_vertices[box_indices[s++]]);
		glEnd();
	}

	glEnable(GL_BLEND);

	glFrontFace(GL_CCW);
	glColor4ub(0, 0, 255, 30);
	glBegin(GL_QUADS);
	for (int s=0; s<24; s++) {
		glVertex4dv(box_vertices[box_indices[s]]);
	}
	glEnd();

	glFrontFace(GL_CW);
	glColor4ub(0, 0, 255, 30);
	glBegin(GL_QUADS);
	for (int s=0; s<24; s++) {
		glVertex4dv(box_vertices[box_indices[s]]);
	}
	glEnd();

	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);
}

//______________________________________________________________
