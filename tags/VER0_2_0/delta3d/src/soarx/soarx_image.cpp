//______________________________________________________________
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>
#include "soarx_framework.h"
#include "soarx_generic.h"
#include "soarx_image.h"

#include "notify.h"

using namespace dtSOARX;

//______________________________________________________________
void* Factory_Image(ISystem* i_sys)
{
	return new Image();
}

//______________________________________________________________
IImage::~IImage() {}

//______________________________________________________________
Image::Image() :
m_data_size(0),
m_image(0)
{
	m_dimensions.width = 0;
	m_dimensions.height = 0;
	m_dimensions.depth = 0;
	m_dimensions.channels = 1;
	m_dimensions.bpc = 8;
}

//______________________________________________________________
Image::~Image()
{
	releasev(m_image);
}

//______________________________________________________________
int Image::Create(IImage::Dimensions d)
{
	d.width = max(d.width, 1);
	d.height = max(d.height, 1);
	d.depth = max(d.depth, 1);
	d.channels = clamp(d.channels, 1, 4);
	d.bpc &= (8 | 16 | 32);
	m_data_size = d.width * d.height * d.depth * d.channels * (d.bpc >> 3);
	m_dimensions = d;
	releasev(m_image);
	m_image = new char[m_data_size];
	return m_data_size;
}

//______________________________________________________________
int Image::GetPixel(int c, int x, int y, int z)
{
	int cx = m_dimensions.width;
	int cy = m_dimensions.height;

	x += x < 0 ? cx : 0;
	x -= x >= cx ? cx : 0;

	y += y < 0 ? cy : 0;
	y -= y >= cy ? cy : 0;

	z = 0;

	int component_size = (m_dimensions.bpc >> 3);
	int pixel_size =  component_size * m_dimensions.channels;
	int line_size = m_dimensions.width * pixel_size;
	int page_size = m_dimensions.height * line_size;
	int position = z*page_size + y*line_size + x*pixel_size + c*component_size;

	int result = 0;

	if (component_size == 1) {
		u8* p = (u8*)(m_image + position);
		result = *p;
	} else if (component_size == 2) {
		u16* p = (u16*)(m_image + position);
		result = *p;
	} else {
	}

	return result;
}

//______________________________________________________________
int Image::SetPixel(int v, int c, int x, int y, int z)
{
	int cx = m_dimensions.width;
	int cy = m_dimensions.height;

	x += x < 0 ? cx : 0;
	x -= x >= cx ? cx : 0;

	y += y < 0 ? cy : 0;
	y -= y >= cy ? cy : 0;

	z = 0;

	int component_size = (m_dimensions.bpc >> 3);
	int pixel_size =  component_size * m_dimensions.channels;
	int line_size = m_dimensions.width * pixel_size;
	int page_size = m_dimensions.height * line_size;
	int position = z*page_size + y*line_size + x*pixel_size + c*component_size;

	int result = 0;

	if (component_size == 1) {
		u8* p = (u8*)(m_image + position);
		result = *p;
		*p = v;
	} else if (component_size == 2) {
		u16* p = (u16*)(m_image + position);
		result = *p;
		*p = v;
	} else {
	}

	return result;
}

//______________________________________________________________
int Image::ProcessImage(ImageProc fp, void* ptr)
{
	char* d = m_image;
	for (int z=0; z<m_dimensions.depth; z++) {
		for (int y=0; y<m_dimensions.height; y++) {
			for (int x=0; x<m_dimensions.width; x++) {
				for (int c=0; c<m_dimensions.channels; c++) {
					if (m_dimensions.bpc == 8) {
						u8* v = (u8*)d;
						*v = fp(c, x, y, z, *v, ptr);
						v++;
						d = (char*)v;
					} else if (m_dimensions.bpc == 16) {
						u16* v = (u16*)d;
						*v = fp(c, x, y, z, *v, ptr);
						v++;
						d = (char*)v;
					} else if (m_dimensions.bpc == 32) {
						u32* v = (u32*)d;
						*v = fp(c, x, y, z, *v, ptr);
						v++;
						d = (char*)v;
					}
				}
			}
		}
	}

	return 0;
}

//______________________________________________________________
int Image::ProcessLine(int line, ImageProc fp, void* ptr)
{
	char* d = m_image;

	int line_size = (m_dimensions.bpc >> 3) * m_dimensions.channels * m_dimensions.width;
	int number_of_lines = m_data_size / line_size;
	d += line_size * line;

	int z = line / m_dimensions.height;
	int y = line % m_dimensions.height;

	for (int x=0; x<m_dimensions.width; x++) {
		for (int c=0; c<m_dimensions.channels; c++) {
			if (m_dimensions.bpc == 8) {
				u8* v = (u8*)d;
				*v = fp(c, x, y, z, *v, ptr);
				v++;
				d = (char*)v;
			} else if (m_dimensions.bpc == 16) {
				u16* v = (u16*)d;
				*v = fp(c, x, y, z, *v, ptr);
				v++;
				d = (char*)v;
			} else if (m_dimensions.bpc == 32) {
				u32* v = (u32*)d;
				*v = fp(c, x, y, z, *v, ptr);
				v++;
				d = (char*)v;
			}
		}
	}

	return number_of_lines - line - 1;
}

//______________________________________________________________
int Image::Load(const char* path)
{
	return Load(path, false);
}

//______________________________________________________________
int Image::LoadInfo(const char* path)
{
	return Load(path, true);
}

//______________________________________________________________
int Image::Load(const char* path, bool info)
{
	int l = static_cast<int>(strlen(path));
	FILE* f = 0;

	if (!strcmp(path + l - 4, ".png")) {
		f = fopen(path, "rb");
		if (f != 0) {
			Load_PNG(f, info);
		}
	} else if (!strcmp(path + l - 4, ".bmp")) {
		f = fopen(path, "rb");
		if (f != 0) {
			Load_BMP(f, info);
		}
	} else if (!strcmp(path + l - 4, ".tga")) {
		f = fopen(path, "rb");
		if (f != 0) {
			Load_TGA(f, info);
		}
	} else if (!strcmp(path + l - 4, ".jpg")) {
		f = fopen(path, "rb");
		if (f != 0) {
			Load_JPEG(f, info);
		}
	} else if (!strcmp(path + l - 5, ".jpeg")) {
		f = fopen(path, "rb");
		if (f != 0) {
			Load_JPEG(f, info);
		}
	}

	if (f != 0) {
		fclose(f);
		m_data_size = m_dimensions.width * m_dimensions.height * m_dimensions.depth * m_dimensions.channels * (m_dimensions.bpc >> 3);
		return 0;
	} else {
		return -1;
	}
}

//______________________________________________________________
int Image::Save(const char* path)
{
	int l = static_cast<int>(strlen(path));
	FILE* f = 0;

	if (!strcmp(path + l - 4, ".png")) {
		f = fopen(path, "wb");
		if (f != 0) {
			Save_PNG(f);
		}
	} else if (!strcmp(path + l - 4, ".bmp")) {
		f = fopen(path, "wb");
		if (f != 0) {
			Save_BMP(f);
		}
	} else if (!strcmp(path + l - 4, ".tga")) {
		f = fopen(path, "wb");
		if (f != 0) {
			Save_TGA(f);
		}
	} else if (!strcmp(path + l - 4, ".jpg")) {
		f = fopen(path, "wb");
		if (f != 0) {
			Save_JPEG(f);
		}
	} else if (!strcmp(path + l - 5, ".jpeg")) {
		f = fopen(path, "wb");
		if (f != 0) {
			Save_JPEG(f);
		}
	}

	if (f != 0) {
		fclose(f);
	}

	return 0;
}

//______________________________________________________________
int Image::Save_BMP(FILE* f)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	int header_size = sizeof(BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);
	int file_size = m_data_size + header_size;

	bmfh.bfType = ('M' << 8) | ('B');
	bmfh.bfSize = file_size;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = header_size;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = m_dimensions.width;
	bmih.biHeight = m_dimensions.height;
	bmih.biPlanes = 1;
	bmih.biBitCount = m_dimensions.bpc * m_dimensions.channels;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = 0;
	bmih.biYPelsPerMeter = 0;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, f);
	fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, f);
	fwrite(m_image, m_data_size, 1, f);
	return 0;
}

//______________________________________________________________
int Image::Save_TGA(FILE* f)
{
	return 0;
}

//______________________________________________________________
int Image::Save_PNG(FILE* f)
{
	return 0;
}

//______________________________________________________________
int Image::Save_JPEG(FILE* f)
{
	return 0;
}

//______________________________________________________________
int Image::Load_BMP(FILE* f, bool info)
{
	return 0;
}

//______________________________________________________________
int Image::Load_TGA(FILE* f, bool info)
{
	return 0;
}

//______________________________________________________________
int Image::Load_PNG(FILE* f, bool info)
{
	return 0;
}

//______________________________________________________________
int Image::Load_JPEG(FILE* f, bool info)
{
	return 0;
}

//______________________________________________________________
IImage::Dimensions Image::GetDimensions()
{
	return m_dimensions;
}

//______________________________________________________________
char* Image::GetData()
{
	return m_image;
}

//______________________________________________________________
int Image::CreateGradient(IImage* src_image, float scale)
{
	Image* s = (Image*)src_image;

	for (int i=0; i<m_dimensions.width; i++) {
		for (int j=0; j<m_dimensions.height; j++) {
			int o = s->GetPixel(0, i, j);
			float h = scale * (s->GetPixel(0, i+1, j) - o) + 128.0f;
			float v = scale * (s->GetPixel(0, i, j+1) - o) + 128.0f;
			h = clamp(h, 0.0f, 255.0f);
			v = clamp(v, 0.0f, 255.0f);
			SetPixel((unsigned char)h, 1, i, j);
			SetPixel((unsigned char)v, 2, i, j);
		}
	}
	return 0;
}

//______________________________________________________________
