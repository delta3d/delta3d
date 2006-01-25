#include <dtCore/spotlight.h>
#include <dtCore/scene.h>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(SpotLight)

SpotLight::SpotLight( int number, const std::string& name, LightingMode mode )
:  PositionalLight( number, name, mode )
{
   RegisterInstance(this);

   //set some default spotlight parameters
   SetSpotCutoff( 22.5f ); //spot angle of 45 degrees
   SetSpotExponent( 1.0f ); 

   // Default collision category = 10
   SetCollisionCategoryBits( UNSIGNED_BIT(10) );
}

SpotLight::SpotLight( const osg::LightSource& osgLightSource, const std::string& name, LightingMode mode )
:  PositionalLight( osgLightSource, name, mode )
{
   RegisterInstance(this);

   //set some default spotlight parameters
   SetSpotCutoff( 22.5f ); //spot angle of 45 degrees
   SetSpotExponent( 1.0f );

   // Default collision category = 10
   SetCollisionCategoryBits( UNSIGNED_BIT(10) );
}

SpotLight::~SpotLight()
{
   mLightSource = 0;

   DeregisterInstance(this);
}
