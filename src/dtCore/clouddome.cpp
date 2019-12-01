#include <prefix/dtcoreprefix.h>
#include <dtCore/clouddome.h>
#include <dtCore/moveearthtransform.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtUtil/noisetexture.h>
#include <dtUtil/datapathutils.h>

#include <osg/BlendFunc>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Texture3D>
#include <osgDB/ReadFile>
#include <climits>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(CloudDome)

////////////////////////////////////////////////////////////////////////////////
CloudDome::CloudDome(int   octaves,
                     int   frequency,
                     float amp,
                     float persistence,
                     float cutoff,
                     float exponent,
                     float radius,
                     int   segments)
   : EnvEffect("CloudDome")
   , mDome(0)
   , mImage_3D(0)
   , mTex3D(0)
   , mFileName("")
   , mOctaves(octaves)
   , mFrequency(frequency)
   , mPersistence(persistence)
   , mAmplitude(amp)
   , mXform(0)
   , ctime(0)
   , mEnable(true)
   , shaders_enabled(true)
   , mRadius(radius)
   , mSegments(segments)
   , mCloudProg(0)
   , mCloudVert(0)
   , mCloudFrag(0)
   , mScale(1/mRadius*1000)
   , mExponent(exponent)
   , mCutoff(cutoff)
   , mSpeedX(0.1f)
   , mSpeedY(0.05f)
   , mBias(1.0f)
   , mCloudColor(1.0f, 1.0f, 1.0f)
   , mOffset(.01f, .01f, 0.0f)
{
   RegisterInstance(this);
   SetOSGNode(new osg::Group());
   GetOSGNode()->setName(this->GetName());
   Create();
   dtCore::System::GetInstance().TickSignal.connect_slot(this, &CloudDome::OnSystem);
}

////////////////////////////////////////////////////////////////////////////////
CloudDome::CloudDome(float radius,
                     int   segments,
                     const std::string& filename)
   : EnvEffect("CloudDome")
   , mDome(0)
   , mImage_3D(0)
   , mTex3D(0)
   , mFileName(filename)
   , mOctaves(6)
   , mFrequency(2)
   , mPersistence(0.5f)
   , mAmplitude(0.7f)
   , mXform(0)
   , ctime(0.0)
   , mEnable(true)
   , shaders_enabled(true)
   , mRadius(radius)
   , mSegments(segments)
   , mCloudProg(0)
   , mCloudVert(0)
   , mCloudFrag(0)
   , mScale(float(1/mRadius*1000))
   , mExponent(5.0f)
   , mCutoff(0.5f)
   , mSpeedX(0.1f)
   , mSpeedY(0.05f)
   , mBias(1.0f)
   , mCloudColor(1.0f,1.0f,1.0f)
   , mOffset(0.01f,0.01f,0.0f)
{
   RegisterInstance(this);

   SetOSGNode(new osg::Group());
   GetOSGNode()->setName(this->GetName());

   Create();
   dtCore::System::GetInstance().TickSignal.connect_slot(this, &CloudDome::OnSystem);
}

////////////////////////////////////////////////////////////////////////////////
CloudDome::~CloudDome()
{
   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void CloudDome::LoadShaderSource(osg::Shader* obj, const std::string& fileName)
{
   std::string fqFileName = dtUtil::FindFileInPathList(fileName);
   if (fqFileName.length() != 0)
   {
      obj->loadShaderSourceFromFile(fqFileName.c_str());
   }
   else
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, "clouddome.cpp",
        "File '%s' not found.", fileName.c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Geode* CloudDome::CreateDome(float radius, int segs)
{
   int i, j;
   float lev[] = { 1.0, 5.0, 10.0, 15.0, 30.0, 45.0 , 60.0, 75, 90.0  };
   float cc[][4] =
   {
      { 1.0, 0.0, 0.0,  0.0 },  // has bright colors for debugging when
      { 0.0, 1.0, 0.0,  0.3 },  // shaders are disabled
      { 0.0, 0.0, 1.0,  0.7 },
      { 1.0, 0.0, 0.0,  1.0 },
      { 0.0, 1.0, 0.0,  1.0 },
      { 0.0, 0.0, 1.0,  1.0 },
      { 1.0, 0.0, 0.0,  1.0 },
      { 0.0, 1.0, 0.0,  1.0 },
      { 0.0, 0.0, 1.0,  1.0 }
   };

   --segs; // We want segments to span from  0 to segs-1

   float x, y, z;
   float alpha, theta;
   int nlev = sizeof(lev) / sizeof(float);

   RefPtr<osg::Geometry> geom = new osg::Geometry;

   RefPtr<osg::Vec3Array> coords = new osg::Vec3Array(segs*nlev);
   RefPtr<osg::Vec4Array> colors = new osg::Vec4Array(segs*nlev);
   RefPtr<osg::Vec2Array> tcoords = new osg::Vec2Array(segs*nlev);

   int ci = 0;

   for (i = 0; i < nlev; ++i)
   {
      for (j = 0; j < segs; ++j, ++ci)
      {
         alpha = osg::DegreesToRadians(lev[i]);
         theta = osg::DegreesToRadians(float(j * 360 / (segs-1)));

         x = radius * cosf(alpha) * cosf(theta);
         y = radius * cosf(alpha) * -sinf(theta);
         z = radius * sinf(alpha);

         (*coords)[ci].set(x, y, z);
         (*colors)[ci].set(cc[i][0], cc[i][1], cc[i][2], cc[i][3]);
         (*tcoords)[ci].set(float(j / segs), float(i / float(nlev-1)));
      }
   }

   for (i = 0; i < nlev - 1; ++i)
   {
      RefPtr<osg::DrawElementsUShort> drawElements = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLE_STRIP);

      for (j = 0; j < segs; ++j)
      {
         drawElements->push_back((i+1)*segs + j);
         drawElements->push_back((i+0)*segs + j);
      }

      geom->addPrimitiveSet(drawElements.get());
   }

   geom->setVertexArray(coords.get());
   geom->setTexCoordArray(0, tcoords.get());

   geom->setColorArray(colors.get());
   geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

   osg::Geode* geode = new osg::Geode;
   geode->addDrawable(geom.get());
   geode->setName("CloudDome");

   return geode;
}

////////////////////////////////////////////////////////////////////////////////
void CloudDome::Create()
{
   mXform = new MoveEarthySkyWithEyePointTransform();

   mDome = CreateDome(mRadius, mSegments);
   osg::StateSet* domeStateSet = mDome->getOrCreateStateSet();

   if (mFileName != "")
   {
      mImage_3D = osgDB::readImageFile(mFileName);
      if (!mImage_3D.valid())
      {
         LOG_WARNING("CloudDome can't load image file." );
         LOG_WARNING("Creating new 128^3 3d texture..." );

         NoiseTexture noise3d( mOctaves, mFrequency, mAmplitude, mPersistence, 128, 128, 128 );
         mImage_3D = noise3d.MakeNoiseTexture(GL_ALPHA);
      }
   }
   else
   {
      LOG_DEBUG("Creating 128^3 3d texture..." );
      NoiseTexture noise3d( mOctaves, mFrequency, mAmplitude, mPersistence, 128, 128, 128 );
      mImage_3D = noise3d.MakeNoiseTexture(GL_ALPHA);
   }

   mTex3D = new osg::Texture3D;
   mTex3D->setImage(mImage_3D.get());

   mTex3D->setFilter(osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR);
   mTex3D->setFilter(osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR);
   mTex3D->setWrap(osg::Texture3D::WRAP_S, osg::Texture3D::REPEAT);
   mTex3D->setWrap(osg::Texture3D::WRAP_T, osg::Texture3D::REPEAT);
   mTex3D->setWrap(osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT);

   domeStateSet->setTextureAttributeAndModes(0, mTex3D.get(), osg::StateAttribute::ON);

   // Blending Function
   // This operates on the fragments AFTER the frag-shader to get alpha blending
   // with the skydome background
   RefPtr<osg::BlendFunc> trans = new osg::BlendFunc();
   trans->setFunction(osg::BlendFunc::SRC_ALPHA ,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
   domeStateSet->setAttributeAndModes(trans.get());

   RefPtr<osg::Program> progObj = new osg::Program;
   mProgList.push_back(progObj);
   domeStateSet->setAttributeAndModes(progObj.get(), osg::StateAttribute::ON);

   // Set the program objects
   mCloudProg = new osg::Program;
   mProgList.push_back(mCloudProg);

   mCloudVert = new osg::Shader(osg::Shader::VERTEX);
   mCloudFrag = new osg::Shader(osg::Shader::FRAGMENT);

   mCloudProg->addShader(mCloudFrag.get());
   mCloudProg->addShader(mCloudVert.get());

   LoadShaderSource(mCloudVert.get(), dtUtil::GetDeltaRootPath()+"/data/shaders/cloud.vert");
   LoadShaderSource(mCloudFrag.get(), dtUtil::GetDeltaRootPath()+"/data/shaders/cloud.frag");

   RefPtr<osg::Uniform> scale = new osg::Uniform("Scale", mScale / 1000);
   RefPtr<osg::Uniform> cutoff = new osg::Uniform("Cutoff", mCutoff);
   RefPtr<osg::Uniform> exponent = new osg::Uniform("Exponent", mExponent);
   RefPtr<osg::Uniform> bias = new osg::Uniform("Bias", mBias);
   RefPtr<osg::Uniform> offset = new osg::Uniform("Offset", mOffset);
   RefPtr<osg::Uniform> cloudColor = new osg::Uniform("CloudColor", mCloudColor);

   RefPtr<osg::Uniform> noise = new osg::Uniform(osg::Uniform::SAMPLER_3D, "Noise");
   noise->set(0);  // TODO: Change texture files at runtime

   domeStateSet->addUniform(scale.get());
   domeStateSet->addUniform(cutoff.get());
   domeStateSet->addUniform(exponent.get());
   domeStateSet->addUniform(bias.get());
   domeStateSet->addUniform(offset.get());
   domeStateSet->addUniform(cloudColor.get());
   domeStateSet->addUniform(noise.get());
   domeStateSet->setDataVariance(osg::Object::DYNAMIC);

   // TODO: Add the fog uniform parameter
   domeStateSet->setAttributeAndModes(mCloudProg.get(), osg::StateAttribute::ON);

   mXform->addChild(mDome.get());

   if (osg::Group* group = GetOSGNode()->asGroup())
   {
      group->addChild(mXform.get());
   }
}

////////////////////////////////////////////////////////////////////////////////
void CloudDome::Repaint(const osg::Vec3& skyColor,
                        const osg::Vec3& fogColor,
                        double sunAngle,
                        double sunAzimuth,
                        double visibility)
{
}

////////////////////////////////////////////////////////////////////////////////
void CloudDome::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      Update(deltaSim);
   }
}

////////////////////////////////////////////////////////////////////////////////
void CloudDome::Update(const double deltaFrameTime)
{
   ctime += deltaFrameTime;
   
   if (ctime > INT_MAX)
   {
      ctime = 0;
   }

   mOffset.set(ctime * mSpeedX / 10, ctime * mSpeedY / 20, 0.0f);

   osg::StateSet* domeStateSet = mDome->getOrCreateStateSet();

   if (mEnable)
   {
      if (!shaders_enabled)
      {
         for (unsigned int i = 0; i < mProgList.size(); i++)
         {
            domeStateSet->setAttributeAndModes(mProgList[i].get(), osg::StateAttribute::ON);
         }
         shaders_enabled = true;
      }

      // Set Uniform variables
      domeStateSet->getUniform("Offset")->set(mOffset);
      domeStateSet->getUniform("Scale")->set(mScale / 1000);
      domeStateSet->getUniform("Exponent")->set(mExponent / 1000);
      domeStateSet->getUniform("Cutoff")->set(mCutoff);
      domeStateSet->getUniform("Bias")->set(mBias);
      domeStateSet->getUniform("CloudColor")->set(mCloudColor);
   }
   else
   {
      for (unsigned int i = 0; i < mProgList.size(); i++)
      {
         domeStateSet->setAttributeAndModes(mProgList[i].get(), osg::StateAttribute::OFF);
      }
      shaders_enabled = false;
   }
}

////////////////////////////////////////////////////////////////////////////////
