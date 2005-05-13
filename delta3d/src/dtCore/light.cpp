#include "dtCore/light.h"
#include "dtCore/scene.h"
#include "dtCore/notify.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Light)

Light::Light( int number, const std::string& name, LightingMode mode )
: mLightingMode( mode ), mEnabled ( false )
{
   RegisterInstance( this );
   SetName( name );

   if( number < 0 || number >= MAX_LIGHTS )
      dtCore::Notify(WARN, "Light number %d is out of bounds, use values 0-7.",number);

   osg::Light* light = new osg::Light;
   light->setLightNum( number );

   mLightSource = new osg::LightSource;
   mLightSource->setLight( light );
}

Light::Light( const osg::LightSource& lightSource, const std::string& name, LightingMode mode )
: mLightingMode( mode ), mEnabled( false )
{
   RegisterInstance( this );
   SetName( name );

   mLightSource = new osg::LightSource( lightSource );
}

Light::~Light()
{
   DeregisterInstance(this);
}

/*!
* Changes the LightingMode of this Light. Can be set to either GLOBAL or LOCAL.
* GLOBAL mode illuminates the entire scene. LOCAL mode only illuminates
* children of this Light.
*
* @param mode : The child to add to this Transformable
*/
void Light::SetLightingMode( LightingMode mode )
{
   bool wasEnabled = GetEnabled();
   SetEnabled( false );

   mLightingMode = mode;

   SetEnabled( wasEnabled ); 
}

void Light::SetEnabled( bool enabled )
{
   mEnabled = enabled;

   unsigned int   state;
   if( mEnabled ) state = osg::StateAttribute::ON;
   else           state = osg::StateAttribute::OFF;

   mLightSource->setLocalStateSetModes( state );

   osg::Light* osgLight = mLightSource->getLight();

   if( GetLightingMode() == GLOBAL && GetSceneParent() )
      GetSceneParent()->GetSceneNode()->getOrCreateStateSet()->setAssociatedModes( osgLight, state );
}

void Light::GetAmbient( float& r, float& g, float& b, float& a ) const
{ 
   osg::Vec4f color = mLightSource->getLight()->getAmbient();

   r = color[0]; 
   g = color[1]; 
   b = color[2]; 
   a = color[3];
}

void Light::GetDiffuse( float& r, float& g, float& b, float& a ) const
{
   osg::Vec4f color = mLightSource->getLight()->getDiffuse();

   r = color[0]; 
   g = color[1];
   b = color[2]; 
   a = color[3];
}

void Light::GetSpecular( float& r, float& g, float& b, float& a ) const
{
   osg::Vec4f color = mLightSource->getLight()->getSpecular();

   r = color[0]; 
   g = color[1]; 
   b = color[2]; 
   a = color[3];
}

void Light::AddedToScene( Scene *scene )
{ 
   if( scene )
   {
      DeltaDrawable::AddedToScene( scene );
      SetEnabled( true );
      scene->RegisterLight( this );
   }
   else
   {
      SetEnabled( false );
      mParentScene->UnRegisterLight( this );
      DeltaDrawable::AddedToScene( scene );
   }
}
