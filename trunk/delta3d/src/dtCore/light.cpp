#include "dtCore/light.h"
#include "dtCore/scene.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Light)

///An update callback for a visual Object
class LightCallback : public osg::NodeCallback
{
public:

   LightCallback( Light* light )
      :  mLight( light )
   {
   }

   virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
   {      
      Transform xform;
      mLight->GetTransform( &xform );

      float x,y,z,h,p,r;
      xform.Get(&x,&y,&z,&h,&p,&r);

      osg::LightSource* lightSourceNode = static_cast<osg::LightSource*>( node );

      osg::Vec4 position = lightSourceNode->getLight()->getPosition();

      position[0] = x; 
      position[1] = y; 
      position[2] = z;
      //position[3] = position[3] to perserve directional lighting value

      lightSourceNode->getLight()->setPosition( position );

      //rotMatY(h) * rotMatX(p) * rotMatZ(r) * <forward vector>
      sgMat4 hRot, pRot, rRot;
      
      sgMakeRotMat4( hRot, h, 0.0f, 0.0f );
      sgMakeRotMat4( pRot, 0.0f, p, 0.0f );
      sgMakeRotMat4( rRot, 0.0f, 0.0f, r );

      sgMat4 hpRot, hprRot;
      sgMultMat4( hpRot, hRot, pRot );
      sgMultMat4( hprRot, hpRot, rRot );

      sgVec3 xyz;
      sgVec3 forwardVector = {0.0f, 1.0f, 0.0f}; 

      sgXformVec3( xyz, forwardVector, hprRot );

      lightSourceNode->getLight()->setDirection( osg::Vec3( xyz[0], xyz[1], xyz[2] ) );

      traverse( node, nv );
   }
private:
   Light* mLight;
};

Light::Light( int number, const std::string name, const LightingMode mode )
:  mLightingMode( mode ), mLightSource( new osg::LightSource )
{
   //ensure light number within 0-7
   while( number < 0 )
   {
      number++;
   }

   while( number >= MAX_LIGHTS )
   {
      number--;
   }

   SetName( name );

   osg::Light* light = new osg::Light;
   light->setLightNum( number );

   mLightSource->setLight( light );
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting

   mLightSource.get()->setUpdateCallback( new LightCallback( this ) );
   
}

Light::Light( osg::LightSource* const osgLightSource, const std::string name, const LightingMode mode )
: mLightSource( osgLightSource ), mLightingMode( mode )
{
   //ensure light number within 0-7
   /*
   while( osgLightSource->getLight()->getLightNum() < 0 )
   {
      number++;
   }

   while( number >= MAX_LIGHTS )
   {
      number--;
   }
   */

   SetName( name );
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting

   mLightSource.get()->setUpdateCallback( new LightCallback(this ) );
}

Light::~Light()
{
   mLightSource = NULL;
}

void 
Light::SetDirectionalLighting( bool directional )
{
   osg::Vec4 position = mLightSource->getLight()->getPosition();
   position[3] = float( !directional );
   mLightSource->getLight()->setPosition( position );
}

bool 
Light::GetDirectionalLighting()
{
   osg::Vec4 position = mLightSource->getLight()->getPosition();
   return bool(position[3]);
}

// attenuation factor = 1 / ( k_c + k_l*(d) + k_q*(d^2) )
// where k_c = constant, k_l = linear, k_q = quadractric
void 
Light::SetAttenuation( AttenuationType type, float value )
{
   switch( type )
   {
   case CONSTANT:
      mLightSource->getLight()->setConstantAttenuation( value );
      break;
   case LINEAR:
      mLightSource->getLight()->setLinearAttenuation( value );
      break;
   case QUADRATIC:
      mLightSource->getLight()->setQuadraticAttenuation( value );
      break;
   default:
      break;
   }
}  

float 
Light::GetAttenuation( AttenuationType type )
{
   switch( type )
   {
   case CONSTANT:
      return mLightSource->getLight()->getConstantAttenuation();
      break;
   case LINEAR:
      return mLightSource->getLight()->getLinearAttenuation();
      break;
   case QUADRATIC:
      return mLightSource->getLight()->getQuadraticAttenuation();
      break;
   default:
      return 0;
      break;
   }
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