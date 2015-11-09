#include <prefix/dtcoreprefix.h>
#include <dtCore/positionallight.h>
#include <dtCore/transform.h>

#include <dtUtil/matrixutil.h>

#include <osg/Light>
#include <osg/LightSource>
#include <osg/MatrixTransform>

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
      mPositionalLight->GetTransform( trans, Transformable::ABS_CS );

      osg::Matrix absMatrix;
      Transformable::GetAbsoluteMatrix( mPositionalLight->GetMatrixNode(), absMatrix );

      osg::Light* osgLight = mPositionalLight->GetLightSource()->getLight();

      float x, y, z, h, p, r;
      trans.Get( x, y, z, h, p, r);

      osg::Vec4 position( x, y, z, 1.0f ); //force positional lighting with w of 1.0f
      osgLight->setPosition( position * osg::Matrix::inverse( absMatrix ) );

      //note: the actual direction is handled by the Transformable's MatrixNode.
      //this just tells the Light that it has a direction.
      osgLight->setDirection( osg::Vec3( 0.0f, 1.0f, 0.0f ) );

      traverse( node, nv );
   }
private:
   PositionalLight* mPositionalLight;
};

PositionalLight::PositionalLight( int number, const std::string& name, LightingMode mode )
   :  Light( number, name, mode )
{
   mLightSource->setUpdateCallback( new PositionalLightCallback( this ) );
}

PositionalLight::PositionalLight( const osg::LightSource& osgLightSource, const std::string& name, LightingMode mode )
   : Light( osgLightSource, name, mode )
{
   mLightSource->setUpdateCallback( new PositionalLightCallback( this ) );
}

PositionalLight::~PositionalLight()
{
   mLightSource = NULL;
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

void PositionalLight::SetAttenuation( const osg::Vec3& constantLinearQuadratic )
{
   SetAttenuation(constantLinearQuadratic[0], constantLinearQuadratic[1], constantLinearQuadratic[2]);
}

void PositionalLight::GetAttenuation( float& constant, float& linear, float& quadratic ) const
{
   constant = mLightSource->getLight()->getConstantAttenuation();
   linear = mLightSource->getLight()->getLinearAttenuation();
   quadratic = mLightSource->getLight()->getQuadraticAttenuation();
}

osg::Vec3 PositionalLight::GetAttenuation() const
{
   osg::Vec3 constantLinearQuadratic;
   GetAttenuation(constantLinearQuadratic[0], constantLinearQuadratic[1], constantLinearQuadratic[2]);
   return constantLinearQuadratic;
}

