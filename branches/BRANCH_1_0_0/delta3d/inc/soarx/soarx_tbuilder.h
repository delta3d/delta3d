//______________________________________________________________
#ifndef DELTA_SOARX_TBUILDER
#define DELTA_SOARX_TBUILDER

#define DATA_LAYOUT_EMBEDDED_QUADTREE

#include "soarx/soarx_framework.h"
#include "soarx/soarx_terrain.h"
#include "soarx/soarx_image.h"

#include <osg/Image>
#include <osg/Shape>

namespace dtSOARX
{
   typedef void* HANDLE;

   //______________________________________________________________
   class TBuilder
   {
   private:
	   Image* src_image;
	   Image* dst_image;
	   f32* detail;
	   Data* base;
	   Data* baseq;
	   Index base_indices[9];
	   HANDLE base_file;
	   HANDLE base_file_map;
	   HANDLE detail_file;
	   HANDLE detail_file_map;
	   HANDLE baseq_file;
	   HANDLE baseq_file_map;

   public:
	   int level;
	   int base_levels;

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

   public:
	   TBuilder();
	   ~TBuilder();

	   typedef int (TBuilder::*TFunc)(Index i, f32 value);

      /**
       * Builds the various files necessary for the SOARX algorithm.
       *
       * @param path the path in which to build the files
       * @param prefix the filename prefix (e.g., "w122_n36_0")
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
	   int Build(const char* path,
	             const char* prefix,
	             int mapBits,
	             int baseBits,
	             int detailBits,
	             float horizontalResolution,
	             float verticalResolution,
	             float verticalBias,
	             float detailVerticalResolution,
	             osg::HeightField* hf,
	             bool buildDetail);
	   
	   static int CreateBaseMap(int ch, int cx, int cy, int cz, int value, void* ptr);
	   static int CreateDetailMap(int ch, int cx, int cy, int cz, int value, void* ptr);
	   static int CreateScaleMap(int ch, int cx, int cy, int cz, int value, void* ptr);
	   static int CreateBumpMap(int ch, int cx, int cy, int cz, int value, void* ptr);
	   static int CreateCloudMap(int ch, int cx, int cy, int cz, int value, void* ptr);
	   static int ProcessBaseMap(int ch, int cx, int cy, int cz, int value, void* ptr);
	   static int ProcessDetailMap(int ch, int cx, int cy, int cz, int value, void* ptr);
	   static int ProcessScaleMap(int ch, int cx, int cy, int cz, int value, void* ptr);

	   int BuildTextures();

	   Data* GetData(Index i);
	   f32* GetDetail(Index i);
	   Vertex GetVertex(Index i);

	   f32 CalculateError(Index i, Index j);
	   void Preprocess();
	   void Repair(Index i, Index c);
	   void CheckChildren1(Index i, u32 shift);
	   void CheckChildren2(Index i, u32 shift);
	   void Phase1();
	   void Phase1(Index i, Index j);
	   void Phase2();
	   void BuildEmbeddedQuadtree();
	   void BuildEmbeddedQuadtree(Index i, Index j);
   };
};

//______________________________________________________________

#endif // DELTA_SOARX_TBUILDER

