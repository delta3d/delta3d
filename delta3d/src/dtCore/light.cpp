#include "dtCore/light.h"
#include "dtCore/scene.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Light)

Light::Light( int number, const std::string name, const LightingMode mode )
:  mLightingMode( mode ), mLightSource( new osg::LightSource )
{
   assert( number >= 0 && number <= MAX_LIGHT_NUMBER ); 

   SetName(name);

   osg::Light* light = new osg::Light;
   light->setLightNum( number );

   mLightSource->setLight( light );
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting
}

Light::Light( osg::LightSource* const osgLightSource, const std::string name, const LightingMode mode )
: mLightSource( osgLightSource ), mLightingMode( mode )
{
   SetName(name);
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting
}

Light::~Light()
{
   mLightSource = NULL;
}

void
Light::AddedToScene( Scene* scene )
{
   scene->RegisterLight( this ); 
}

void
Light::RemovedFromScene( Scene* scene )
{
   scene->UnRegisterLight( this );
}