// soarxterrain.cpp: Implementation of the SOARXTerrain class.
//
//////////////////////////////////////////////////////////////////////

#include "soarx/soarxterrain.h"

#include "dtCore/notify.h"

#include <iostream>

#include <osg/Geode>
#include <osg/Switch>
#include <osgDB/ImageOptions>

using namespace dtCore;
using namespace dtABC;
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
   "  vec4 detail = texture2D(detailGradient, gl_TexCoord[0].st);"
   "  vec4 scale = texture2D(detailScale, gl_TexCoord[1].st);"
   "  vec4 base = texture2D(baseGradient, gl_TexCoord[1].st);"
//   "  vec4 combined = (detail - 0.5) * scale + (base - 0.5);"
   "  vec4 combined = (detail - 0.5) * scale + 0.5 + (base - 0.5);"
   "  vec3 normal = normalize(vec3(combined.y, 0.5, combined.z));"
   "  gl_FragColor = texture2D(baseColor, gl_TexCoord[1].st);"
   "  gl_FragColor.rgb *= max(0.0, dot(normal, gl_LightSource[0].position));"
   "  float fog;"
   "  const float LOG2E = 1.442695;"   // = 1/log(2)
//   "  if (gl_FogFragCoord < 0.0) {gl_FogFragCoord = 0.0;}"
   "  fog = exp2(-gl_Fog.density * .3 * gl_Fog.density * gl_FogFragCoord * gl_FogFragCoord * LOG2E);"
   "  fog = clamp(fog, 0.0, 1.0);"
   "  gl_FragColor = mix(gl_Fog.color, gl_FragColor, fog);"
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
			   if (mTerrain->mUseLCC)
					mTerrain->AddVegetation(i,j);
            }
         }

//		 Notify(WARN, "%s in cull", (node->getName()).c_str());

		 traverse(node, nv);
      }


   private:

      /**
       * The owning SOARXTerrain object.
       */
	   SOARXTerrain* mTerrain;
};


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

//		 Notify(WARN, "%s in transform", (node->getName()).c_str());

		 traverse(node,nv);
	  }

   private:

	   SOARXTerrain* mTerrain;
};


class VegeUpdateCallback : public osg::NodeCallback 
{
public:

	VegeUpdateCallback()
	{
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		traverse(node, nv);
	}

private:

};



class VegeCullCallback : public osg::NodeCallback 
{
public:
	VegeCullCallback()
	{
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{

		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::BoundingSphere bs = node->getBound();
//			if (node->getNodeMask()==2) Notify(WARN, "%s is NodeMasked 2", (node->getName()).c_str());
//			if (node->getBound()._radius == -1) node->dirtyBound();

			osg::Vec3 eyept = cv->getEyeLocal();

			float fViewLen = 50000.0f;
			float fHeight = fViewLen + tan(((60.0f/180.0f)*3.1415926f) *0.5f);
			float fWidth = fHeight;
			osg::Vec3 P(0.0f, 0.0f, fViewLen*0.5f);
			osg::Vec3 Q(fWidth, fHeight, fViewLen);
			osg::Vec3 vDiff(P - Q);
			float fsRadius = vDiff.length();
			osg::Vec3 vLookVector = cv->getLookVectorLocal();
			osg::Vec3 fsCenter(eyept + vLookVector*(fViewLen*0.5f));

//			Notify(WARN, "fCenter = %f, %f, %f", fCenter[0], fCenter[1], fCenter[2]);

			if ((bs._radius+fsRadius) < (cv->getDistanceToEyePoint(bs._center,FALSE)))
			{
//				Notify(WARN, "%s is now TOO FAR AWAY", (node->getName()).c_str());
				return;
			}
			else
			{
				float fDepth = fHeight/tan(((60.0f/180.0f)*3.1415926f)*0.5f);
				float fCorner = sqrt(fWidth*fWidth + fHeight*fHeight);
				float fcFov = atan(fCorner/fDepth);
				float fccos = cos(fcFov);
				float fcsin = sin(fcFov);
				
				float fInvSin = 1.0f/fcsin;
				float fCosSqr = fccos*fccos;

				osg::Vec3 fcAxis = vLookVector;
				osg::Vec3 fcVertex = eyept;

				osg::Vec3 kCmV = bs._center - fcVertex;
				osg::Vec3 kD = kCmV + fcAxis*(bs._radius*fInvSin);
				float fDSqrLen = kD.length2();
				float fE = kD*fcAxis;
				if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr )
				{
					float fSinSqr = fcsin*fcsin;

					fDSqrLen = kCmV.length2();
					fE = -kCmV*fcAxis;
					if (( fE > 0.0f) && (fE*fE >= fDSqrLen*fSinSqr))
					{
						float fRSqr = bs._radius*bs._radius;
						if (!(fDSqrLen <= fRSqr)) 
							return;
					}
					traverse(node, nv); 
				}
				else 
					return;
			}
		}
	}

private:
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
//	 mMaxTextureSize(4096)
	 mMaxTextureSize(1024)
{
   SetName(name);

   SetGeoOrigin(0.0, 0.0, 0.0);

   mClearFlag = false;
   mUseLCC = false;

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
   Notify(NOTICE, "Destr SOARX");
   //CleanUp();
   DeregisterInstance(this);

   Notify(NOTICE, "Deregistered SOARX");
}

/**
 *  Cleanup routines	
 */
void SOARXTerrain::CleanUp()
{
/*	if (mUseLCC)
	{
		if (mObjects.size()!=0) 
		{
			vector<dtCore::RefPtr<dtCore::Object> >::iterator it;

			for(it = mObjects.begin();
				it != mObjects.end();
				it++)
			{
				//
			}
			mObjects.clear();
		}
		if (mGroups.size()!=0)
		{
			Notify (NOTICE, "mGroups.size() = %i", mGroups.size());
			for(vector<dtCore::RefPtr<osg::Group> >::iterator it = mGroups.begin();
				it != mGroups.end();
				it++)
			{
				(*it).get()->removeChild(0,(*it).get()->getNumChildren()); 
			}
			mGroups.clear();
		}
	}
*/
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
      else if(!strcmp(element->Value(), "LCCImage"))
      {
         if((str = element->Attribute("filename")) != NULL)
         {
			mUseLCC = LoadLCCConfiguration("data/lccdata.xml");
            if (mUseLCC) 
				LoadGeospecificLCCImage(str);
         }
      }
      else if(!strcmp(element->Value(), "GeospecificImage"))
      {
         if((str = element->Attribute("filename")) != NULL)
         {
            LoadGeospecificImage(str);
         }
	  }      
	  else if(!strcmp(element->Value(), "MaxTexture"))
	  {
		  if((str = element->Attribute("Size")) != NULL)
		  {
			  Notify(NOTICE, "MaxTextureSize = %s", str);
			  mMaxTextureSize = atoi(str);
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
* Buggy "histogram" of an image by a particular LCC type.
*
* @param LCCbase the black/white LCC image of picked points of a particular LCC type
* @param image the slopemap, heightmap, or relative elevation
* @param filename the filename to save the histogram data
* @param binsize the sampling size of the image (i.e. the delta height or slope).
* Note: images must be the same size
**/
void SOARXTerrain::LCCHistogram(osg::Image* LCCbase, osg::Image* image, char* filename, int binsize)
{
//ending pixel positions for Monterey Bay test LCC data
//	int endx = 541;
//	int endy = 845;

	//size of the image
	int endx = 1024;
	int endy = 1024;

	unsigned char* lcc_data = NULL;
	unsigned char* image_data = NULL;
	int width = image->s();
	int height = image->t();

	unsigned int hitbin[51];		//range is 5 for 51 bins
	unsigned int missbin[51];		//range is 5 for 51 bins

	unsigned int LCChits =0;
	unsigned int LCCmisses =0;

	unsigned int binnumber=0;
	unsigned int testnumber=0;

	for(int i=0;i<51;i++)
	{
		hitbin[i] = 0;
		missbin[i] = 0;
	}

	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			lcc_data = (unsigned char*)LCCbase->data(x,y);
			image_data = (unsigned char*)image->data(x,y);

			if (lcc_data[0] == 0) // a hit!
			{
				binnumber = int(image_data[0]/binsize);
				hitbin[binnumber]++;
				LCChits++;
			}
			else				// a miss!
			{
				binnumber = int(image_data[0]/binsize);
				missbin[binnumber]++;
				LCCmisses++;
			}
			if (testnumber<10)
				Notify(NOTICE, "testnumber = %i, binnumber = %i", testnumber, binnumber);
			testnumber++;
		}
	}

	FILE *histofile = fopen(filename, "w");
	fprintf(histofile, "%s\n", filename);
	fprintf(histofile, "hits = %i, misses = %i\n", LCChits, LCCmisses);
	fprintf(histofile, "%s, %s, %s\n", "bin#", "hitbin", "missbin");

	for (int i=0;i<51;i++)
	{
		fprintf(histofile, "%i, %i, %i\n", i, hitbin[i], missbin[i]);
	}
	fflush(histofile);
	fclose(histofile);
}


/**
* Tell me interesting stats about an image.
* @param image the handle to the image you want to examine
* @param imagename a descriptive name to call this image
*/
void SOARXTerrain::ImageStats(osg::Image* image, char* imagename)
{
	Notify(NOTICE, "ImageStats:**********************************");

	int width = image->s();
	int height = image->t();
	int depth = image->r();

	Notify(NOTICE, "ImageStats:**********************************");
	Notify(NOTICE, "Image Name = %s", imagename);
	Notify(NOTICE, "Image Width = %i", width);
	Notify(NOTICE, "Image Height = %i", height);
	Notify(NOTICE, "Image Depth = %i", depth);
	Notify(NOTICE, "Image DataType = %i", image->getDataType());
	Notify(NOTICE, "Image Packing = %i", image->getPacking());
	Notify(NOTICE, "Image PixelFormat = %i", image->getPixelFormat());
	Notify(NOTICE, "Image MipMaps = %i", image->getNumMipmapLevels());
	Notify(NOTICE, "Image PixelSize = %i", image->getPixelSizeInBits());  //24 = RGB, 32 = RGBA
	Notify(NOTICE, "Image RowSize = %i", image->getRowSizeInBytes());
	Notify(NOTICE, "Image ImageSize = %i", image->getImageSizeInBytes());
	Notify(NOTICE, "Image TotalSize = %i", image->getTotalSizeInBytes());

/*	UNCOMMENT TO FIND A COLOR AT A PARTICULAR LOCATION

    osg::Vec3 color;

	//pixel position
	int ix = 0;
	int iy = 0;

	unsigned char* data = image->data(ix, iy);

	if(image->getPixelFormat() == GL_LUMINANCE)
		Notify(NOTICE, "grayscale");
	else
		Notify(NOTICE, "RGB color");

	color[0] = data[0];
	color[1] = data[1];
	color[2] = data[2];

	Notify(NOTICE, "RGB for %i,%i = %3.0f, %3.0f, %3.0f", ix, iy, color[0], color[1], color[2]);
*/
}


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
void SOARXTerrain::LoadGeospecificLCCImage(string filename, const double* geoTransform)
{
	GeospecificImage gslcc;

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

			ds->GetGeoTransform(gslcc.mGeoTransform);

			width = ds->GetRasterXSize();
			height = ds->GetRasterYSize();

			int bands = ds->GetRasterCount();

			if(bands == 1)
			{
				gslcc.mImage = new osg::Image;

				GDALColorTableH hCT = NULL;
				hCT = GDALGetRasterColorTable( ds->GetRasterBand(1));
				if( hCT != NULL )			//color palette exists
				{
					gslcc.mImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

					//set RGB values to color map indices
					for(int i=0;i<3;i++)
					{
						ds->GetRasterBand(1)->RasterIO(
							GF_Read, 0, 0, width, height,
							gslcc.mImage->data()+i, width, height, GDT_Byte, 3, 0
							);
					}

					//get point to image data
					unsigned char* ptr = (unsigned char*)gslcc.mImage->data();

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
					gslcc.mImage->allocateImage(width, height, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE);
					ds->GetRasterBand(1)->RasterIO(
						GF_Read, 0, 0, width, height,
						gslcc.mImage->data(), width, height, GDT_Byte, 0, 0
						);
				}
			}
			else if(bands == 3)
			{
				gslcc.mImage = new osg::Image;

				gslcc.mImage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

				for(int i=0;i<3;i++)
				{
					ds->GetRasterBand(i+1)->RasterIO(
						GF_Read, 0, 0, width, height,
						gslcc.mImage->data()+i, width, height, GDT_Byte, 3, 0
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
		gslcc.mImage = osgDB::readImageFile(filename);

		memcpy(gslcc.mGeoTransform, geoTransform, sizeof(double)*6);
	}

	if(gslcc.mImage.valid())
	{
		gslcc.mFilename = filename;

		gslcc.mMinLatitude = gslcc.mMinLongitude = 999;
		gslcc.mMaxLatitude = gslcc.mMaxLongitude = -999;

		for(int i=0;i<4;i++)
		{
			int x = (i&1) ? 0 : width-1,
				y = (i&2) ? 0 : height-1;

			int longitude = (int)floor(gslcc.mGeoTransform[0] + x*gslcc.mGeoTransform[1] + y*gslcc.mGeoTransform[2]),
				latitude = (int)floor(gslcc.mGeoTransform[3] + x*gslcc.mGeoTransform[4] + y*gslcc.mGeoTransform[5]);

			gslcc.mMinLatitude = min(latitude, gslcc.mMinLatitude);
			gslcc.mMinLongitude = min(longitude, gslcc.mMinLongitude);
			gslcc.mMaxLatitude = max(latitude, gslcc.mMaxLatitude);
			gslcc.mMaxLongitude = max(longitude, gslcc.mMaxLongitude);
		}

		double d = gslcc.mGeoTransform[1]*gslcc.mGeoTransform[5]-gslcc.mGeoTransform[2]*gslcc.mGeoTransform[4];

		gslcc.mInverseGeoTransform[0] =
			(gslcc.mGeoTransform[2]*gslcc.mGeoTransform[3]-gslcc.mGeoTransform[5]*gslcc.mGeoTransform[0])/d;

		gslcc.mInverseGeoTransform[1] = gslcc.mGeoTransform[5]/d;

		gslcc.mInverseGeoTransform[2] = -gslcc.mGeoTransform[2]/d;

		gslcc.mInverseGeoTransform[3] =
			(gslcc.mGeoTransform[4]*gslcc.mGeoTransform[0]-gslcc.mGeoTransform[1]*gslcc.mGeoTransform[3])/d;

		gslcc.mInverseGeoTransform[4] = -gslcc.mGeoTransform[4]/d;

		gslcc.mInverseGeoTransform[5] = gslcc.mGeoTransform[1]/d;

		mGeospecificLCCImages.push_back(gslcc);
	}
	else
	{
		Notify(WARN, "SOARXTerrain: Can't load %s", filename.c_str());
	}
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
 * none (in which case the roads will be baked into the base
 * texture)
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
			   else
				   Notify(WARN, "road geom NULL");
            }
         }

         OGREnvelope env;

         roads.mLayer->GetExtent(&env);

         roads.mMinLatitude = (int)floor(env.MinY);
         roads.mMaxLatitude = (int)floor(env.MaxY);
         roads.mMinLongitude = (int)floor(env.MinX);
         roads.mMaxLongitude = (int)floor(env.MaxX);

         if(texture != "")
         {
			Notify(NOTICE, "Reading road texture");
			osg::Image* image = osgDB::readImageFile(texture);

            if(image != NULL)
            {
				roads.mTexture = new osg::Texture2D(image);

               roads.mTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
               roads.mTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
            }
			else
				Notify(WARN, "Couldn't find road texture.");
         }

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
* Determine if we are using LCC data.
* @param LCCtype the LCC index type (e.g. "42" is evergreen forest)
* @param x the x coordinate to check
* @param y the y coordinate to check
* @param limit the limiting value of the probability (i.e. the roll of the dice)
* @return the value of mUseLCC
*/
bool SOARXTerrain::GetVegetation(osg::Image* mCimage, int x, int y, int limit)
{
	unsigned char* c_data = NULL;

	if (mCimage!=NULL) c_data = (unsigned char*)mCimage->data(x,y);
	else return false;

	if (((c_data[0] == c_data[1])) && ((c_data[1] == c_data[2])))
		return (c_data[0] <= limit);
	else
		return false;
}


/**
* Determines the Longitude & Latitude of a specified location.
* @param x the x coordinate to check
* @param y the y coordinate to check
* @return Vec2f (longitude, latitude)
*/
osg::Vec2f SOARXTerrain::GetLongLat(float x, float y)
{
   float latitude  = (y/semiMajorAxis)*SG_RADIANS_TO_DEGREES + mOriginLatitude,
         longitude = (x/semiMajorAxis)*SG_RADIANS_TO_DEGREES + mOriginLongitude;

   return (osg::Vec2f(longitude,latitude));
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

//   Notify(NOTICE, "Performing GetHeight calculation on Lat = %i, Long = %i", latitude, longitude);
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
* Determine if we are using LCC data.
*
* @return the value of mUseLCC
*/

bool SOARXTerrain::GetLCCStatus()
{
   return mUseLCC;
}



/**
* Create hit/miss map of the terrain by LCC type
* @param src_image the Base Color image with LCC encoded within
* @param rgb_selected the RGB color vector of the LCC type selected
* @return the newly created image
*/
osg::Image* SOARXTerrain::MakeLCCImage(osg::Image* src_image, osg::Vec3& rgb_selected)
{
	osg::Image* dst_image = new osg::Image;

	int width = src_image->s();
	int height = src_image->t();

	dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

	unsigned char* src_data = NULL;
	unsigned char* dst_data = NULL;

//	Notify(NOTICE, "RGB selected for LCC = %3.0f, %3.0f, %3.0f", rgb_selected[0], rgb_selected[1], rgb_selected[2]);

//	unsigned int hits =0;
//	unsigned int misses =0;

	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			src_data = (unsigned char*)src_image->data(x,y);
			dst_data = (unsigned char*)dst_image->data(x,y);

			if ((src_data[0] == rgb_selected[0]) &&
			   (src_data[1] == rgb_selected[1]) &&
			   (src_data[2] == rgb_selected[2])){

			   dst_data[0]=0.0;
			   dst_data[1]=0.0;
			   dst_data[2]=0.0;

//			   hits++;
			}
			else
			{
				dst_data[0]=255.0;
				dst_data[1]=255.0;
				dst_data[2]=255.0;

//				misses++;
			}

		//			Notify(NOTICE, "RGB for %i,%i = %3.0f, %3.0f, %3.0f", x, y, color[0], color[1], color[2]);
		}
	}
//	Notify(NOTICE, "hits = %i", hits);
//	Notify(NOTICE, "misses = %i", misses);

	return dst_image;
}


/**
* Create smoothed grayscale map of the terrain by LCC type
* Uses weighted next nearest neighbor to "fuzzy"-up the LCCImage data
* @param src_image the black/white LCC Image by LCC type
* @param rgb_selected the RGB color of the points to smooth (always 0,0,0 - black)
* @return the newly created image
*/
osg::Image* SOARXTerrain::MakeFilteredImage(osg::Image* src_image, osg::Vec3& rgb_selected)
{
	int width = src_image->s();
	int height = src_image->t();

	osg::Image* dst_image = new osg::Image;
	dst_image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

	unsigned char* src_data = NULL;
	unsigned char* dst_data = NULL;

//	Notify(NOTICE, "RGB filter selected = %3.0f, %3.0f, %3.0f", rgb_selected[0], rgb_selected[1], rgb_selected[2]);

	int border = 3;
	float value = 0;

	int neighbor_hits = 0;
	int next_neighbor_hits = 0;
	int third_neighbor_hits = 0;

	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			src_data = (unsigned char*)src_image->data(x,y);
			dst_data = (unsigned char*)dst_image->data(x,y);

			if ((y<border) || (x<border) || (y>height-border) || (x>width-border))
			{
				if ((src_data[0] == rgb_selected[0]) &&
					(src_data[1] == rgb_selected[1]) &&
					(src_data[2] == rgb_selected[2]))
					value = 100;
				else
					value = 0;
			}
			else
			{			//third nearest neighbor algorithm
				if ((src_data[0] == rgb_selected[0]) &&
					(src_data[1] == rgb_selected[1]) &&
					(src_data[2] == rgb_selected[2]))
					value = 50;
				else
					value = 0;

				neighbor_hits = 0;
				next_neighbor_hits = 0;
				third_neighbor_hits = 0;

				unsigned char* tmp_data = NULL;

				tmp_data = (unsigned char*)src_image->data(x,y-1);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x,y+1);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x-1,y);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x+1,y);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x-1,y-1);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					next_neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x-1,y+1);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					next_neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x+1,y-1);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					next_neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x+1,y+1);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					next_neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x-2,y);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					third_neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x+2,y);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					third_neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x,y-2);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					third_neighbor_hits++;

				tmp_data = (unsigned char*)src_image->data(x,y+2);
				if ((tmp_data[0] == rgb_selected[0]) &&
					(tmp_data[1] == rgb_selected[1]) &&
					(tmp_data[2] == rgb_selected[2]))
					third_neighbor_hits++;

				value = value +									//50 for getting a hit
						(6.82f * neighbor_hits) +				//6.82 for getting a neighbor hit
						(3.41f * next_neighbor_hits) +			//3.41 for getting a next neighbor hit
						(2.27f * third_neighbor_hits);			//2.27 for getting a third neighbor hit

				}

			dst_data[0]=abs(value/100.0*255.0 - 255.0);
			dst_data[1]=abs(value/100.0*255.0 - 255.0);
			dst_data[2]=abs(value/100.0*255.0 - 255.0);

		//			Notify(NOTICE, "RGB for %i,%i = %3.0f, %3.0f, %3.0f", x, y, dst_data[0], dst_data[1], dst_data[2]);

		}
	}

	return dst_image;
}


/**
* Create heightmap image
* @param hf the GDAL-derived heightfield
* @return the newly created image
*/
osg::Image* SOARXTerrain::MakeHeightmapImage(osg::HeightField* hf)
{
	osg::Image* image = new osg::Image;

	int width = hf->getNumColumns()-1,
		height = hf->getNumRows()-1;

	image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

	float myscale = 1.0/10.0;

	unsigned char* ptr = (unsigned char*)image->data();

	float scale = 128.0f/hf->getXInterval();

	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			//average four corners
			float h =
				 (hf->getHeight(x, y)
				+ hf->getHeight(x+1,y+1)
				+ hf->getHeight(x, y+1)
				+ hf->getHeight(x+1, y))*0.25f;

			// need to work on boundary conditions
/*			float h = hf->getHeight(x,y);

			if(h<0)   h=0.0f;

			if (((y==0)&&(x<2))||((y==0)&&(x>1020)))
				Notify(NOTICE, "HEIGHTS:(%i,%i: %10.0f ",x,y,h);

			if ((x==0) || (x== width-1)|| (y==0) || (y==height-1))
			{

			}
*/
			if ((h*myscale)<=0)
			{
				*(ptr++) = (unsigned char)0.0f;
				*(ptr++) = (unsigned char)0.0f;
				*(ptr++) = (unsigned char)255.0f;

			}else if ((h*myscale)>=255)
			{
				*(ptr++) = (unsigned char)0.0f;
				*(ptr++) = (unsigned char)255.0f;
				*(ptr++) = (unsigned char).0f;
			}else
			{
				*(ptr++) = (unsigned char)clamp(h*myscale, 0.0f, 255.0f);  //store height
				*(ptr++) = (unsigned char)clamp(h*myscale, 0.0f, 255.0f);  //store height
				*(ptr++) = (unsigned char)clamp(h*myscale, 0.0f, 255.0f);  //store height
			}
		}
	}

	image->ensureValidSizeForTexturing(mMaxTextureSize);

	return image;

}


/**
* Create slopemap from GDAL-derived heightfield data
* @param hf the GDAL-derived heightfield
* @return the newly created image
*/
osg::Image* SOARXTerrain::MakeRelativeElevationImage(osg::HeightField* hf)
{
	osg::Image* image = new osg::Image;

	int width = hf->getNumColumns()-1,
		height = hf->getNumRows()-1;

	image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

	unsigned char* ptr = (unsigned char*)image->data();

	float scale = 128.0f/hf->getXInterval();

	float myscale = 5.0f;

	float relative = 0.0f;
	float relativenorm = 0.0f;
	float h = 0.0f;
	float averageheight = 0.0f;

	//NEED TO IMPLEMENT BORDER CONDITION CASE

	for(int y=0;y<height;y++)
   {
      for(int x=0;x<width;x++)
      {
		  if ((y==0) || (x==0))
			  relative = 0.0f;
		  else
		  {
		    averageheight =
				( hf->getHeight(  x, y)
				+ hf->getHeight(x-1, y)
				+ hf->getHeight(x-1, y-1)
				+ hf->getHeight(x-1, y+1)
				+ hf->getHeight(x+1, y)
				+ hf->getHeight(x+1, y-1)
				+ hf->getHeight(x+1, y+1)
				+ hf->getHeight(  x, y-1)
				+ hf->getHeight(  x, y+1))/9.0f;

		    h = hf->getHeight(x, y);

			relative = h-averageheight;
			relativenorm = relative/(sqrt(relative*relative+1));
		  }

//			if ((y==1)&&(x<2))
//				Notify(NOTICE, "Relative:(%i,%i: h=%5.2f rel=%5.2f relnorm=%5.2f ",x,y,relative,relativenorm);

//			if ((y==1)&&(x>1020))
//				Notify(NOTICE, "Relative:(%i,%i: h=%5.2f rel=%5.2f relnorm=%5.2f ",x,y,relative,relativenorm);

//			if (((y==1)&&(x>1020))|| ((y==1)&&(x<2)))
//				Notify(NOTICE, " ");

/*			if ((relative*myscale+128.0f)<=0)
			{
				*(ptr++) = (unsigned char)0.0f;  //store height
				*(ptr++) = (unsigned char)255.0f;  //store height
				*(ptr++) = (unsigned char)0.0f;  //store height

			}else if ((relative*myscale+128.0f)>255)
			{
				*(ptr++) = (unsigned char)0.0f;  //store height
				*(ptr++) = (unsigned char)0.0f;  //store height
				*(ptr++) = (unsigned char)255.0f;  //store height
			}else
			{
*/				*(ptr++) = (unsigned char)clamp(relative*myscale+128.0f, 0.0f, 255.0f);  //store height
				*(ptr++) = (unsigned char)clamp(relative*myscale+128.0f, 0.0f, 255.0f);  //store height
				*(ptr++) = (unsigned char)clamp(relative*myscale+128.0f, 0.0f, 255.0f);  //store height
//			}

		}
	}

	image->ensureValidSizeForTexturing(mMaxTextureSize);

	return image;
}


/**
* Create relative elevation map from GDAL-derived heightfield data
* @param hf the GDAL-derived heightfield
* @return the newly created image
*/
osg::Image* SOARXTerrain::MakeSlopemapImage(osg::HeightField* hf)
{
	osg::Image* image = new osg::Image;

	int width = hf->getNumColumns()-1,
		height = hf->getNumRows()-1;

	image->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

	unsigned char* ptr = (unsigned char*)image->data();

	float scale = 128.0f/hf->getXInterval();

	float dx = hf->getXInterval();
	float dy = hf->getYInterval();

	float myscale = 256.0f;

	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			float h = hf->getHeight(x, y),
				gx = (h - hf->getHeight(x+1, y))/dx,
				gy = (h - hf->getHeight(x, y+1))/dy;

			float slope = sqrt((gx*gx)+(gy*gy));
			float slopenorm = slope/(sqrt(slope*slope+1));


			//need to work on boundary conditions
//			if ((y==1)&&(x<2))
//				Notify(NOTICE, "Slope:(%i,%i: gx=%5.2f gy=%5.2f slope=%5.2f slopenorm=%2.5f, scale = %3.3f",x,y,gx,gy,slope, slopenorm, dx);

//			if ((y==1)&&(x>1020))
//				Notify(NOTICE, "Slope:(%i,%i: gx=%5.2f gy=%5.2f slope=%5.2f slopenorm=%2.5f, scale = %3.3f",x,y,gx,gy,slope, slopenorm, dy);

//			if (((y==1)&&(x>1020))|| ((y==1)&&(x<2)))
//				Notify(NOTICE, " ");

			if ((slope*myscale)<=0)
			{
				*(ptr++) = (unsigned char)0.0f;  //store height
				*(ptr++) = (unsigned char)0.0f;  //store height
				*(ptr++) = (unsigned char)255.0f;  //store height

			}else if ((slope*myscale)>255)
			{
				*(ptr++) = (unsigned char)0.0f;  //store height
				*(ptr++) = (unsigned char)255.0f;  //store height
				*(ptr++) = (unsigned char)0.0f;  //store height
			}else
			{
				*(ptr++) = (unsigned char)clamp(slope*255.0f, 0.0f, 255.0f);  //store height
				*(ptr++) = (unsigned char)clamp(slope*255.0f, 0.0f, 255.0f);  //store height
				*(ptr++) = (unsigned char)clamp(slope*255.0f, 0.0f, 255.0f);  //store height
			}
		}
	}

	image->ensureValidSizeForTexturing(mMaxTextureSize);

	return image;
}



/**
* Create probability map of the likehihood for a particular LCC type
* @param LCCidx LCC image index
* @param h_image the heightmap image
* @param s_image the slopemap image
* @param r_image the relative elevation image
* @return the newly created image
*/
osg::Image* SOARXTerrain::MakeCombinedImage(
			 osg::Image* f_image,			// filtered LCC image
			 osg::Image* h_image,			// heightmap
			 osg::Image* s_image,			// slopemap image
			 osg::Image* r_image)  			// relative elevation image
{

	if (f_image == NULL) Notify(WARN, "missing filter image" );

	int max_height = 128;		// max height for trees
	int min_height = 1;			// min height for trees
	int max_slope = 170;		// example: .66 slope (60 deg) * 256 = 170

	int good_re = 0;
	int bad_re = 255;

	unsigned char* f_data = NULL;
	unsigned char* h_data = NULL;
	unsigned char* s_data = NULL;
	unsigned char* r_data = NULL;

	int im_width = f_image->s();
	int im_height = f_image->t();

	int hf_width = h_image->s();
	int hf_height = h_image->t();

	int scale = im_width/hf_width;

//	Notify(NOTICE, "scale = %i,  image width = %i,  hf width = %i", scale, im_width, hf_width);

	osg::Image* dst_image = new osg::Image;
	dst_image->allocateImage(im_width, im_height, 1, GL_RGB, GL_UNSIGNED_BYTE);
	unsigned char* dst_data = NULL;

//	Notify(NOTICE, "RGB filter selected = %3.0f, %3.0f, %3.0f", rgb_selected[0], rgb_selected[1], rgb_selected[2]);

	float value = 0;

//	int upperhit = 0;
//	int lowerhit = 0;

	for(int y=0;y<im_height;y++)
	{
		for(int x=0;x<im_width;x++)
		{
			dst_data = (unsigned char*)dst_image->data(x,y);
			f_data = (unsigned char*)f_image->data(x,y);
			h_data = (unsigned char*)h_image->data(x/scale, y/scale);
			s_data = (unsigned char*)s_image->data(x/scale, y/scale);
			r_data = (unsigned char*)r_image->data(x/scale, y/scale);

			//need to work on boundary conditions
//			if ((y<border) || (x<border) || (y>height-border) || (x>width-border))
//			{
//				if ((src_data[0] == rgb_selected[0]) &&
//					(src_data[1] == rgb_selected[1]) &&
//					(src_data[2] == rgb_selected[2]))
//					value = 100;
//				else
//					value = 0;
//			}
//			else
//			{			//third nearest neighbor

			value = f_data[0];									// start with filter data as basis

			if (value <= 254 )									// nonwhite has vegetation possibility
			{
				if (h_data[0] < min_height)						// busted height limits (do this as a curve)
					value = 999;
				if (h_data[0] > max_height)						// busted height limits (do this as a curve)
					value = 998;
				if (abs(s_data[0]) > max_slope)			// busted slope limit  (do this as a curve)
					value = 997;

				if (value <= 254)
				{
					float redelta = r_data[0] - 128.0f;
					if (redelta > 0)
					{						// upward relative elevation (unfavorable)
//						Notify(NOTICE, "upperhit = %i, %4.2f", ++upperhit,redelta);
						value = value + redelta/2.0f;
					}
					else
					{						// downward relevation elevation (favorable)
//						Notify(NOTICE, "lowerhit = %i, %4.2f", ++lowerhit, redelta);
						value = value + redelta/2.0f;
					}
					float sldelta = s_data[0]/255.0f;
						value = value + sldelta*100.0f;		//greater slope is unfavorable (linear)
				}
			}

			if (value == 999)							//below min elevation
			{
				dst_data[0] = (unsigned char) 0.0;
				dst_data[1] = (unsigned char) 0.0;
				dst_data[2] = (unsigned char) 255.0;
			}
			else if (value == 998)						//above max elevation
			{
				dst_data[0] = (unsigned char) 0.0;
				dst_data[1] = (unsigned char) 255.0;
				dst_data[2] = (unsigned char) 0.0;
			}
			else if (value == 997)						//slope too great
			{
				dst_data[0] = (unsigned char) 255.0;
				dst_data[1] = (unsigned char) 0.0;
				dst_data[2] = (unsigned char) 0.0;
			}
			else
			{
				dst_data[0] = (unsigned char)clamp(value, 0.0f, 255.0f);  //store height
				dst_data[1] = (unsigned char)clamp(value, 0.0f, 255.0f);  //store height
				dst_data[2] = (unsigned char)clamp(value, 0.0f, 255.0f);  //store height
			}

		//			Notify(NOTICE, "RGB for %i,%i = %3.0f, %3.0f, %3.0f", x, y, dst_data[0], dst_data[1], dst_data[2]);

		}
	}

	return dst_image;
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

   //NOTE: Currently hard-coded to 1024x1024 image
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
 * Draws an antialiased black pixel into the specified image.
 *
 * @param image the image into which to draw the pixel
 * @param x the x coordinate at which to draw the pixel
 * @param y the y coordinate at which to draw the pixel
 * @param weight the weight of the pixel
 */
inline void DrawRoadPixel(osg::Image* image, float x, float y, float weight)
{
   int fx = (int)floor(x),
       fy = (int)floor(y),
       cx = (int)ceil(x),
       cy = (int)ceil(y);

   if(fx >= 0 && cx < image->s() && fy >= 0 && cy < image->t())
   {
      unsigned char* p1 = image->data(fx, fy);
      unsigned char* p2 = image->data(cx, fy);
      unsigned char* p3 = image->data(fx, cy);
      unsigned char* p4 = image->data(cx, cy);

      float sw = x-fx,
            tw = y-fy,
            w1 = 1 - (1-sw)*(1-tw)*weight,
            w2 = 1 - sw*(1-tw)*weight,
            w3 = 1 - (1-sw)*tw*weight,
            w4 = 1 - sw*tw*weight;

      int i;

      for(i=0;i<3;i++) p1[i] *= w1;
      for(i=0;i<3;i++) p2[i] *= w2;
      for(i=0;i<3;i++) p3[i] *= w3;
      for(i=0;i<3;i++) p4[i] *= w4;
   }
}

/**
 * Draws an antialiased black line into the specified image.
 *
 * @param image the image to draw the line into
 * @param s1 the s coordinate of the line's start point
 * @param t1 the t coordinate of the line's start point
 * @param s2 the s coordinate of the line's end point
 * @param t2 the t coordinate of the line's end point
 * @param weight the weight of the line
 */
static void DrawRoadLine(osg::Image* image, float s1,
                         float t1, float s2,
                         float t2, float weight)
{
   float x1 = s1*image->s(),
         y1 = t1*image->t(),
         x2 = s2*image->s(),
         y2 = t2*image->t();

   if(osg::absolute(y2-y1) > osg::absolute(x2-x1))
   {
      if(y1 > y2)
      {
         swap(x1, x2);
         swap(y1, y2);
      }

      float x = x1, xStep = (x2 - x1)/(y2 - y1);

      for(float y = y1; y <= y2; y += 1.0f)
      {
         DrawRoadPixel(image, x, y, weight);

         x += xStep;
      }
   }
   else
   {
      if(x1 > x2)
      {
         swap(x1, x2);
         swap(y1, y2);
      }

      float y = y1, yStep = (y2 - y1)/(x2 - x1);

      for(float x = x1; x <= x2; x += 1.0f)
      {
         DrawRoadPixel(image, x, y, weight);

         y += yStep;
      }
   }
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
//<<<<<<< soarxterrain.cpp
   osg::Vec3 c1, c2, c3, c4, c12, c34, color, coord, imgcolor;
//=======
//   osg::Vec3 c1, c2, c3, c4, c12, c34, color, coord;
   //unsigned char* data;
//>>>>>>> 1.6

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
            int ix = (int)x, iy = (int)y;

            if(fx >= 0 && cx < (*it).mImage->s() && fy >= 0 && cy < (*it).mImage->t())
            {
               float ax = (float)(x - fx), ay = (float)(y - fy);

               unsigned char* data = (*it).mImage->data(ix, iy);

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

					imgcolor *= (l12*(1.0f-ay) + l34*ay); 
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

					imgcolor = c12*(1.0f-ay) + c34*ay;
               }

			   //let the heightmap pseudocolor override black/missing areas
			   if ((imgcolor[0]!=0)&&(imgcolor[1]!=0)&&(imgcolor[2]!=0))
			   {
				   color[0] = imgcolor[0];
				   color[1] = imgcolor[1];
				   color[2] = imgcolor[2];
			   }

			   imgcolor[0] = 0;
			   imgcolor[1] = 0;
			   imgcolor[2] = 0;

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

   for(vector<Roads>::iterator r = mRoads.begin();
       r != mRoads.end();
       r++)
   {
      if((*r).mTexture == NULL &&
         latitude >= (*r).mMinLatitude && latitude <= (*r).mMaxLatitude &&
         longitude >= (*r).mMinLongitude && longitude <= (*r).mMaxLongitude)
      {
         vector<OGRLineString*> lineStrings;

         for(OGRFeature* feature = (*r).mLayer->GetNextFeature();
             feature != NULL;
             feature = (*r).mLayer->GetNextFeature())
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

         float weight = ((*r).mWidth*image->s())/(SG_DEGREES_TO_RADIANS*semiMajorAxis);

         for(vector<OGRLineString*>::iterator ls = lineStrings.begin();
             ls != lineStrings.end();
             ls++)
         {
            int numPoints = (*ls)->getNumPoints(),
                numEdges = (*ls)->get_IsClosed() ? numPoints : numPoints - 1;

            for(int i=0;i<numEdges;i++)
            {
               (*ls)->getPoint(i, &ogrp1);
               (*ls)->getPoint((i+1)%numPoints, &ogrp2);

               float s1 = ogrp1.getX() - longitude,
                     t1 = ogrp1.getY() - latitude,
                     s2 = ogrp2.getX() - longitude,
                     t2 = ogrp2.getY() - latitude;

               DrawRoadLine(image, s1, t1, s2, t2, weight);
            }
         }
      }
   }

   return image;
}


/**
* Makes the base color texture map for the specified heightfield.
*
* @param hf the heightfield to process
* @param latitude the latitude of the terrain segment
* @param longitude the longitude of the terrain segment
* @return the newly created image
*/
osg::Image* SOARXTerrain::MakeBaseLCCColor(osg::HeightField* hf, int latitude, int longitude)
{
	vector<GeospecificImage> lccimages;

	vector<GeospecificImage>::iterator it;

	int width = hf->getNumColumns()-1,
		height = hf->getNumRows()-1;

	for(it = mGeospecificLCCImages.begin();
		it != mGeospecificLCCImages.end();
		it++)
	{
		if(latitude >= (*it).mMinLatitude && latitude <= (*it).mMaxLatitude &&
			longitude >= (*it).mMinLongitude && longitude <= (*it).mMaxLongitude)
		{
			lccimages.push_back(*it);

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

	osg::Image* lccimage = new osg::Image;

	lccimage->allocateImage(width, height, 1, GL_RGB, GL_UNSIGNED_BYTE);

	unsigned char* ptr = (unsigned char*)lccimage->data();

	float heightVal;
	osg::Vec3 c1, c2, c3, c4, c12, c34, color, coord;

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

			for(it = lccimages.begin();it != lccimages.end();it++)
			{
				double x = (*it).mInverseGeoTransform[0] +
					(*it).mInverseGeoTransform[1]*lon +
					(*it).mInverseGeoTransform[2]*lat,
					y = (*it).mInverseGeoTransform[3] +
					(*it).mInverseGeoTransform[4]*lon +
					(*it).mInverseGeoTransform[5]*lat;

				int fx = (int)floor(x), fy = (int)floor(y),
					cx = (int)ceil(x), cy = (int)ceil(y);
				int ix = (int)x, iy = (int)y;

				if(fx >= 0 && cx < (*it).mImage->s() && fy >= 0 && cy < (*it).mImage->t())
				{
					float ax = (float)(x - fx), ay = (float)(y - fy);

					unsigned char* data = (*it).mImage->data(ix, iy);

					if((*it).mImage->getPixelFormat() == GL_LUMINANCE)
					{
						color[0] *= (data[0]/255.0f);
						color[1] *= (data[0]/255.0f);
						color[2] *= (data[0]/255.0f);
					}
					else
					{
						color[0] = (data[0]/255.0f);
						color[1] = (data[1]/255.0f);
						color[2] = (data[2]/255.0f);
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

	for(vector<Roads>::iterator r = mRoads.begin();
		r != mRoads.end();
		r++)
	{
		if((*r).mTexture == NULL &&
			latitude >= (*r).mMinLatitude && latitude <= (*r).mMaxLatitude &&
			longitude >= (*r).mMinLongitude && longitude <= (*r).mMaxLongitude)
		{
			vector<OGRLineString*> lineStrings;

			for(OGRFeature* feature = (*r).mLayer->GetNextFeature();
				feature != NULL;
				feature = (*r).mLayer->GetNextFeature())
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

			float weight = ((*r).mWidth*lccimage->s())/(SG_DEGREES_TO_RADIANS*semiMajorAxis);

			for(vector<OGRLineString*>::iterator ls = lineStrings.begin();
				ls != lineStrings.end();
				ls++)
			{
				int numPoints = (*ls)->getNumPoints(),
					numEdges = (*ls)->get_IsClosed() ? numPoints : numPoints - 1;

				for(int i=0;i<numEdges;i++)
				{
					(*ls)->getPoint(i, &ogrp1);
					(*ls)->getPoint((i+1)%numPoints, &ogrp2);

					float s1 = ogrp1.getX() - longitude,
						t1 = ogrp1.getY() - latitude,
						s2 = ogrp2.getX() - longitude,
						t2 = ogrp2.getY() - latitude;

					DrawRoadLine(lccimage, s1, t1, s2, t2, weight);
				}
			}
		}
	}

	return lccimage;
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
      if((*it).mTexture != NULL &&
         latitude >= (*it).mMinLatitude && latitude <= (*it).mMaxLatitude &&
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
               static_cast<OGRLineString*>(feature->GetGeometryRef());

            OGRMultiLineString* mls =
               static_cast<OGRMultiLineString*>(feature->GetGeometryRef());

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
                  v1.mLocation[2] = GetHeight(origin[0]+v1.mLocation[0], origin[1]+v1.mLocation[1])+6.0;
                  v2.mLocation[2] = GetHeight(origin[0]+v2.mLocation[0], origin[1]+v2.mLocation[1])+6.0;

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

   static osg::HeightField* hf = NULL;

   for(int i=2;i>=0;i--)
   {
      char soarxName[64], dtedName[64];
      string soarxPath, dtedPath;
 //     osg::HeightField* hf = NULL;

	  hf = NULL;

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
            dtCore::RefPtr<osg::Image> detailGradient;
            dtCore::RefPtr<osg::Image> detailScale;

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

               osgDB::writeImageFile(*(detailGradient.get()), detailGradientPath);
            }

            if(osgDB::fileExists(detailScalePath))
            {
               detailScale = osgDB::readImageFile(detailScalePath);
            }
            else
            {
               Notify(NOTICE, "SOARXTerrain: Making detail scale image for level %d...", i);

               detailScale = MakeDetailScale(i);

               osgDB::writeImageFile(*(detailScale.get()), detailScalePath);
            }

            mDetailGradient[i] = new osg::Texture2D(detailGradient.get());

            mDetailGradient[i]->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
            mDetailGradient[i]->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

            mDetailScale[i] = new osg::Texture2D(detailScale.get());

            mDetailScale[i]->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
            mDetailScale[i]->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);
         }

         dtCore::RefPtr<osg::Image> baseGradient;
         dtCore::RefPtr<osg::Image> baseColor;

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
            osgDB::writeImageFile(*(baseGradient.get()), baseGradientPath);
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
            osgDB::writeImageFile(*(baseColor.get()), baseColorPath);
         }

		 if (mUseLCC)
		 {
			 dtCore::RefPtr<osg::Image> baseLCCColor;

			 string baseLCCColorPath = mCachePath + "/" + cellName + ".baselcc.color.bmp";

			 if(osgDB::fileExists(baseLCCColorPath))
			 {
				 baseLCCColor = osgDB::readImageFile(baseLCCColorPath);
			 }
			 else
			 {
				 Notify(NOTICE, "SOARXTerrain: Making base LCC color image for %s...", cellName);
				 baseLCCColor = MakeBaseLCCColor(hf, latitude, longitude);
				 baseLCCColor->ensureValidSizeForTexturing(mMaxTextureSize);
				 osgDB::writeImageFile(*(baseLCCColor.get()), baseLCCColorPath);
			 }
		 
//			 Notify(NOTICE, "SOARXTerrain: Making Land Cover Classification image for level %d...", i);

			 for(vector<LCCs>::iterator l = mLCCs.begin();
						l != mLCCs.end();
						l++)
			 {
						int idx = (*l).idx;

						char idxnum[3];
						sprintf(idxnum, "%i",idx);
//						cout << "index is .... " << idx << endl;
//						cout << "idxnum is .... " << idxnum << endl;

						string LCCfilterPath = mCachePath + "/" + cellName + ".lcc.filter." + idxnum +".bmp";

						if(osgDB::fileExists(LCCfilterPath))
						{
							//not really needed - the file exists!
							//LCCfilter = osgDB::readImageFile(LCCfilterPath);
						}
						else
						{
							dtCore::RefPtr<osg::Image> LCCimage;
							string LCCimagePath = mCachePath + "/" + cellName + ".lcc.image." + idxnum +".bmp";

							if(osgDB::fileExists(LCCimagePath))
							{
								LCCimage = osgDB::readImageFile(LCCimagePath);
							}
							else
							{
								Notify(NOTICE, "SOARXTerrain: Making LCC image for LCC type %s...", idxnum);
								osg::Vec3 selectedRGB((*l).rgb[0],(*l).rgb[1],(*l).rgb[2]);	
								LCCimage = MakeLCCImage(baseLCCColor.get(), selectedRGB);
								LCCimage->ensureValidSizeForTexturing(mMaxTextureSize);
								osgDB::writeImageFile(*(LCCimage.get()), LCCimagePath);
							}

							dtCore::RefPtr<osg::Image> LCCfilter;
							Notify(NOTICE, "SOARXTerrain: Making LCC smoothed image for LCC type %s...", idxnum);
							osg::Vec3 filterRGB(0.0,0.0,0.0);					//select black
							LCCfilter = MakeFilteredImage((LCCimage.get()), filterRGB);
							LCCfilter->ensureValidSizeForTexturing(mMaxTextureSize);
							osgDB::writeImageFile(*(LCCfilter.get()), LCCfilterPath);

							LCCimage = NULL;
							LCCfilter = NULL;

//							dtCore::RefPtr<osgDB::ImageOptions> options = new osgDB::ImageOptions;
//							options->_destinationImageWindowMode = osgDB::ImageOptions::PIXEL_WINDOW;
//							options->_destinationPixelWindow.set(0,0,mMaxTextureSize,mMaxTextureSize);
//							osgDB::Registry::instance()->setOptions(options.get());

//							LCCfilter = osgDB::readImageFile(LCCfilterPath);
//							ImageStats(LCCfilter.get(), "LCCFilterread");

						}


			 }  // end iterator loop


			if(hf != NULL)   // this is normal
			{
				//Notify(NOTICE, "HeightField hf not NULL - this is good!");
				//Notify(NOTICE, "hf columns = %i  rows = %i", hf->getNumColumns(),hf->getNumRows());

				osg::Image* HFimage;
				string HFimagePath = mCachePath + "/" + cellName + ".hf.image.bmp";
				if(osgDB::fileExists(HFimagePath))
				{
//					HFimage = osgDB::readImageFile(HFimagePath);
				}
				else
				{
					Notify(NOTICE, "SOARXTerrain: Making heightmap image for level %d...", i);
					HFimage = MakeHeightmapImage(hf);
					HFimage->ensureValidSizeForTexturing(mMaxTextureSize);
					osgDB::writeImageFile(*HFimage, HFimagePath);
				}

				osg::Image* SLimage;
				string SLimagePath = mCachePath + "/" + cellName + ".sl.image.bmp";
				if(osgDB::fileExists(SLimagePath))
				{
//					SLimage = osgDB::readImageFile(SLimagePath);
				}
				else
				{
					Notify(NOTICE, "SOARXTerrain: Making slopemap image for level %d...", i);
					SLimage = MakeSlopemapImage(hf);
					SLimage->ensureValidSizeForTexturing(mMaxTextureSize);
					osgDB::writeImageFile(*SLimage, SLimagePath);
				}

				osg::Image* REimage;
				string REimagePath = mCachePath + "/" + cellName + ".re.image.bmp";
				if(osgDB::fileExists(REimagePath))
				{
//					REimage = osgDB::readImageFile(REimagePath);
				}
				else
				{
					Notify(NOTICE, "SOARXTerrain: Making relative elevation image for level %d...", i);
					REimage = MakeRelativeElevationImage(hf);
					REimage->ensureValidSizeForTexturing(mMaxTextureSize);
					osgDB::writeImageFile(*REimage, REimagePath);
				}

				for(vector<LCCs>::iterator l = mLCCs.begin();
					l != mLCCs.end();
					l++)
				{
					int idx = (*l).idx;
//					cout << "index is .... " << idx  <<endl;;

						char idxnum[3];
						sprintf(idxnum, "%i",idx);

						string CimagePath = mCachePath + "/" + cellName + ".c.image." + idxnum +".bmp";
						string LCCfilterPath = mCachePath + "/" + cellName + ".lcc.filter." + idxnum +".bmp";

						if(osgDB::fileExists(CimagePath))
						{
							Notify(NOTICE, "SOARXTerrain: Probability map for LCC type %i exists.",idx);
							//mCimage[LCCindices[k]] = osgDB::readImageFile(CimagePath);
						}
						else
						{
							dtCore::RefPtr<osg::Image> LCCfilter;
//							osg::Image* LCCfilter = new osg::Image;
//							LCCfilter->allocateImage(mMaxTextureSize, mMaxTextureSize, 1, GL_RGB, GL_UNSIGNED_BYTE);

							dtCore::RefPtr<osgDB::ImageOptions> options = new osgDB::ImageOptions;
							options->_destinationImageWindowMode = osgDB::ImageOptions::PIXEL_WINDOW;
							options->_destinationPixelWindow.set(0,0,mMaxTextureSize,mMaxTextureSize);
							osgDB::Registry::instance()->setOptions(options.get());

							LCCfilter = osgDB::readImageFile(LCCfilterPath);

//							Notify(NOTICE, "filter image sizes = %i", LCCfilter->s());
//							Notify(NOTICE, "mMaxTextSize = %i", mMaxTextureSize);

							dtCore::RefPtr<osg::Image> mCimage;
							Notify(NOTICE, "SOARXTerrain: Making probability map for LCC type %i.",idx);
							mCimage = MakeCombinedImage(LCCfilter.get(), HFimage, SLimage, REimage);
							mCimage->ensureValidSizeForTexturing(mMaxTextureSize);
							osgDB::writeImageFile(*(mCimage.get()), CimagePath);
							mCimage=NULL;
							LCCfilter=NULL;

				//			LCCHistogram(LCCimage,SLimage,"sl.txt",5);
				//			LCCHistogram(LCCimage,HFimage,"hf.txt",5);
				//			LCCHistogram(LCCimage,REimage,"re.txt",5);
						}
					
				}
			}
			else			//HF is NULL - this is a potentially bad thing - relying on pre-created images
			{
				//Notify(NOTICE, "HeightField hf is NULL - danger!");

				for(vector<LCCs>::iterator l = mLCCs.begin();
					l != mLCCs.end();
					l++)
				{
					int idx = (*l).idx;
//					cout << "index is .... " << idx << endl;

					char idxnum[3];
					sprintf(idxnum, "%i",idx);

						string CimagePath = mCachePath + "/" + cellName + ".c.image." + idxnum +".bmp";

						if(osgDB::fileExists(CimagePath))
						{
//							Notify(NOTICE, "SOARXTerrain: Combined image for LCC type %i for level %d exists.",idx, i);
							//mCimage[LCCindices[k]] = osgDB::readImageFile(CimagePath);
						}
						else
						{
							Notify(NOTICE, "Error.  Exitting.");
							exit(-42);
						}
					
				}
			}  //end else HF is NULL
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

         osg::Texture2D* baseGradientTex = new osg::Texture2D(baseGradient.get());

         baseGradientTex->setWrap(osg::Texture::WRAP_S, osg::Texture::MIRROR);
         baseGradientTex->setWrap(osg::Texture::WRAP_T, osg::Texture::MIRROR);

         ss->setTextureAttributeAndModes(2, baseGradientTex);

         osg::Texture2D* baseColorTex = new osg::Texture2D(baseColor.get());

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

         if(mRoads.size() > 0)
         {
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
         }

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



/**
* Sets if LCC objects are updated/culled/rendered.
* @param mask 0/false=hide, 1/true=display
*/
void SOARXTerrain::SetLCCVisibility(bool mask)
{
	if (mSegmentLCCCellMap.size() > 0)
	{
		for(map<Segment, LCCCells*>::iterator it = mSegmentLCCCellMap.begin();
			it != mSegmentLCCCellMap.end();
			it++)
		{
			(*it).second->mRootVegeGroup->setNodeMask(mask);
		}
	}
	else
	{
		Notify(WARN, "LCC not loaded");
	}
}


/**
* Loads the specified configuration file.
*
* @param filename the name of the configuration file to load
*/
bool SOARXTerrain::LoadLCCConfiguration(string filename)
{
	TiXmlDocument lccconfig;

	if(lccconfig.LoadFile(filename.c_str()))
	{
		ParseLCCConfiguration(lccconfig.RootElement());

		return true;
	}
	else
	{
		Notify(WARN, "Can't load lccdata.xml");
		return false;
	}
}
/**
	* Parses the specified XML configuration element.
	*
	* @param configElement the configuration element to parse
	*/
void SOARXTerrain::ParseLCCConfiguration(TiXmlElement* configElement)
{
	const char* str;

	for(TiXmlElement* element = configElement->FirstChildElement();
		element != NULL;
		element = element->NextSiblingElement())
	{

		if(!strcmp(element->Value(), "LCCType"))
		{
			LCCs lcc;

			if((str = element->Attribute("index")) != NULL)
			{
				lcc.idx = atoi(str);
			}
			if((str = element->Attribute("R")) != NULL)
			{
				lcc.rgb[0]=atoi(str);
			}
			if((str = element->Attribute("G")) != NULL)
			{
				lcc.rgb[1]=atoi(str);
			}
			if((str = element->Attribute("B")) != NULL)
			{
				lcc.rgb[2]=atoi(str);
			}
			if((str = element->Attribute("Name")) != NULL)
			{
				lcc.name = str;
			}
			if((str = element->Attribute("Model")) != NULL)
			{
				lcc.model = str;
			}

/*			Notify(WARN, "idx = %i, r=%i, g=%i, b=%i, %s, %s",
				lcc.idx,
				lcc.rgb[0],
				lcc.rgb[1],
				lcc.rgb[2],
				lcc.name.c_str(),
				lcc.model.c_str());
*/
			if (lcc.model != "none")
			{
				mLCCs.push_back(lcc);
			}
		}

	}

}



/**
* Place the vegetation into cell specified by lat-long.
*
* @param latitude the latitude of the origin
* @param longitude the longitude of the origin
*/
void SOARXTerrain::AddVegetation(int latitude, int longitude)
{
	Segment segment(latitude, longitude);

	if(mSegmentLCCCellMap.count(segment) > 0)
	{
		return;
	}

	LCCCells* lcccell = new SOARXTerrain::LCCCells;

	//	cout << latitude << "," << longitude << endl;

	char latString[20], longString[20];
	sprintf(longString, "%c%03d", longitude < 0 ? 'w' : 'e', osg::absolute(longitude));
	sprintf(latString, "%c%02d", latitude < 0 ? 's' : 'n', osg::absolute(latitude));

	char filename[64];
	int ii = 1;
	sprintf(filename, "%s_%s_%d", longString, latString, ii);
	string myfilename = filename;

	for(int i=2;i>=0;i--)
	{
		char soarxName[64];
		string soarxPath;
		sprintf(soarxName, "%s_%s_%d.base.t", longString, latString, i);

		soarxPath = osgDB::findFileInDirectory(
			soarxName,
			mCachePath,
			osgDB::CASE_INSENSITIVE
			);

		//		cout << soarxName << endl;
		//		cout << soarxPath << endl;

		if(osgDB::fileExists(soarxPath))
		{
			char cellName[64];
			sprintf(cellName, "%s_%s_%d", longString, latString, i);
			// Place objects after terrain initialized

			lcccell->mRootVegeGroup = new osg::Group();		
//			lcccell->mRootVegeGroup->setUpdateCallback(new VegeUpdateCallback);
			lcccell->mRootVegeGroup->setCullCallback(new VegeCullCallback);
			lcccell->mRootVegeGroup->setName("mRootVegeGroup");
			lcccell->mRootVegeGroup->setDataVariance(osg::Object::STATIC);
			mNode->addChild(lcccell->mRootVegeGroup.get());

			//create quadtree
			char myname[20];
			osg::Group* QuadGroup2[4];
			for (int i = 0; i < 4; i++)
			{
				sprintf(myname, "QuadGroup2[%i]",i);
				QuadGroup2[i] = new osg::Group;
				QuadGroup2[i]->setName(myname);
//				QuadGroup2[i]->setUpdateCallback(new VegeUpdateCallback);
				QuadGroup2[i]->setCullCallback(new VegeCullCallback);
				QuadGroup2[i]->dirtyBound();
				lcccell->mRootVegeGroup.get()->addChild(QuadGroup2[i]);
			}
			osg::Group* QuadGroup4[16];
			for (int i = 0; i < 16; i++)
			{
				sprintf(myname, "QuadGroup4[%i]",i);
				QuadGroup4[i] = new osg::Group;
				QuadGroup4[i]->setName(myname);
//				QuadGroup4[i]->setUpdateCallback(new VegeUpdateCallback);
				QuadGroup4[i]->setCullCallback(new VegeCullCallback);
				QuadGroup4[i]->dirtyBound();
				QuadGroup2[i/4]->addChild(QuadGroup4[i]);
			}
			osg::Group* QuadGroup8[64];
			for (int i = 0; i < 64; i++)
			{
				sprintf(myname, "QuadGroup8[%i]",i);
				QuadGroup8[i] = new osg::Group;
				QuadGroup8[i]->setName(myname);
//				QuadGroup8[i]->setUpdateCallback(new VegeUpdateCallback);
				QuadGroup8[i]->setCullCallback(new VegeCullCallback);
				QuadGroup8[i]->dirtyBound();
				QuadGroup4[i/4]->addChild(QuadGroup8[i]);
			}
			osg::Group* QuadGroup16[256];
			for (int i = 0; i < 256; i++)
			{
				sprintf(myname, "QuadGroup16[%i]",i);
				QuadGroup16[i] = new osg::Group;
				QuadGroup16[i]->setName(myname);
//				QuadGroup16[i]->setUpdateCallback(new VegeUpdateCallback);
				QuadGroup16[i]->setCullCallback(new VegeCullCallback);
				QuadGroup16[i]->dirtyBound();
				QuadGroup8[i/4]->addChild(QuadGroup16[i]);
			}
			osg::Group* QuadGroup32[1024];
			for (int i = 0; i < 1024; i++)
			{
				sprintf(myname, "QuadGroup32[%i]",i);
				QuadGroup32[i] = new osg::Group;
				QuadGroup32[i]->setName(myname);
//				QuadGroup32[i]->setUpdateCallback(new VegeUpdateCallback);
				QuadGroup32[i]->setCullCallback(new VegeCullCallback);
				QuadGroup32[i]->dirtyBound();
				QuadGroup16[i/4]->addChild(QuadGroup32[i]);
			}


			//constants
			const int MAX_VEGE_PER_GROUP = 100;
			const int MAX_VEGE_PER_CELL = 400000;
			const int LOOKS_PER_PIXEL_X = 1;
			const int LOOKS_PER_PIXEL_Y = 1;
			const int MAX_VEGE_GROUPS_PER_CELL = 4096;

			// do once variables
			float deltaz = 0.0f;
//			float deltaz = 40.0f;
			float random_x = 0.0f, random_y=0.0f;
			//		  const int probabilitylimit = 170;  //  170/256=66%
			const int probabilitylimit = 128;  //  128/256=50%
			int multiplier = mMaxTextureSize/1024;

			//set up random number seed
			srand(static_cast<unsigned>(time(NULL)));

			//find cell origin
			osg::Vec3 cellorigin(
				(longitude - mOriginLongitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
				(latitude - mOriginLatitude)*SG_DEGREES_TO_RADIANS*semiMajorAxis,
				-mOriginElevation
				);
			cout<< cellorigin[0] << "," << cellorigin[1] << "," << cellorigin[2] << endl;


			//create LeafGroups (these hold a bunch of objects in the smallest atomic unit)
			osg::Group* LeafGroup[MAX_VEGE_GROUPS_PER_CELL];
			for (int vg = 0; vg < MAX_VEGE_GROUPS_PER_CELL; vg++)
			{
				sprintf(myname, "LeafGroup[%i]",vg);
				LeafGroup[vg] = new osg::Group();
				LeafGroup[vg]->setName(myname);
				LeafGroup[vg]->setCullingActive(true);
				LeafGroup[vg]->setDataVariance(osg::Object::STATIC);
				LeafGroup[vg]->setCullCallback(new VegeCullCallback);

			}


			for(vector<LCCs>::iterator l = mLCCs.begin();		//cycle through all the LCC types
				l != mLCCs.end();
				l++)
			{
				if ((*l).model!="none")		//make sure we've named a model to use
				{
					int vegecount = 0;				
					
					(*l).vegeObject = static_cast<osg::Group*> 
						(osgDB::readNodeFile((*l).model));
					(*l).vegeObject->setDataVariance(osg::Object::STATIC);
					mGroupies.push_back((*l).vegeObject);

					Notify(NOTICE, "Placing LCCtype %i '%s'....",(*l).idx, (*l).name.c_str());

					//create object
//					dtCore::RefPtr<osg::Object> obj = new osg::Object(name);
//					obj.get()->setName("placed object "+name);

					//load up model
//					Notify(DEBUG_INFO, "Object:Loading %s...", (*l).model.c_str());
//					osgDB::Registry::CacheHintOptions cache = osgDB::Registry::CACHE_ALL;
//					osg::Node *model = osgDB::readNodeFile(mFilename, cache);
//					if (model != NULL)
//					{
//						if (obj.get()->getNumChildren() != 0)
//						{
//							obj.get()->removeChild(0,obj.get()->getNumChildren() );
//						}
//						obj.get()->AddChild( model );
//					}
//					else
//					{
//						Notify(WARN, "Object: Can't load %s", (*l).model.c_str());
//						break;
//					}

					// identify which probability map we'll be using
					char idxnum[3];
					sprintf(idxnum, "%i",(*l).idx);
					string mCimagePath = mCachePath + "/" + cellName + ".c.image." + idxnum +".bmp";
					//load up that probability map
					dtCore::RefPtr<osgDB::ImageOptions> options = new osgDB::ImageOptions;
					options->_destinationImageWindowMode = osgDB::ImageOptions::PIXEL_WINDOW;
					options->_destinationPixelWindow.set(0,0,mMaxTextureSize,mMaxTextureSize);
					osgDB::Registry::instance()->setOptions(options.get());
					dtCore::RefPtr<osg::Image> mCimage;
					mCimage = osgDB::readImageFile(mCimagePath);

					for (int y = 0; y < 1024; y++)
					{
						for (int my = 0; my < LOOKS_PER_PIXEL_Y; my++)
						{
							for (int x = 0; x < 1024; x++)
							{
								for (int mx = 0; mx <LOOKS_PER_PIXEL_X; mx++)
								{
									// deterministic version - good for debugging
									//if (GetVegetation(mCimage.get(), x*multiplier,y*multiplier,probabilitylimit) && (vegecount < MAX_VEGE_PER_CELL))
										// stochastic version	
									if (GetVegetation(mCimage.get(), x*multiplier,y*multiplier,255.0f*(float)rand()/(RAND_MAX+1.0f)) && (vegecount < MAX_VEGE_PER_CELL))
									{
										vg = (int)(x/16)+((y/16)*16);
										random_x = (float)rand()/(RAND_MAX+1.0f)-0.5f;
										random_y = (float)rand()/(RAND_MAX+1.0f)-0.5f;

										// Position of plant:
										osg::Vec3 vegePosit;

										// Initialize tranform to be used for positioning the plant
										// assign values for the transform
										osg::PositionAttitudeTransform* vegeXform = new osg::PositionAttitudeTransform();
										vegeXform->setName("vegeXform");
										//vegeXform->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);

										float tempx = cellorigin[0]+(x+random_x*0.75f)*108.7+108.7/LOOKS_PER_PIXEL_X*mx; 
										float tempy = cellorigin[1]+(y+random_y*0.75f)*108.7+108.7/LOOKS_PER_PIXEL_Y*my; 
										vegePosit.set(tempx,tempy,GetHeight(tempx,tempy)+deltaz);
										
										osg::Quat attitude;
										attitude.makeRotate(3.14, osg::Vec3(0, 0, 1));
										vegeXform->setAttitude(attitude);


										float random_scale_x = (float)rand()/(RAND_MAX+1.0f)*0.5f;
//										float random_scale_y = (float)rand()/(RAND_MAX+1.0f)*0.5f;
//										float random_scale_z = (float)rand()/(RAND_MAX+1.0f)*0.5f;

//										osg::Vec3 vegeScale(3.0f,3.0f,3.0f);
										osg::Vec3 vegeScale(.5f+random_scale_x,.5f+random_scale_x,.5f+random_scale_x);
										vegeXform->setScale(vegeScale);

										// Set the position and add the vegeobject to the PAT
										vegeXform->setPosition(vegePosit);
										vegeXform->addChild((*l).vegeObject);

										LeafGroup[vg]->addChild(vegeXform);

										lcccell->mPATs.push_back(vegeXform);  

										vegecount++;
									}//getvegetation
								}//mx
							}//x
						}//my
					}//y
					mCimage=NULL;

					Notify(NOTICE, "%s count = %i", (*l).name.c_str(), vegecount);
				}

			}

			//stats counters
			int groupcount = 0;
			int maxchildren = 0;
			int minchildren = 999;
			int vegechild = 0;					

			//only add in groups that are non-empty
			for (int vg = 0; vg < MAX_VEGE_GROUPS_PER_CELL; vg++)
			{
				vegechild = LeafGroup[vg]->getNumChildren();

				if (vegechild != 0)
				{
					if (vegechild > maxchildren)	maxchildren = vegechild;
					if (vegechild < minchildren)	minchildren = vegechild;

					//	Notify(NOTICE, "%i, %i", vg, vegechild);
					//	Notify(NOTICE, "Adding VegeGround #%i:%i to Scene; max = %i, min = %i", groupcount++, vg, maxchildren, minchildren);

					QuadGroup32[vg/4]->addChild(LeafGroup[vg]);
				}
				else
					LeafGroup[vg] = NULL;
			}

			//delete empty nodes
			for (int i = 0; i < 1024; i++)
			{
				if(QuadGroup32[i]->getNumChildren() == 0)
				{
					QuadGroup16[i/4]->removeChild(QuadGroup32[i]);
					QuadGroup32[i]=NULL;
				}
				else
					mGroupies.push_back(QuadGroup32[i]);
			}
			for (int i = 0; i < 256; i++)
			{
				if(QuadGroup16[i]->getNumChildren() == 0)
				{
					QuadGroup8[i/4]->removeChild(QuadGroup16[i]);
					QuadGroup16[i] = NULL;
				}
				else
					mGroupies.push_back(QuadGroup16[i]);
			}
			for (int i = 0; i < 64; i++)
			{
				if(QuadGroup8[i]->getNumChildren() == 0)
				{
					QuadGroup4[i/4]->removeChild(QuadGroup8[i]);
					QuadGroup8[i]=NULL;
				}
				else
					mGroupies.push_back(QuadGroup8[i]);
			}
			for (int i = 0; i < 16; i++)
			{
				if(QuadGroup4[i]->getNumChildren() == 0)
				{
					QuadGroup2[i/4]->removeChild(QuadGroup4[i]);
					QuadGroup4[i]=NULL;
				}
				else
					mGroupies.push_back(QuadGroup4[i]);
			}
			for (int i = 0; i < 4; i++)
			{
				if(QuadGroup2[i]->getNumChildren() == 0)
				{
					lcccell->mRootVegeGroup.get()->removeChild(QuadGroup2[i]);
					QuadGroup2[i]=NULL;
				}
				else
					mGroupies.push_back(QuadGroup2[i]);
			}

			Notify(NOTICE, "Placement Done!");

			osgUtil::Optimizer optimzer;
//			optimzer.optimize(lcccell->mRootVegeGroup.get());
			optimzer.optimize(lcccell->mRootVegeGroup.get(),
				osgUtil::Optimizer::SHARE_DUPLICATE_STATE |
				osgUtil::Optimizer::MERGE_GEOMETRY |
				osgUtil::Optimizer::REMOVE_REDUNDANT_NODES |
				//				osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS |
				osgUtil::Optimizer::CHECK_GEOMETRY);
			Notify(NOTICE, "Optimization Done!");

			//	lcc->mRootVegeGroup.get()->setCullingActive(true);
		}
	}

	mSegmentLCCCellMap[segment] = lcccell;
}

/*
long distance(int dx, int dy )
{
	unsigned int min, max;   if ( dx < 0 ) dx = -dx;
	if ( dy < 0 ) dy = -dy;   if ( dx < dy )
	{
		min = dx;
		max = dy;
	} else {
		min = dy;
		max = dx;
	}   // coefficients equivalent to ( 123/128 * max ) and ( 51/128 * min )
//	return ((( max << 8 ) + ( max << 3 ) - ( max << 4 ) - ( max << 1 ) +
//		( min << 7 ) - ( min << 5 ) + ( min << 3 ) - ( min << 1 )) >> 8 );
	double inside = double((dx*dx)+(dy*dy));
	return sqrt(inside);
}

void SOARXTerrain::WhosActive(sgVec3& xyz)
{
	vector<osg::Group*>::iterator it;

	if (mGroupies.size()!=0)
		for(it = mGroupies.begin();
			it != mGroupies.end();
			it++)
		{
			osg::BoundingSphere BS = (*it)->getBound();

			double dist = distance(int(BS._center[0]-xyz[0]), int(BS._center[1]-xyz[1]));

			if (BS._radius == -1)
			{
				(*it)->setNodeMask(0);
				Notify(WARN, "BS = -1");
			}
			else if (BS._radius+25000.0f < dist)
			{
				Notify(WARN, "%s is NodeMasked 0: dist", (*it)->getName().c_str());
				(*it)->setNodeMask(0);
			}
			else
			{
				Notify(WARN, "%s:bb = %5.2f, bb+25k = %5.2f dist = %5.2f",  
					(*it)->getName().c_str(),
					BS._radius, 
					BS._radius+25000.0f,
					dist);
				(*it)->setNodeMask(1);
			}
		}
	else
		Notify(WARN, "size = 0");

}
*/