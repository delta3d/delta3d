#include "dtCore/light.h"
#include "dtCore/scene.h"
#include "dtCore/notify.h"

using namespace dtCore;

Light::Light( int number, LightingMode mode, osg::LightSource* lightSource )
: mLightingMode( mode ), mLightSource( lightSource ), mEnabled ( false )
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

   if( GetLightingMode() == GLOBAL && GetSceneParent() )
   {
      osg::Light* osgLight = mLightSource->getLight();
      GetSceneParent()->GetSceneHandler()->GetSceneView()->getGlobalStateSet()->setAssociatedModes( osgLight, state );
   }

   mLightSource->setLocalStateSetModes( state );
}

void
Light::SetLightModel( osg::LightModel* model, bool enabled )
{ 
   osg::StateAttribute::Values value;
   if( enabled ) value = osg::StateAttribute::ON;
   else value = osg::StateAttribute::OFF;

   mLightSource->getOrCreateStateSet()->setAttributeAndModes( model, value );
}

void Light::AddedToScene( Scene *scene )
{ 
   DeltaDrawable::AddedToScene( scene );

   if( scene )
   {
      SetEnabled( true );
      scene->RegisterLight( this );
   }
   else
   {
      SetEnabled( false );
      mParentScene->UnRegisterLight( this );
   }
}
