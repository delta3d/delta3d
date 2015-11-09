#include <prefix/dtcoreprefix.h>
#include <dtCore/light.h>

#include <dtCore/scene.h>

#include <dtUtil/log.h>
#include <osg/LightSource>
#include <osg/MatrixTransform>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Light)

////////////////////////////////////////////////////////////////////////////////
Light::Light(int number, const std::string& name, LightingMode mode)
   : Transformable(name)
   , mLightingMode(mode)
   , mEnabled (false)
{
   RegisterInstance(this);
   SetName(name);

   if(number < 0 || number >= MAX_LIGHTS)
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,
         "Light number %d is out of bounds, use values 0-7.",number);
   }

   osg::Light* light = new osg::Light;
   light->setLightNum(number);

   mLightSource = new osg::LightSource;
   mLightSource->setLight(light);

   GetMatrixNode()->addChild(mLightSource.get());

   ApplyDefaults();
}

////////////////////////////////////////////////////////////////////////////////
Light::Light(const osg::LightSource& lightSource, const std::string& name, LightingMode mode)
   : mLightingMode(mode), mEnabled(false)
{
   RegisterInstance(this);
   SetName(name);

   mLightSource = new osg::LightSource(lightSource);

   GetMatrixNode()->addChild(mLightSource.get());
}

////////////////////////////////////////////////////////////////////////////////
Light::~Light()
{
   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
const osg::LightSource* Light::GetLightSource() const
{
   return mLightSource.get();
}

////////////////////////////////////////////////////////////////////////////////
osg::LightSource* Light::GetLightSource()
{
   return mLightSource.get();
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetLightingMode(LightingMode mode)
{
   bool wasEnabled = GetEnabled();
   SetEnabled(false);

   mLightingMode = mode;

   SetEnabled(wasEnabled);
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetEnabled(bool enabled)
{
   mEnabled = enabled;
   unsigned int state = (mEnabled) ? osg::StateAttribute::ON: osg::StateAttribute::OFF;

   mLightSource->setLocalStateSetModes(state);

   osg::Light* osgLight = mLightSource->getLight();

   if(GetLightingMode() == GLOBAL && GetSceneParent())
   {
      GetSceneParent()->GetSceneNode()->getOrCreateStateSet()->setAssociatedModes(osgLight, state);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetNumber(int number)
{
   mLightSource->getLight()->setLightNum(number);
}

////////////////////////////////////////////////////////////////////////////////
int Light::GetNumber() const
{
   return mLightSource->getLight()->getLightNum();
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetAmbient(float r, float g, float b, float a)
{
   SetAmbient( osg::Vec4( r, g, b, a) );
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetAmbient(const osg::Vec4& rgba)
{
   mLightSource->getLight()->setAmbient(rgba);
}

////////////////////////////////////////////////////////////////////////////////
void Light::GetAmbient(float& r, float& g, float& b, float& a) const
{
   const osg::Vec4& color = GetAmbient();

   r = color[0];
   g = color[1];
   b = color[2];
   a = color[3];
}

////////////////////////////////////////////////////////////////////////////////
const osg::Vec4& Light::GetAmbient() const
{
   return mLightSource->getLight()->getAmbient();
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetDiffuse(float r, float g, float b, float a)
{
   SetDiffuse(osg::Vec4(r, g, b, a));
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetDiffuse(const osg::Vec4& rgba)
{
   mLightSource->getLight()->setDiffuse(rgba);
}

////////////////////////////////////////////////////////////////////////////////
void Light::GetDiffuse(float& r, float& g, float& b, float& a) const
{
   const osg::Vec4& color = GetDiffuse();

   r = color[0];
   g = color[1];
   b = color[2];
   a = color[3];
}

////////////////////////////////////////////////////////////////////////////////
const osg::Vec4& Light::GetDiffuse() const
{
   return mLightSource->getLight()->getDiffuse();
}

////////////////////////////////////////////////////////////////////////////////
void Light::GetSpecular(float& r, float& g, float& b, float& a) const
{
   const osg::Vec4& color = GetSpecular();

   r = color[0];
   g = color[1];
   b = color[2];
   a = color[3];
}

////////////////////////////////////////////////////////////////////////////////
const osg::Vec4& Light::GetSpecular() const
{
   return mLightSource->getLight()->getSpecular();
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetSpecular(float r, float g, float b, float a)
{
   SetSpecular(osg::Vec4( r, g, b, a));
}

////////////////////////////////////////////////////////////////////////////////
void Light::SetSpecular(const osg::Vec4& rgba)
{
   mLightSource->getLight()->setSpecular(rgba);
}

////////////////////////////////////////////////////////////////////////////////
void Light::AddedToScene(Scene *scene)
{
   if(scene)
   {
      DeltaDrawable::AddedToScene(scene);
      SetEnabled(true);
      scene->RegisterLight(this);
   }
   else
   {
      // Prevent crash in case this is called twice with scene=NULL
      if(GetSceneParent() != NULL)
      {
         GetSceneParent()->UnRegisterLight(this);
      }

      SetEnabled(false);
      DeltaDrawable::AddedToScene(scene);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Light::AddChild(DeltaDrawable *child)
{
   if (DeltaDrawable::AddChild(child))
   {
      mLightSource->addChild(child->GetOSGNode());
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
void Light::RemoveChild(DeltaDrawable *child)
{
   mLightSource->removeChild(child->GetOSGNode());

   DeltaDrawable::RemoveChild(child);
}

////////////////////////////////////////////////////////////////////////////////
void Light::ApplyDefaults()
{
   SetDiffuse(osg::Vec4(0.9f, 0.9f, 0.9f, 1.0f));
   SetAmbient(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
   SetSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
}
