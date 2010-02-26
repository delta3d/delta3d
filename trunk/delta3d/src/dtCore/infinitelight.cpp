#include <prefix/dtcoreprefix.h>
#include <dtCore/infinitelight.h>

#include <dtCore/transform.h>
#include <dtUtil/matrixutil.h>

#include <osg/Light>
#include <osg/LightSource>

////////////////////////////////////////////////////////////////////////////////

namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(InfiniteLight)

   ////////////////////////////////////////////////////////////////////////////////
   InfiniteLight::InfiniteLight(int number, const std::string& name, LightingMode mode)
   : Light(number, name, mode)
   {
      RegisterInstance(this);

      mLightSource->getLight()->setPosition(osg::Vec4(0.0f, -1.0f, 0.0f, 0.0f));
      //default to "noon"
      SetTransform(dtCore::Transform(0.0f, 0.0f, 0.0f, 0.0f, -90.0f, 0.0f));
   }

   ////////////////////////////////////////////////////////////////////////////////
   InfiniteLight::InfiniteLight(const osg::LightSource& source, const std::string& name, LightingMode mode)
   : Light(source, name, mode)
   {
      RegisterInstance(this);

      mLightSource->getLight()->setPosition(osg::Vec4(0.0f, -1.0f, 0.0f, 0.0f));
      //default to "noon"
      SetTransform(dtCore::Transform(0.0f, 0.0f, 0.0f, 0.0f, -90.0f, 0.0f));
   }

   ////////////////////////////////////////////////////////////////////////////////
   InfiniteLight::~InfiniteLight()
   {
      mLightSource = 0;

      DeregisterInstance(this);
   }

} //namespace dtCore
