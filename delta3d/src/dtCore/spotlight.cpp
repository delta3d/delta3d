#include "dtCore/spotlight.h"
#include "dtCore/scene.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(SpotLight)

SpotLight::SpotLight( int number, const std::string name, const LightingMode mode )
:  PositionalLight( number, name, mode )
{
   //set some default spotlight parameters
   SetSpotCutoff( 22.5f ); //spot angle of 45 degrees
   SetSpotExponent( 1.0f ); 
}

SpotLight::SpotLight( osg::LightSource* const osgLightSource, const std::string name, const LightingMode mode )
:  PositionalLight( osgLightSource, name, mode )
{
   //set some default spotlight parameters
   SetSpotCutoff( 22.5f ); //spot angle of 45 degrees
   SetSpotExponent( 1.0f );
}

SpotLight::~SpotLight()
{
   mLightSource = NULL;
}

