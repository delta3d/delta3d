//______________________________________________________________
// File: SOAR Core
//
// Descritpion:
// This file contains the most important components of the SOAR
// framework: refinement and culling. This file is the brain of
// Ranger MkII.
//
//______________________________________________________________
// Include headers:
#include "soarx_framework.h"
#include "soarx_generic.h"
#include "soarxdrawable.h"
#include "soarx_camera.h"

#include "notify.h"

using namespace dtSOARX;

//______________________________________________________________
void SOARXDrawable::GetVertex(Vertex& v)
{
	Index base_index = v.index;
	Index detail_index = v.index;
	Index interpolate_index = v.index;

	base_index >>= map_bits - base_bits;
	detail_index &= detail_size - 1;
	interpolate_index &= embedded_size - 1;

	int ibase = (base_index.y << base_bits) + base_index.y + base_index.x;
	int idetail = (detail_index.y << detail_bits) + detail_index.x;

	Data v0 = base[ibase];
	float d0 = detail[idetail];

	v.position.x = detail_horizontal_resolution * v.index.x;
	v.position.z = detail_horizontal_resolution * v.index.y;
	
	if (interpolate_index.x || interpolate_index.y) {
		Data v1 = base[ibase+1];
		Data v2 = base[ibase+base_size];
		Data v3 = base[ibase+base_size+1];
		f32 x = interpolate_index.x * temp;
		f32 z = interpolate_index.y * temp;

		v.position.y = 
			lerp(z, lerp(x, v0.height, v1.height), lerp(x, v2.height, v3.height)) + 
			lerp(z, lerp(x, v0.scale, v1.scale), lerp(x, v2.scale, v3.scale)) * d0;
	} else {
		v.position.y = v0.height + v0.scale*d0;  //TODO: remove last part
		v.radius = v0.radius;
		v.error = v0.error;
	}
}

//______________________________________________________________
inline void SOARXDrawable::Append(Vertex& v)
// Insert a vertex into the vertex array
{
   if(v.index.x == 0 || v.index.y == 0 ||
      v.index.x == map_size-1 || v.index.y == map_size-1)
   {
      // It's on the edge, so add it to the skirt
      
      skirtIndices.push_back(va_index);
      
      skirtVertices.push_back(v.position);
      
      skirtVertices.back().y -= skirtHeight;
   }
   
	vertex_array[va_index] = v.position;
	index_array[ia_index] = va_index;
	va_index++;
	ia_index++;
}

//______________________________________________________________
inline void SOARXDrawable::TurnCorner()
// Inserts a degenerate triange into the strip.
// This only requires sending one index.
{
	index_array[ia_index++] = index_array[ia_index-2];
}

//______________________________________________________________
inline bool SOARXDrawable::Active(Vertex& v, u32& planes)
// Decides if Vertex v should be in the mesh, or not.
// In order to make this decision, it has to calculate its
// position and the 2D/3D projected error. Hierarchical
// view-frustum culling is also performed here.
{
	static int clip_planes[] = {1, 2, 4, 8, 16, 32};
	const int radius_boost = 450;	// Ad hoc constant for lazy frustum culling

	// Reject vertices after max level (end recursion)
	if (level+1 == map_levels) {
		return false;
	}

#ifdef DATA_LAYOUT_EMBEDDED_QUADTREE
	//if (level+1 == base_levels) {
	//	return false;
	//}
#endif

	// Calculate real 2D position of vertex
	v.position.x = detail_horizontal_resolution * v.index.x;
	v.position.z = detail_horizontal_resolution * v.index.y;

	// Vertices below a level are not clipped
	bool base_vertex = (level + 1) < base_levels;

	if (base_vertex) // if vertex is base geometry
	{
#ifdef DATA_LAYOUT_EMBEDDED_QUADTREE
		Data v0 = baseq[v.index.q];
#else
		Index base_index = v.index;
		base_index >>= map_bits - base_bits;
		int ibase = (base_index.y << base_bits) + base_index.y + base_index.x;
		Data v0 = base[ibase];
#endif

		v.position.y = v0.height;
		v.radius = v0.radius;
		v.error = v0.error;

		// Avoid culling if bounding sphere is already outside frustum
		if (planes)
		{
			f32 radius = v.radius + radius_boost;
			// For each frustum plane
			for (int i=0; i<6; i++) {
				// Cull against plane only if not outside
				if (planes & clip_planes[i]) {
					f32 dp = v.position.dprod(m_refinement_camera->m_frustum_planes[i]);
					if (dp < -radius) {
						return false;
					} else if (dp > radius) {
						planes ^= clip_planes[i];
					}
				}
			}
		}

		// First check projected detail-error for both real and detail vertices
		f32 dx = m_refinement_camera->m_position.x - v.position.x;
		f32 dy = m_refinement_camera->m_position.y - v.position.y;
		f32 dz = m_refinement_camera->m_position.z - v.position.z;

		f32 d = (dx * dx) + (dz * dz); // 2D distance
		f32 o = radii[level+1]; // actually, this is radii squared
		if (o > d) {
			return true;
		}

		// If not accepted, then check real projected error
		d += (dy * dy); // 3D distance
		o = magic * (v.error) + v.radius;
		o = o*o;
		return o>d;

	} else { // if vertex is detail geometry

		// First check projected detail-error for both real and detail vertices
		f32 dx = m_refinement_camera->m_position.x - v.position.x;
		f32 dz = m_refinement_camera->m_position.z - v.position.z;
		f32 d = (dx * dx) + (dz * dz); // 2D distance
		f32 o = radii[level+1]; // actually, this is radii squared

		if (o>d) { // if active, calculate interpolated position and accept
			Index base_index = v.index;
			Index detail_index = v.index;
			base_index >>= map_bits - base_bits;
			detail_index &= detail_size - 1;
			int ibase = (base_index.y << base_bits) + base_index.y + base_index.x;
			int idetail = (detail_index.y << detail_bits) + detail_index.x;
			Index interpolate_index = v.index;
			interpolate_index &= embedded_size - 1;

			f32 x = interpolate_index.x * temp;
			f32 z = interpolate_index.y * temp;

			Data v0 = base[ibase];
			Data v1 = base[ibase+1];
			Data v2 = base[ibase+base_size];
			Data v3 = base[ibase+base_size+1];
			float d0 = detail[idetail];

			v.position.y = 
				lerp(z, lerp(x, v0.height, v1.height), lerp(x, v2.height, v3.height)) + 
				lerp(z, lerp(x, v0.scale, v1.scale), lerp(x, v2.scale, v3.scale)) * d0;

			return true;

		} else { // else reject

			return false;

		}
	}
}

//______________________________________________________________
void SOARXDrawable::RefineTop()
// Refines the top level of the mesh.
{
	va_index = 0;
	ia_index = 0;
	skirtVertices.clear();
	skirtIndices.clear();
	first = true;

	level = 1;
	Append(base_vertices[3]);
	Append(base_vertices[3]);

	left_only = false;
	Refine(base_vertices[8], base_vertices[10], bottom, side, 63);
	Append(base_vertices[4]);
	left_only = false;
	Refine(base_vertices[8], base_vertices[11], side, bottom, 63);
	Append(base_vertices[0]);

	left_only = false;
	Refine(base_vertices[7], base_vertices[11], bottom, side, 63);
	Append(base_vertices[4]);
	left_only = false;
	Refine(base_vertices[7], base_vertices[12], side, bottom, 63);
	Append(base_vertices[1]);

	left_only = false;
	Refine(base_vertices[6], base_vertices[12], bottom, side, 63);
	Append(base_vertices[4]);
	left_only = false;
	Refine(base_vertices[6], base_vertices[9], side, bottom, 63);
	Append(base_vertices[2]);

	left_only = false;
	Refine(base_vertices[5], base_vertices[9], bottom, side, 63);
	Append(base_vertices[4]);
	left_only = false;
	Refine(base_vertices[5], base_vertices[10], side, bottom, 63);
	Append(base_vertices[3]);
	
	for(unsigned int i=0;i<skirtVertices.size();i++)
	{
	   index_array[ia_index++] = skirtIndices[i];
	   
	   index_array[ia_index++] = va_index;
	   vertex_array[va_index++] = skirtVertices[i];
	}
}


//______________________________________________________________
void SOARXDrawable::RefineL(Vertex& i, Vertex& j, bool in, u32 planes)
// Left-only refinement is not tail-recursive
{
	++level;

	Vertex left(Index(i.index, j.index, (level & 1), false));
	if (Active(left, planes)) {
		RefineL(j, left, !in, planes);
	} else {
		if (first) {
			in == side ? Append(i) : Append(j);
			first = false;
		} else {
			TurnCorner();
		}
		if (in == side) {
			Append(j);
		}
	}
	Append(i);

	--level;
}

//______________________________________________________________
inline void SOARXDrawable::RefineR(Vertex i, Vertex& j, bool out, u32 planes)
// Right-only refinement is tail-recursive, so it can be
// converted to a simple loop. This eliminates function call
// and stack maintenance overhead.
{
	u32 level_save = level;
	Vertex right;
	bool is_right;

	do {
		++level;
		Append(j);
		right.index = Index(i.index, j.index, (level & 1), true);
		is_right = Active(right, planes);
		i = j;
		j = right;
		out = !out;
	} while (is_right);

	if (out == bottom) {
		TurnCorner();
	}

	level = level_save;
}

//______________________________________________________________
void SOARXDrawable::Refine(Vertex& i, Vertex& j, bool in, bool out, u32 planes)
// General refinement procedure.
{
	++level;

	// check both left and right child nodes
	Vertex left(Index(i.index, j.index, (level & 1), false));
	Vertex right(Index(i.index, j.index, (level & 1), true));
	u32 left_planes = planes;
	u32 right_planes = planes;
	bool is_left = Active(left, left_planes);
	bool is_right = Active(right, right_planes);

	if (is_left) { 
		if (is_right) {// left - right
			Refine(j, left, !in, side, left_planes);
			Append(i);
			if (left_only) {
				Append(j);
				RefineR(j, right, !out, right_planes);
			} else {
				Refine(j, right, side, !out, right_planes);
			}
			left_only = false;
		} else { // left only
			left_only = true;
			RefineL(j, left, !in, left_planes);
			Append(i);
			if (out == bottom) {
				Append(j);
			}
		}
	} else {
		if (first) {
			in == side ? Append(i) : Append(j);
			first = false;
		} else {
			TurnCorner();
		}

		if (is_right) { // right only
			if (in == bottom) { // in::bottom - out:side
				Append(i);
			}
			Append(j);
			RefineR(j, right, !out, right_planes);
		} else { // none
			if (in == side) { // in:side - out:?
				Append(j);
				if (out == side) { // in:side - out:side
					Append(i);
				}
			} else { // in:bottom - out:side
				Append(i);
			}
		}
	}

	--level;
}

//______________________________________________________________
