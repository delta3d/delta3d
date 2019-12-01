#include <prefix/dtcoreprefix.h>
#include <dtCore/spotlight.h>
#include <osg/LightSource>

namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(SpotLight)

   SpotLight::SpotLight( int number, const std::string& name, LightingMode mode )
   :  PositionalLight( number, name, mode )
   {
      RegisterInstance(this);

      //set some default spotlight parameters
      SetSpotCutoff( 22.5f ); //spot angle of 45 degrees
      SetSpotExponent( 1.0f ); 
   }

   SpotLight::SpotLight( const osg::LightSource& osgLightSource, const std::string& name, LightingMode mode )
   :  PositionalLight( osgLightSource, name, mode )
   {
      RegisterInstance(this);

      //set some default spotlight parameters
      SetSpotCutoff( 22.5f ); //spot angle of 45 degrees
      SetSpotExponent( 1.0f );
   }

   SpotLight::~SpotLight()
   {
      mLightSource = 0;

      DeregisterInstance(this);
   }

   ///SpotCutoff is half the angle the SpotLight's cone, default is 22.5
   void SpotLight::SetSpotCutoff( float spot_cutoff )
   {
      mLightSource->getLight()->setSpotCutoff( spot_cutoff );
   }

   float SpotLight::GetSpotCutoff() const
   {
      return mLightSource->getLight()->getSpotCutoff();
   }

   ///The higher the SpotExponent, the more concentrated the light will be in the center of the cone, default is 1.0
   void SpotLight::SetSpotExponent( float spot_exponent )
   {
      mLightSource->getLight()->setSpotExponent( spot_exponent );
   }

   float SpotLight::GetSpotExponent() const
   { 
      return mLightSource->getLight()->getSpotExponent();
   }
}
