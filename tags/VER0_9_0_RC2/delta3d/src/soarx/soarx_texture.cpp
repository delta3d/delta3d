//______________________________________________________________
#include <string>
#include "soarx/soarxdrawable.h"
#include "soarx/soarx_image.h"

#include "dtCore/notify.h"

#include <osg/GL>

using namespace dtSOARX;

//______________________________________________________________
void CreateNormalTexture(GLubyte* t)
{
	const float grad_scale_inv = 1.0f / 128.0f;

	v4f lights[] = {
		v4f(1.0f, 1.5f, 1.0f, 0.0f),
		v4f(-1.0f, 1.0f, -1.0f, 0.0f),
	};

	v4f colors[] = {
		v4f(1.0f, 1.0f, 1.0f, 1.0f),
		v4f(1.0f, 1.0f, 1.0f, 1.8f),
	};

	const int n = 2;

	for (int i=0; i<n; i++) {
		lights[i].normalize();
		colors[i].hdiv();
	}

//	float grad_scale_inv = 2.0f/128.0f;
	for (int dy=-128; dy<128; dy++) {
		for (int dx=-128; dx<128; dx++) {
			v4f normal(-dx*grad_scale_inv, 1, -dy*grad_scale_inv, 0);
			normal.normalize();
			v4f color(0, 0, 0, 0); // ambient color
			for (int i=0; i<n; i++) {
				f32 angle = normal.dprod(lights[i]);
				color.max(colors[i] * max(0.0f, angle));
			}

			color *= 255.0f;
			color.clamp(0, 255.0f);
			*t++ = static_cast<u8>(color.x);
			*t++ = static_cast<u8>(color.y);
			*t++ = static_cast<u8>(color.z);
		}
	}
}

int glLoadTexture(const char* path)
{
	GLint iformat[] = {GL_ALPHA, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};
//	GLint iformat[] = {GL_COMPRESSED_ALPHA, GL_COMPRESSED_LUMINANCE_ALPHA, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA};
	GLint format[] = {GL_ALPHA, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};
	GLenum type[] = {GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT};

	IImage* img = new Image;
	
	if (img->Load(path) == -1) {
		return -1;
	}
	IImage::Dimensions dim = img->GetDimensions();
	char* d = img->GetData();

	if (d != 0) {
		if (dim.depth > 1 ) {
			// glTexImage3D(GL_TEXTURE_3D, 0, iformat[dim.channels-1], dim.width, dim.height, dim.depth, 0, format[dim.channels-1], type[(dim.bpc >> 3) - 1], d);
		} else if (dim.height > 1) {
			::glTexImage2D(GL_TEXTURE_2D, 0, iformat[dim.channels-1], dim.width, dim.height, 0, format[dim.channels-1], type[(dim.bpc >> 3) - 1], d);
		} else if (dim.width >= 1) {
			::glTexImage1D(GL_TEXTURE_1D, 0, iformat[dim.channels-1], dim.width, 0, format[dim.channels-1], type[(dim.bpc >> 3) - 1], d);
		}

	}

	release(img);
	return 0;
}

//______________________________________________________________
void SOARXDrawable::LoadTextures()
{
	static std::string image_extension(".png");
	static std::string terrain_name;
	static std::string terrain_base_color;
	static std::string terrain_base_gradient;
	static std::string terrain_detail_gradient;
	static std::string terrain_detail_scale;
	static std::string terrain_bump_gradient;
	static std::string terrain_cloud_color;

   std::string pathPlusPrefix(mPath);
   pathPlusPrefix.append(mPrefix);
   
	terrain_name.assign(pathPlusPrefix);
	// terrain_name.append(sys->GetGlobalString("Terrain.name"));
	// terrain_name.append("/");
	terrain_base_color.assign(terrain_name);
	terrain_base_color.append("base.color");
	terrain_base_color.append(image_extension);
	terrain_base_gradient.assign(terrain_name);
	terrain_base_gradient.append("base.gradient");
	terrain_base_gradient.append(image_extension);
	terrain_detail_gradient.assign(terrain_name);
	terrain_detail_gradient.append("detail.gradient");
	terrain_detail_gradient.append(image_extension);
	terrain_detail_scale.assign(terrain_name);
	terrain_detail_scale.append("detail.scale");
	terrain_detail_scale.append(image_extension);
	terrain_bump_gradient.assign(terrain_name);
	terrain_bump_gradient.append("bump.gradient");
	terrain_bump_gradient.append(image_extension);
	terrain_cloud_color.assign(mPath);
	terrain_cloud_color.append("cloud.color");
	terrain_cloud_color.append(image_extension);

	glGenTextures(8, m_texture);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m_texture[BASE_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glLoadTexture(terrain_base_gradient.c_str());
	
	glBindTexture(GL_TEXTURE_2D, m_texture[DETAIL_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glLoadTexture(terrain_detail_gradient.c_str());
	
	glBindTexture(GL_TEXTURE_2D, m_texture[BUMP_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glLoadTexture(terrain_bump_gradient.c_str());

	glBindTexture(GL_TEXTURE_2D, m_texture[SCALE_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glLoadTexture(terrain_detail_scale.c_str());
	
	// Create screen texture:
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_texture[SCREEN_TEX]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, window_size.x, window_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Load cloud texture:
	glBindTexture(GL_TEXTURE_2D, m_texture[CLOUD_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glLoadTexture(terrain_cloud_color.c_str());
	
	// Load color texture:
	glBindTexture(GL_TEXTURE_2D, m_texture[COLOR_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glLoadTexture(terrain_base_color.c_str());
	

	// Create normal texture:

	v4b color[] = {
		v4b(255, 255, 255, 0),
		v4b(235, 120, 0, 0),
		v4b(100, 225, 0, 0),
		v4b(100, 100, 100, 0),
		v4b(0, 0, 150, 0)
	};

	std::string str;
	int tx;
	int ty;
	GLubyte *text;
	GLubyte* t;
	float sc = 64.0f;

	tx = 256;
	ty = 256;
	float sci = 1.0f / sc;
	sci = 1.0f / 256.0f;
	t = text = new GLubyte[tx*ty*3];

	str.assign("simple"); // sys->GetGlobalString("Terrain.irradiance")); 

	if (str == "simple") {
		CreateNormalTexture(text);
	} else {
		for (int j=0; j<ty; j++) {
			for (int i=0; i<tx; i++) {
				f32 x = i * sci;
				f32 z = j * sci;
				f32 xz = x*z;
				v4f c(1-x-z+xz, x-xz, z-xz, xz);

				v4f red(255.0f, 255.0f, 0.0f, 0.0f);
				v4f green(255.0f, 100.0f, 225.0f, 0.0f);
				v4f blue(255.0f, 0.0f, 0.0f, 150.0f);

				*t++ = static_cast<u8>(c.dprod(red));
				*t++ = static_cast<u8>(c.dprod(green));
				*t++ = static_cast<u8>(c.dprod(blue));
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, m_texture[NORMAL_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tx, ty, 0, GL_RGB, GL_UNSIGNED_BYTE, text);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, tx, ty, 0, GL_ALPHA, GL_UNSIGNED_BYTE, text);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
//	glLoadTexture("normal.png");
	delete text;

	dtCore::Notify(dtCore::INFO, "Textures loaded.\n");
}
