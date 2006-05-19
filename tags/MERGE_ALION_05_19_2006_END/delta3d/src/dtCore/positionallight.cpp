#include <dtCore/positionallight.h>

#include <dtUtil/matrixutil.h>

#include <osg/Light>
#include <osg/LightSource>

#include <osg/ShapeDrawable>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(PositionalLight)

/**
* Callback to sync internal Transform with underlying osg::Light.
* TODO: Overwrite SetTransform to do all of this.
*/
class PositionalLightCallback : public osg::NodeCallback
{
public:

   PositionalLightCallback( PositionalLight *poslight )
      :  mPositionalLight( poslight )
   {}

   virtual void operator()( osg::Node *node, osg::NodeVisitor* nv )
   {
      Transform trans;
      mPositionalLight->GetTransform( &trans, Transformable::ABS_CS );

      osg::Matrix absMatrix;
      Transformable::GetAbsoluteMatrix( mPositionalLight->GetMatrixNode(), absMatrix );
            
      osg::Light* osgLight = mPositionalLight->GetLightSource()->getLight();

      float x, y, z, h, p, r, sx, sy, sz;
      trans.Get( x, y, z, h, p, r, sx, sy, sz );

      osg::Vec4 position( x, y, z, 1.0f ); //force positional lighting with w of 1.0f
      osgLight->setPosition( position * osg::Matrix::inverse( absMatrix ) );

      //rotMatY(h) * rotMatX(p) * rotMatZ(r) * <forward vector>
      osg::Matrix hRot, pRot, rRot;

      dtUtil::MatrixUtil::HprToMatrix(hRot, osg::Vec3(h,      0.0f,   0.0f));
      dtUtil::MatrixUtil::HprToMatrix(pRot, osg::Vec3(0.0f,   p,      0.0f));
      dtUtil::MatrixUtil::HprToMatrix(rRot, osg::Vec3(0.0f,   0.0f,   r));

      osg::Matrix rotation = hRot * pRot * rRot;
      osg::Vec3 xyz = rotation.preMult( osg::Vec3( 0.0f, 1.0f, 0.0f ) );
      osgLight->setDirection( xyz );

      traverse( node, nv );
   }
private:
   PositionalLight* mPositionalLight;
};

PositionalLight::PositionalLight( int number, const std::string& name, LightingMode mode )
   :  Light( number, name, mode ),
      Transformable(name)
{
   GetMatrixNode()->addChild( mLightSource.get() );

   mLightSource.get()->setUpdateCallback( new PositionalLightCallback( this ) );
   
   // Default collision category = 9
   SetCollisionCategoryBits( UNSIGNED_BIT(9) );
}

PositionalLight::PositionalLight( const osg::LightSource& osgLightSource, const std::string& name, LightingMode mode )
   : Light( osgLightSource, name, mode )
{
   GetMatrixNode()->addChild( mLightSource.get() );

   mLightSource.get()->setUpdateCallback( new PositionalLightCallback( this ) );

   // Default collision category = 9
   SetCollisionCategoryBits( UNSIGNED_BIT(9) );
}

PositionalLight::~PositionalLight()
{
   mLightSource = 0;
}

/**
* Determines how fast the light fades as one moves away from the light. It is
* determined by the following equation: 
*
* attenuation factor = 1 / ( constant + linear*(distance) + quadratic*(distance^2) )
*
* @param constant the constant factor
* @param linear the linear factor
* @param quadractic the quadractic factor
*/
void PositionalLight::SetAttenuation( float constant, float linear, float quadratic )
{
   mLightSource->getLight()->setConstantAttenuation( constant );
   mLightSource->getLight()->setLinearAttenuation( linear );
   mLightSource->getLight()->setQuadraticAttenuation( quadratic );
}  

void PositionalLight::GetAttenuation( float& constant, float& linear, float& quadratic )
{
   constant = mLightSource->getLight()->getConstantAttenuation();
   linear = mLightSource->getLight()->getLinearAttenuation();
   quadratic = mLightSource->getLight()->getQuadraticAttenuation();
}

bool PositionalLight::AddChild( DeltaDrawable *child )
{
   if ( DeltaDrawable::AddChild(child) )
   {
      mLightSource->addChild( child->GetOSGNode() );
      return true;
   }
   else 
      return false;
}

void PositionalLight::RemoveChild( DeltaDrawable *child )
{
   mLightSource->removeChild( child->GetOSGNode() );

   DeltaDrawable::RemoveChild(child);
}
