#include "dtCore/positionallight.h"
#include "dtCore/scene.h"
#include "dtUtil/matrixutil.h"

#include <osg/ShapeDrawable>

#include "dtCore/notify.h"

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
      Transform trans;
      mPositionalLight->GetTransform( &trans, Transformable::ABS_CS );

      osg::Matrix absMatrix;
      Transformable::GetAbsoluteMatrix( mPositionalLight->GetMatrixNode(), absMatrix );
            
      osg::Light* osgLight = mPositionalLight->GetLightSource()->getLight();

      float x, y, z, h, p, r;
      trans.Get( &x, &y, &z, &h, &p, &r );

      osg::Vec4 position( x, y, z, 1.0f ); //force positional lighting with w of 1.0f
      osgLight->setPosition( position * osg::Matrix::inverse( absMatrix ) );

      //rotMatY(h) * rotMatX(p) * rotMatZ(r) * <forward vector>
      sgMat4 hRot, pRot, rRot;

      sgMakeRotMat4( hRot, h, 0.0f, 0.0f );
      sgMakeRotMat4( pRot, 0.0f, p, 0.0f );
      sgMakeRotMat4( rRot, 0.0f, 0.0f, r );

      osg::Matrix rotation = osg::Matrix((float*)hRot) * osg::Matrix((float*)pRot) * osg::Matrix((float*)rRot);
      osg::Vec3 xyz = rotation.preMult( osg::Vec3( 0.0f, 1.0f, 0.0f ) );
      osgLight->setDirection( xyz );

      traverse( node, nv );
   }
private:
   PositionalLight* mPositionalLight;
};

PositionalLight::PositionalLight( int number, const std::string name, const LightingMode mode )
   : Light( number, mode, NULL )//,
     //Transformable()
{
   GetMatrixNode()->addChild( mLightSource.get() );
   SetName( name );

   mLightSource.get()->setUpdateCallback( new PositionalLightCallback( this ) );
}

PositionalLight::PositionalLight( osg::LightSource* const osgLightSource, const std::string name, const LightingMode mode )
   : Light( osgLightSource->getLight()->getLightNum(), mode, osgLightSource )//,
     //Transformable()
{
   GetMatrixNode()->addChild( mLightSource.get() );
   SetName( name );

   mLightSource.get()->setUpdateCallback( new PositionalLightCallback( this ) );
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

void
PositionalLight::AddChild( DeltaDrawable *child )
{
   mLightSource->addChild( child->GetOSGNode() );
}

/*!
 * Remove a child from this Transformable.  This will detach the child from its
 * parent so that its free to be repositioned on its own.
 *
 * @param *child : The child Transformable to be removed
 */
void
PositionalLight::RemoveChild( DeltaDrawable *child )
{
   mLightSource->removeChild( child->GetOSGNode() );
}

void
PositionalLight::RenderGeometry( const bool enable )
{
   mRenderingGeometry = enable;

   if( enable )
   {
      //make sphere
      float radius = 0.5f;
      osg::Matrix relMat = GetMatrixNode()->getMatrix();
   
      osg::Sphere* sphere = new osg::Sphere(  osg::Vec3( relMat(3,0), relMat(3,1), relMat(3,2) ), radius );
      AddGeometry( sphere );
   }
   else
   {
      AddGeometry( 0 );
   }
}
