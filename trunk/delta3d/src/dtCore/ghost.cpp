#include "dtCore/ghost.h"
#include "dtCore/scene.h"

#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/Material>

using namespace dtCore;

void
Ghost::AddGeometry( osg::Shape* shape )
{
   if ( shape )
   {
      mGeometryGeode = new osg::Geode();

      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio( 0.5f );
      
      osg::ShapeDrawable* sd = new osg::ShapeDrawable( shape, hints );
      
      mGeometryGeode.get()->addDrawable( sd );

      osg::Material *mat = new osg::Material();
      mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f) );
      mat->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f ) );
      mat->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f ) );

      osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
      polyoffset->setFactor( -1.0f );
      polyoffset->setUnits( -1.0f );

      osg::StateSet *ss = mGeometryGeode.get()->getOrCreateStateSet();
      ss->setAttributeAndModes( mat, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );
      ss->setMode( GL_BLEND, osg::StateAttribute::ON );
      ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
      ss->setAttributeAndModes( polyoffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON) ;
      GetMatrixNode()->addChild( mGeometryGeode.get() );
   }
   else
   {
      GetMatrixNode()->removeChild( mGeometryGeode.get() );
      mGeometryGeode = NULL;
   }
}
