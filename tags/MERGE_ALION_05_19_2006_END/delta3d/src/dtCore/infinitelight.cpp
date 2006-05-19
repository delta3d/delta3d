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
      osg::Matrix hprRot;
      dtUtil::MatrixUtil::HprToMatrix( hprRot, osg::Vec3( az, el, 0.0f ) );

      osg::Vec3 forwardVector( 0.0f, -1.0f, 0.0f );
      osg::Vec3 xyz = hprRot.preMult( forwardVector );

      //force w=0.0f to ensure "infinite" light
      mLightSource->getLight()->setPosition( osg::Vec4( xyz, 0.0f ) );  
   }

   void InfiniteLight::GetAzimuthElevation( float& az, float& el ) const
   {
      osg::Vec4 position = mLightSource->getLight()->getPosition();

      osg::Vec3 xyz = osg::Vec3( -position[0], -position[1], position[2] );

      az = osg::RadiansToDegrees( -atan2f( xyz[0], xyz[1] ) );
      el = osg::RadiansToDegrees( -atan2f( xyz[2], sqrt( ( osg::square( xyz[0] ) + osg::square( xyz[1] ) ) ) ) );
   }
}
