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

      lightSourceNode->getLight()->setPosition( osg::Vec4( x, y, z, 1.0 ) );

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
   assert( number >= 0 && number <= MAX_LIGHT_NUMBER ); 

   SetName(name);

   osg::Light* light = new osg::Light;
   light->setLightNum( number );

   mLightSource->setLight( light );
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting

   mLightSource.get()->setUpdateCallback( new LightCallback(this) );
}

Light::Light( osg::LightSource* const osgLightSource, const std::string name, const LightingMode mode )
: mLightSource( osgLightSource ), mLightingMode( mode )
{
   SetName(name);
   mLightSource->setLocalStateSetModes( osg::StateAttribute::ON ); //enable local lighting

   mLightSource.get()->setUpdateCallback( new LightCallback(this) );
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