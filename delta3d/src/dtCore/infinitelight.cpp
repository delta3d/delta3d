#include <dtCore/infinitelight.h>

#include <dtUtil/matrixutil.h>

#include <osg/Light>
#include <osg/LightSource>

namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(InfiniteLight)

   InfiniteLight::InfiniteLight( int number, const std::string& name, LightingMode mode )
   : Light( number, name, mode )
   {
      RegisterInstance(this);

      osg::Vec4 position = mLightSource->getLight()->getPosition();

      position[3] = 0.0f;

      mLightSource->getLight()->setPosition( position );
   }

   InfiniteLight::InfiniteLight( const osg::LightSource& source, const std::string& name, LightingMode mode )
   : Light( source, name, mode )
   {
      RegisterInstance(this);

      osg::Vec4 position = mLightSource->getLight()->getPosition();

      position[3] = 0.0f;

      mLightSource->getLight()->setPosition( position );
   }

   InfiniteLight::~InfiniteLight()
   {
      mLightSource = 0;

      DeregisterInstance(this);
   }

   void InfiniteLight::SetAzimuthElevation( float az, float el )
   {
      
      mLightSource->getLight()->setPosition(
         osg::Vec4(
         sinf(osg::DegreesToRadians(az))*cosf(osg::DegreesToRadians(el)),
         cosf(osg::DegreesToRadians(az))*cosf(osg::DegreesToRadians(el)),
         sinf(osg::DegreesToRadians(el)),
         0.0f )//force w=0.0f to ensure "infinite" light
         );
   }

   void InfiniteLight::GetAzimuthElevation( float& az, float& el ) const
   {
      osg::Vec4 position = mLightSource->getLight()->getPosition();

      osg::Vec3 xyz = osg::Vec3( -position[0], -position[1], position[2] );

      az = osg::RadiansToDegrees( -atan2f( xyz[0], xyz[1] ) );
      el = osg::RadiansToDegrees( -atan2f( xyz[2], sqrt( ( osg::square( xyz[0] ) + osg::square( xyz[1] ) ) ) ) );
   }
}
