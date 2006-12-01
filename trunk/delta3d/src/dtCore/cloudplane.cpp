#include <prefix/dtcoreprefix-src.h>
#include <dtCore/cloudplane.h>
#include <dtCore/moveearthtransform.h>
#include <dtCore/system.h>
#include <dtUtil/noisetexture.h>
#include <dtUtil/log.h>

#include <osg/BlendFunc>
#include <osg/Fog>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/TexMat>
#include <osg/Vec2>
#include <osg/Vec3>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

const int MAX_HEIGHT = 2000;

using namespace dtCore;
IMPLEMENT_MANAGEMENT_LAYER(CloudPlane)

CloudPlane::CloudPlane( int   octaves,
                        float cutoff,
                        int   frequency,
                        float amp,
                        float persistence,
                        float density,
                        int   texSize,
                        float height,
                        const std::string& name,
                        const std::string& textureFilePath)
   : EnvEffect(name),
     mOctaves(octaves),
     mCutoff(cutoff),
     mFrequency(frequency),
     mAmplitude(amp),
     mPersistence(persistence),
     mDensity(density),
     mHeight(height),
     mTexSize(texSize),
     mWind(NULL),
     mCloudColor(NULL),
     mTexCoords(NULL),
     mColors(NULL)
{
	RegisterInstance(this);
	if(mHeight > MAX_HEIGHT)
		mHeight = MAX_HEIGHT;

	SetOSGNode(new osg::Group);
   GetOSGNode()->setNodeMask(0xf0000000);

	Create(textureFilePath);
	AddSender(&System::GetInstance());
}

CloudPlane::~CloudPlane()
{
	DeregisterInstance(this);
   RemoveSender(&System::GetInstance());
}

///Save generated texture to file
///@return success of save
bool CloudPlane::SaveTexture(const std::string &textureFilePath)
{
   if(!mImage.valid()) 
   { 
      return false; 
   }

   return osgDB::writeImageFile(*mImage, textureFilePath);
}

///Load generated texture from file
///@return success of load
bool CloudPlane::LoadTexture(const std::string &textureFilePath)
{
   osg::Image* image = osgDB::readImageFile(textureFilePath);
   if(image != NULL)
   {
      mImage = image;
      return true;
   }
   else
   {
      return false;
   }
}

osg::Texture2D* CloudPlane::CreateCloudTexture( const std::string& filename )
{
   osg::Texture2D* texture = NULL;
   mImage = filename == "" ? NULL : osgDB::readImageFile(filename);

   if( ! mImage.valid() )
   {
      LOG_DEBUG("Creating 2D cloud texture..." );	
      texture = createPerlinTexture();
      if( texture == NULL )
      {
         LOG_ERROR("Could not write 2D cloud texture to file." + filename );	
      }
   }
   else
   {
      texture = new osg::Texture2D(mImage.get());
   }
   return texture;
}


osg::Texture2D* CloudPlane::createPerlinTexture()
{

   //float bias = 1.5f;

   dtUtil::NoiseTexture noise2d(mOctaves, mFrequency, mAmplitude, mPersistence, mTexSize, mTexSize);
	mImage = noise2d.MakeNoiseTexture(GL_ALPHA);

    // Exponentiation of the image
    unsigned char *dataPtr = mImage->data();
    unsigned char data;
    
    for (int j = 0; j < mTexSize; ++j)
    {
        for (int k = 0; k < mTexSize; ++k)
        {
            data = *(dataPtr);

            if(data < mCutoff * 255)
                data = 0;
            else
                data -= (unsigned char)(mCutoff * 255);

            data = 255 - (unsigned char) (pow(mDensity, data) * 255);
            
            //if(data > 255) data = 255; //not needed?
            
            *(dataPtr++) = data;
        }
    }

 	return new osg::Texture2D(mImage.get());
}


void CloudPlane::Create(const std::string& textureFilePath) 
{
	mXform = new MoveEarthySkyWithEyePointTransform();
	mXform->setCullingActive(false);

	mGeode = new osg::Geode();
	mGeode->setName("CloudPlane");

	mCloudColor = new osg::Vec4;

	mWind = new osg::Vec2(.05f / mHeight, .05f/ mHeight);

	int planeSize = 20000;

	mPlane = createPlane(planeSize, mHeight);
	osg::StateSet *stateset = mPlane->getOrCreateStateSet();

   mCloudTexture = CreateCloudTexture(textureFilePath);
   stateset->setTextureAttributeAndModes(0, mCloudTexture.get());

	// Texture filtering
	mCloudTexture->setUseHardwareMipMapGeneration(true);
	mCloudTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	mCloudTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	mCloudTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	mCloudTexture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

   mTexMat = new osg::TexMat();
   stateset->setTextureAttribute(0, mTexMat.get());

	// Texture Blending
	osg::TexEnv* texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::BLEND);
	stateset->setTextureAttribute(0, texenv);

	// Transparent Bin
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   //stateset->setRenderBinDetails(9, "Test Transparent Bin");
   
	// Blending Function
	osg::BlendFunc *trans = new osg::BlendFunc();
	trans->setFunction(osg::BlendFunc::SRC_ALPHA ,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
   stateset->setAttributeAndModes(trans);
   
	// Add fog - Every EnvEffect must do it itself, if we want fog enabled for it
	mFog = new osg::Fog();
	mFog->setMode(osg::Fog::LINEAR);
	stateset->setAttributeAndModes(mFog.get());
	stateset->setMode( GL_FOG, osg::StateAttribute::ON );


	mGeode->addDrawable(mPlane.get());

	mXform->addChild(mGeode.get());
   GetOSGNode()->asGroup()->addChild(mXform.get());

   //init the colors to something believable
   osg::Vec3 sky( 1.0f, 1.0f, 1.0f );
   osg::Vec3 fogColor( 1.0f, 1.0f, 1.0f );
   Repaint( sky, fogColor, 45.0, 45.0, 10000.0 );
}

void CloudPlane::Repaint(  const osg::Vec3& skyColor, 
                           const osg::Vec3& fogColor,
                           double sunAngle, 
                           double sunAzimuth,
                           double visibility)
{
	mFog->setColor( osg::Vec4(fogColor[0], fogColor[1], fogColor[2], 1.0f) );
	mFog->setEnd( visibility );
	
	int pm( 500 );
	if( sunAngle < 13 )
   {
		mCloudColor->set( 0.3f, 0.3f, 0.3f, 1.0f );
   }
	else if( sunAngle > 13 && sunAngle <= 18 )
	{
		float fr = (18 - sunAngle)  / 5.0f; 
		mCloudColor->set( fr*(pm/mHeight), 0.9f *fr*(pm/mHeight), 0.76f *fr*(pm/mHeight), 1.0f );
	}
	else
   {
		mCloudColor->set( 1.0f, 1.0f, 1.0f, 1.0f );
   }

	(*mColors)[0].set( fogColor[0], fogColor[1], fogColor[2], 0.0f );
	(*mColors)[1].set( (*mCloudColor)[0], (*mCloudColor)[1], (*mCloudColor)[2], 1 );

	mPlane->setColorArray( mColors );
}

void CloudPlane::OnMessage(MessageData *data)
{
	if (data->message == "preframe")
	{
		double *deltaFrameTime = (double*)data->userData;
		Update(*deltaFrameTime);
	}
}

void CloudPlane::Update(const double deltaFrameTime)
{
	// Change the texture coordinates of clouds

   osg::Matrix mat = mTexMat->getMatrix();
   mat(3,0) += (*mWind)[0];
   mat(3,1) += (*mWind)[1];
   mTexMat->setMatrix(mat);
}

osg::Geometry* CloudPlane::createPlane(float size, float height)
{
	int numTilesX = 3;
	int numTilesY = 3;

	float step1 = .3f;
	float step2 = 1 - step1;
	float steps[4] = { 0, step1, step2, 1};

	osg::Vec3 v000(osg::Vec3(-size * 0.5f, -size * 0.5f, height));
	osg::Vec3 dx(osg::Vec3( size / 10, 0.0f, 0.0f));
	osg::Vec3 dy(osg::Vec3(0.0f,  size / 10, 0.0f));

	/**                    **/
	/** Vertex Coordinates **/
	/**    16 vertices     **/
	osg::Vec3Array* coords = new osg::Vec3Array;

	for(int y = 0; y < 4; ++y)
		for(int x = 0; x < 4; ++x)
			coords->push_back(v000 + dx * steps[x] * 10 + dy * steps[y] * 10);								


	/**                    **/
	/** Coordinate Indices **/
	/**                    **/
	int numIndicesPerRow=numTilesX + 1;
	osg::UByteArray* coordIndices = new osg::UByteArray; 

	for(int iy=0;iy<numTilesY;++iy)
	{
		for(int ix=0;ix<numTilesX;++ix)
		{
			// four vertices per quad.
			coordIndices->push_back(ix     + (iy+1)	*numIndicesPerRow);
			coordIndices->push_back(ix     + iy		*numIndicesPerRow);
			coordIndices->push_back((ix+1) + iy		*numIndicesPerRow);
			coordIndices->push_back((ix+1) + (iy+1)	*numIndicesPerRow);
		}
	}
	

	// Due to the fact that the texture is tileable one could want
	// to apply the texture 'factor' times on the plane.
	// For experimentation one could use values less than 1.
	// 'factor' is not a parameter yet
	int factor = 1;
	step1 *= factor; step2 *= factor;
	steps[1] = step1;
	steps[2] = step2;
	steps[3] = step1 + step2;


	/**                     **/
	/** Texture Coordinates **/
	/**                     **/
	mTexCoords = new osg::Vec2Array(36);

	for(int y = 0; y < 3; ++y)
	{
		for(int x = 0; x < 3; ++x)
		{
			(*mTexCoords)[4 * x + 12 * y + 0].set(steps[x],   steps[ y + 1 ]);
			(*mTexCoords)[4 * x + 12 * y + 1].set(steps[x],   steps[ y ]);
			(*mTexCoords)[4 * x + 12 * y + 2].set(steps[x+1], steps[ y ]);
			(*mTexCoords)[4 * x + 12 * y + 3].set(steps[x+1], steps[ y + 1 ]);
		}		
	}	

	
	/**						   **/
	/** Colors & Color Indices **/
	/**                        **/
	mColors = new osg::Vec4Array;
	osg::UByteArray* colorIndices = new osg::UByteArray();

	mColors->push_back(osg::Vec4(0.2f,0.2f,0.4f,0.0f)); // fog color - alpha=0
	mColors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.f));  // black	 - alpha=1

	int _color[36] = {  0,0,0,1,
						1,0,0,1,
						1,0,0,0,
						0,0,1,1,
						1,1,1,1,
						1,1,0,0,
						0,0,1,0,
						0,1,1,0,
						0,1,0,0};

	for (int c=0; c<36; ++c)
		colorIndices->push_back(_color[c]);

	/**                    **/
	/** Normals Array      **/
	/**                    **/
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,0.0f,1.0f)); // set up a single normal for the plane

	osg::Geometry* geom = new osg::Geometry;

	
	// Set Arrays
	geom->setVertexArray(coords);
	geom->setVertexIndices(coordIndices);

	geom->setColorArray(mColors);
	geom->setColorIndices(colorIndices);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);  // To get the correct smoothing at the edges

	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	geom->setTexCoordArray(0, mTexCoords);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, coordIndices->size()));

	return geom;
}
