#include <prefix/dtcoreprefix-src.h>
#include <dtCore/infinitelight.h>

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

      osg::Vec4 position = mLightSource->getLight()->getPosition();

      position[3] = 0.0f;

      mLightSource->getLight()->setPosition(position);
   }

   ////////////////////////////////////////////////////////////////////////////////
   InfiniteLight::InfiniteLight(const osg::LightSource& source, const std::string& name, LightingMode mode)
   : Light(source, name, mode)
   {
      RegisterInstance(this);

      osg::Vec4 position = mLightSource->getLight()->getPosition();

      position[3] = 0.0f;

      mLightSource->getLight()->setPosition(position);
   }

   ////////////////////////////////////////////////////////////////////////////////
   InfiniteLight::~InfiniteLight()
   {
      mLightSource = 0;

      DeregisterInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InfiniteLight::SetAzimuthElevation(float az, float el)
   {
      // create the directional vector
      osg::Vec4 direction(sinf(osg::DegreesToRadians(az))*cosf(osg::DegreesToRadians(el)),
                          cosf(osg::DegreesToRadians(az))*cosf(osg::DegreesToRadians(el)),
                          sinf(osg::DegreesToRadians(el)),
                          0.0f );  //force w=0.0f to ensure "infinite" light

      mLightSource->getLight()->setPosition(direction);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InfiniteLight::SetAzimuthElevation(const osg::Vec2& azEl)
   {
      SetAzimuthElevation(azEl[0], azEl[1]);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InfiniteLight::GetAzimuthElevation(float& az, float& el) const
   {
      osg::Vec4 xyz = mLightSource->getLight()->getPosition();
      xyz.normalize();

      float x(xyz[0]);
      if( x < 0.0f )
      {
         az = 360.0 + osg::RadiansToDegrees(atan2f(x , xyz[1]));
      }
      else
      {
         az = osg::RadiansToDegrees(atan2f(x , xyz[1]));
      }

      el = osg::RadiansToDegrees(asinf(xyz[2]));
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 InfiniteLight::GetAzimuthElevation() const
   {
      osg::Vec2 azEl;
      GetAzimuthElevation(azEl[0], azEl[1]);
      return azEl;
   }
}
