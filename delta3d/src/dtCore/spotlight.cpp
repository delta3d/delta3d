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

void
SpotLight::RenderGeometry( const bool enable )
{
   mRenderingGeometry = enable;

   if( enable )
   {
      //make cone
      float radius = 0.5f;
      float height = 0.5f;
      osg::Matrix relMat = GetMatrixNode()->getMatrix();
 
      osg::Cone* cone = new osg::Cone( osg::Vec3( relMat(3,0), relMat(3,1), relMat(3,2) ), radius, height);
      AddGeometry( cone );
   }
   else
   {
      AddGeometry( 0 );
   }
}

