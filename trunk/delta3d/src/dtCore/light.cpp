#include "dtCore/light.h"
#include "dtCore/scene.h"

using namespace dtCore;

Light::Light( int number, LightingMode mode, osg::LightSource* lightSource )
: mLightingMode( mode ), mLightSource( lightSource ), mSceneParent( NULL ), mEnabled ( false )
{
   if( number < 0 || number >= MAX_LIGHTS )
      dtCore::Notify(WARN, "Light number %d is out of bounds, use values 0-7.",number);

   osg::Light* light = new osg::Light;
   light->setLightNum( number );

   if( !lightSource ) mLightSource = new osg::LightSource;
   mLightSource->setLight( light );
}

Light::~Light() {}

void 
Light::SetEnabled( bool enabled )
{
   mEnabled = enabled;

   osg::StateAttribute::Values state;

   if( mEnabled ) state = osg::StateAttribute::ON;
   else           state = osg::StateAttribute::OFF;

   if( GetLightingMode() == GLOBAL && mSceneParent )
   {
      osg::Light* osgLight = mLightSource->getLight();
      mSceneParent->GetSceneHandler()->GetSceneView()->getGlobalStateSet()->setAssociatedModes( osgLight, state );
   }

   mLightSource->setLocalStateSetModes( state );
}

/*
void 
Light::Init( int number, LightingMode mode, osg::LightSource* lightSource )
{
   if( number < 0 || number >= MAX_LIGHTS )
      dtCore::Notify(WARN, "Light number %d is out of bounds, use values 0-7.",number);

   mSceneParent = NULL;
   mEnabled = false;

   osg::Light* light = new osg::Light;
   light->setLightNum( number );

   if( lightSource )
      mLightSource = lightSource;
   else
      mLightSource = new osg::LightSource;

   mLightingMode = mode;

   mLightSource->setLight( light );
}
*/
