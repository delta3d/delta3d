#include <prefix/dtcoreprefix.h>
#include <dtCore/cloudplane.h>
#include <dtCore/moveearthtransform.h>
#include <dtCore/system.h>
#include <dtUtil/noisetexture.h>
#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>

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
#include <osg/Version>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

const int MAX_HEIGHT = 2000;

using namespace dtCore;
IMPLEMENT_MANAGEMENT_LAYER(CloudPlane)


////////////////////////////////////////////////////////////////////////////////
CloudPlane::CloudPlane(int   octaves,
                       float cutoff,
                       int   frequency,
                       float amp,
                       float persistence,
                       float density,
                       int   texSize,
                       float height,
                       const std::string& name,
                       const std::string& textureFilePath)
: EnvEffect(name)
, mOctaves(octaves)
, mCutoff(cutoff)
, mFrequency(frequency)
, mAmplitude(amp)
, mPersistence(persistence)
, mDensity(density)
, mHeight(height)
, mTexSize(texSize)
, mWind(osg::Vec2(0.0f,0.0f))
, mCloudColor(osg::Vec4(0.0f,0.0f,0.0f,0.0f))
, mSkyColor()
, mTexCoords(NULL)
, mColors(NULL)
, mCloudTexture(new osg::Texture2D())
{
   RegisterInstance(this);
   if (mHeight > MAX_HEIGHT)
      mHeight = MAX_HEIGHT;

   SetOSGNode(new osg::Group);
   GetOSGNode()->setNodeMask(dtUtil::NodeMask::BACKGROUND);

   Create(textureFilePath);
   dtCore::System::GetInstance().TickSignal.connect_slot(this, &CloudPlane::OnSystem);

}

////////////////////////////////////////////////////////////////////////////////
CloudPlane::CloudPlane(float height, const std::string& name, const std::string& textureFilePath)
: EnvEffect(name)
, mOctaves(0)
, mCutoff(0)
, mFrequency(0)
, mAmplitude(0)
, mPersistence(0)
, mDensity(0)
, mHeight(height)
, mTexSize(0)
, mWind(osg::Vec2(0.0f,0.0f))
, mCloudColor(osg::Vec4(0.0f,0.0f,0.0f,0.0f))
, mSkyColor()
, mTexCoords(NULL)
, mColors(NULL)
, mCloudTexture(new osg::Texture2D())
{
   RegisterInstance(this);

   if (mHeight > MAX_HEIGHT)
   {
      mHeight = MAX_HEIGHT;
   }

   SetOSGNode(new osg::Group);
   GetOSGNode()->setNodeMask(dtUtil::NodeMask::BACKGROUND);

   Create(textureFilePath);
   dtCore::System::GetInstance().TickSignal.connect_slot(this, &CloudPlane::OnSystem);
}

////////////////////////////////////////////////////////////////////////////////
CloudPlane::~CloudPlane()
{
   DeregisterInstance(this);
}


////////////////////////////////////////////////////////////////////////////////
void CloudPlane::SetColor(const osg::Vec4& newColor)
{
   mFog->setColor( newColor );
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec4 CloudPlane::GetColor() const
{
   return mFog->getColor();
}

////////////////////////////////////////////////////////////////////////////////
bool CloudPlane::SaveTexture(const std::string& textureFilePath)
{
   if (!(mCloudTexture->getImage()))
   {
      return false;
   }

   return osgDB::writeImageFile(*mCloudTexture->getImage(), textureFilePath);
}

////////////////////////////////////////////////////////////////////////////////
bool CloudPlane::LoadTexture(const std::string& textureFilePath)
{
   osg::Image* image = osgDB::readImageFile(textureFilePath);

   if (image != NULL)
   {
      LOG_DEBUG("Loading Cloud Plane.");

      image->setPixelFormat(GL_ALPHA);

      mCloudTexture->setImage(image);

      osg::StateSet *stateset = mPlane->getOrCreateStateSet();
      stateset->setTextureAttributeAndModes(0, mCloudTexture.get());
      FilterTexture();

      return true;
   }
   else
   {
      LOG_ERROR("Failed to Load Cloud Plane." + textureFilePath);

      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::CreateCloudTexture(const std::string& filename)
{
   osg::Image* img = NULL;

   if (!filename.empty())
   {
      img = osgDB::readImageFile(filename);
      if (img != NULL)
      {
         mCloudTexture->setImage(img);
      }
   }

   if (img == NULL)
   {
      LOG_DEBUG("Creating 2D cloud texture..." );
      createPerlinTexture();
   }
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::createPerlinTexture()
{
   dtUtil::NoiseTexture noise2d(mOctaves, mFrequency, mAmplitude, mPersistence, mTexSize, mTexSize);
   mCloudTexture->setImage(noise2d.MakeNoiseTexture(GL_ALPHA));

    // Exponentiation of the image
    unsigned char* dataPtr = mCloudTexture->getImage()->data();
    unsigned char data;

    for (int j = 0; j < mTexSize; ++j)
    {
        for (int k = 0; k < mTexSize; ++k)
        {
            data = *(dataPtr);

            if (data < mCutoff * 255)
                data = 0;
            else
                data -= (unsigned char)(mCutoff * 255);

            data = 255 - (unsigned char) (pow(mDensity, data) * 255);

            *(dataPtr++) = data;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::SetWind(float x, float y)
{
   mWind.set(x, y);
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::SetWind(osg::Vec2 wv)
{
   mWind = wv;
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::Create(const std::string& textureFilePath)
{
   mXform = new MoveEarthySkyWithEyePointTransform();
   mXform->setCullingActive(false);

   mGeode = new osg::Geode();
   mGeode->setName("CloudPlane");

   mWind = osg::Vec2(.05f / mHeight, .05f/ mHeight);

   int planeSize = 20000;

   mPlane = createPlane(planeSize, mHeight);
   osg::StateSet *stateset = mPlane->getOrCreateStateSet();

   CreateCloudTexture(textureFilePath);
   stateset->setTextureAttributeAndModes(0, mCloudTexture.get());

   // Texture filtering
   FilterTexture();

   mTexMat = new osg::TexMat();
   stateset->setTextureAttribute(0, mTexMat.get());

   // Texture Blending
   osg::TexEnv* texenv = new osg::TexEnv;
   texenv->setMode(osg::TexEnv::BLEND);
   stateset->setTextureAttribute(0, texenv);

   // Transparent Bin
   //stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   stateset->setRenderBinDetails(9, "DepthSortedBin");

   // Blending Function
   osg::BlendFunc* trans = new osg::BlendFunc();
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
   osg::Vec3 sky(1.0f, 1.0f, 1.0f);
   osg::Vec3 fogColor(1.0f, 1.0f, 1.0f);
   Repaint(sky, fogColor, 45.0, 45.0, 10000.0);
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::Repaint(const osg::Vec3& skyColor,
                         const osg::Vec3& fogColor,
                         double sunAngle,
                         double sunAzimuth,
                         double visibility)
{
   mFog->setColor(osg::Vec4(fogColor[0], fogColor[1], fogColor[2], 1.0f));
   mFog->setEnd(visibility);

   int pm(500);
   if (sunAngle < 13)
   {
      mCloudColor.set(0.3f, 0.3f, 0.3f, 1.0f);
   }
   else if (sunAngle > 13 && sunAngle <= 18)
   {
      float fr = (18 - sunAngle)  / 5.0f;
      mCloudColor.set(fr*(pm/mHeight), 0.9f *fr*(pm/mHeight), 0.76f *fr*(pm/mHeight), 1.0f);
   }
   else
   {
      mCloudColor.set(1.0f, 1.0f, 1.0f, 1.0f);
   }
   
   osg::Vec4 fog_color(fogColor, 0.0f);
   osg::Vec4 cloud_color(mCloudColor);
   cloud_color[3] = 1.f;
   UpdateColors(fog_color,cloud_color);
   mPlane->setColorArray(mColors);
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      Update(deltaSim);
   }
}

////////////////////////////////////////////////////////////////////////////////
void CloudPlane::Update(const double deltaFrameTime)
{
   // Change the texture coordinates of clouds
   osg::Matrix mat = mTexMat->getMatrix();
   mat(3,0) += (mWind)[0];
   mat(3,1) += (mWind)[1];
   mTexMat->setMatrix(mat);
}

////////////////////////////////////////////////////////////////////////////////
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
   osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;

   for (int y = 0; y < 4; ++y)
      for (int x = 0; x < 4; ++x)
         coords->push_back(v000 + dx * steps[x] * 10 + dy * steps[y] * 10);


   /**                    **/
   /** Coordinate Indices **/
   /**                    **/
   int numIndicesPerRow=numTilesX + 1;
   osg::ref_ptr<osg::UByteArray> coordIndices = new osg::UByteArray;

   for (int iy=0;iy<numTilesY;++iy)
   {
      for (int ix=0;ix<numTilesX;++ix)

      {
         // four vertices per quad.
         coordIndices->push_back(ix     + (iy+1)   *numIndicesPerRow);
         coordIndices->push_back(ix     + iy      *numIndicesPerRow);
         coordIndices->push_back((ix+1) + iy      *numIndicesPerRow);
         coordIndices->push_back((ix+1) + (iy+1)   *numIndicesPerRow);
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

   for (int y = 0; y < 3; ++y)
   {
      for (int x = 0; x < 3; ++x)
      {
         (*mTexCoords)[4 * x + 12 * y + 0].set(steps[x],   steps[ y + 1 ]);
         (*mTexCoords)[4 * x + 12 * y + 1].set(steps[x],   steps[ y ]);
         (*mTexCoords)[4 * x + 12 * y + 2].set(steps[x+1], steps[ y ]);
         (*mTexCoords)[4 * x + 12 * y + 3].set(steps[x+1], steps[ y + 1 ]);
      }
   }

   mColors = new osg::Vec4Array(36);
   osg::Vec4 fog_color(0.2f,0.2f,0.4f,0.0f);
   osg::Vec4 black_color(1.0f,1.0f,1.0f,1.0f);
   UpdateColors(fog_color, black_color);

   /**                    **/
   /** Normals Array      **/
   /**                    **/
   osg::Vec3Array* normals = new osg::Vec3Array;
   normals->push_back(osg::Vec3(0.0f,0.0f,1.0f)); // set up a single normal for the plane

   osg::Geometry* geom = new osg::Geometry;


   // Set Arrays
   //this was modified for the osg upgrade to 3.2, where we can no longer set the index array
   {
      osg::Vec3Array* coords_lookup = coords;
      osg::Vec3Array* coords_expanded = new osg::Vec3Array;
      for (unsigned i = 0; i < coordIndices->size(); i++)
      {
         int index = (*coordIndices)[i];
         coords_expanded->push_back((*coords_lookup)[index]);
      }
      geom->setVertexArray(coords_expanded);
   }

#if OSG_VERSION_LESS_THAN(3,2,0)
   geom->setColorArray(mColors);
   geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);  
#else
   geom->setColorArray(mColors, osg::Array::BIND_PER_VERTEX);
#endif 

   geom->setNormalArray(normals);
   geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

   geom->setTexCoordArray(0, mTexCoords);

   geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, coordIndices->size()));

   return geom;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::CloudPlane::UpdateColors( const osg::Vec4& fog, const osg::Vec4& black )
{
   if (!mColors)
      return;

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
      (*mColors)[c] =  _color[c] ? black : fog;
}


////////////////////////////////////////////////////////////////////////////////
void CloudPlane::FilterTexture()
{
   mCloudTexture->setUseHardwareMipMapGeneration(true);
   mCloudTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
   mCloudTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
   mCloudTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
   mCloudTexture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
}
