#include <prefix/dtcoreprefix-src.h>
#include <dtCore/makeskydome.h>
#include <dtCore/skydome.h>
#include <osg/Geometry>
#include <osg/Depth>
//#include <osg/Geode>
#include <cassert>

using namespace dtCore;

const unsigned int MakeSkyDome::VERTS_IN_CIRCUM = 19;

MakeSkyDome::MakeSkyDome( const SkyDome &skyDome ):
mSkyDome(&skyDome),
mRadius(6000.f),
mGeom(new osg::Geometry() )
{
   mLevelHeight.push_back(-9.0);
   mLevelHeight.push_back(-9.0);
   mLevelHeight.push_back(0.0);
   mLevelHeight.push_back(7.2);
   mLevelHeight.push_back(15.0);
   mLevelHeight.push_back(90.0);

   mCCArray.push_back( osg::Vec3(0.15, 0.25, 0.1) );
   mCCArray.push_back( osg::Vec3(0.6, 0.6, 0.7) );
   mCCArray.push_back( osg::Vec3(0.4, 0.4, 0.7) );
   mCCArray.push_back( osg::Vec3(0.2, 0.2, 0.6) );
   mCCArray.push_back( osg::Vec3(0.1, 0.1, 0.6) );
   mCCArray.push_back( osg::Vec3(0.1, 0.1, 0.7) );
   
   mCoordArray = new osg::Vec3Array(VERTS_IN_CIRCUM*mLevelHeight.size());
   mColorArray = new osg::Vec4Array(VERTS_IN_CIRCUM*mLevelHeight.size());
}

MakeSkyDome::~MakeSkyDome()
{

}

osg::Geode* MakeSkyDome::Compute()
{
   SetCoordinatesAndColors();

   CreateTriangleStrips();

   mGeom->setVertexArray( mCoordArray );
   mGeom->setColorArray( mColorArray );
   mGeom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

   mGeom->setStateSet( CreateStateSet() );

   osg::Geode *geode = new osg::Geode;
   geode->addDrawable( mGeom );

   geode->setName( "Sky" );

   return geode;
}

unsigned int MakeSkyDome::GetNumLevels( )
{
   return ( mLevelHeight.size() - (mSkyDome->GetCapEnabled()?0:1) );

}

void MakeSkyDome::SetCoordinatesAndColors()
{
   unsigned int ci = mSkyDome->GetCapEnabled()?VERTS_IN_CIRCUM:0;

   // Set dome coordinates & colors
   for( unsigned int i = mSkyDome->GetCapEnabled()?1:0; i < GetNumLevels(); i++ )
   {
      for( unsigned int j = 0; j < VERTS_IN_CIRCUM; j++ )
      {
         float alpha = osg::DegreesToRadians(mLevelHeight[i+(mSkyDome->GetCapEnabled()?0:1)]);
         float theta = osg::DegreesToRadians((float)(j*20));

         float x = mRadius * cosf( alpha ) * cosf( theta );
         float y = mRadius * cosf( alpha ) * -sinf( theta );
         float z = mRadius * sinf( alpha );

         assert(ci < mCoordArray->size());
         (*mCoordArray)[ci].set(x,y,z);

         assert(ci < mColorArray->size());
         assert(i < mCCArray.size() );
         (*mColorArray)[ci].set( mCCArray[i].x(), mCCArray[i].y(), mCCArray[i].z(), 1.f );

         ci++;
      }
   }

   SetCapCoordinatesAndColors();
}

void MakeSkyDome::SetCapCoordinatesAndColors()
{
   if( mSkyDome->GetCapEnabled() )
   {
      osg::Vec3 capCenter( 0.0, 0.0, mRadius * osg::DegreesToRadians(sinf(mLevelHeight[0])) );
      for( unsigned int j = 0; j < VERTS_IN_CIRCUM; j++ )
      {
         (*mCoordArray)[j] = capCenter;

         assert(j < mColorArray->size() );
         (*mColorArray)[j].set(mCCArray.back().x(), mCCArray.back().y(), mCCArray.back().z(), 1.f );
      }
   }
}


void MakeSkyDome::CreateTriangleStrips()
{
   for( unsigned int i = 0; i < GetNumLevels()-1; i++ )
   {
      osg::DrawElementsUShort* drawElements = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLE_STRIP);
      drawElements->reserve(38);

      for( int j = 0; j < VERTS_IN_CIRCUM; j++ )
      {
         drawElements->push_back((i+1)*VERTS_IN_CIRCUM+j);
         drawElements->push_back((i+0)*VERTS_IN_CIRCUM+j);
      }

      mGeom->addPrimitiveSet(drawElements);
   }
}


osg::StateSet* MakeSkyDome::CreateStateSet() const
{
   osg::StateSet *dstate = new osg::StateSet;

   dstate->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
   dstate->setMode( GL_CULL_FACE, osg::StateAttribute::ON );

   // clear the depth to the far plane.
   osg::Depth* depth = new osg::Depth;
   depth->setFunction(osg::Depth::ALWAYS);
   depth->setWriteMask(false);   
   dstate->setAttributeAndModes(depth,osg::StateAttribute::ON );
   dstate->setMode(GL_FOG, osg::StateAttribute::OFF );
   dstate->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);

   dstate->setRenderBinDetails(-2,"RenderBin");

   return dstate;
}
