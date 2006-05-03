#include <dtCore/skydome.h>
#include <dtCore/moveearthtransform.h>

#include <osg/Depth>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Node>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(SkyDome)

SkyDome::SkyDome(const std::string& name)
:EnvEffect(name)
{
   RegisterInstance(this);

   mNode = new osg::Group();
   mBaseColor.set(0.5f, 0.5f, 0.2f);
   Config();
}

SkyDome::~SkyDome()
{
   DeregisterInstance(this);
}

// Build the sky dome
void dtCore::SkyDome::Config()
{
   osg::Group *group = new osg::Group();

    // use a transform to make the sky and base around with the eye point.
   mXform = new MoveEarthySkyWithEyePointTransformAzimuth();
   mXform->SetAzimuth( 0.0f ); //zero out the rotation of the dome

    // transform's value isn't knowm until in the cull traversal so its bounding
    // volume is can't be determined, therefore culling will be invalid,
    // so switch it off, this cause all our paresnts to switch culling
    // off as well. But don't worry culling will be back on once underneath
    // this node or any other branch above this transform.
   mXform->setCullingActive(false);
   
   mXform->addChild(MakeDome());
   group->addChild(mXform.get());
   group->setNodeMask(0xf0000000);

   mNode->asGroup()->addChild(group);
}

osg::Node* dtCore::SkyDome::MakeDome()
{
   //5 levels with 18 points each spaced 20 degrees apart

    int i, j;
    float lev[] = { -9.0, 0.0, 7.2, 15.0, 90.0  };
    float cc[][3] =
    {
        { 0.15, 0.25, 0.1 },
        { 0.6, 0.6, 0.7 },
        { 0.4, 0.4, 0.7 },
        { 0.2, 0.2, 0.6 },
        { 0.1, 0.1, 0.6 },
    };
    float x, y, z;
    float alpha, theta;
    float radius = 6000.0f;
    int nlev = sizeof( lev )/sizeof(float);

    osg::Geometry *geom = new osg::Geometry;

    osg::Vec3Array& coords = *(new osg::Vec3Array(19*nlev));

    osg::Vec4Array& colors = *(new osg::Vec4Array(19*nlev));
    
    int ci = 0;

    for( i = 0; i < nlev; i++ )
    {
        for( j = 0; j < 19; j++ )
        {
            alpha = osg::DegreesToRadians(lev[i]);
            theta = osg::DegreesToRadians((float)(j*20));

            x = radius * cosf( alpha ) * cosf( theta );
            y = radius * cosf( alpha ) * -sinf( theta );
            z = radius * sinf( alpha );

            coords[ci][0] = x;
            coords[ci][1] = y;
            coords[ci][2] = z;

            colors[ci][0] = cc[i][0];
            colors[ci][1] = cc[i][1];
            colors[ci][2] = cc[i][2];
            colors[ci][3] = 1.f;

            ci++;
        }
    }

    for( i = 0; i < nlev-1; i++ )
    {
        osg::DrawElementsUShort* drawElements = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLE_STRIP);
        drawElements->reserve(38);

        for( j = 0; j < 19; j++ )
        {
            drawElements->push_back((i+1)*19+j);
            drawElements->push_back((i+0)*19+j);
        }

        geom->addPrimitiveSet(drawElements);
    }
    
    geom->setVertexArray( &coords );
    geom->setColorArray( &colors );
    geom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

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

    geom->setStateSet( dstate );

    mGeode = new osg::Geode;
    mGeode->addDrawable( geom );

    mGeode->setName( "Sky" );

    return mGeode.get();
}


void dtCore::SkyDome::SetBaseColor(const osg::Vec3& color)
{
   osg::Geometry *geom = mGeode->getDrawable(0)->asGeometry();
   osg::Array *array = geom->getColorArray();
   if (array && array->getType() == osg::Array::Vec4ArrayType)
   {
      mBaseColor.set(color);

      osg::Vec4Array *color = static_cast<osg::Vec4Array*>(array);
      for (int i=0; i<19; i++)
      {
         (*color)[i].set(mBaseColor[0], mBaseColor[1], mBaseColor[2], 1.f);
      }
      geom->dirtyDisplayList();
   }
}

/** 0 degrees = horizon
 *  90 degrees = high noon
 *  - degrees = below horizon
 */
void dtCore::SkyDome::Repaint(   const osg::Vec3& skyColor, 
                                 const osg::Vec3& fogColor,
                                 double sunAngle, 
                                 double sunAzimuth,
                                 double visibility )
{
   double diff;
   osg::Vec3 outer_param, outer_amt, outer_diff;
   osg::Vec3 middle_param, middle_amt, middle_diff;
   int i, j;

   //rotate the dome to line up with the sun's azimuth.
   mXform->SetAzimuth( sunAzimuth );

   // Check for sunrise/sunset condition
   if( sunAngle > -10.0 && sunAngle < 10.0 )
   {
      // 0.0 - 0.4
      outer_param.set(
         (10.0 - fabs(sunAngle)) / 20.0,
         (10.0 - fabs(sunAngle)) / 40.0,
         -(10.0 - fabs(sunAngle)) / 30.0 );

      middle_param.set(
         (10.0 - fabs(sunAngle)) / 40.0,
         (10.0 - fabs(sunAngle)) / 80.0,
         0.0 );

      outer_diff = outer_param / 9.0;

      middle_diff = middle_param / 9.0;
   } 
   else 
   {
      outer_param.set( 0.0, 0.0, 0.0 );
      middle_param.set( 0.0, 0.0, 0.0 );

      outer_diff.set( 0.0, 0.0, 0.0 );
      middle_diff.set( 0.0, 0.0, 0.0 );
   }

   outer_amt.set( outer_param );
   middle_amt.set( middle_param );

   // First, recaclulate the basic colors

   osg::Vec4 center_color;
   osg::Vec4 upper_color[19];
   osg::Vec4 middle_color[19];
   osg::Vec4 lower_color[19];
   osg::Vec4 bottom_color[19];

   double vis_factor, cvf = visibility;
   if (cvf > 20000.f)
   {
      cvf = 20000.f;
   }

   if ( visibility < 3000.0 ) 
   {
      vis_factor = (visibility - 1000.0) / 2000.0;
      if ( vis_factor < 0.0 ) 
      {
         vis_factor = 0.0;
      }
   } 
   else 
   {
      vis_factor = 1.0;
   }

   for ( j = 0; j < 3; j++ ) 
   {
      diff = skyColor[j] - fogColor[j];
      center_color[j] = skyColor[j] - diff * ( 1.0 - vis_factor );
   }
   center_color[3] = 1.0;


   for ( i = 0; i < 9; i++ ) 
   {
      for ( j = 0; j < 3; j++ ) 
      {
         diff = skyColor[j] - fogColor[j];

         upper_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.7 + 0.3 * cvf/20000.f) );
         middle_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.1 + 0.85 * cvf/20000.f) )
            + middle_amt[j];
         lower_color[i][j] = fogColor[j] + outer_amt[j];

         if ( upper_color[i][j] > 1.0 ) { upper_color[i][j] = 1.0; }
         if ( upper_color[i][j] < 0.0 ) { upper_color[i][j] = 0.0; }
         if ( middle_color[i][j] > 1.0 ) { middle_color[i][j] = 1.0; }
         if ( middle_color[i][j] < 0.0 ) { middle_color[i][j] = 0.0; }
         if ( lower_color[i][j] > 1.0 ) { lower_color[i][j] = 1.0; }
         if ( lower_color[i][j] < 0.0 ) { lower_color[i][j] = 0.0; }
      }
      upper_color[i][3] = middle_color[i][3] = lower_color[i][3] = 1.0;

      for ( j = 0; j < 3; j++ ) 
      {
         outer_amt[j] -= outer_diff[j];
         middle_amt[j] -= middle_diff[j];
      }
   }

   outer_amt.set( 0.0, 0.0, 0.0 );
   middle_amt.set( 0.0, 0.0, 0.0 );

   for ( i = 9; i < 19; i++ ) 
   {
      for ( j = 0; j < 3; j++ ) 
      {
         diff = skyColor[j] - fogColor[j];

         upper_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.7 + 0.3 * cvf/20000.f) );
         middle_color[i][j] = skyColor[j] - diff *
            ( 1.0 - vis_factor * (0.1 + 0.85 * cvf/20000.f) )
            + middle_amt[j];
         lower_color[i][j] = fogColor[j] + outer_amt[j];

         if ( upper_color[i][j] > 1.0 ) { upper_color[i][j] = 1.0; }
         if ( upper_color[i][j] < 0.0 ) { upper_color[i][j] = 0.0; }
         if ( middle_color[i][j] > 1.0 ) { middle_color[i][j] = 1.0; }
         if ( middle_color[i][j] < 0.0 ) { middle_color[i][j] = 0.0; }
         if ( lower_color[i][j] > 1.0 ) { lower_color[i][j] = 1.0; }
         if ( lower_color[i][j] < 0.0 ) { lower_color[i][j] = 0.0; }
      }
      upper_color[i][3] = middle_color[i][3] = lower_color[i][3] = 1.0;

      for ( j = 0; j < 3; j++ ) 
      {
         outer_amt[j] += outer_diff[j];
         middle_amt[j] += middle_diff[j];
      }
   }

   for ( i = 0; i < 19; i++ ) 
   {
      bottom_color[i].set(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
   }

   //repaint the lower ring
   osg::Geometry *geom = mGeode->getDrawable(0)->asGeometry();
   osg::Array *array = geom->getColorArray();
   if (array && array->getType()==osg::Array::Vec4ArrayType)
   {
      osg::Vec4Array *color = static_cast<osg::Vec4Array*>(array);
      for (int i=0; i<19; i++)
      {
         (*color)[i].set(bottom_color[i][0], bottom_color[i][1], bottom_color[i][2], 1.f);
         (*color)[i+19].set(lower_color[i][0], lower_color[i][1], lower_color[i][2], 1.f);
         (*color)[i+19+19].set(middle_color[i][0], middle_color[i][1], middle_color[i][2], 1.f);
         (*color)[i+19+19+19].set(upper_color[i][0], upper_color[i][1], upper_color[i][2], 1.f);
         (*color)[i+19+19+19+19].set(center_color[0], center_color[1], center_color[2], 1.f);
      }

   }

   geom->dirtyDisplayList();
}

}
