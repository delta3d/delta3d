#include "dtCore/positionallight.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(PositionalLight)

///An update callback for a visual Object
class PositionalLightCallback : public osg::NodeCallback
{
public:

   PositionalLightCallback( PositionalLight* poslight )
      :  mPositionalLight( poslight )
   {
   }

   virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
   {      
      Transform xform;
      mPositionalLight->GetTransform( &xform );

      float x,y,z,h,p,r;
      xform.Get(&x,&y,&z,&h,&p,&r);

      osg::LightSource* lightSourceNode = static_cast<osg::LightSource*>( node );

      osg::Vec4 position = lightSourceNode->getLight()->getPosition();

      position[0] = x; 
      position[1] = y; 
      position[2] = z;
      position[3] = 1.0f; //force positional lighting

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
   PositionalLight* mPositionalLight;
};

PositionalLight::PositionalLight( int number, const std::string name, const LightingMode mode )
: Light()
{
   Init( number, mode, NULL);
   SetName( name );

   osg::Vec4 position = mLightSource->getLight()->getPosition();

   position[3] = 1.0f;

   mLightSource->getLight()->setPosition( position );

   mLightSource.get()->setUpdateCallback( new PositionalLightCallback( this ) );
}

PositionalLight::PositionalLight( osg::LightSource* const osgLightSource, const std::string name, const LightingMode mode )
: Light()
{
   Init( osgLightSource->getLight()->getLightNum(), mode, osgLightSource );
   SetName( name );

   osg::Vec4 position = mLightSource->getLight()->getPosition();

   position[3] = 1.0f;

   mLightSource->getLight()->setPosition( position );

   mLightSource.get()->setUpdateCallback( new PositionalLightCallback(this ) );
}

PositionalLight::~PositionalLight()
{
   mLightSource = NULL;
}


// attenuation factor = 1 / ( k_c + k_l*(d) + k_q*(d^2) )
// where k_c = constant, k_l = linear, k_q = quadractric
void 
PositionalLight::SetAttenuation( const float constant, const float linear, const float quadratic )
{
   mLightSource->getLight()->setConstantAttenuation( constant );
   mLightSource->getLight()->setLinearAttenuation( linear );
   mLightSource->getLight()->setQuadraticAttenuation( quadratic );
}  

void 
PositionalLight::GetAttenuation( float* constant, float* linear, float* quadratic )
{
   *constant = mLightSource->getLight()->getConstantAttenuation();
   *linear = mLightSource->getLight()->getLinearAttenuation();
   *quadratic = mLightSource->getLight()->getQuadraticAttenuation();
}
