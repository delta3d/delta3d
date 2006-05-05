#include <dtCore/light.h>

#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <osg/LightSource>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Light)

Light::Light( int number, const std::string& name, LightingMode mode )
:  DeltaDrawable(name),
   mLightingMode( mode ), 
   mEnabled ( false )
{
   RegisterInstance( this );
   SetName( name );

   if( number < 0 || number >= MAX_LIGHTS )
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__, 
         "Light number %d is out of bounds, use values 0-7.",number);
   }

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

osg::Node* Light::GetOSGNode()
{ 
   return mLightSource.get();
} 

///Get the const internal osg::LightSource
const osg::LightSource* Light::GetLightSource() const
{ 
   return mLightSource.get();
}

///Get the non-const internal osg::LightSource
osg::LightSource* Light::GetLightSource()
{
   return mLightSource.get();
}

/**
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
   {
      GetSceneParent()->GetSceneNode()->getOrCreateStateSet()->setAssociatedModes( osgLight, state );
   }
}

void Light::SetNumber( int number )
{
   mLightSource->getLight()->setLightNum( number );
}

///Returns the number of the light as specified in the constructor
int Light::GetNumber() const
{
   return mLightSource->getLight()->getLightNum();
}

///sets the ambient light color
void Light::SetAmbient( float r, float g, float b, float a )
{ 
   mLightSource->getLight()->setAmbient( osg::Vec4( r, g, b, a) );
}

void Light::GetAmbient( float& r, float& g, float& b, float& a ) const
{ 
   osg::Vec4f color = mLightSource->getLight()->getAmbient();

   r = color[0]; 
   g = color[1]; 
   b = color[2]; 
   a = color[3];
}

///sets the diffuse light color
void Light::SetDiffuse( float r, float g, float b, float a )
{ 
   mLightSource->getLight()->setDiffuse( osg::Vec4( r, g, b, a) );
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

///sets the specular light color
void Light::SetSpecular( float r, float g, float b, float a )
{ 
   mLightSource->getLight()->setSpecular( osg::Vec4( r, g, b, a) );
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
