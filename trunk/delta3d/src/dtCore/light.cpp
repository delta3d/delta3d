#include "dtCore/light.h"

using namespace dtCore;

Light::Light( int number, const std::string name, const LightingMode mode )
: mName( name ), mLightingMode( mode ), mLightSource( new osg::LightSource ), mLightModel( NULL )
{
   assert( number >= 0 && number <= 8 ); //FIX: get from scene

   osg::Light* light = new osg::Light;
   light->setLightNum( number );

   mLightSource->setLight( light );
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting
}

Light::Light( osg::LightSource* const osgLightSource, const std::string name, const LightingMode mode, osg::LightModel* const model )
: mName( name ), mLightingMode( mode ), mLightSource( osgLightSource ), mLightModel( model )
{
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting

   if( model )
      mLightSource->getOrCreateStateSet()->setAttributeAndModes( model, osg::StateAttribute::ON );
}

Light::~Light()
{
   mLightSource = NULL;
}

