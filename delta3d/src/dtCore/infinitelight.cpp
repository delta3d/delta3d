#include "dtCore/infinitelight.h"
#include "dtCore/scene.h"

#include "sg.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(InfiniteLight)

InfiniteLight::InfiniteLight( int number, const std::string name, const LightingMode mode )
: Light( number, mode, NULL )
{
   SetName( name );

   osg::Vec4 position = mLightSource->getLight()->getPosition();

   position[3] = 0.0f;

   mLightSource->getLight()->setPosition( position );
}

InfiniteLight::InfiniteLight( osg::LightSource* const source, const std::string name, const LightingMode mode )
: Light( source->getLight()->getLightNum(), mode, source )
{
   SetName( name );

   osg::Vec4 position = mLightSource->getLight()->getPosition();

   position[3] = 0.0f;

   mLightSource->getLight()->setPosition( position );
}
x

InfiniteLight::~InfiniteLight()
{
   mLightSource = 0;
}


void 
InfiniteLight::SetDirection( const float h, const float p, const float r )
{
   //rotMatY(h) * rotMatX(p) * rotMatZ(r) * <forward vector>
   sgMat4 hRot, pRot, rRot;

   sgMakeRotMat4( hRot, h, 0.0f, 0.0f );
   sgMakeRotMat4( pRot, 0.0f, p, 0.0f );
   sgMakeRotMat4( rRot, 0.0f, 0.0f, r );

   sgMat4 hpRot, hprRot;
   sgMultMat4( hpRot, hRot, pRot );
   sgMultMat4( hprRot, hpRot, rRot );

   sgVec3 xyz;
   sgVec3 forwardVector = { 0.0f, 1.0f, 0.0f }; 

   sgXformVec3( xyz, forwardVector, hprRot );

   mLightSource->getLight()->setPosition( osg::Vec4( xyz[0], xyz[1], xyz[2], 0.0f ) );

}

void 
InfiniteLight::GetDirection( float* h, float* p, float* r ) const
{
   osg::Vec4 position = mLightSource->getLight()->getPosition();

   sgVec3 hpr, xyz;
   xyz[0] = position[0];
   xyz[1] = position[1];
   xyz[2] = position[2];
   sgHPRfromVec3( hpr, xyz );

   *h = hpr[0];
   *p = hpr[1];
   *r = hpr[2];
}
