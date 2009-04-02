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
   /// Change the azimuth and elevation of the light (aka heading and pitch)
   /// This is where azimuth and elevation are defined in Delta3D.
   /// The derivation of equations for x,y,z assume that
   /// when azimuth and elevation are both 0,
   /// the directional unit vector is along the y-axis.
   /// @param az The azimuth angle (degrees), e.g. the swing in the y-x plane.
   /// @param el The elevation angle (degrees), e.g. the swing up or down from the y-x plane.
   /// @pre el<=90.0 && el>-90
   DEPRECATE_FUNC void InfiniteLight::SetAzimuthElevation(float az, float el)
   {
      DEPRECATE("InfiniteLight::SetAzimuthElevation", "Transformable::SetTransform");

      // create the directional vector
      osg::Vec4 direction(sinf(osg::DegreesToRadians(az))*cosf(osg::DegreesToRadians(el)),
                          cosf(osg::DegreesToRadians(az))*cosf(osg::DegreesToRadians(el)),
                          sinf(osg::DegreesToRadians(el)),
                          0.0f );  //force w=0.0f to ensure "infinite" light

      mLightSource->getLight()->setPosition(direction);
   }

   ////////////////////////////////////////////////////////////////////////////////
   DEPRECATE_FUNC void InfiniteLight::SetAzimuthElevation(const osg::Vec2& azEl)
   {
      // create the directional vector
      osg::Vec4 direction(sinf(osg::DegreesToRadians(azEl[0]))*cosf(osg::DegreesToRadians(azEl[1])),
         cosf(osg::DegreesToRadians(azEl[0]))*cosf(osg::DegreesToRadians(azEl[1])),
         sinf(osg::DegreesToRadians(azEl[1])),
         0.0f );  //force w=0.0f to ensure "infinite" light

      mLightSource->getLight()->setPosition(direction);
   }

   ////////////////////////////////////////////////////////////////////////////////
   /// Values returned may be different than those set due to math limitations, but
   /// they are guarunteed to be equal internally.
   /// @param az The azimuth angle (degrees), e.g. the swing in the y-x plane.
   /// @param el The elevation angle (degrees), e.g. the swing up or down from the y-x plane.
   DEPRECATE_FUNC void InfiniteLight::GetAzimuthElevation(float& az, float& el) const
   {
      DEPRECATE("InfiniteLight::GetAzimuthElevation", "Transformable::GetTransform");

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
   DEPRECATE_FUNC osg::Vec2 InfiniteLight::GetAzimuthElevation() const
   {
      float az, el;

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



      return osg::Vec2(az, el);
   }
}
