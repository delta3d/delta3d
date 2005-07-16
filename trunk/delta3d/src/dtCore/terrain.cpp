// terrain.cpp: Implementation of the Terrain class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>

#include "dtCore/scene.h"
#include "dtCore/terrain.h"
#include "dtCore/notify.h"

#include <math.h>

using namespace dtCore;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(Terrain)


/**
 * The length of the semi-major axis, in meters (WGS 84).
 */
const double semiMajorAxis = 6378137.0;

/**
 * The reciprocal of the flattening parameter (WGS 84).
 */
const double flatteningReciprocal = 298.257223563;


/**
 * The terrain callback class.  Loads terrain segments
 * around viewer.
 */
class dtCore::TerrainCallback : public osg::NodeCallback
{
   public:
   
      /**
       * Constructor.
       *
       * @param terrain the owning Terrain object
       */
      TerrainCallback(Terrain* terrain)
         : mTerrain(terrain)
      {}
      
      /**
       * Callback function.
       *
       * @param node the node to operate on
       * @param nv the active node visitor
       */
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         if(mTerrain->mClearFlag)
         {
            mTerrain->mNode->removeChild(0, mTerrain->mNode->getNumChildren());
   
            mTerrain->mLoadedSegments.clear();
            
            mTerrain->mClearFlag = false;
         }
         
         osg::Vec3 eyepoint = nv->getEyePoint();
         
         //float bd = mTerrain->GetLoadDistance();
         
         double latitude, longitude, elevation;
         
         mTerrain->LocalToGeodetic(eyepoint, &latitude, &longitude, &elevation);

         float latSize = osg::RadiansToDegrees(mTerrain->GetLoadDistance()/semiMajorAxis),
            longSize = latSize/cosf(osg::DegreesToRadians(latitude));
         
         int minLat = (int)floor(latitude-latSize),
             maxLat = (int)floor(latitude+latSize),
             minLong = (int)floor(longitude-longSize),
             maxLong = (int)floor(longitude+longSize);
         
         for(int i=minLat;i<=maxLat;i++)
         {
            for(int j=minLong;j<=maxLong;j++)
            {
               mTerrain->LoadSegment(i, j);
            }
         }
         
         traverse(node, nv);
      }
      
      
   private:
   
      /**
       * The owning Terrain object.
       */
      Terrain* mTerrain;
};


class TransformCallback : public osg::NodeCallback
{
   public:

      TransformCallback(Terrain* terrain)
      {
         mTerrain = terrain;
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      { 
         Transform transform;

         mTerrain->GetTransform(&transform);
         
         osg::Matrix matrix;

         transform.Get(matrix);
         
         osg::MatrixTransform* mt = 
            (osg::MatrixTransform*)mTerrain->GetOSGNode();
         
         mt->setMatrix( matrix );
         
         traverse(node, nv);
      }
      
   private:
      
      Terrain* mTerrain;
};


/**
 * Constructor.
 *
 * @param name the instance name
 */
Terrain::Terrain(string name)
   : mLoadDistance(30000.0f)
{
   SetName(name);
   
   SetGeoOrigin(0.0, 0.0, 0.0);
   
   mClearFlag = false;
   
   mLowerHeightColorMap[-11000.0f].set(0, 0, 0); // Marianas Trench
   mLowerHeightColorMap[0.0f].set(0, 0.3, 0.6);
   
   mUpperHeightColorMap[0.0f].set(0.647, 0.482, 0.224);
   mUpperHeightColorMap[300.0f].set(0.710, 0.647, 0.388);
   mUpperHeightColorMap[600.0f].set(0.741, 0.741, 0.482);
   mUpperHeightColorMap[1200.0f].set(0.290, 0.612, 0.290);
   mUpperHeightColorMap[2000.0f].set(0.482, 0.741, 0.322);
   mUpperHeightColorMap[2500.0f].set(0.647, 0.809, 0.518);
   mUpperHeightColorMap[3000.0f].set(1, 1, 1);
   mUpperHeightColorMap[9000.0f].set(1, 1, 1); // Everest
   
   RegisterInstance(this);
   
   mNode = new osg::MatrixTransform;
   
   mNode->setUpdateCallback(
      new TransformCallback(this)
   );
   
   mNode->setCullCallback(
      new TerrainCallback(this)
   );
   
   mNode->setCullingActive(false);
}

/**
 * Destructor.
 */
Terrain::~Terrain()
{
   DeregisterInstance(this);
}

/**
 * Sets the location of the origin in geographic coordinates.
 *
 * @param latitude the latitude of the origin
 * @param longitude the longitude of the origin
 * @param elevation the elevation of the origin
 */
void Terrain::SetGeoOrigin(double latitude, double longitude, double elevation)
{
   mOriginLatitude = latitude;
   mOriginLongitude = longitude;
   mOriginElevation = elevation;
   
   GeodeticToGeocentric(
      mOriginLatitude, mOriginLongitude, mOriginElevation,
      mGeocentricOrigin, mGeocentricOrigin + 1, mGeocentricOrigin + 2
   );
   
   mClearFlag = true;
}

/**
 * Retrieves the location of the origin in geographic coordinates.
 *
 * @param latitude the location in which to store the latitude
 * @param longitude the location in which to store the longitude
 * @param elevation the location in which to store the elevation
 */
void Terrain::GetGeoOrigin(double* latitude, double* longitude, double* elevation)
{
   *latitude = mOriginLatitude;
   *longitude = mOriginLongitude;
   *elevation = mOriginElevation;
}

/**
 * Sets the load distance: the distance from the eyepoint for which terrain
 * is guaranteed to be loaded (if present).
 *
 * @param loadDistance the new load distance
 */
void Terrain::SetLoadDistance(float loadDistance)
{
   mLoadDistance = loadDistance;
}

/**
 * Returns the load distance.
 *
 * @return the current load distance
 */
float Terrain::GetLoadDistance()
{
   return mLoadDistance;
}

/**
 * Adds a path from which to load DTED heightfield data.  Paths are
 * searched in the order that they are added.
 *
 * @param path the path to add
 */
void Terrain::AddDTEDPath(std::string path)
{
   mDTEDPaths.push_back(path);
}

/**
 * Removes a DTED path.
 *
 * @param path the path to remove
 */
void Terrain::RemoveDTEDPath(string path)
{
   for(osgDB::FilePathList::iterator it = mDTEDPaths.begin();
       it != mDTEDPaths.end();
       it++)
   {
      if(*it == path)
      {
         mDTEDPaths.erase(it);
         return;
      }
   }
}

/**
 * Returns the number of paths in the DTED search list.
 *
 * @return the number of paths
 */
unsigned int Terrain::GetNumDTEDPaths()
{
   return mDTEDPaths.size();
}

/**
 * Returns the DTED path at the specified index.
 *
 * @param index the index of the path to retrieve
 */
string Terrain::GetDTEDPath(unsigned int index)
{
   return mDTEDPaths[index];
}

/**
 * Sets the path of the terrain cache.  If the path is unset,
 * terrain data will not be cached.
 *
 * @param path the new path
 */
void Terrain::SetCachePath(string path)
{
   mCachePath = path;
}

/**
 * Returns the path of the terrain cache.
 *
 * @return the current path
 */
string Terrain::GetCachePath()
{
   return mCachePath;
}

/**
 * Reloads the terrain around the eyepoint.
 */
void Terrain::Reload()
{
   mClearFlag = true;
}

/**
 * Gets the color corresponding to the specified height.
 *
 * @param height the height value to map
 * @return the corresponding color
 */
osg::Vec3 Terrain::HeightColorMap::GetColor(float height)
{
   osg::Vec3 color;
   
   if(size() >= 2)
   {
      iterator c1, c2 = upper_bound(height);
      
      if(c2 == begin())
      {
         c1 = c2;
         
         c2++;
      }
      else if(c2 == end())
      {
         c2--;
         
         c1 = c2;
         
         c1--;
      }
      else
      {
         c1 = c2;
         
         c1--;
      }
      
      float t = (height-(*c1).first)/((*c2).first-(*c1).first);
      
      color = (*c1).second + ((*c2).second-(*c1).second)*t;
   }
   else
   {
      Notify(WARN, "Terrain::HeightColorMap: Must have at least two entries");
   }
   
   return color;
}
            
/**
 * Sets the color map for height values at or below sea level.
 *
 * @param hcm the new height color map
 */
void Terrain::SetLowerHeightColorMap(const HeightColorMap& hcm)
{
   mLowerHeightColorMap = hcm;
}

/**
 * Returns the color map for height values at or below sea level.
 *
 * @return the current color map
 */
const Terrain::HeightColorMap& Terrain::GetLowerHeightColorMap()
{
   return mLowerHeightColorMap;
}

/**
 * Sets the color map for height values above sea level.
 *
 * @param hcm the new height color map
 */
void Terrain::SetUpperHeightColorMap(const Terrain::HeightColorMap& hcm)
{
   mUpperHeightColorMap = hcm;
}

/**
 * Returns the color map for height values above sea level.
 *
 * @return the current color map
 */
const Terrain::HeightColorMap& Terrain::GetUpperHeightColorMap()
{
   return mUpperHeightColorMap;
}
         
/**
 * Returns this object's OpenSceneGraph node.
 *
 * @return the OpenSceneGraph node
 */
osg::Node* Terrain::GetOSGNode()
{
   return mNode.get();
}

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
void Terrain::GeocentricToGeodetic(double x, double y, double z,
                                   double* latitude, double* longitude, 
                                   double* elevation)
{
   double p = sqrt(x*x + y*y),
          a = semiMajorAxis,
          f = 1.0/flatteningReciprocal,
          b = a - a*f,
          theta = atan( (z*a)/(p*b) ),
          epsqu = (a*a - b*b)/(b*b),
          esqu = 2.0*f - f*f;
                   
   *latitude = atan(
      (z + epsqu * b * pow(sin(theta), 3)) /
      (p - esqu * a * pow(cos(theta), 3))
   );
                
   *longitude = atan2(y, x);
                
   *elevation = p/cos(*latitude) - 
                a/sqrt(1.0-esqu*pow(sin(*latitude), 2.0));
        
   *latitude *= osg::RadiansToDegrees(1.0f);
   *longitude *= osg::RadiansToDegrees(1.0f);
}

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
void Terrain::GeodeticToGeocentric(double latitude, double longitude, double elevation,
                                   double* x, double* y, double* z)
{
   double rlatitude = osg::DegreesToRadians(latitude),
          rlongitude = osg::DegreesToRadians(longitude),
          a = semiMajorAxis,
          f = 1.0/flatteningReciprocal,
          esqu = 2.0*f - f*f,
          n = a/sqrt(1.0-esqu*pow(sin(rlatitude), 2.0));
                   
   *x = (n + elevation)*cos(rlatitude)*cos(rlongitude);
            
   *y = (n + elevation)*cos(rlatitude)*sin(rlongitude);
            
   *z = (n*(1.0-esqu) + elevation)*sin(rlatitude);
}

/**
 * Converts a point in the local coordinate system to geodetic
 * coordinates.
 *
 * @param pt the local point to convert
 * @param latitude the location in which to store the latitude
 * @param longitude the location in which to store the longitude
 * @param elevation the location in which to store the elevation
 */
void Terrain::LocalToGeodetic(const osg::Vec3& pt,
                              double* latitude,
                              double* longitude,
                              double* elevation)
{
   sgdVec3 refPos;
   
   GeodeticToGeocentric(
      mOriginLatitude, mOriginLongitude, mOriginElevation,
      refPos, refPos + 1, refPos + 2
   );
   
   sgdVec3 offset = { pt[0], pt[1], pt[2] },
           xVec = { 1, 0, 0 },
           zVec = { 0, 0, 1 };
   
   sgdMat4 mat;
   
   sgdMakeRotMat4(mat, 90.0 - mOriginLatitude, xVec);
   
   sgdXformVec3(offset, mat);
   
   sgdMakeRotMat4(mat, mOriginLongitude + 90.0, zVec);
   
   sgdXformVec3(offset, mat);
   
   GeocentricToGeodetic(
      refPos[0] + offset[0], refPos[1] + offset[1], refPos[2] + offset[2],
      latitude, longitude, elevation
   );
}

/**
 * Converts a set of geodetic coordinates to a point in the local
 * coordinate system.
 *
 * @param latitude the latitude of the location to convert
 * @param longitude the longitude of the location to convert
 * @param elevation the elevation of the location to convert
 * @return the converted point
 */
osg::Vec3 Terrain::GeodeticToLocal(double latitude,
                                   double longitude,
                                   double elevation)
{
   sgdVec3 pos;
   
   GeodeticToGeocentric(
      latitude, longitude, elevation,
      pos, pos + 1, pos + 2   
   );
   
   sgdVec3 xVec = { 1, 0, 0 },
           zVec = { 0, 0, 1 },
           offset;
   
   sgdSubVec3(offset, pos, mGeocentricOrigin);
   
   sgdMat4 mat;
   
   sgdMakeRotMat4(mat, -mOriginLongitude - 90.0, zVec);
   
   sgdXformVec3(offset, mat);
   
   sgdMakeRotMat4(mat, mOriginLatitude - 90.0, xVec);
   
   sgdXformVec3(offset, mat);
   
   return osg::Vec3(offset[0], offset[1], offset[2]);
}

/**
 * Makes a base texture for the specified heightfield by mapping heights to
 * colors.
 *
 * @param hf the heightfield to read
 * @return the newly created base texture
 */
osg::Image* Terrain::MakeBaseTexture(osg::HeightField* hf)
{
   osg::Image* image = new osg::Image;
   
   int width = hf->getNumColumns(),
       height = hf->getNumRows();
   
   image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   
   unsigned char* ptr = (unsigned char*)image->data();
   
   float heightVal;
   osg::Vec3 color;
   
   for(int y=0;y<height;y++)
   {
      for(int x=0;x<width;x++)
      {
         heightVal = hf->getHeight(x, y);
         
         if(heightVal > 0.0f)
         {
            color = mUpperHeightColorMap.GetColor(heightVal);
         }
         else
         {
            color = mLowerHeightColorMap.GetColor(heightVal);
         }
         
         *(ptr++) = (unsigned char)(color[0]*255);
         *(ptr++) = (unsigned char)(color[1]*255);
         *(ptr++) = (unsigned char)(color[2]*255);
      }
   }
   
   //image->computeMipMaps();
   
   return image;
}

/**
 * The detail texture image.
 */
RefPtr<osg::Image> Terrain::mDetailTextureImage;

/**
 * Creates and returns the detail texture image.
 *
 * @return the newly created detail texture image
 */
static osg::Image* makeDetailTextureImage()
{
   osg::Image* image = new osg::Image;
   
   image->allocateImage(256, 256, 1, GL_RGB, GL_UNSIGNED_BYTE);
   
   unsigned char* ptr = (unsigned char*)image->data();
   
   sgPerlinNoise_2D texNoise;
   
   for(int y=0;y<256;y++)
   {
      for(int x=0;x<256;x++)
      {
         float val = 0.7f + texNoise.getNoise(x*0.1f, y*0.1f)*0.3f;
         
         *(ptr++) = (unsigned char)(val*255);
         *(ptr++) = (unsigned char)(val*255);
         *(ptr++) = (unsigned char)(val*255);
      }
   }
   
   //image->computeMipMaps();
   
   return image;
}

/**
 * Loads a single terrain segment.
 *
 * @param latitude the latitude of the segment to load
 * @param longitude the longitude of the segment to load
 */
void Terrain::LoadSegment(int latitude, int longitude)
{
   Segment coord(latitude, longitude);
   
   if(mLoadedSegments.count(coord) > 0)
   {
      return;
   }
   else
   {
      mLoadedSegments.insert(coord);
   }
   
   char latString[20], longString[20];
        
   sprintf(longString, "%c%d", longitude < 0 ? 'w' : 'e', osg::absolute(longitude));
   sprintf(latString, "%c%d", latitude < 0 ? 's' : 'n', osg::absolute(latitude));
   
   for(int i=5;i>=0;i--)
   {
      char dtedName[64];
      
      sprintf(dtedName, "%s/%s.dt%d", longString, latString, i);
      
      string path = osgDB::findFileInPath(
         dtedName, 
         mDTEDPaths, 
         osgDB::CASE_INSENSITIVE
      );
      
      if(path != "")
      {
         osgDB::ReaderWriter* gdalReader = 
            osgDB::Registry::instance()->getReaderWriterForExtension("gdal");
      
         osgDB::ReaderWriter::ReadResult rr =
            gdalReader->readHeightField(path);
         
         if(rr.success())
         {
            osgTerrain::HeightFieldNode* hfn = new osgTerrain::HeightFieldNode;
            
            float centerLat = latitude+0.5f,
                  centerLong = longitude+0.5f;
                  
            osg::Quat rot(osg::DegreesToRadians(90-centerLat), osg::X_AXIS);
            
            rot *= osg::Quat(osg::DegreesToRadians(centerLong+90), osg::Z_AXIS);
            
            rot *= osg::Quat(osg::DegreesToRadians(-mOriginLongitude - 90), osg::Z_AXIS);
            
            rot *= osg::Quat(osg::DegreesToRadians(mOriginLatitude - 90), osg::X_AXIS);
            
            rr.getHeightField()->setRotation(rot);
            
            rr.getHeightField()->setXInterval(
               (osg::DegreesToRadians(semiMajorAxis))/(rr.getHeightField()->getNumColumns()-1)
            );
            
            rr.getHeightField()->setYInterval(
               (osg::DegreesToRadians(semiMajorAxis))/(rr.getHeightField()->getNumRows()-1)
            );
            
            rr.getHeightField()->setOrigin(
               GeodeticToLocal(latitude, longitude, 0.0)
            );
            
            hfn->setHeightField(rr.getHeightField());
            
            osg::Image* base;
            
            char imageName[64];
            
            sprintf(imageName, "%s_%s_%d.base.color.bmp", longString, latString, i);
      
            string imagePath = osgDB::findFileInDirectory(imageName, mCachePath);
         
            if(imagePath != "")
            {
               base = osgDB::readImageFile(imagePath);
            }
            else
            {
               base = MakeBaseTexture(rr.getHeightField());
                
               base->ensureValidSizeForTexturing(4096);
      
               if(mCachePath != "")
               {
                  ostringstream imagePath;
                  
                  imagePath << mCachePath << "/" << imageName;
                  
                  osgDB::writeImageFile(*base, imagePath.str());
               }
            }
            
            hfn->setBaseTextureImage(base);
            
            if(mDetailTextureImage == NULL)
            {
               mDetailTextureImage = makeDetailTextureImage();
            }
            
            hfn->setDetailTextureImage(mDetailTextureImage.get());
            
            hfn->setRenderer(
               new SimpleHeightFieldRenderer(this, latitude, longitude, i)
            );
            
            osg::LOD* lod = new osg::LOD;
            
            lod->addChild(hfn, 0.0f, mLoadDistance*10);
         
            mNode->addChild(lod);
         
            Notify(NOTICE, "Terrain: Loaded %s", path.c_str());
            
            return;
         }
         else
         {
            Notify(WARN, "Terrain: Can't load %s", path.c_str());
         }
      }
   }
}


/**
 * Constructor.
 *
 * @param terrain a reference to the creating Terrain object
 * @param latitude the latitude of the segment
 * @param longitude the longitude of the segment
 * @param level the DTED level of the segment
 */
SimpleHeightFieldRenderer::SimpleHeightFieldRenderer(Terrain* terrain,
                                                     int latitude,
                                                     int longitude,
                                                     int level)
   : mTerrain(terrain),
     mLatitude(latitude),
     mLongitude(longitude),
     mLevel(level)
{
   mGeode = new osg::Geode;
   
   osg::StateSet* ss = mGeode->getOrCreateStateSet();
   
   ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
   
   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

/**
 * Copy constructor.
 */
SimpleHeightFieldRenderer::SimpleHeightFieldRenderer(const SimpleHeightFieldRenderer& shfr,
                                                     const osg::CopyOp& copyop)
   : HeightFieldRenderer(shfr, copyop),
     mGeode(shfr.mGeode)
{}

/**
 * Destructor.
 */
SimpleHeightFieldRenderer::~SimpleHeightFieldRenderer()
{}

/**
 * Creates the heightfield Drawable.
 */
void SimpleHeightFieldRenderer::CreateHeightFieldDrawable()
{
   osg::HeightField* hf = getHeightFieldNode()->getHeightField();
   
   osg::Geometry* geom = new osg::Geometry;
   
   osg::Vec3Array* vertices = new osg::Vec3Array;
   
   geom->setVertexArray(vertices);
   
   osg::Vec2Array* baseTexCoords = new osg::Vec2Array;
   
   geom->setTexCoordArray(0, baseTexCoords);
   
   geom->setTexCoordArray(1, baseTexCoords);
   
   osg::Vec2Array* detailTexCoords = new osg::Vec2Array;
   
   geom->setTexCoordArray(2, detailTexCoords);
   
   float ds = 1.0f/hf->getNumColumns(),
         dt = 1.0f/hf->getNumRows(),
         s, t = dt/2;

   double latitude = mLatitude, longitude,
          dLat = 1.0f/(hf->getNumColumns()-1),
          dLong = 1.0f/(hf->getNumRows()-1);
   
   unsigned int i, j, height = hf->getNumRows(), width = hf->getNumColumns();
   
   for(i=0;i<height;i++)
   {
      s = ds/2;
      longitude = mLongitude;
      
      for(j=0;j<width;j++)
      {
         vertices->push_back(
            mTerrain->GeodeticToLocal(
               latitude,
               longitude,
               hf->getHeight(j, i)
            )
         );
         
         baseTexCoords->push_back(osg::Vec2(s, t));
         
         detailTexCoords->push_back(osg::Vec2(s*1000, t*1000));
         
         s += ds;
         longitude += dLong;
      }
      
      t += dt;
      latitude += dLat;
   }
   
   GLuint* buf = new GLuint[width*2];
   GLuint* ptr;
   
   for(i=0;i<height-1;i++)
   {
      ptr = buf;
      
      for(j=0;j<width;j++)
      {
         *(ptr++) = i*width + j + width;
         *(ptr++) = i*width + j;
      }
      
      geom->addPrimitiveSet(
         new osg::DrawElementsUInt(
            GL_TRIANGLE_STRIP,
            width*2,
            buf
         )
      );
   }
   
   delete[] buf;
   
   char latString[20], longString[20], imageName[64];
        
   sprintf(longString, "%c%d", mLongitude < 0 ? 'w' : 'e', osg::absolute(mLongitude));
   sprintf(latString, "%c%d", mLatitude < 0 ? 's' : 'n', osg::absolute(mLatitude));
   
   sprintf(imageName, "%s_%s_%d.base.gradient.bmp", longString, latString, mLevel);
   
   osg::Image* normalMap;
   
   string imagePath = osgDB::findFileInDirectory(imageName, mTerrain->mCachePath);
         
   if(imagePath != "")
   {
      normalMap = osgDB::readImageFile(imagePath);
   }
   else
   {
      normalMap = new osg::Image;
      
      normalMap->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   
      unsigned char* ip = (unsigned char*)normalMap->data();
   
      osg::Vec3 v1, v2, normal;
   
      for(i=0;i<height;i++)
      {
         for(j=0;j<width;j++)
         {      
            if(j==0)
            {
               v1 = (*vertices)[i*width+j+1] - (*vertices)[i*width+j];
            }
            else if(j==width-1)
            {
               v1 = (*vertices)[i*width+j] - (*vertices)[i*width+j-1];
            }
            else
            {
               v1 = (*vertices)[i*width+j+1] - (*vertices)[i*width+j-1];
            }
            
            if(i==0)
            {
               v2 = (*vertices)[(i+1)*width+j] - (*vertices)[i*width+j];
            }
            else if(i==height-1)
            {
               v2 = (*vertices)[i*width+j] - (*vertices)[(i-1)*width+j];
            }
            else
            {
               v2 = (*vertices)[(i+1)*width+j] - (*vertices)[(i-1)*width+j];
            }
            
            normal = v1^v2;
            
            normal.normalize();
            
            *(ip++) = (unsigned char)((normal.x()+1.0f)*0.5f*255);
            *(ip++) = (unsigned char)((normal.y()+1.0f)*0.5f*255);
            *(ip++) = (unsigned char)((normal.z()+1.0f)*0.5f*255);
         }
      }
      
      normalMap->ensureValidSizeForTexturing(4096);
      
      if(mTerrain->mCachePath != "")
      {
         ostringstream imagePath;
                  
         imagePath << mTerrain->mCachePath << "/" << imageName;
                  
         osgDB::writeImageFile(*normalMap, imagePath.str());
      }
   }
   
   getHeightFieldNode()->setNormalMapImage(normalMap);

   mGeode->addDrawable(geom);
}
        
/**
 * Initializes the renderer.
 */ 
void SimpleHeightFieldRenderer::initialize()
{
   if(!getHeightFieldNode()->containsNode(mGeode.get()))
   {
      getHeightFieldNode()->addChild(mGeode.get());
   }
   
   if(mGeode->getNumDrawables() > 0)
   {
      mGeode->removeDrawable(0, mGeode->getNumDrawables());
   }
   
   if(getHeightFieldNode()->getHeightField() != NULL)
   {
      CreateHeightFieldDrawable();

      osg::StateSet* ss = mGeode->getOrCreateStateSet();
      
      osg::Texture2D* normalMap = new osg::Texture2D(
         getHeightFieldNode()->getNormalMapImage()
      );

      normalMap->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
      normalMap->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);
      
      ss->setTextureAttributeAndModes(0, normalMap);
      
      osg::TexEnvCombine* tec = new osg::TexEnvCombine;
      
      tec->setCombine_RGB(osg::TexEnvCombine::DOT3_RGB);
      
      tec->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
      
      tec->setSource1_RGB(osg::TexEnvCombine::CONSTANT);
      
      tec->setConstantColor(osg::Vec4(0.5, 0.5, 1.0, 0.0));
      
      ss->setTextureAttributeAndModes(0, tec);
      
      osg::Texture2D* baseTexture = new osg::Texture2D(
         getHeightFieldNode()->getBaseTextureImage()
      );

      baseTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
      baseTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);
      
      ss->setTextureAttributeAndModes(1, baseTexture);
      
      tec = new osg::TexEnvCombine;
      
      tec->setCombine_RGB(osg::TexEnvCombine::MODULATE);
      
      tec->setSource0_RGB(osg::TexEnvCombine::PREVIOUS);
      
      tec->setSource1_RGB(osg::TexEnvCombine::TEXTURE);
      
      ss->setTextureAttributeAndModes(1, tec);
      
      osg::Texture2D* detailTexture = new osg::Texture2D(
         getHeightFieldNode()->getDetailTextureImage()
      );
      
      detailTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      detailTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      
      ss->setTextureAttributeAndModes(2, detailTexture);
      
      tec = new osg::TexEnvCombine;
      
      tec->setCombine_RGB(osg::TexEnvCombine::MODULATE);
      
      tec->setSource0_RGB(osg::TexEnvCombine::PREVIOUS);
      
      tec->setSource1_RGB(osg::TexEnvCombine::TEXTURE);
      
      ss->setTextureAttributeAndModes(2, tec);
   }
}

/**
 * Notifies the renderer that the heightfield has been modified.
 */
void SimpleHeightFieldRenderer::heightFieldHasBeenModified()
{}

/**
 * Update callback.
 *
 * @param nv the update visitor
 */
void SimpleHeightFieldRenderer::update(osgUtil::UpdateVisitor* nv)
{
   mGeode->accept(*nv);
}

/**
 * Cull callback.
 *
 * @param cv the cull visitor
 */
void SimpleHeightFieldRenderer::cull(osgUtil::CullVisitor* cv)
{
   mGeode->accept(*cv);
}
