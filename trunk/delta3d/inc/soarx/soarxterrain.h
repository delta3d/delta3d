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

// soarxterrain.h: Declaration of the SOARXTerrain class.
//
//  The original SOARX algorithm developed by Andras Balogh
//        http://web.interware.hu/bandi/ranger.html
//  Needless to say, we were very impressed and are in his debt
//
//////////////////////////////////////////////////////////////////////

#ifndef DT_SOARX_TERRAIN
#define DT_SOARX_TERRAIN
#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

#include <sstream>

#include <osg/ref_ptr>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/Texture2D>
#include <osg/Switch>
#include <osg/PositionAttitudeTransform>

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include <osgGL2/ProgramObject>

#include <osgUtil/TriStripVisitor>

#include <tinyxml.h>

#include <gdal_priv.h>
#include <gdalwarper.h>

#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>

#include "dtCore/deltadrawable.h"
#include "dtCore/physical.h"
#include "dtCore/transformable.h"

#include "soarx/soarxdrawable.h"
#include "soarx/soarx_tbuilder.h"

namespace dtSOARX
{
   class SOARXTerrainCallback;
   
   
   /**
    * A piece of terrain using the SOARX library.
    */
   class DT_EXPORT SOARXTerrain : public dtCore::Transformable,
                                  public dtCore::DeltaDrawable,
                                  public dtCore::Physical
   {
      friend class SOARXTerrainCallback;
      
      
      DECLARE_MANAGEMENT_LAYER(SOARXTerrain)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         SOARXTerrain(std::string name = "SOARXTerrain");

         /**
          * Destructor.
          */
         virtual ~SOARXTerrain();

		 /**
		 *  Cleanup routines
		 */
		 void CleanUp();

         /**
          * Loads the specified configuration file.
          *
          * @param filename the name of the configuration file to load
          */
         void LoadConfiguration(std::string filename);
         
         /**
          * Parses the specified XML configuration element.
          *
          * @param configElement the configuration element to parse
          */
         void ParseConfiguration(TiXmlElement* configElement);
         
         /**
          * Sets the location of the origin in geographic coordinates.
          *
          * @param latitude the latitude of the origin
          * @param longitude the longitude of the origin
          * @param elevation the elevation of the origin
          */
         void SetGeoOrigin(double latitude, double longitude, double elevation);
         
         /**
          * Retrieves the location of the origin in geographic coordinates.
          *
          * @param latitude the location in which to store the latitude
          * @param longitude the location in which to store the longitude
          * @param elevation the location in which to store the elevation
          */
         void GetGeoOrigin(double* latitude, double* longitude, double* elevation);
         
         /**
          * Sets the load distance: the distance from the eyepoint for which terrain
          * is guaranteed to be loaded (if present).
          *
          * @param loadDistance the new load distance
          */
         void SetLoadDistance(float loadDistance);
         
         /**
          * Returns the load distance.
          *
          * @return the current load distance
          */
         float GetLoadDistance();
         
         /**
          * Adds a path from which to load DTED heightfield data.  Paths are
          * searched in the order that they are added.
          *
          * @param path the path to add
          */
         void AddDTEDPath(std::string path);
         
         /**
          * Removes a DTED path.
          *
          * @param path the path to remove
          */
         void RemoveDTEDPath(std::string path);
         
         /**
          * Returns the number of paths in the DTED search list.
          *
          * @return the number of paths
          */
         unsigned int GetNumDTEDPaths();
         
         /**
          * Returns the DTED path at the specified index.
          *
          * @param index the index of the path to retrieve
          */
         std::string GetDTEDPath(unsigned int index);
         
         /**
          * Sets the path of the terrain cache.  If the path is unset,
          * terrain data will not be cached.
          *
          * @param path the new path
          */
         void SetCachePath(std::string path);
         
         /**
          * Returns the path of the terrain cache.
          *
          * @return the current path
          */
         std::string GetCachePath();
         
         /**
          * Reloads the terrain around the eyepoint.
          */
         void Reload();
         
         /**
          * Maps height values to colors with interpolation/extrapolation.
          */
         struct DT_EXPORT HeightColorMap : public std::map<float, osg::Vec3>
         {
            /**
             * Gets the color corresponding to the specified height.
             *
             * @param height the height value to map
             * @return the corresponding color
             */
            osg::Vec3 GetColor(float height);
         };
         
         /**
          * Sets the color map for height values at or below sea level.
          *
          * @param hcm the new height color map
          */
         void SetLowerHeightColorMap(const HeightColorMap& hcm);
         
         /**
          * Returns the color map for height values at or below sea level.
          *
          * @return the current color map
          */
         const HeightColorMap& GetLowerHeightColorMap();
         
         /**
          * Sets the color map for height values above sea level.
          *
          * @param hcm the new height color map
          */
         void SetUpperHeightColorMap(const HeightColorMap& hcm);
         
         /**
          * Returns the color map for height values above sea level.
          *
          * @return the current color map
          */
         const HeightColorMap& GetUpperHeightColorMap();
         
         /**
          * Sets the maximum texture size, which should be a power of two
          * (default is 4096).
          *
          * @param maxTextureSize the new maximum texture size
          */
         void SetMaxTextureSize(int maxTextureSize);
         
         /**
          * Returns the maximum texture size.
          *
          * @return the maximum texture size
          */
         int GetMaxTextureSize();
         
         /**
          * Loads a geospecific image and drapes it over the terrain.  If
          * the image is monochrome, it will be modulated by the height
          * color map.
          *
          * @param filename the name of the image to load
          * @param geoTransform an array of six double values representing
          * the geotransform of the image, or NULL to read the geotransform
          * from the image itself
          */
         void LoadGeospecificImage(std::string filename,
                                   const double* geoTransform = NULL);

		 /**
		 * Loads a geospecific LCC image and drapes it over the terrain.  If
		 * the image is monochrome, it will be modulated by the height
		 * color map.
		 *
		 * @param filename the name of the image to load
		 * @param geoTransform an array of six double values representing
		 * the geotransform of the image, or NULL to read the geotransform
		 * from the image itself
		 */
		 void LoadGeospecificLCCImage(std::string filename,
			 const double* geoTransform = NULL);
 

		 /**
		 * Tell me interesting stats about an image.
		 * @param image the handle to the image you want to examine
		 * @param imagename a descriptive name to call this image
		 */        
		 void ImageStats(osg::Image* image, char* imagename);


         /**
          * Loads road data from the specified filename.
          *
          * @param filename the name of the vector data file to load
          * @param query a SQL query that selects road features from
          * the data file
          * @param width the width of the roads to create
          * @param texture the name of the texture to use, or "" for
          * none (in which case the roads will be baked into the base
          * texture)
          * @param sScale the texture s scale
          * @param tScale the texture t scale
          */
         void LoadRoads(std::string filename,
                        std::string query = "SELECT * FROM *",
                        float width = 7.0f,
                        std::string texture = "",
                        float sScale = 1.0f,
                        float tScale = 1.0f);
         
         /**
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode();
         
         /**
          * Sets the threshold parameter.
          *
          * @param threshold the new threshold
          */
         void SetThreshold(float threshold);
         
         /**
          * Returns the threshold parameter.
          *
          * @return the current threshold
          */
         float GetThreshold();
         
         /**
          * Sets the detail multiplier parameter.
          *
          * @param detailMultiplier the new detail multiplier
          */
         void SetDetailMultiplier(float detailMultiplier);
         
         /**
          * Returns the detail multiplier parameter.
          *
          * @return the current detail multiplier
          */
         float GetDetailMultiplier();
         
         /**
          * Determines the height of the terrain at the specified location.
          *
          * @param x the x coordinate to check
          * @param y the y coordinate to check
          * @return the height at the specified location
          */
         float GetHeight(float x, float y);

		 
         /**
          * Determines the Longitude & Latitude of a specified location.
          * @param x the x coordinate to check
          * @param y the y coordinate to check
          * @return Vec2f (longitude, latitude)
          */
		 osg::Vec2f GetLongLat(float x, float y);

         
         /**
          * Retrieves the normal of the terrain at the specified location.
          *
          * @param x the x coordinate to check
          * @param y the y coordinate to check
          * @param normal the location at which to store the normal
          */
         void GetNormal(float x, float y, sgVec3 normal);
        
 
         
         /**
          * Determine if we are using LCC data.
          *
          * @return the value of mUseLCC
          */
         bool GetLCCStatus();


		 /**
		 * Determine if we are using LCC data.
		 * @param LCCtype the LCC index type (e.g. "42" is evergreen forest)
		 * @param x the x coordinate to check
		 * @param y the y coordinate to check
		 * @param limit the limiting value of the probability (i.e. the roll of the dice)
		 * @return the value of mUseLCC
		 */
//		 boolean GetVegetation(int LCCtype, int x, int y, int limit);


		 /**
		 * Sets if LCC objects are updated/culled/rendered.
		 * @param mask 0/false=hide, 1/true=display
		 */
		 void SetLCCVisibility(bool mask);

         
      private:

         /**
          * Makes the detail gradient texture map for the specified DTED level.
          *
          * @param level the DTED level to make the texture map for
          * @return the newly created image
          */
         osg::Image* MakeDetailGradient(int level);
         
         /**
          * Makes the detail scale texture map for the specified DTED level.
          *
          * @param level the DTED level to make the texture map for
          * @return the newly created image
          */
         osg::Image* MakeDetailScale(int level);
         
         /**
          * Makes the base gradient texture map for the specified heightfield.
          *
          * @param hf the heightfield to process
          * @return the newly created image
          */
         osg::Image* MakeBaseGradient(osg::HeightField* hf);
         
         /**
          * Makes the base color texture map for the specified heightfield.
          *
          * @param hf the heightfield to process
          * @param latitude the latitude of the terrain segment
          * @param longitude the longitude of the terrain segment
          * @return the newly created image
          */
         osg::Image* MakeBaseColor(osg::HeightField* hf, int latitude, int longitude);


		 /**
		 * Makes the base LCC color texture map for the specified heightfield.
		 *
		 * @param hf the heightfield to process
		 * @param latitude the latitude of the terrain segment
		 * @param longitude the longitude of the terrain segment
		 * @return the newly created image
		 */
		 osg::Image* MakeBaseLCCColor(osg::HeightField* hf, int latitude, int longitude);


         
         /**
          * Makes roads for the specified segment.
          *
          * @param latitude the latitude of the terrain segment
          * @param longitude the longitude of the terrain segment
          * @param origin the origin of the terrain cell
          * @return the newly created road node
          */
         osg::Node* MakeRoads(int latitude, int longitude, const osg::Vec3& origin);
         

		 /**
		 * Create hit/miss map of the terrain by LCC type
		 * @param src_image the Base Color image with LCC encoded within
		 * @param rgb_selected the RGB color of the LCC type selected
		 * @return the newly created image
		 */
		 osg::Image* MakeLCCImage(osg::Image* src_image, osg::Vec3& rgb_selected);

		 /**
		 * Create smoothed grayscale map of the terrain by LCC type
		 * Uses weighted next nearest neighbor to "fuzzy"-up the LCCImage data
		 * @param src_image the black/white LCC Image by LCC type
		 * @param rgb_selected the RGB color of the points to smooth (always 0,0,0 - black)
		 * @return the newly created image
		 */
		 osg::Image* MakeFilteredImage(osg::Image* src_image, osg::Vec3& rgb_selected);

		 /**
		 * Create heightmap image
		 * @param hf the GDAL-derived heightfield
		 * @return the newly created image
		 */
		 osg::Image* MakeHeightmapImage(osg::HeightField* hf);

		 /**
		 * Create slopemap from GDAL-derived heightfield data
		 * @param hf the GDAL-derived heightfield
		 * @return the newly created image
		 */
		 osg::Image* MakeSlopemapImage(osg::HeightField* hf);

		 /**
		 * Create relative elevation map from GDAL-derived heightfield data
		 * @param hf the GDAL-derived heightfield
		 * @return the newly created image
		 */
		 osg::Image* MakeRelativeElevationImage(osg::HeightField* hf);


		 /**
		 * Create probability map of the likehihood for a particular LCC type
		 * @param LCCidx LCC image index
		 * @param h_image the heightmap image
		 * @param s_image the slopemap image
		 * @param r_image the relative elevation image
		 * @return the newly created image
		 */
		 osg::Image* MakeCombinedImage(
			 osg::Image* f_image,			// LCC filtered image
			 osg::Image* h_image,			// heightmap
			 osg::Image* s_image,			// slopemap image
			 osg::Image* r_image);			// relative elevation image

		 /**
         /**
          * Loads a single terrain segment.
          *
          * @param latitude the latitude of the segment to load
          * @param longitude the longitude of the segment to load
          */
         void LoadSegment(int latitude, int longitude);
         
         /**
          * ODE collision function: Gets the contact points between two
          * geoms.
          *
          * @param o1 the first (SOARXTerrain) geom
          * @param o2 the second geom
          * @param flags collision flags
          * @param contact the array of contact geoms to fill
          * @param skip the distance between contact geoms in the array
          * @return the number of contact points generated
          */
         static int Collider(dGeomID o1, dGeomID o2, int flags,
                             dContactGeom* contact, int skip);
         
         /**
          * ODE collision function: Finds the collider function appropriate
          * to detect collisions between SOARXTerrain geoms and other
          * geoms.
          *
          * @param num the class number of the geom class to check
          * @return the appropriate collider function, or NULL for none
          */
         static dColliderFn* GetColliderFn(int num);
         
         /**
          * ODE collision function: Computes the axis-aligned bounding box
          * for SOARXTerrain instances.
          *
          * @param g the geom to check
          * @param aabb the location in which to store the axis-aligned
          * bounding box
          */
         static void GetAABB(dGeomID g, dReal aabb[6]);
         
         /**
          * ODE collision function: Checks whether the specified axis-aligned
          * bounding box intersects with a SOARXTerrain instance.
          *
          * @param o1 the first (SOARXTerrain) geom
          * @param o2 the second geom
          * @param aabb2 the axis-aligned bounding box of the second geom
          * @return 1 if it intersects, 0 if it does not
          */
         static int AABBTest(dGeomID o1, dGeomID o2, dReal aabb2[6]);


		 /**
		 * Buggy "histogram" of an image by a particular LCC type.
		 *
		 * @param LCCbase the black/white LCC image of picked points of a particular LCC type
		 * @param image the slopemap, heightmap, or relative elevation
		 * @param filename the filename to save the histogram data
		 * @param binsize the sampling size of the image (i.e. the delta height or slope).
		 */
		 void LCCHistogram(osg::Image* LCCbase, osg::Image* image, char* filename, int binsize);


         
         /**
          * The container node.
          */
         osg::ref_ptr<osg::MatrixTransform> mNode;
         
		 /**
		  *	  Listing of objects (plants, trees, etc) 
		  */
		 std::vector<osg::ref_ptr<dtCore::Object> > mObjects;

		 /**
		  *	  Listing of groups
		  */
		 std::vector<osg::ref_ptr<osg::Group> > mGroups;		 

         /**
          * The GLSL program object.
          */
         osg::ref_ptr<osgGL2::ProgramObject> mProgramObject;
         
         /**
          * The latitude of the origin in geographic coordinates.
          */
         double mOriginLatitude;
         
         /**
          * The longitude of the origin in geographic coordinates.
          */
         double mOriginLongitude;
         
         /**
          * The elevation of the origin in geographic coordinates.
          */
         double mOriginElevation;
         
         /**
          * The distance from the eyepoint for which terrain will be loaded.
          */
         float mLoadDistance;
         
         /**
          * The list of paths from which to load DTED heightfield data.
          */
         osgDB::FilePathList mDTEDPaths;
         
         /**
          * The path of the terrain cache, or "" if caching is disabled.
          */
         std::string mCachePath;
         
         /**
          * The height color map for height values at or below sea level.
          */
         HeightColorMap mLowerHeightColorMap;
         
         /**
          * The height color map for height values above sea level.
          */
         HeightColorMap mUpperHeightColorMap;
         
         /**
          * The maximum texture size.
          */
         int mMaxTextureSize;
         

         /**
          * Detail gradient textures for each of the three DTED levels.
          */
         osg::ref_ptr<osg::Texture2D> mDetailGradient[3];
         
         /**
          * Detail scale textures for each of the three DTED levels.
          */
         osg::ref_ptr<osg::Texture2D> mDetailScale[3];
         
         /**
          * Identifies a single terrain segment.
          */
         struct Segment
         {
            int mLatitude, mLongitude;
            
            Segment(int latitude, int longitude)
               : mLatitude(latitude),
                 mLongitude(longitude)
            {}
            
            bool operator<(const Segment& segment) const
            {
               if(mLatitude < segment.mLatitude) return true;
               else if(mLatitude > segment.mLatitude) return false;
               else return (mLongitude < segment.mLongitude);
            }
         };
         
         /**
          * Maps loaded segments to SOARXDrawable objects.
          */
         std::map<Segment, SOARXDrawable*> mSegmentDrawableMap;
         
         /**
          * A geospecific image.
          */
         struct GeospecificImage
         {
            osg::ref_ptr<osg::Image> mImage;
            
            std::string mFilename;
            
            int mMinLatitude, mMaxLatitude,
                mMinLongitude, mMaxLongitude;
                
            double mGeoTransform[6];
            
            double mInverseGeoTransform[6];
         };
         
         /**
          * The list of geospecific images.
          */
         std::vector<GeospecificImage> mGeospecificImages;


		 /**
		 * The list of geospecific LCC images.
		 */
		 std::vector<GeospecificImage> mGeospecificLCCImages;


         
         /**
          * Maps filenames to loaded OGR data sources.
          */
         std::map<std::string, OGRDataSource*> mFilenameOGRDataSourceMap;
         
         /**
          * Road data.
          */
         struct Roads
         {
            OGRLayer* mLayer;
            
            int mMinLatitude, mMaxLatitude,
                mMinLongitude, mMaxLongitude;
            
            osg::ref_ptr<osg::Texture2D> mTexture;
             
            float mWidth, mSScale, mTScale;
         };

		 /**
		  *	An LCC Cell's data
		  */
		 struct LCCCells
		 {
			 /**
			 *	  Listing/location of objects (plants, trees, etc) 
			 */
			 std::vector<osg::PositionAttitudeTransform*> mPATs;


			 /**
			 * The top node of the vegetation scene graph.
			 */
			 osg::ref_ptr<osg::Group> mRootVegeGroup;
		 };


		 /**
		 * Maps loaded segments to LCCCells.
		 */
		 std::map<Segment, LCCCells*> mSegmentLCCCellMap;


		 /**
		  *	 LCC type data
		  */
		 struct LCCs
		 {
			 int idx;
			 int rgb[3];
			 std::string name;
			 std::string model;
			 osg::Group* vegeObject;
		 };


		 /**
		  *	 The list of active LCCs
		  */
		 std::vector<LCCs> mLCCs;

         
         /**
          * The list of road data.
          */
         std::vector<Roads> mRoads;
         

		 /**
		 * The list of LCC Cell data.
		 */
		 std::vector<LCCCells> mLCCCells;


		 /**
		 * The list of segments.
		 */
		 std::vector<Segment> mSegments;

		 /**
         /**
          * The threshold parameter.
          */
         float mThreshold;
         
         /**
          * The detail multiplier parameter.
          */
         float mDetailMultiplier;
         
         /**
          * Flags the segments as needing to be cleared.
          */
         bool mClearFlag;

         /**
          * Flags the need to use LCC data.
          */
         bool mUseLCC;


		 /**
		  *	 Hack to retain scope of quadtree groups
		  */
		 std::vector<osg::Group*> mGroupies;

		 /**
		 * Loads the specified configuration file.
		 *
		 * @param filename the name of the configuration file to load
		 */
		 bool LoadLCCConfiguration(std::string filename);

		 /**
		 * Parses the specified XML configuration element.
		 *
		 * @param configElement the configuration element to parse
		 */
		 void ParseLCCConfiguration(TiXmlElement* configElement);


		 /**
		 * Place the vegetation into cell specified by lat-long.
		 *
		 * @param latitude the latitude of the origin
		 * @param longitude the longitude of the origin
		 */		 
		 void AddVegetation(int latitude, int longitude);

		 bool GetVegetation(osg::Image* mCimage, int x, int y, int limit);

   };
};

#endif // DT_SOARX_TERRAIN