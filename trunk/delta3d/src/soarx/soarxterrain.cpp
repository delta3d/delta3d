// soarxterrain.cpp: Implementation of the SOARXTerrain class.
//
//////////////////////////////////////////////////////////////////////

#include "soarxterrain.h"

#include "notify.h"

#include <iostream>

#include <osg/Geode>

using namespace dtCore;
using namespace dtSOARX;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(SOARXTerrain)


/**
 * The ODE SOARXTerrain class identifier.
 */
static int dSOARXTerrainClass = 0;

/**
 * The length of the semi-major axis, in meters (WGS 84).
 */
const double semiMajorAxis = 6378137.0;

/**
 * The reciprocal of the flattening parameter (WGS 84).
 */
const double flatteningReciprocal = 298.257223563;


/**
 * The fragment shader.
 */
const char* fragmentShader =
   "uniform sampler2D detailGradient;"
   "uniform sampler2D detailScale;"
   "uniform sampler2D baseGradient;"
   "uniform sampler2D baseColor;"
   "void main(void)"
   "{"
   "  vec4 detail = texture2D(detailGradient, vec2(gl_TexCoord[0]));"
   "  vec4 scale = texture2D(detailScale, vec2(gl_TexCoord[1]));"
   "  vec4 base = texture2D(baseGradient, vec2(gl_TexCoord[1]));"
   "  vec4 combined = (detail - 0.5) * scale + (base - 0.5);"
   "  vec3 normal = normalize(vec3(combined.y, 0.5, combined.z));"
   "  gl_FragColor = texture2D(baseColor, vec2(gl_TexCoord[1]));"
   "  gl_FragColor.rgb *= dot(normal, vec3(gl_LightSource[0].position));"
   "}";


/**
 * The terrain callback class.  Loads terrain segments
 * around viewer.
 */
class dtSOARX::SOARXTerrainCallback : public osg::NodeCallback
{
   public:
   
      /**
       * Constructor.
       *
       * @param terrain the owning SOARXTerrain object
       */
      SOARXTerrainCallback(SOARXTerrain* terrain)
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
   
            mTerrain->mSegmentDrawableMap.clear();
            
            mTerrain->mClearFlag = false;
         }
         
         osg::Vec3 eyepoint = nv->getEyePoint();
         
         double latitude = (eyepoint[1]/semiMajorAxis)*SG_RADIANS_TO_DEGREES + mTerrain->mOriginLatitude,
                longitude = (eyepoint[0]/semiMajorAxis)*SG_RADIANS_TO_DEGREES + mTerrain->mOriginLongitude;
         
         float latSize = (mTerrain->GetLoadDistance()/semiMajorAxis)*SG_RADIANS_TO_DEGREES,
               longSize = latSize;
         
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
       * The owning SOARXTerrain object.
       */
      SOARXTerrain* mTerrain;
};


/**
 * A transformation callback.
 */
class TransformCallback : public osg::NodeCallback
{
   public:

      TransformCallback(SOARXTerrain* terrain)
      {
         mTerrain = terrain;
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      { 
         Transform transform;

         mTerrain->GetTransform(&transform);
         
         sgMat4 matrix;

         transform.Get(matrix);
         
         osg::MatrixTransform* mt = 
            (osg::MatrixTransform*)mTerrain->GetOSGNode();
         
         mt->setMatrix(
            osg::Matrix(
               matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
               matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
               matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
               matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]
            )
         );
         
         traverse(node, nv);
      }
      
   private:
      
      SOARXTerrain* mTerrain;
};


/**
 * Constructor.
 *
 * @param name the instance name
 */
SOARXTerrain::SOARXTerrain(string name)
   : mLoadDistance(10000.0f),
     mThreshold(2.0f),
     mDetailMultiplier(7.0f),
     mMaxTextureSize(4096)
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
   
   mNode = new osg::MatrixTransform;

   mNode->setUpdateCallback(
      new TransformCallback(this)
   );
   
   mNode->setCullCallback(
      new SOARXTerrainCallback(this)
   );
   
   mNode->setCullingActive(false);
   
   mProgramObject = new osgGL2::ProgramObject;

   mProgramObject->addShader(
      new osgGL2::ShaderObject(osgGL2::ShaderObject::FRAGMENT, fragmentShader)
   );
   
   mProgramObject->setSampler("detailGradient", 0);
   mProgramObject->setSampler("detailScale", 1);
   mProgramObject->setSampler("baseGradient", 2);
   mProgramObject->setSampler("baseColor", 3);
   
   if(dSOARXTerrainClass == 0)
   {
      dGeomClass gc;
      
      gc.bytes = sizeof(SOARXTerrain*);
      gc.collider = GetColliderFn;
      gc.aabb = GetAABB;
      gc.aabb_test = AABBTest;
      gc.dtor = 0;
      
      dSOARXTerrainClass = dCreateGeomClass(&gc);
   }
   
   dGeomID geom = dCreateGeom(dSOARXTerrainClass);
   
   *(SOARXTerrain**)dGeomGetClassData(geom) = this;
   
   SetCollisionGeom(geom);
   
   RegisterInstance(this);
}

/**
 * Destructor.
 */
SOARXTerrain::~SOARXTerrain()
{
   DeregisterInstance(this);
}

/**
 * Loads the specified configuration file.
 *
 * @param filename the name of the configuration file to load
 */
void SOARXTerrain::LoadConfiguration(string filename)
{
   TiXmlDocument configuration;
   
   if(configuration.LoadFile(filename.c_str()))
   {
      ParseConfiguration(configuration.RootElement());
   }
}
         
/**
 * Parses the specified XML configuration element.
 *
 * @param configElement the configuration element to parse
 */
void SOARXTerrain::ParseConfiguration(TiXmlElement* configElement)
{
   const char* str;
   
   if((str = configElement->Attribute("cachePath")) != NULL)
   {
      SetCachePath(str);
   }
   
   for(TiXmlElement* element = configElement->FirstChildElement();
       element != NULL;
       element = element->NextSiblingElement())
   {
      if(!strcmp(element->Value(), "GeoOrigin"))
      {
         double latitude = 0.0, longitude = 0.0, elevation = 0.0;
         
         if((str = element->Attribute("latitude")) != NULL)
         {
            sscanf(str, "%lg", &latitude);
         }
         if((str = element->Attribute("longitude")) != NULL)
         {
            sscanf(str, "%lg", &longitude);
         }
         if((str = element->Attribute("elevation")) != NULL)
         {
            sscanf(str, "%lg", &elevation);
         }
         
         SetGeoOrigin(latitude, longitude, elevation);
      }
      else if(!strcmp(element->Value(), "DTED"))
      {
         if((str = element->Attribute("path")) != NULL)
         {
            AddDTEDPath(str);
         }
      }
      else if(!strcmp(element->Value(), "GeospecificImage"))
      {
         if((str = element->Attribute("filename")) != NULL)
         {
            LoadGeospecificImage(str);
         }
      }
      else if(!strcmp(element->Value(), "Roads"))
      {
         string filename, query("SELECT * FROM *"), texture;
         float width = 7.0f, sScale = 1.0f, tScale = 1.0f;
         
         if((str = element->Attribute("filename")) != NULL)
         {
            filename = str;
         }
         if((str = element->Attribute("query")) != NULL)
         {
            query = str;
         }
         if((str = element->Attribute("width")) != NULL)
         {
            sscanf(str, "%g", &width);
         }
         if((str = element->Attribute("texture")) != NULL)
         {
            texture = str;
         }
         if((str = element->Attribute("sScale")) != NULL)
         {
            sscanf(str, "%g", &sScale);
         }
         if((str = element->Attribute("tScale")) != NULL)
         {
            sscanf(str, "%g", &tScale);
         }
         
         LoadRoads(filename, query, width, texture, sScale, tScale);
      }
   }
}
         
/**
 * Sets the location of the origin in geographic coordinates.
 *
 * @param latitude the latitude of the origin
 * @param longitude the longitude of the origin
 * @param elevation the elevation of the origin
 */
void SOARXTerrain::SetGeoOrigin(double latitude, double longitude, double elevation)
{
   mOriginLatitude = latitude;
   mOriginLongitude = longitude;
   mOriginElevation = elevation;
   
   mClearFlag = true;
}

/**
 * Retrieves the location of the origin in geographic coordinates.
 *
 * @param latitude the location in which to store the latitude
 * @param longitude the location in which to store the longitude
 * @param elevation the location in which to store the elevation
 */
void SOARXTerrain::GetGeoOrigin(double* latitude, double* longitude, double* elevation)
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
void SOARXTerrain::SetLoadDistance(float loadDistance)
{
   mLoadDistance = loadDistance;
}

/**
 * Returns the load distance.
 *
 * @return the current load distance
 */
float SOARXTerrain::GetLoadDistance()
{
   return mLoadDistance;
}

/**
 * Adds a path from which to load DTED heightfield data.  Paths are
 * searched in the order that they are added.
 *
 * @param path the path to add
 */
void SOARXTerrain::AddDTEDPath(std::string path)
{
   mDTEDPaths.push_back(path);
}

/**
 * Removes a DTED path.
 *
 * @param path the path to remove
 */
void SOARXTerrain::RemoveDTEDPath(string path)
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
unsigned int SOARXTerrain::GetNumDTEDPaths()
{
   return mDTEDPaths.size();
}

/**
 * Returns the DTED path at the specified index.
 *
 * @param index the index of the path to retrieve
 */
string SOARXTerrain::GetDTEDPath(unsigned int index)
{
   return mDTEDPaths[index];
}

/**
 * Sets the path of the terrain cache.  If the path is unset,
 * terrain data will not be cached.
 *
 * @param path the new path
 */
void SOARXTerrain::SetCachePath(string path)
{
   mCachePath = path;
}

/**
 * Returns the path of the terrain cache.
 *
 * @return the current path
 */
string SOARXTerrain::GetCachePath()
{
   return mCachePath;
}

/**
 * Reloads the terrain around the eyepoint.
 */
void SOARXTerrain::Reload()
{
   mClearFlag = true;
}

/**
 * Gets the color corresponding to the specified height.
 *
 * @param height the height value to map
 * @return the corresponding color
 */
osg::Vec3 SOARXTerrain::HeightColorMap::GetColor(float height)
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
      Notify(WARN, "SOARXTerrain::HeightColorMap: Must have at least two entries");
   }
   
   return color;
}
            
/**
 * Sets the color map for height values at or below sea level.
 *
 * @param hcm the new height color map
 */
void SOARXTerrain::SetLowerHeightColorMap(const HeightColorMap& hcm)
{
   mLowerHeightColorMap = hcm;
}

/**
 * Returns the color map for height values at or below sea level.
 *
 * @return the current color map
 */
const SOARXTerrain::HeightColorMap& SOARXTerrain::GetLowerHeightColorMap()
{
   return mLowerHeightColorMap;
}

/**
 * Sets the color map for height values above sea level.
 *
 * @param hcm the new height color map
 */
void SOARXTerrain::SetUpperHeightColorMap(const HeightColorMap& hcm)
{
   mUpperHeightColorMap = hcm;
}

/**
 * Returns the color map for height values above sea level.
 *
 * @return the current color map
 */
const SOARXTerrain::HeightColorMap& SOARXTerrain::GetUpperHeightColorMap()
{
   return mUpperHeightColorMap;
}

/**
 * Sets the maximum texture size, which should be a power of two
 * (default is 2048).
 *
 * @param maxTextureSize the new maximum texture size
 */
void SOARXTerrain::SetMaxTextureSize(int maxTextureSize)
{
   mMaxTextureSize = maxTextureSize;
}

/**
 * Returns the maximum texture size. 
 *
 * @return the maximum texture size
 */
int SOARXTerrain::GetMaxTextureSize()
{
   return mMaxTextureSize;
}
         
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
void SOARXTerrain::LoadGeospecificImage(string filename, const double* geoTransform)
{
   GeospecificImage gs;
   
   int width, height;
   
   if(geoTransform == NULL)
   {
      GDALAllRegister();
      
      GDALDataset* ds = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);
	  
      if(ds != NULL)
      {
         OGRSpatialReference sr(ds->GetProjectionRef()), wgs84;
         
         wgs84.SetWellKnownGeogCS("WGS84");
         
         if(!sr.IsSame(&wgs84))
         {
            Notify(NOTICE, "SOARXTerrain: Warping %s to WGS84", filename.c_str());

            void* transformArg;            
            double newGeoTransform[6];
            int newWidth = 0, newHeight = 0;
            
            char* wgs84wkt;
            
            wgs84.exportToWkt(&wgs84wkt);
          
            transformArg = GDALCreateGenImgProjTransformer(
               ds,
               ds->GetProjectionRef(), 
               NULL,
               wgs84wkt, 
               FALSE, 0, 1
            );
            
            GDALSuggestedWarpOutput(
               ds, 
               GDALGenImgProjTransform,
               transformArg, 
               newGeoTransform,
               &newWidth,
               &newHeight 
            );

            GDALDestroyGenImgProjTransformer(transformArg);

            GDALDataset* newDS = (GDALDataset*)GDALCreate(
               GDALGetDriverByName("GTiff"),
               "temp.tif",
               newWidth,
               newHeight,
               ds->GetRasterCount(),
               ds->GetRasterBand(1)->GetRasterDataType(),
               NULL
            );

			newDS->SetProjection(wgs84wkt);
			newDS->SetGeoTransform(newGeoTransform);

			GDALColorTableH hCT;
			hCT = GDALGetRasterColorTable( ds->GetRasterBand(1));
			if( hCT != NULL )
			{
				GDALSetRasterColorTable( newDS->GetRasterBand(1), hCT );
			}

            GDALWarpOptions* warpOptions = GDALCreateWarpOptions();

            warpOptions->hSrcDS = ds;
            warpOptions->hDstDS = newDS;

            warpOptions->nBandCount = ds->GetRasterCount();

            warpOptions->panSrcBands = 
               (int*)CPLMalloc(sizeof(int)*warpOptions->nBandCount);
               
            warpOptions->panDstBands = 
               (int*)CPLMalloc(sizeof(int)*warpOptions->nBandCount);
            
            for(int i=0;i<warpOptions->nBandCount;i++)
            {
               warpOptions->panSrcBands[i] = warpOptions->panDstBands[i] = i+1;
            }
            
            warpOptions->pTransformerArg = 
               GDALCreateGenImgProjTransformer(
                  ds, 
                  ds->GetProjectionRef(), 
                  newDS,
                  newDS->GetProjectionRef(), 
                  FALSE, 0, 1
               );
            
            warpOptions->pfnTransformer = GDALGenImgProjTransform;

            warpOptions->pfnProgress = GDALTermProgress;   

            GDALWarpOperation warpOperation;

            warpOperation.Initialize(warpOptions);
    
            warpOperation.ChunkAndWarpImage(
               0, 0, newWidth, newHeight
            );

            GDALDestroyGenImgProjTransformer(warpOptions->pTransformerArg);
            GDALDestroyWarpOptions(warpOptions);

            GDALClose(ds);
            
            ds = newDS;
         }
         
         ds->GetGeoTransform(gs.mGeoTransform);
      
         width = ds->GetRasterXSize();
         height = ds->GetRasterYSize();
      
         int bands = ds->GetRasterCount();
         
		 if(bands == 1)
         {
			 gs.mImage = new osg::Image;

			GDALColorTableH hCT = NULL;
			hCT = GDALGetRasterColorTable( ds->GetRasterBand(1));
			if( hCT != NULL )			//color palette exists
			{
				gs.mImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

				//set RGB values to color map indices
				for(int i=0;i<3;i++)
				{
					ds->GetRasterBand(1)->RasterIO(
						GF_Read, 0, 0, width, height,
						gs.mImage->data()+i, width, height, GDT_Byte, 3, 0
						);
				}
			
				//get point to image data
				unsigned char* ptr = (unsigned char*)gs.mImage->data();

				//get color table info
				GDALColorTableH	hTable;
				hTable = GDALGetRasterColorTable( ds->GetRasterBand(1) );

				GDALColorEntry	sEntry;

				for(int y=0;y<height;y++)
				{
					for(int x=0;x<width;x++)
					{
						GDALGetColorEntryAsRGB( hTable,*(ptr), &sEntry );
						*(ptr++) = sEntry.c1;
						*(ptr++) = sEntry.c2;
						*(ptr++) = sEntry.c3;
					}
				}

			}
			else   // no color palette exists -> grayscale
			{
				gs.mImage->allocateImage(width, height, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE);
				ds->GetRasterBand(1)->RasterIO(
					GF_Read, 0, 0, width, height,
					gs.mImage->data(), width, height, GDT_Byte, 0, 0
					);
			}


         }
         else if(bands == 3)
         {
            gs.mImage = new osg::Image;
      
            gs.mImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
      
            for(int i=0;i<3;i++)
            {
               ds->GetRasterBand(i+1)->RasterIO(
                  GF_Read, 0, 0, width, height,
                  gs.mImage->data()+i, width, height, GDT_Byte, 3, 0
               );
            }
         }
         else
         {
            Notify(WARN, "SOARXTerrain: %s has %d raster bands", filename.c_str(), bands);
         }
         
         delete ds;
      }
   }
   else
   {
      gs.mImage = osgDB::readImageFile(filename);
      
      memcpy(gs.mGeoTransform, geoTransform, sizeof(double)*6);
   }
   
   if(gs.mImage.valid())
   {
      gs.mFilename = filename;
      
      gs.mMinLatitude = gs.mMinLongitude = 999;
      gs.mMaxLatitude = gs.mMaxLongitude = -999;
         
      for(int i=0;i<4;i++)
      {
         int x = (i&1) ? 0 : width-1,
             y = (i&2) ? 0 : height-1;
         
         int longitude = (int)floor(gs.mGeoTransform[0] + x*gs.mGeoTransform[1] + y*gs.mGeoTransform[2]),
             latitude = (int)floor(gs.mGeoTransform[3] + x*gs.mGeoTransform[4] + y*gs.mGeoTransform[5]);
           
         gs.mMinLatitude = min(latitude, gs.mMinLatitude);
         gs.mMinLongitude = min(longitude, gs.mMinLongitude);
         gs.mMaxLatitude = max(latitude, gs.mMaxLatitude);
         gs.mMaxLongitude = max(longitude, gs.mMaxLongitude);
      }

      double d = gs.mGeoTransform[1]*gs.mGeoTransform[5]-gs.mGeoTransform[2]*gs.mGeoTransform[4];
      
      gs.mInverseGeoTransform[0] = 
         (gs.mGeoTransform[2]*gs.mGeoTransform[3]-gs.mGeoTransform[5]*gs.mGeoTransform[0])/d;
      
      gs.mInverseGeoTransform[1] = gs.mGeoTransform[5]/d;
      
      gs.mInverseGeoTransform[2] = -gs.mGeoTransform[2]/d;
      
      gs.mInverseGeoTransform[3] = 
         (gs.mGeoTransform[4]*gs.mGeoTransform[0]-gs.mGeoTransform[1]*gs.mGeoTransform[3])/d;
         
      gs.mInverseGeoTransform[4] = -gs.mGeoTransform[4]/d;
      
      gs.mInverseGeoTransform[5] = gs.mGeoTransform[1]/d;
      
      mGeospecificImages.push_back(gs);
   }
   else
   {
      Notify(WARN, "SOARXTerrain: Can't load %s", filename.c_str());
   }
}

/**
 * Loads road data from the specified filename.
 *
 * @param filename the name of the vector data file to load
 * @param query a SQL query that selects road features from
 * the data file
 * @param width the width of the roads to create
 * @param texture the name of the texture to use, or "" for
 * none
 * @param sScale the texture s scale
 * @param tScale the texture t scale
 */
void SOARXTerrain::LoadRoads(string filename,
                             string query,
                             float width,
                             string texture,
                             float sScale,
                             float tScale)
{
   OGRRegisterAll();
   
   OGRDataSource* ds;
   
   if(mFilenameOGRDataSourceMap.count(filename) > 0)
   {
      ds = mFilenameOGRDataSourceMap[filename];
   }
   else
   {
      ds = OGRSFDriverRegistrar::Open(filename.c_str(), 0, NULL);
      
      if(ds != NULL)
      {
         mFilenameOGRDataSourceMap[filename] = ds;
      }
   }
   
   if(ds != NULL)
   {
      Roads roads;
   
      roads.mLayer = ds->ExecuteSQL(query.c_str(), NULL, NULL);
      
      if(roads.mLayer != NULL)
      {
         OGRSpatialReference wgs84;
         
         wgs84.SetWellKnownGeogCS("WGS84");
         
         if(roads.mLayer->GetSpatialRef() != NULL &&
            !wgs84.IsSame(roads.mLayer->GetSpatialRef()))
         {
            Notify(NOTICE, "SOARXTerrain: Transforming roads to WGS84...");
            
            OGRCoordinateTransformation* ct = 
               OGRCreateCoordinateTransformation(roads.mLayer->GetSpatialRef(), &wgs84);
               
            for(OGRFeature* feature = roads.mLayer->GetNextFeature();
                feature != NULL;
                feature = roads.mLayer->GetNextFeature())
            {
               OGRGeometry* geom = feature->GetGeometryRef();
            
               if(geom != NULL)
               {
                  geom->transform(ct);
               }
            }
         }
         
         OGREnvelope env;
         
         roads.mLayer->GetExtent(&env);
         
         roads.mMinLatitude = (int)floor(env.MinY);
         roads.mMaxLatitude = (int)floor(env.MaxY);
         roads.mMinLongitude = (int)floor(env.MinX);
         roads.mMaxLongitude = (int)floor(env.MaxX);
         
         osg::Image* image = osgDB::readImageFile(texture);
         
         roads.mTexture = new osg::Texture2D(image);
         
         roads.mTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
         roads.mTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
            
         roads.mWidth = width;
         roads.mSScale = sScale;
         roads.mTScale = tScale;
         
         mRoads.push_back(roads);
         
         return;
      }
   }
   
   Notify(WARN, "SOARXTerrain: Can't load %s", filename.c_str());
}
                 
/**
 * Returns this object's OpenSceneGraph node.
 *
 * @return the OpenSceneGraph node
 */
osg::Node* SOARXTerrain::GetOSGNode()
{
   return mNode.get();
}

/**
 * Sets the threshold parameter.
 *
 * @param threshold the new threshold
 */
void SOARXTerrain::SetThreshold(float threshold)
{
   mThreshold = threshold;
   
   for(map<Segment, SOARXDrawable*>::iterator it = mSegmentDrawableMap.begin();
       it != mSegmentDrawableMap.end();
       it++)
   {
      (*it).second->SetThreshold(mThreshold);
   }
}

/**
 * Returns the threshold parameter.
 *
 * @return the current threshold
 */
float SOARXTerrain::GetThreshold()
{
   return mThreshold;
}

/**
 * Sets the detail multiplier parameter.
 *
 * @param detailMultiplier the new detail multiplier
 */
void SOARXTerrain::SetDetailMultiplier(float detailMultiplier)
{
   mDetailMultiplier = detailMultiplier;
   
   for(map<Segment, SOARXDrawable*>::iterator it = mSegmentDrawableMap.begin();
       it != mSegmentDrawableMap.end();
       it++)
   {
      (*it).second->SetDetailMultiplier(mDetailMultiplier);
   }
}

/**
 * Returns the detail multiplier parameter.
 *
 * @return the current detail multiplier
 */
float SOARXTerrain::GetDetailMultiplier()
{
   return mDetailMultiplier;
}
         
/**
 * Determines the height of the terrain at the specified location.
 *
 * @param x the x coordinate to check
 * @param y the y coordinate to check
 * @return the height at the specified location
 */
float SOARXTerrain::GetHeight(float x, float y)
{
   int latitude = (int)floor((y/semiMajorAxis)*SG_RADIANS_TO_DEGREES + mOriginLatitude),
       longitude = (int)floor((x/semiMajorAxis)*SG_RADIANS_TO_DEGREES + mOriginLongitude);
   
   Segment segment(latitude, longitude);
   
   LoadSegment(latitude, longitude);

   return mSegmentDrawableMap[segment]->GetHeight(
      x - (longitude - mOriginLongitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
      y - (1 + latitude - mOriginLatitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis
   );
}

/**
 * Retrieves the normal of the terrain at the specified location.
 *
 * @param x the x coordinate to check
 * @param y the y coordinate to check
 * @param normal the location at which to store the normal
 */
void SOARXTerrain::GetNormal(float x, float y, sgVec3 normal)
{
   float z = GetHeight(x, y);
   
   sgVec3 v1 = { 0.1f, 0.0f, GetHeight(x + 0.1f, y) - z },
          v2 = { 0.0f, 0.1f, GetHeight(x, y + 0.1f) - z };
                   
   sgVectorProductVec3(normal, v1, v2);
            
   sgNormalizeVec3(normal);
}

/**
 * Makes the detail gradient texture map for the specified DTED level.
 *
 * @param level the DTED level to make the texture map for
 * @return the newly created image
 */
osg::Image* SOARXTerrain::MakeDetailGradient(int level)
{
   osg::Image* image = new osg::Image;
   
   int width = 1024, height = 1024;
   
   image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   
   unsigned char* ptr = (unsigned char*)image->data();
   
   int baseSize, embeddedSize;
   
   switch(level)
   {
      case 0:
         baseSize = 129;
         embeddedSize = 1 << (16 - 7);
         break;
         
      case 1:
         baseSize = 1025;
         embeddedSize = 1 << (16 - 10);
         break;
         
      case 2:
         baseSize = 4097;
         embeddedSize = 1 << (16 - 12);
         break;
   }
   
   float baseHorizontalResolution = (semiMajorAxis*SG_DEGREES_TO_RADIANS)/(baseSize-1);
   
   float scale = (128.0f*0.0012f)/(baseHorizontalResolution/embeddedSize);
   
   for(int y=0;y<height;y++)
   {
      for(int x=0;x<width;x++)
      {
         int h = TBuilder::CreateDetailMap(0, x, y, 0, 0, NULL),
             gx = h - TBuilder::CreateDetailMap(0, x+1, y, 0, 0, NULL),
             gy = h - TBuilder::CreateDetailMap(0, x, y+1, 0, 0, NULL);

         *(ptr++) = 0;
         *(ptr++) = (unsigned char)clamp(scale*gx + 128.0f, 0.0f, 255.0f);
         *(ptr++) = (unsigned char)clamp(scale*gy + 128.0f, 0.0f, 255.0f);
      }
   }
   
   return image;
}

/**
 * Makes the detail scale texture map for the specified DTED level.
 *
 * @param level the DTED level to make the texture map for
 * @return the newly created image
 */
osg::Image* SOARXTerrain::MakeDetailScale(int level)
{
   osg::Image* image = new osg::Image;
   
   int width, height;
   
   switch(level)
   {
      case 0:
         width = height = 128;
         break;
      
      case 1:
         width = height = 1024;
         break;
         
      case 2:
         width = height = 4096;
         break;
   }
   
   image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   
   unsigned char* ptr = (unsigned char*)image->data();
   
   for(int y=0;y<height;y++)
   {
      for(int x=0;x<width;x++)
      {
         unsigned char h = (unsigned char)(TBuilder::CreateScaleMap(0, x, y, 0, 0, NULL) >> 8);
         
         *(ptr++) = h;
         *(ptr++) = h;
         *(ptr++) = h;
      }
   }
   
   return image;
}

/**
 * Makes the base gradient texture map for the specified heightfield.
 *
 * @param hf the heightfield to process
 * @return the newly created image
 */
osg::Image* SOARXTerrain::MakeBaseGradient(osg::HeightField* hf)
{
   osg::Image* image = new osg::Image;
   
   int width = hf->getNumColumns()-1,
       height = hf->getNumRows()-1;
   
   image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   
   unsigned char* ptr = (unsigned char*)image->data();
   
   float scale = 128.0f/hf->getXInterval();
   
   for(int y=0;y<height;y++)
   {
      for(int x=0;x<width;x++)
      {
         float h = hf->getHeight(x, y),
               gx = h - hf->getHeight(x+1, y),
               gy = h - hf->getHeight(x, y+1);
         
         *(ptr++) = 0;
         *(ptr++) = (unsigned char)clamp(scale*gx + 128.0f, 0.0f, 255.0f);
         *(ptr++) = (unsigned char)clamp(scale*gy + 128.0f, 0.0f, 255.0f);
      }
   }
   
   image->ensureValidSizeForTexturing(mMaxTextureSize);
   
   return image;
}

/**
 * Gets an interpolated height value.
 *
 * @param hf the HeightField to sample
 * @param x the x coordinate of the location to sample
 * @param y the y coordinate of the location to sample
 * @return the interpolated height value
 */
static float GetInterpolatedHeight(osg::HeightField* hf, double x, double y)
{
   int fx = (int)floor(x), cx = (int)ceil(x),
       fy = (int)floor(y), cy = (int)ceil(y);
         
   double v1 = hf->getHeight(fx, fy),
          v2 = hf->getHeight(cx, fy),
          v3 = hf->getHeight(fx, cy),
          v4 = hf->getHeight(cx, cy),
          v12 = v1 + (v2-v1)*(x-fx),
          v34 = v3 + (v4-v3)*(x-fx);
         
   return v12 + (v34-v12)*(y-fy);
}

/**
 * Makes the base color texture map for the specified heightfield.
 *
 * @param hf the heightfield to process
 * @param latitude the latitude of the terrain segment
 * @param longitude the longitude of the terrain segment
 * @return the newly created image
 */
osg::Image* SOARXTerrain::MakeBaseColor(osg::HeightField* hf, int latitude, int longitude)
{
   vector<GeospecificImage> images;
   
   vector<GeospecificImage>::iterator it;
   
   int width = hf->getNumColumns()-1,
       height = hf->getNumRows()-1;
   
   for(it = mGeospecificImages.begin();
       it != mGeospecificImages.end();
       it++)
   {
      if(latitude >= (*it).mMinLatitude && latitude <= (*it).mMaxLatitude &&
         longitude >= (*it).mMinLongitude && longitude <= (*it).mMaxLongitude)
      {
         images.push_back(*it);
         
         width = osg::maximum(
            width, 
            osg::Image::computeNearestPowerOfTwo(
               abs(1.0/(*it).mGeoTransform[1])
            )
         );
         
         height = osg::maximum(
            height,
            osg::Image::computeNearestPowerOfTwo(
               abs(1.0/(*it).mGeoTransform[5])
            )
         );
      }
   }
   
   if(width > mMaxTextureSize) width = mMaxTextureSize;
   if(height > mMaxTextureSize) height = mMaxTextureSize;
   
   osg::Image* image = new osg::Image;
   
   image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);
   
   unsigned char* ptr = (unsigned char*)image->data();
   
   float heightVal, l1, l2, l3, l4, l12, l34;
   osg::Vec3 c1, c2, c3, c4, c12, c34, color, coord;
   unsigned char* data;
   
   double latStep = 1.0/height, lonStep = 1.0/width,
          lat = latitude+latStep*0.5, lon,
          sStep = (hf->getNumColumns()-1.0)/width, tStep = (hf->getNumRows()-1.0)/height,
          s, t = tStep*0.5;
   
   for(int y=0;y<height;y++)
   {
      lon = longitude + lonStep*0.5;
      s = sStep*0.5;
      
      for(int x=0;x<width;x++)
      {
         heightVal = GetInterpolatedHeight(hf, s, t);
         
         if(heightVal > 0.0f)
         {
            color = mUpperHeightColorMap.GetColor(heightVal);
         }
         else
         {
            color = mLowerHeightColorMap.GetColor(heightVal);
         }
         
         for(it = images.begin();it != images.end();it++)
         {
            double x = (*it).mInverseGeoTransform[0] +
                       (*it).mInverseGeoTransform[1]*lon + 
                       (*it).mInverseGeoTransform[2]*lat,
                   y = (*it).mInverseGeoTransform[3] +
                       (*it).mInverseGeoTransform[4]*lon + 
                       (*it).mInverseGeoTransform[5]*lat;
            
            int fx = (int)floor(x), fy = (int)floor(y),
                cx = (int)ceil(x), cy = (int)ceil(y);
            
            if(fx >= 0 && cx < (*it).mImage->s() && fy >= 0 && cy < (*it).mImage->t())
            {
               float ax = (float)(x - fx), ay = (float)(y - fy);
               
               if((*it).mImage->getPixelFormat() == GL_LUMINANCE)
               {
                  data = (*it).mImage->data(fx, fy);
                  l1 = data[0]/255.0f;
            
                  data = (*it).mImage->data(cx, fy);
                  l2 = data[0]/255.0f;
                  
                  data = (*it).mImage->data(fx, cy);
                  l3 = data[0]/255.0f;
                  
                  data = (*it).mImage->data(cx, cy);
                  l4 = data[0]/255.0f;
                  
                  l12 = l1*(1.0f-ax) + l2*ax;
                  l34 = l3*(1.0f-ax) + l4*ax;
                  
                  color *= (l12*(1.0f-ay) + l34*ay);
               }
               else
               {
                  data = (*it).mImage->data(fx, fy);
                  c1.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);
            
                  data = (*it).mImage->data(cx, fy);
                  c2.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);
                  
                  data = (*it).mImage->data(fx, cy);
                  c3.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);
                  
                  data = (*it).mImage->data(cx, cy);
                  c4.set(data[0]/255.0f, data[1]/255.0f, data[2]/255.0f);
                  
                  c12 = c1*(1.0f-ax) + c2*ax;
                  c34 = c3*(1.0f-ax) + c4*ax;
                  
                  color = c12*(1.0f-ay) + c34*ay;
               }
            }
         }
         
         *(ptr++) = (unsigned char)(color[0]*255);
         *(ptr++) = (unsigned char)(color[1]*255);
         *(ptr++) = (unsigned char)(color[2]*255);
         
         lon += lonStep;
         s += sStep;
      }
      
      lat += latStep;
      t += tStep;
   }
   
   return image;
}

/**
 * A working polygon vertex.
 */
struct WorkingVertex
{
   osg::Vec3 mLocation;
   osg::Vec3 mNormal;
   osg::Vec2 mTexCoord;
};

/**
 * A working polygon.
 */
typedef vector<WorkingVertex> WorkingPolygon;

/**
 * Returns the vertex that lies between v1 and v2 on the specified plane.
 *
 * @param v1 the first vertex
 * @param v2 the second vertex
 * @param plane the splitting plane
 * @return the new vertex
 */
static WorkingVertex SplitLineSegment(WorkingVertex& v1, WorkingVertex& v2, osg::Plane& plane)
{
   float t = (-plane[0]*v1.mLocation[0] - plane[1]*v1.mLocation[1] -
              plane[2]*v1.mLocation[2] - plane[3]) /
             (plane[0]*(v2.mLocation[0]-v1.mLocation[0]) + plane[1]*(v2.mLocation[1]-v1.mLocation[1]) +
             plane[2]*(v2.mLocation[2]-v1.mLocation[2]));
   
   WorkingVertex newVertex;
   
   newVertex.mLocation = v1.mLocation + (v2.mLocation - v1.mLocation)*t;
   newVertex.mNormal = v1.mNormal + (v2.mNormal - v1.mNormal)*t;
   newVertex.mTexCoord = v1.mTexCoord + (v2.mTexCoord - v1.mTexCoord)*t;
   
   newVertex.mNormal.normalize();
   
   return newVertex;
}

/**
 * Epsilon value for clipping.
 */
const float epsilon = 1e-6;

/**
 * Clips a polygon to the boundaries of its terrain cell.
 *
 * @param polygon the polygon to clip
 */
static void ClipPolygonToCellBoundaries(WorkingPolygon& polygon)
{
   osg::Plane boundaries[4];
   
   boundaries[0].set(1, 0, 0, 0);
   boundaries[1].set(0, -1, 0, 0);
   boundaries[2].set(-1, 0, 0, SG_DEGREES_TO_RADIANS*semiMajorAxis);
   boundaries[3].set(0, 1, 0, SG_DEGREES_TO_RADIANS*semiMajorAxis);
   
   for(int i=0;i<4;i++)
   {
      WorkingPolygon newPolygon;
      
      for(unsigned int j=0;j<polygon.size();j++)
      {
         int k = (j+1)%polygon.size();
         
         float jDist = boundaries[i].distance(polygon[j].mLocation),
               kDist = boundaries[i].distance(polygon[k].mLocation);
               
         if(jDist < -epsilon)
         {
            if(kDist > +epsilon)
            {
               WorkingVertex newVertex = SplitLineSegment(
                  polygon[j],
                  polygon[k],
                  boundaries[i]
               );
               
               newPolygon.push_back(newVertex);
            }
         }
         else if(jDist > +epsilon)
         {
            newPolygon.push_back(polygon[j]);
            
            if(kDist < -epsilon)
            {
               WorkingVertex newVertex = SplitLineSegment(
                  polygon[j],
                  polygon[k],
                  boundaries[i]
               );
               
               newPolygon.push_back(newVertex);
            }
         }
         else
         {
            newPolygon.push_back(polygon[j]);
         }
      }
      
      polygon = newPolygon;
   }
}

struct RoadEdge;

/**
 * A vertex in the road network.
 */
struct RoadVertex
{
   osg::Vec3 mLocation;
   
   set<RoadEdge*> mEdges;   
   
   bool operator<(const RoadVertex& rv) const
   {
      return mLocation < rv.mLocation;
   }
};

/**
 * An edge in the road network.
 */
struct RoadEdge
{
   RoadVertex* mVertices[2];
   
   float mTValues[2];
   
   WorkingPolygon mPolygon;
   
   bool operator<(const RoadEdge& re) const
   {
      RoadVertex* rv[4];
      
      rv[0] = osg::minimum(mVertices[0], mVertices[1]);
      rv[1] = osg::maximum(mVertices[0], mVertices[1]);
      rv[2] = osg::minimum(re.mVertices[0], re.mVertices[1]);
      rv[3] = osg::maximum(re.mVertices[0], re.mVertices[1]);
      
      if(rv[0] < rv[2])
      {
         return true;
      }
      else if(rv[0] > rv[2])
      {
         return false;
      }
      else
      {
         return rv[1] < rv[3];
      }
   }
};

/**
 * Makes roads for the specified segment.
 *
 * @param latitude the latitude of the terrain segment
 * @param longitude the longitude of the terrain segment
 * @param origin the origin of the terrain cell
 * @return the newly created road node
 */
osg::Node* SOARXTerrain::MakeRoads(int latitude, int longitude, const osg::Vec3& origin)
{
   osg::Geode* geode = new osg::Geode;
   
   OGRSpatialReference wgs84;
         
   wgs84.SetWellKnownGeogCS("WGS84");
   
   for(vector<Roads>::iterator it = mRoads.begin();
       it != mRoads.end();
       it++)
   {
      if(latitude >= (*it).mMinLatitude && latitude <= (*it).mMaxLatitude &&
         longitude >= (*it).mMinLongitude && longitude <= (*it).mMaxLongitude)
      {
         osg::Geometry* geom = new osg::Geometry;
         
         osg::StateSet* ss = geom->getOrCreateStateSet();
         
         ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
         
         ss->setTextureAttributeAndModes(0, (*it).mTexture.get());
         
         OGRPolygon filter;
         OGRLinearRing ring;
         
         ring.addPoint(latitude, longitude);
         ring.addPoint(latitude+1.0, longitude);
         ring.addPoint(latitude+1.0, longitude+1.0);
         ring.addPoint(latitude, longitude+1.0);
         
         ring.assignSpatialReference(&wgs84);
         filter.assignSpatialReference(&wgs84);
         
         filter.addRing(&ring);
         
         (*it).mLayer->SetSpatialFilter(&filter);
         
         (*it).mLayer->ResetReading();
         
         set<RoadVertex> vertices;
         set<RoadEdge> edges;
         
         vector<OGRLineString*> lineStrings;
         
         for(OGRFeature* feature = (*it).mLayer->GetNextFeature();
             feature != NULL;
             feature = (*it).mLayer->GetNextFeature())
         {
            OGRLineString* ls =
               dynamic_cast<OGRLineString*>(feature->GetGeometryRef());
               
            OGRMultiLineString* mls = 
               dynamic_cast<OGRMultiLineString*>(feature->GetGeometryRef());
            
            if(ls != NULL)
            {
               lineStrings.push_back(ls);
            }
            else if(mls != NULL)
            {
               for(int i=0;i<mls->getNumGeometries();i++)
               {
                  lineStrings.push_back((OGRLineString*)mls->getGeometryRef(i));
               }
            }
         }
         
         OGRPoint ogrp1, ogrp2;
         
         float halfWidth = (*it).mWidth/2,
               distTScale = (*it).mTScale*(*it).mSScale/(*it).mWidth;
         
         osg::BoundingBox bb(
            -halfWidth, -SG_DEGREES_TO_RADIANS*semiMajorAxis-halfWidth, -10000.0f,
            SG_DEGREES_TO_RADIANS*semiMajorAxis+halfWidth, halfWidth, 10000.0f
         );
         
         for(vector<OGRLineString*>::iterator ls = lineStrings.begin();
             ls != lineStrings.end();
             ls++)
         {
            int numPoints = (*ls)->getNumPoints(),
                numEdges = (*ls)->get_IsClosed() ? numPoints : numPoints - 1;
            
            float t2 = 0.0f;
            
            for(int i=0;i<numEdges;i++)
            {
               RoadVertex v1, v2;
               
               (*ls)->getPoint(i, &ogrp1);
               
               v1.mLocation[0] = (ogrp1.getX() - longitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
               v1.mLocation[1] = (ogrp1.getY() - latitude - 1)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
               v1.mLocation[2] = 0.0f;
               
               (*ls)->getPoint((i+1)%numPoints, &ogrp2);
               
               v2.mLocation[0] = (ogrp2.getX() - longitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
               v2.mLocation[1] = (ogrp2.getY() - latitude - 1)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
               v2.mLocation[2] = 0.0f;
               
               float t1 = t2;
               
               t2 += distTScale*(v2.mLocation-v1.mLocation).length();
               
               if((bb.contains(v1.mLocation) || bb.contains(v2.mLocation)) &&
                   v1.mLocation != v2.mLocation)
               {
                  v1.mLocation[2] = GetHeight(origin[0]+v1.mLocation[0], origin[1]+v1.mLocation[1])+5.0;
                  v2.mLocation[2] = GetHeight(origin[0]+v2.mLocation[0], origin[1]+v2.mLocation[1])+5.0;
                  
                  RoadEdge re;
                  
                  re.mVertices[0] = &(*vertices.insert(v1).first);
                  re.mVertices[1] = &(*vertices.insert(v2).first);
                  
                  re.mTValues[0] = t1;
                  re.mTValues[1] = t2;
                  
                  RoadEdge* rep = &(*edges.insert(re).first);
                  
                  (*re.mVertices[0]).mEdges.insert(rep);
                  (*re.mVertices[1]).mEdges.insert(rep);
               }
            }
         }

         osg::Vec3Array* verts = new osg::Vec3Array;
         osg::Vec3Array* normals = new osg::Vec3Array;
         osg::Vec2Array* texCoords = new osg::Vec2Array;
         
         geom->setVertexArray(verts);
         geom->setNormalArray(normals);
         geom->setTexCoordArray(0, texCoords);
         
         int first = 0;
         
         for(set<RoadEdge>::iterator re = edges.begin();
             re != edges.end();
             re++)
         {
            WorkingPolygon polygon;
            
            unsigned int i;
            
            for(i=0;i<2;i++)
            {
               unsigned int j = (i+1)%2;
               
               osg::Vec3 forward, dir;
            
               forward = (*re).mVertices[j]->mLocation - (*re).mVertices[i]->mLocation;
               
               forward.normalize();
               
               bool foundVerts = false;
            
               if((*re).mVertices[i]->mEdges.size() == 2)
               {
                  for(set<RoadEdge*>::iterator ore = (*re).mVertices[i]->mEdges.begin();
                      ore != (*re).mVertices[i]->mEdges.end();
                      ore++)
                  {
                     if((*ore) != &(*re))
                     {
                        if((*ore)->mPolygon.size() == 4)
                        {
                           for(unsigned int k=0;k<2;k++)
                           {
                              if((*ore)->mVertices[k] == (*re).mVertices[i])
                              {
                                 polygon.push_back((*ore)->mPolygon[k*2+1]);
                                 polygon.push_back((*ore)->mPolygon[k*2]);
                                 foundVerts = true;
                              }
                           }
                        }
                        else
                        {
                           for(unsigned int k=0;k<2;k++)
                           {
                              if((*ore)->mVertices[k] != (*re).mVertices[i])
                              {
                                 dir = (*re).mVertices[j]->mLocation - (*ore)->mVertices[k]->mLocation;
                                 dir.normalize();
                              }
                           }
                        }
                     }
                  }
               }
               else
               {
                  dir = forward;
               }
               
               if(!foundVerts)
               {
                  osg::Vec3 v1 = osg::Z_AXIS^dir;
                  
                  v1.normalize();
                  
                  WorkingVertex wv1, wv2;
                  
                  float w = halfWidth/osg::maximum(dir*forward, 0.5f);
               
                  wv1.mLocation = (*re).mVertices[i]->mLocation+(v1*w);
                  wv1.mNormal = dir^v1;
                  
                  wv2.mLocation = (*re).mVertices[i]->mLocation-(v1*w);
                  wv2.mNormal = wv1.mNormal;
                  
                  if(i==0)
                  {
                     wv1.mTexCoord.set(0.0f, (*re).mTValues[i]);
                     wv2.mTexCoord.set((*it).mSScale, (*re).mTValues[i]);
                  }
                  else
                  {
                     wv2.mTexCoord.set(0.0f, (*re).mTValues[i]);
                     wv1.mTexCoord.set((*it).mSScale, (*re).mTValues[i]);
                  }
                  
                  polygon.push_back(wv1);
                  polygon.push_back(wv2);
               }
            }
            
            (*re).mPolygon = polygon;
            
            ClipPolygonToCellBoundaries(polygon);
            
            for(i=0;i<polygon.size();i++)
            {
               verts->push_back(polygon[i].mLocation);
               normals->push_back(polygon[i].mNormal);
               texCoords->push_back(polygon[i].mTexCoord);
            }
            
            geom->addPrimitiveSet(
               new osg::DrawArrays(osg::PrimitiveSet::POLYGON, first, polygon.size())
            );
            
            first += polygon.size();
         }
         
         osgUtil::TriStripVisitor tsv;
         
         tsv.stripify(*geom);
         
         geode->addDrawable(geom);
      }
   }
   
   return geode;
}

/**
 * Resizes an osg::HeightField for use in the SOARX algorithm, which requires
 * that heightfields be square and 2^(integer n)+1 on each side.
 *
 * @param hf the HeightField to resize
 * @return the new, resized HeightField
 */
static osg::HeightField* ResizeHeightField(osg::HeightField* hf)
{
   osg::HeightField* newHF = new osg::HeightField;
   
   int newSize =
      osg::Image::computeNearestPowerOfTwo(
         osg::maximum(hf->getNumColumns(), hf->getNumRows())
      )+1;
   
   newHF->allocate(newSize, newSize);
   
   double x, xStep = (hf->getNumColumns()-1.0)/(newSize-1.0),
          y = 0.0, yStep = (hf->getNumRows()-1.0)/(newSize-1.0);
   
   for(int i=0;i<newSize;i++)
   {
      x = 0.0;
      
      for(int j=0;j<newSize;j++)
      {
         newHF->setHeight(j, i, GetInterpolatedHeight(hf, x, y));
         
         x += xStep;
      }
      
      y += yStep;
   }
   
   return newHF;
}

/**
 * Loads a single terrain segment.
 *
 * @param latitude the latitude of the segment to load
 * @param longitude the longitude of the segment to load
 */
void SOARXTerrain::LoadSegment(int latitude, int longitude)
{
   Segment segment(latitude, longitude);
   
   if(mSegmentDrawableMap.count(segment) > 0)
   {
      return;
   }
   
   char latString[20], longString[20];
        
   sprintf(longString, "%c%03d", longitude < 0 ? 'w' : 'e', osg::absolute(longitude));
   sprintf(latString, "%c%02d", latitude < 0 ? 's' : 'n', osg::absolute(latitude));
   
   for(int i=2;i>=0;i--)
   {
      char soarxName[64], dtedName[64];
      string soarxPath, dtedPath;
      osg::HeightField* hf = NULL;
      
      sprintf(soarxName, "%s_%s_%d.base.t", longString, latString, i);
      sprintf(dtedName, "%s/%s.dt%d", longString, latString, i);
      
      soarxPath = osgDB::findFileInDirectory(
         soarxName,
         mCachePath,
         osgDB::CASE_INSENSITIVE
      );
      
      if(soarxPath == "")
      {
         dtedPath = osgDB::findFileInPath(
            dtedName, 
            mDTEDPaths, 
            osgDB::CASE_INSENSITIVE
         );
         
         if(dtedPath != "")
         {
            osgDB::ReaderWriter* gdalReader = 
               osgDB::Registry::instance()->getReaderWriterForExtension("gdal");
      
            osgDB::ReaderWriter::ReadResult rr =
               gdalReader->readHeightField(dtedPath);
         
            if(rr.success())
            {
               hf = ResizeHeightField(rr.getHeightField());
            
               hf->setXInterval((semiMajorAxis*SG_DEGREES_TO_RADIANS)/(hf->getNumColumns()-1));
               hf->setYInterval((semiMajorAxis*SG_DEGREES_TO_RADIANS)/(hf->getNumRows()-1));
            }
         }
      }

      if(soarxPath != "" || hf != NULL)
      {
         if(mDetailGradient[i].get() == NULL || mDetailScale[i].get() == NULL)
         {
            osg::Image* detailGradient;
            osg::Image* detailScale;
            
            char suffix[32];
            
            sprintf(suffix, ".%d.bmp", i);
            
            string detailGradientPath = mCachePath + "/detail.gradient" + suffix;
            string detailScalePath = mCachePath + "/detail.scale" + suffix;
            
            if(osgDB::fileExists(detailGradientPath))
            {
               detailGradient = osgDB::readImageFile(detailGradientPath);
            }
            else
            {
               Notify(NOTICE, "SOARXTerrain: Making detail gradient image for level %d...", i);
               
               detailGradient = MakeDetailGradient(i);
               
               osgDB::writeImageFile(*detailGradient, detailGradientPath);
            }
            
            if(osgDB::fileExists(detailScalePath))
            {
               detailScale = osgDB::readImageFile(detailScalePath);
            }
            else
            {
               Notify(NOTICE, "SOARXTerrain: Making detail scale image for level %d...", i);
               
               detailScale = MakeDetailScale(i);
               
               osgDB::writeImageFile(*detailScale, detailScalePath);
            }
            
            mDetailGradient[i] = new osg::Texture2D(detailGradient);
            
            mDetailGradient[i]->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
            mDetailGradient[i]->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
            
            mDetailScale[i] = new osg::Texture2D(detailScale);
            
            mDetailScale[i]->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
            mDetailScale[i]->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);
         }
         
         osg::Image* baseGradient;
         osg::Image* baseColor;
         
         char cellName[64];
         
         sprintf(cellName, "%s_%s_%d", longString, latString, i);
         
         string baseGradientPath = mCachePath + "/" + cellName + ".base.gradient.bmp";
         string baseColorPath = mCachePath + "/" + cellName + ".base.color.bmp";
         
         if(osgDB::fileExists(baseGradientPath))
         {
            baseGradient = osgDB::readImageFile(baseGradientPath);
         }
         else
         {
            Notify(NOTICE, "SOARXTerrain: Making base gradient image for %s...", cellName);
            
            baseGradient = MakeBaseGradient(hf);
               
            baseGradient->ensureValidSizeForTexturing(mMaxTextureSize);
            
            osgDB::writeImageFile(*baseGradient, baseGradientPath);
         }
         
         if(osgDB::fileExists(baseColorPath))
         {
            baseColor = osgDB::readImageFile(baseColorPath);
         }
         else
         {
            Notify(NOTICE, "SOARXTerrain: Making base color image for %s...", cellName);
            
            baseColor = MakeBaseColor(hf, latitude, longitude);
               
            baseColor->ensureValidSizeForTexturing(mMaxTextureSize);
            
            osgDB::writeImageFile(*baseColor, baseColorPath);
         }
         
         osg::MatrixTransform* mt = new osg::MatrixTransform;
         
         osg::Vec3 origin(
            (longitude - mOriginLongitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
            (1 + latitude - mOriginLatitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
            -mOriginElevation
         );
         
         mt->setMatrix(osg::Matrix::translate(origin));
         
         osg::Geode* geode = new osg::Geode;
         
         osg::StateSet* ss = geode->getOrCreateStateSet();
         
         ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
         ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

         ss->setAttributeAndModes(mProgramObject.get());

         ss->setTextureAttributeAndModes(0, mDetailGradient[i].get());
         
         osg::TexGen* detailGradientTexGen = new osg::TexGen;
         
         int baseSize, baseBits;
         
         switch(i)
         {
            case 0:
               baseSize = 129;
               baseBits = 7;
               break;
               
            case 1:
               baseSize = 1025;
               baseBits = 10;
               break;
               
            case 2:
               baseSize = 4097;
               baseBits = 12;
               break;
         }
         
         float baseHorizontalResolution = (semiMajorAxis*SG_DEGREES_TO_RADIANS)/(baseSize-1);
         
         int embeddedSize = 1 << (16-baseBits);
         
         float dt = 1.0f / ((baseHorizontalResolution/embeddedSize) * 1024);
         
         detailGradientTexGen->setMode(osg::TexGen::OBJECT_LINEAR);
         detailGradientTexGen->setPlane(osg::TexGen::S, osg::Plane(dt, 0, 0, 0));
         detailGradientTexGen->setPlane(osg::TexGen::T, osg::Plane(0, 0, -dt, 1));
         
         ss->setTextureAttributeAndModes(0, detailGradientTexGen);
         
         ss->setTextureAttributeAndModes(1, mDetailScale[i].get());
         
         osg::TexGen* detailScaleTexGen = new osg::TexGen;
         
         float bt = 1.0f / (baseHorizontalResolution*baseSize);
         
         detailScaleTexGen->setMode(osg::TexGen::OBJECT_LINEAR);
         detailScaleTexGen->setPlane(osg::TexGen::S, osg::Plane(bt, 0, 0, 0));
         detailScaleTexGen->setPlane(osg::TexGen::T, osg::Plane(0, 0, -bt, 1));
         
         ss->setTextureAttributeAndModes(1, detailScaleTexGen);
         
         osg::Texture2D* baseGradientTex = new osg::Texture2D(baseGradient);
         
         baseGradientTex->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
         baseGradientTex->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);
         
         ss->setTextureAttributeAndModes(2, baseGradientTex);
         
         osg::Texture2D* baseColorTex = new osg::Texture2D(baseColor);
         
         baseColorTex->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
         baseColorTex->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);
         
         ss->setTextureAttributeAndModes(3, baseColorTex);
         
         SOARXDrawable* soarxd = new SOARXDrawable;
         
         ostringstream cachePath;
         
         cachePath << mCachePath << "/"; 
         
         char prefix[40];
         
         sprintf(prefix, "%s_%s_%d.", longString, latString, i);
         
         soarxd->Load(
            cachePath.str().c_str(), 
            prefix,
            hf,
            baseBits,
            baseHorizontalResolution
         );
         
         soarxd->SetThreshold(mThreshold);
         soarxd->SetDetailMultiplier(mDetailMultiplier);
         
         geode->addDrawable(soarxd);
         
         mt->addChild(geode);
         
         osg::LOD* lod = new osg::LOD;
         
         lod->addChild(mt, 0.0f, mLoadDistance*10);
      
         mNode->addChild(lod);
      
         mSegmentDrawableMap[segment] = soarxd;
         
         osg::Node* roads;
         
         string roadsPath = mCachePath + "/" + cellName + ".roads.ive";
         
         if(osgDB::fileExists(roadsPath))
         {
            roads = osgDB::readNodeFile(roadsPath);
         }
         else
         {
            Notify(NOTICE, "SOARXTerrain: Making roads for %s...", cellName);
            
            roads = MakeRoads(latitude, longitude, origin);
               
            osgDB::writeNodeFile(*roads, roadsPath);
         }
         
         mt->addChild(roads);
         
         Notify(NOTICE, "SOARXTerrain: Loaded %s", cellName);
         
         return;
      }
   }
}

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
int SOARXTerrain::Collider(dGeomID o1, dGeomID o2, int flags,
                           dContactGeom* contact, int skip)
{
   SOARXTerrain* st = *(SOARXTerrain**)dGeomGetClassData(o1);
   
   int numContacts = 0,
       maxContacts = flags & 0xFFFF;
   
   int geomClass = dGeomGetClass(o2);
   
   const dReal* position = dGeomGetPosition(o2);
   const dReal* rotation = dGeomGetRotation(o2);
   
   sgMat4 mat = {
      { rotation[0], rotation[4], rotation[8], 0.0f },
      { rotation[1], rotation[5], rotation[9], 0.0f },
      { rotation[2], rotation[6], rotation[10], 0.0f },
      { position[0], position[1], position[2], 1.0f }
   };
   
   if(geomClass == dBoxClass)
   {
      dVector3 lengths;
      
      dGeomBoxGetLengths(o2, lengths);
      
      lengths[0] *= 0.5f;
      lengths[1] *= 0.5f;
      lengths[2] *= 0.5f;
      
      sgVec3 corners[8] =
      {
         {-lengths[0], -lengths[1], -lengths[2]},
         {-lengths[0], -lengths[1], +lengths[2]},
         {-lengths[0], +lengths[1], -lengths[2]},
         {-lengths[0], +lengths[1], +lengths[2]},
         {+lengths[0], -lengths[1], -lengths[2]},
         {+lengths[0], -lengths[1], +lengths[2]},
         {+lengths[0], +lengths[1], -lengths[2]},
         {+lengths[0], +lengths[1], +lengths[2]}
      };
      
      for(int i=0;i<8 && i<maxContacts;i++)
      {
         sgXformPnt3(corners[i], mat);
         
         sgVec3 point =
         {
            corners[i][0], 
            corners[i][1], 
            st->GetHeight(corners[i][0], corners[i][1])
         };
      
         sgVec3 normal;
      
         st->GetNormal(corners[i][0], corners[i][1], normal);
      
         sgVec4 plane;
      
         sgMakePlane(plane, normal, point);
      
         float dist = sgDistToPlaneVec3(plane, corners[i]);
      
         if(dist <= 0.0f)
         {
            contact->pos[0] = corners[i][0] - dist*normal[0];
            contact->pos[1] = corners[i][1] - dist*normal[1];
            contact->pos[2] = corners[i][2] - dist*normal[2];
            
            contact->normal[0] = -normal[0];
            contact->normal[1] = -normal[1];
            contact->normal[2] = -normal[2];
            
            contact->depth = -dist;
            
            contact->g1 = o1;
            contact->g2 = o2;
            
            numContacts++;
            
            contact = (dContactGeom*)(((char*)contact) + skip);
         }   
      }
   }
   else if(geomClass == dSphereClass)
   {
      dReal radius = dGeomSphereGetRadius(o2);
      
      sgVec3 center = { 0.0f, 0.0f, 0.0f };
      
      sgXformPnt3(center, mat);
      
      sgVec3 point =
      {
         center[0], 
         center[1], 
         st->GetHeight(center[0], center[1])
      };
      
      sgVec3 normal;
      
      st->GetNormal(center[0], center[1], normal);
      
      sgVec4 plane;
      
      sgMakePlane(plane, normal, point);
      
      float dist = sgDistToPlaneVec3(plane, center);
      
      if(dist <= radius && maxContacts >= 1)
      {
         contact->pos[0] = center[0] - dist*normal[0];
         contact->pos[1] = center[1] - dist*normal[1];
         contact->pos[2] = center[2] - dist*normal[2];
         
         contact->normal[0] = -normal[0];
         contact->normal[1] = -normal[1];
         contact->normal[2] = -normal[2];
         
         contact->depth = radius - dist;
         
         contact->g1 = o1;
         contact->g2 = o2;
         
         numContacts++;
      }
   }
   
   return numContacts;
}

/**
 * ODE collision function: Finds the collider function appropriate
 * to detect collisions between SOARXTerrain geoms and other
 * geoms.
 *
 * @param num the class number of the geom class to check
 * @return the appropriate collider function, or NULL for none
 */
dColliderFn* SOARXTerrain::GetColliderFn(int num)
{
   if(num == dBoxClass ||
      num == dSphereClass)
   {
      return Collider;
   }
   else
   {
      return NULL;
   }
}

/**
 * ODE collision function: Computes the axis-aligned bounding box
 * for SOARXTerrain instances.
 *
 * @param g the geom to check
 * @param aabb the location in which to store the axis-aligned
 * bounding box
 */
void SOARXTerrain::GetAABB(dGeomID g, dReal aabb[6])
{
   SOARXTerrain* st = *(SOARXTerrain**)dGeomGetClassData(g);

   int minLat, minLon, maxLat, maxLon;
   
   map<Segment, SOARXDrawable*>::iterator it = st->mSegmentDrawableMap.begin();
   
   minLat = (*it).first.mLatitude;
   minLon = (*it).first.mLongitude;
   
   it = st->mSegmentDrawableMap.end();
   
   it--;
   
   maxLat = (*it).first.mLatitude+1;
   maxLon = (*it).first.mLongitude+1;
   
   aabb[0] = (minLon - st->mOriginLongitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis;
   aabb[1] = (minLat - st->mOriginLatitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis;
   aabb[2] = -10000.0f;
   aabb[3] = (maxLon - st->mOriginLongitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis;
   aabb[4] = (maxLat - st->mOriginLatitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis;
   aabb[5] = 10000.0f;
   
   Notify(NOTICE) << minLon << " " << minLat << " " << maxLon << " " << maxLat << endl;
}

/**
 * ODE collision function: Checks whether the specified axis-aligned
 * bounding box intersects with an SOARXTerrain instance.
 *
 * @param o1 the first (SOARXTerrain) geom
 * @param o2 the second geom
 * @param aabb2 the axis-aligned bounding box of the second geom
 * @return 1 if it intersects, 0 if it does not
 */
int SOARXTerrain::AABBTest(dGeomID o1, dGeomID o2, dReal aabb2[6])
{
   SOARXTerrain* st = *(SOARXTerrain**)dGeomGetClassData(o1);
   
   if(st->GetHeight(aabb2[0], aabb2[1]) >= aabb2[2] ||
      st->GetHeight(aabb2[0], aabb2[4]) >= aabb2[2] ||
      st->GetHeight(aabb2[3], aabb2[1]) >= aabb2[2] ||
      st->GetHeight(aabb2[3], aabb2[4]) >= aabb2[2])
   {
      return 1;
   }
   else
   {
      return 0;
   }
}