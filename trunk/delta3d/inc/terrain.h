#ifndef DELTA_TERRAIN
#define DELTA_TERRAIN

// terrain.h: Declaration of the Terrain class.
//
//////////////////////////////////////////////////////////////////////


#include <sstream>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/LOD>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>
#include <osg/PrimitiveSet>
#include <osg/ShapeDrawable>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TexGen>
#include <osg/Texture2D>

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include <osgTerrain/HeightFieldNode>

#include "drawable.h"
#include "notify.h"
#include "transformable.h"

namespace dtCore
{
   class SimpleHeightFieldRenderer;
   class TerrainCallback;
   
   
   /**
    * A terrain object.
    */
   class DT_EXPORT Terrain : public Transformable,
                             public Drawable
   {
      friend class SimpleHeightFieldRenderer;
      friend class TerrainCallback;
      
      
      DECLARE_MANAGEMENT_LAYER(Terrain)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Terrain(std::string name = "Terrain");

         /**
          * Destructor.
          */
         virtual ~Terrain();
         
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
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode();
         
         
      private:
         
         /**
          * Converts a set of geocentric coordinates to the equivalent geodetic
          * coordinates.  Uses the formula given at
          * <A HREF="http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html">
          * http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html</A>.
          *
          * @param x the geocentric x coordinate
          * @param y the geocentric y coordinate
          * @param z the geocentric z coordinate
          * @param latitude the location in which to store the geodetic latitude
          * @param longitude the location in which to store the geodetic longitude
          * @param elevation the location in which to store the geodetic elevation
          */
         static void GeocentricToGeodetic(double x, double y, double z,
                                          double* latitude, double* longitude, double* elevation);

         /**
          * Converts a set of geodetic coordinates to the equivalent geocentric
          * coordinates.  Uses the formula given at
          * <A HREF="http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html">
          * http://www.colorado.edu/geography/gcraft/notes/datum/datum_f.html</A>.
          *
          * @param latitude the geodetic latitude
          * @param longitude the geodetic longitude
          * @param elevation the geodetic elevation
          * @param x the location in which to store the geocentric x coordinate
          * @parma y the location in which to store the geocentric y coordinate
          * @param z the location in which to store the geocentric z coordinate
          */
         static void GeodeticToGeocentric(double latitude, double longitude, double elevation,
                                          double* x, double* y, double* z);
                                          
         /**
          * Converts a point in the local coordinate system to geodetic
          * coordinates.
          *
          * @param pt the local point to convert
          * @param latitude the location in which to store the latitude
          * @param longitude the location in which to store the longitude
          * @param elevation the location in which to store the elevation
          */
         void LocalToGeodetic(const osg::Vec3& pt,
                              double* latitude,
                              double* longitude,
                              double* elevation);
         
         /**
          * Converts a set of geodetic coordinates to a point in the local
          * coordinate system.
          *
          * @param latitude the latitude of the location to convert
          * @param longitude the longitude of the location to convert
          * @param elevation the elevation of the location to convert
          * @return the converted point
          */
         osg::Vec3 GeodeticToLocal(double latitude,
                                   double longitude,
                                   double elevation);
         
         /**
          * Makes a base texture for the specified heightfield by mapping heights to
          * colors.
          *
          * @param hf the heightfield to read
          * @return the newly created base texture
          */
         osg::Image* MakeBaseTexture(osg::HeightField* hf);
         
         /**
          * Loads a single terrain segment.
          *
          * @param latitude the latitude of the segment to load
          * @param longitude the longitude of the segment to load
          */
         void LoadSegment(int latitude, int longitude);

         /**
          * The detail texture (noise) image.
          */
         static osg::ref_ptr<osg::Image> mDetailTextureImage;
         
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
          * The origin in geocentric coordinates.
          */
         sgdVec3 mGeocentricOrigin;
         
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
          * The OSG node.
          */
         osg::ref_ptr<osg::MatrixTransform> mNode;
         
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
          * The set of loaded segments.
          */
         std::set<Segment> mLoadedSegments;
         
         /**
          * Flags the segments as needing to be cleared.
          */
         bool mClearFlag;
   };
   
   
   /**
    * A simple, direct heightfield renderer.
    */
   class DT_EXPORT SimpleHeightFieldRenderer : public osgTerrain::HeightFieldRenderer
   {
      public:
      
         /**
          * Constructor.
          *
          * @param terrain a reference to the creating Terrain object
          * @param latitude the latitude of the segment
          * @param longitude the longitude of the segment
          * @param level the DTED level of the segment
          */
         SimpleHeightFieldRenderer(Terrain* terrain = NULL,
                                   int latitude = 0,
                                   int longitude = 0,
                                   int level = 0);
         
         /**
          * Copy constructor.
          */
         SimpleHeightFieldRenderer(const SimpleHeightFieldRenderer&,
                                   const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
         
         META_Object(dtCore, SimpleHeightFieldRenderer)
         
         /**
          * Initializes the renderer.
          */ 
         virtual void initialize();

         /**
          * Notifies the renderer that the heightfield has been modified.
          */
         virtual void heightFieldHasBeenModified();

         /**
          * Update callback.
          *
          * @param nv the update visitor
          */
         virtual void update(osgUtil::UpdateVisitor* nv);

         /**
          * Cull callback.
          *
          * @param cv the cull visitor
          */
         virtual void cull(osgUtil::CullVisitor* cv);
   
      
      protected:
      
         /**
          * Destructor.
          */
         ~SimpleHeightFieldRenderer();
         
         /**
          * Creates the heightfield Drawable.
          */
         void CreateHeightFieldDrawable();
         
         
      private:
      
         /**
          * The creating Terrain object.
          */
         osg::ref_ptr<Terrain> mTerrain;
         
         /**
          * The latitude, longitude, and DTED level of the heightfield.
          */
         int mLatitude, mLongitude, mLevel;
         
         /**
          * The Geode that holds the heightfield Drawable.
          */
         osg::ref_ptr<osg::Geode> mGeode;
   };
};


#endif // DELTA_TERRAIN
