//______________________________________________________________
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "soarx/soarx_tbuilder.h"
#include "soarx/soarx_generic.h"
#include "soarx/soarx_image.h"

#include "dtCore/notify.h"

#include <iostream>

using namespace std;

using namespace dtCore;
using namespace dtSOARX;

static Noise n;

//______________________________________________________________
TBuilder::TBuilder()
{
	//sys->Log("TBuilder services loaded (%s: %s %s)\n", BUILD_TYPE, __DATE__, __TIME__);
	//sys->Subscribe("Terrain.BuildMap", Callback(this, Build));
}

//______________________________________________________________
TBuilder::~TBuilder()
{
	//sys->Log("TBuilder services released.\n");
}

/**
 * Builds the various files necessary for the SOARX algorithm.
 *
 * @param path the path in which to build the files
 * @param prefix the filename prefix (e.g., "w122_n36_0."), or NULL for none
 * @param mapBits the number of map bits
 * @param baseBits the number of base bits
 * @param detailBits the number of detail bits
 * @param horizontalResolution the horizontal resolution
 * @param verticalResolution the vertical resolution 
 * @param verticalBias the vertical bias
 * @param detailVerticalResolution the detail vertical resolution
 * @param hf the osg::HeightField to use for the base map
 * @param buildDetail if true, build the detail file as well as the
 * base file
 */
int TBuilder::Build(const char* path,
                    const char* prefix,
                    int mapBits,
                    int baseBits,
                    int detailBits,
                    float horizontalResolution,
                    float verticalResolution,
                    float verticalBias,
                    float detailVerticalResolution,
                    osg::HeightField* hf,
                    bool buildDetail)
{
   Notify(NOTICE, "SOARXTerrain: Making base map for %s..", prefix);
   
	//sys->Subscribe("Event.Update", Callback(this, BuildSteps));

	src_image = new Image;
	dst_image = new Image;

	map_bits = mapBits; // static_cast<int>(sys->GetGlobalNumber("Terrain.map_bits"));
	base_bits = baseBits; // static_cast<int>(sys->GetGlobalNumber("Terrain.base_bits"));
	detail_bits = detailBits; // static_cast<int>(sys->GetGlobalNumber("Terrain.detail_bits"));
	base_horizontal_resolution = horizontalResolution; // static_cast<float>(sys->GetGlobalNumber("Terrain.horizontal_resolution"));
	base_vertical_resolution = verticalResolution; // static_cast<float>(sys->GetGlobalNumber("Terrain.vertical_resolution"));
	base_vertical_bias = verticalBias; // static_cast<float>(sys->GetGlobalNumber("Terrain.vertical_bias"));
	embedded_bits = map_bits - base_bits;
	map_size = (1 << map_bits) + 1;
	base_size = (1 << base_bits) + 1;
	detail_size = 1 << detail_bits;
	embedded_size = 1 << embedded_bits;
	detail_horizontal_resolution = base_horizontal_resolution / static_cast<float>(embedded_size);
	detail_vertical_resolution = detailVerticalResolution; // static_cast<float>(sys->GetGlobalNumber("Terrain.detail_vertical_resolution"));
	detail_vertical_bias = -32768.0f * detail_vertical_resolution;
	base_levels = base_bits << 1;

	//sys->Log("Map dimensions: %dx%d (%d levels)\n", map_size, map_size, map_bits << 1);

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

	// map terrain file into address space
	LARGE_INTEGER base_map_size;
	LARGE_INTEGER baseq_map_size;
	LARGE_INTEGER detail_map_size;
	base_map_size.QuadPart = sizeof(Data) * (base_size * (base_size+1));
	detail_map_size.QuadPart = sizeof(f32) * (detail_size * detail_size);

	u32 n = base_bits;
	u32 t = (1<<((n<<1)+2));
	u32 r = t - ((t<<1)-8)/3 + 1;
	baseq_map_size.QuadPart = r * sizeof(Data);

   if(buildDetail)
   {
      detail_file = ::CreateFile(detail_file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
      ::SetFilePointerEx(detail_file, detail_map_size, 0, FILE_BEGIN);
	   ::SetEndOfFile(detail_file);
	   
	   detail_file_map = ::CreateFileMapping(detail_file, 0, PAGE_READWRITE, 0, 0, 0);
	   detail = reinterpret_cast<f32*>(::MapViewOfFile(detail_file_map, FILE_MAP_ALL_ACCESS, 0, 0, 0));
   }
   
	base_file = ::CreateFile(base_file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	::SetFilePointerEx(base_file, base_map_size, 0, FILE_BEGIN);
	::SetEndOfFile(base_file);

	base_file_map = ::CreateFileMapping(base_file, 0, PAGE_READWRITE, 0, 0, 0);
	base = reinterpret_cast<Data*>(::MapViewOfFile(base_file_map, FILE_MAP_ALL_ACCESS, 0, 0, 0));
	
#ifdef DATA_LAYOUT_EMBEDDED_QUADTREE
	baseq_file = ::CreateFile(baseq_file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	::SetFilePointerEx(baseq_file, baseq_map_size, 0, FILE_BEGIN);
	::SetEndOfFile(baseq_file);
	baseq_file_map = ::CreateFileMapping(baseq_file, 0, PAGE_READWRITE, 0, 0, 0);
	baseq = reinterpret_cast<Data*>(::MapViewOfFile(baseq_file_map, FILE_MAP_ALL_ACCESS, 0, 0, 0));
#endif

	// prepare base indices
	int cx = base_size - 1;
	int ch = (base_size - 1) >> 1;
	base_indices[0] = Index(ch, ch);
	base_indices[1] = Index(ch, 0);
	base_indices[2] = Index(cx, 0);
	base_indices[3] = Index(cx, ch);
	base_indices[4] = Index(cx, cx);
   base_indices[5] = Index(ch, cx);
	base_indices[6] = Index(0, cx);
	base_indices[7] = Index(0, ch);
	base_indices[8] = Index(0, 0);

	const float grad_scale = 128.0f;

	int counter = 0;
   IImage::Dimensions src_dim;
	int line = 0;
	std::string progress_bar;
	std::string image_extension(".png");
	std::string terrain_name;
	std::string terrain_base_color;
	std::string terrain_base_elevation;
	std::string terrain_base_gradient;
	std::string terrain_detail_elevation;
	std::string terrain_detail_gradient;
	std::string terrain_detail_scale;
	std::string terrain_bump_elevation;
   std::string terrain_bump_gradient;
	std::string terrain_cloud_color;

   for(unsigned int i=0;i<hf->getNumRows();i++)
	{
	   for(unsigned int j=0;j<hf->getNumColumns();j++)
	   {
	      Data* data_ptr = GetData(Index(j, i));
	      
	      data_ptr->height = hf->getHeight(j, hf->getNumRows()-i-1);
	      data_ptr->error = 0.0f;
	      data_ptr->radius = 0.0f;
	   }
	}

   src_dim.width = base_size - 1;
	src_dim.height = base_size - 1;
	src_dim.depth = 1;
	src_dim.channels = 1;
	src_dim.bpc = 16;
	src_image->Create(src_dim);
   
   line = 0;
	while(src_image->ProcessLine(line++, CreateScaleMap, this) > 0);
	
	line = 0;
	while(src_image->ProcessLine(line++, ProcessScaleMap, this) > 0);
	
	if(buildDetail)
	{
	   Notify(NOTICE, "SOARXTerrain: Making detail map...");
	   
	   src_dim.width = detail_size;
      src_dim.height = detail_size;
	   src_dim.depth = 1;
	   src_dim.channels = 1;
	   src_dim.bpc = 16;
	   src_image->Create(src_dim);
   	
	   line = 0;
	   while(src_image->ProcessLine(line++, CreateDetailMap, this) > 0);
   	
	   line = 0;
	   while(src_image->ProcessLine(line++, ProcessDetailMap, this) > 0);
	   
	   // unmap terrain file
	   ::UnmapViewOfFile(detail);
	   ::CloseHandle(detail_file_map);
	   ::CloseHandle(detail_file);
	}
	
   Phase1();
   Phase2();
   
#ifdef DATA_LAYOUT_EMBEDDED_QUADTREE
	BuildEmbeddedQuadtree();

	::UnmapViewOfFile(baseq);
	::CloseHandle(baseq_file_map);
	::CloseHandle(baseq_file);
#endif
	::UnmapViewOfFile(base);
	::CloseHandle(base_file_map);
	::CloseHandle(base_file);
		   
	return 0;
}

//______________________________________________________________
int TBuilder::CreateBaseMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	int a[] = {64, 128, 1024, 1024, 2048, 1024, 512, 256, 64, 58};

	int d = 0;
	int tx = cx<<2;
	int ty = cy<<2;

	for (int o=0; o<10; o++) {
		int c = 1024<<(12-(10-o));
		int x = cx<<(12-(10-o));
		int y = cy<<(12-(10-o));
		int p = n(x, y);
		int px = n(x-c, y);
		int py = n(x, y-c);
		int pxy = n(x-c, y-c);
		d += imul(ilerp(ty, ilerp(tx, p, px), ilerp(tx, py, pxy)), a[o]);
		d = clamp(d, 4095);
	}

	d <<= 3;
	d += 32767;

	return static_cast<u16>(d);
}

//______________________________________________________________
int TBuilder::CreateDetailMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	int a[] = {0, 0, 0, 0, 2048, 1024, 512, 256, 96, 58};

	int d = 0;
	int tx = cx<<2;
	int ty = cy<<2;

	for (int o=4; o<10; o++) {
		int c = 1024<<(12-(10-o));
		int x = cx<<(12-(10-o));
		int y = cy<<(12-(10-o));
		int p = n(x, y);
		int px = n(x-c, y);
		int py = n(x, y-c);
		int pxy = n(x-c, y-c);
		d += imul(ilerp(ty, ilerp(tx, p, px), ilerp(tx, py, pxy)), a[o]);
		d = clamp(d, 4095);
	}

	d <<= 3;
	d += 32767;

	return static_cast<u16>(d);
}

//______________________________________________________________
int TBuilder::CreateScaleMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	int a[] = {64, 128, 1024, 1024, 2048, 1024, 512, 5000, 4000, 4000};

	int d = 0;
	int tx = cx<<2;
	int ty = cy<<2;

	for (int o=8; o<10; o++) {
		int c = 1024<<(12-(10-o));
		int x = cx<<(12-(10-o));
		int y = cy<<(12-(10-o));
		int p = n(x, y);
		int px = n(x-c, y);
		int py = n(x, y-c);
		int pxy = n(x-c, y-c);
		d += imul(ilerp(ty, ilerp(tx, p, px), ilerp(tx, py, pxy)), a[o]);
		d = clamp(d, 4095);
	}

	d <<= 3;
	d += 40000;
	d = clamp(d, 0, 65535);
	//d <<= 4;
	//d |= 0x4000;

	return static_cast<u16>(d);
}

//______________________________________________________________
int TBuilder::CreateBumpMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	int a[] = {64, 128, 1024, 1024, 2048, 1024, 512, 256, 64, 58};

	int d = 0;
	int tx = cx<<2;
	int ty = cy<<2;

	for (int o=0; o<10; o++) {
		int c = 1024<<(12-(10-o));
		int x = cx<<(12-(10-o));
		int y = cy<<(12-(10-o));
		int p = n(x, y);
		int px = n(x-c, y);
		int py = n(x, y-c);
		int pxy = n(x-c, y-c);
		d += imul(ilerp(ty, ilerp(tx, p, px), ilerp(tx, py, pxy)), a[o]);
		d = clamp(d, 4095);
	}

	d <<= 3;
	d += 32767;

	return static_cast<u16>(d);
}

//______________________________________________________________
int TBuilder::CreateCloudMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	int a[] = {64, 128, 1024, 1024, 2048, 1024, 512, 256, 64, 58};

	int d = 0;
	int tx = cx<<2;
	int ty = cy<<2;

	for (int o=0; o<10; o++) {
		int c = 1024<<(12-(10-o));
		int x = cx<<(12-(10-o));
		int y = cy<<(12-(10-o));
		int p = n(x, y);
		int px = n(x-c, y);
		int py = n(x, y-c);
		int pxy = n(x-c, y-c);
		d += imul(ilerp(ty, ilerp(tx, p, px), ilerp(tx, py, pxy)), a[o]);
		d = clamp(d, 4095);
	}

	d <<= 3;
	d += 62767;
	d = clamp(d, 30000, 65535);

	return static_cast<u16>(d);
}


//______________________________________________________________
int TBuilder::ProcessBaseMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	TBuilder* t = static_cast<TBuilder*>(ptr);
	Index i(cx, cy);
	Data* data_ptr = t->GetData(i);
	data_ptr->height = value * t->base_vertical_resolution + t->base_vertical_bias;
	data_ptr->error = 0.0f;
	data_ptr->radius = 0.0f;
	return static_cast<u16>(value);
}

//______________________________________________________________
int TBuilder::ProcessDetailMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	TBuilder* t = static_cast<TBuilder*>(ptr);
	Index i(cx, cy);
	float* detail_ptr = t->GetDetail(i);
	*detail_ptr = (value - 32768.0f) * t->detail_vertical_resolution;
	return static_cast<u16>(value);
}

//______________________________________________________________
int TBuilder::ProcessScaleMap(int ch, int cx, int cy, int cz, int value, void* ptr)
{
	TBuilder* t = static_cast<TBuilder*>(ptr);
	Index i(cx, cy);
	Data* data_ptr = t->GetData(i);
	data_ptr->scale = value / 65535.0f;
	return static_cast<u16>(value);
}

//______________________________________________________________
inline Data* TBuilder::GetData(Index i)
{
	i.clamp(base_size - 1);
	return base + i.y * base_size + i.x;
}

//______________________________________________________________
inline f32* TBuilder::GetDetail(Index i)
{
	i &= (detail_size - 1);
	return detail + i.y * detail_size + i.x;
}

//______________________________________________________________
inline Vertex TBuilder::GetVertex(Index i)
{
	Vertex v;
	i.clamp(base_size - 1);
	Data* d = GetData(i);
	v.position(base_horizontal_resolution * i.x, d->height, base_horizontal_resolution * i.y, 1);
	v.error = d->error;
	v.radius = d->radius;
	return v;
}

//______________________________________________________________
f32 TBuilder::CalculateError(Index i, Index j)
{
	Index l(j.x + i.y - j.y, j.y + j.x - i.x);
	Index r(j.x + j.y - i.y, j.y + i.x - j.x);

	Vertex vj = GetVertex(j);
	Vertex vl = GetVertex(l);
	Vertex vr = GetVertex(r);

	v4f jp(vj.position);
	v4f lp(vl.position);
	v4f rp(vr.position);

	v4f lerp;
	lerp = lp + rp;
	lerp.scale(0.5f);

	return (jp-lerp).length();
}

//______________________________________________________________
void TBuilder::Phase1()
{
	level = 0;
	Index c = base_indices[0];
	for (u32 i=1; i<9; i++) {
		Index j = base_indices[i++];
		Phase1(c, j);
	}

}

//______________________________________________________________
void TBuilder::Phase1(Index i, Index j)
{
	level++;

	f32 err = CalculateError(i, j);
	GetData(j)->error = err;

	if (level < base_levels-1) {
		Phase1(j, Index(i, j, 0, false));
		Phase1(j, Index(i, j, 0, true));
	}

	level--;
}

//______________________________________________________________
void TBuilder::Repair(Index i, Index c)
{
	Vertex vi = GetVertex(i);
	Vertex vc = GetVertex(c);

	f32 d = (vi.position - vc.position).length() + vc.radius;

	Data* di = GetData(i);

	if (d > di->radius) {
		di->radius = d;
	}

	if (vc.error > di->error) {
		di->error = vc.error;
	}
}

//______________________________________________________________
void TBuilder::CheckChildren1(Index i, u32 shift)
{
	Index t(i);
	i <<= shift;

	u32 w = 1 << (shift-1);

	Repair(i, Index(i.x + w, i.y + w));

	if (i.x > 0 && i.y > 0) {
		Repair(i, Index(i.x - w, i.y - w));
	} 
	if (i.x > 0) {
		Repair(i, Index(i.x - w, i.y + w));
	} 
	if (i.y > 0) {
		Repair(i, Index(i.x + w, i.y - w));
	}
}

//______________________________________________________________
void TBuilder::CheckChildren2(Index i, u32 shift)
{
	Index t(i);
	i <<= shift;
	Repair(i, Index((t.x - 1) << shift, t.y << shift));
	Repair(i, Index(t.x << shift, (t.y - 1) << shift));
	Repair(i, Index((t.x + 1) << shift, t.y << shift));
	Repair(i, Index(t.x << shift, (t.y + 1) << shift));
}

//______________________________________________________________
void TBuilder::Phase2()
{
	for (u32 counter_bits=base_bits; counter_bits>0; counter_bits--) {
		u32 shift = base_bits-counter_bits; // 0..map_bits-1
		u32 counter = (1 << (counter_bits-1)); // map_size/2..1
		if (shift > 0) { // only from second level
			for (u32 y=0; y<=counter; y++) {
				for (u32 x=0; x<=counter; x++) {
					CheckChildren1(Index(((x<<1) + 1), (y<<1)), shift);
					CheckChildren1(Index((x<<1), ((y<<1) + 1)), shift);
				}
			}
		}
		for (u32 y=0; y<counter; y++) {
			for (u32 x=0; x<counter; x++) {
				CheckChildren2(Index(((x<<1) + 1), ((y<<1) + 1)), shift);
			}
		}
	}
}

//______________________________________________________________
void TBuilder::BuildEmbeddedQuadtree(Index i, Index j)
{
	if (level == base_levels - 2) {
		return;
	}

	level++;

	Index left(i, j, level & 1, false);
	BuildEmbeddedQuadtree(j, left);
	baseq[left.q] = *GetData(left);

	Index right(i, j, level & 1, true);
	BuildEmbeddedQuadtree(j, right);
	baseq[right.q] = *GetData(right);

	level--;
}

//______________________________________________________________
void TBuilder::BuildEmbeddedQuadtree()
{
	int c4 = (base_size - 1);
	int c2 = c4 >> 1;
	int c1 = c2 >> 1;
	int c3 = c2 + c1;

	Index base_indices[13];

	base_indices[0] = Index(0, 0, c4);
	base_indices[1] = Index(1, c4, c4);
	base_indices[2] = Index(2, c4, 0);
	base_indices[3] = Index(3, 0, 0);
	base_indices[4] = Index(4, c2, c2);
	base_indices[5] = Index(5, c2, 0);
	base_indices[6] = Index(6, c4, c2);
	base_indices[7] = Index(7, c2, c4);
	base_indices[8] = Index(8, 0, c2);
	base_indices[9] = Index(9, c3, c1);
	base_indices[10] = Index(14, c1, c1);
	base_indices[11] = Index(19, c1, c3);
	base_indices[12] = Index(24, c3, c3);

	for (int i=0; i<13; i++) {
		baseq[base_indices[i].q] = *GetData(base_indices[i]);
	}

	level = 1;
	BuildEmbeddedQuadtree(base_indices[8], base_indices[10]);
	BuildEmbeddedQuadtree(base_indices[8], base_indices[11]);
	BuildEmbeddedQuadtree(base_indices[7], base_indices[11]);
	BuildEmbeddedQuadtree(base_indices[7], base_indices[12]);
	BuildEmbeddedQuadtree(base_indices[6], base_indices[12]);
	BuildEmbeddedQuadtree(base_indices[6], base_indices[9]);
	BuildEmbeddedQuadtree(base_indices[5], base_indices[9]);
	BuildEmbeddedQuadtree(base_indices[5], base_indices[10]);
}

//______________________________________________________________
