#include "dtCore/skydome.h"
#include "dtCore/notify.h"
#include "dtCore/scene.h"

#include <osg/Depth>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>

using  namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(SkyDome)

SkyDome::SkyDome(std::string name)
:EnvEffect(name)
{
   RegisterInstance(this);

   mNode = new osg::Group();
   mBaseColor.set(0.5f, 0.5f, 0.2f);
   Config();
}

SkyDome::~SkyDome(void)
{
   DeregisterInstance(this);
}

// Build the sky dome
void dtCore::SkyDome::Config(void)
{
   osg::Group *group = new osg::Group();

    // the base and sky subgraphs go to set the earth sky of the
    // model and clear the color and depth buffer for us, by using
    // osg::Depth, and setting their bin numbers to less than 0,
    // to force them to draw before the rest of the scene.
   //osg::ClearNode* clearNode = new osg::ClearNode;
   //clearNode->setRequiresClear(false);

    // use a transform to make the sky and base around with the eye point.
   mXform = new MoveEarthySkyWithEyePointTransform();
   mXform->mAzimuth = 0.f; //zero out the rotation of the dome

    // transform's value isn't knowm until in the cull traversal so its bounding
    // volume is can't be determined, therefore culling will be invalid,
    // so switch it off, this cause all our paresnts to switch culling
    // off as well. But don't worry culling will be back on once underneath
    // this node or any other branch above this transform.
   mXform->setCullingActive(false);
   
   mXform->addChild(MakeDome());
   //transform->addChild(makeBase() );

     // add the transform to the earth sky.
   //clearNode->addChild(transform);

   //group->addChild(clearNode);
   group->addChild(mXform);
   group->setNodeMask(0xf0000000);

   dynamic_cast<osg::Group*>(mNode.get())->addChild(group);
}

osg::Node* dtCore::SkyDome::MakeDome(void)
{
   //5 levels with 18 points each spaced 20 degrees apart

    int i, j;
    float lev[] = { -9.0, 0.0, 7.2, 15.0, 90.0  };
    float cc[][4] =
    {
        { 0.15, 0.25, 0.1 },
        { 0.6, 0.6, 0.7 },
        { 0.4, 0.4, 0.7 },
        { 0.2, 0.2, 0.6 },
        { 0.1, 0.1, 0.6 },
        { 0.1, 0.1, 0.6 },
        { 0.1, 0.1, 0.6 },
    };
    float x, y, z;
    float alpha, theta;
    float radius = 6000.0f;
    int nlev = sizeof( lev )/sizeof(float);

    osg::Geometry *geom = new osg::Geometry;

    osg::Vec3Array& coords = *(new osg::Vec3Array(19*nlev));

    osg::Vec4Array& colors = *(new osg::Vec4Array(19*nlev));

    //osg::Vec2Array& tcoords = *(new osg::Vec2Array(19*nlev));
    
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
//            colors[ci][0] = 0.5f;
//            colors[ci][1] = 0.5f;
//            colors[ci][2] = 0.5;
           colors[ci][3] = 1.f;

            //tcoords[ci][0] = (float)j/18.0;
            //tcoords[ci][1] = (float)i/(float)(nlev-1);

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
    //geom->setTexCoordArray( 0, &tcoords );

    geom->setColorArray( &colors );
    geom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );


    //osg::Texture2D *tex = new osg::Texture2D;
    //tex->setImage(osgDB::readImageFile("target.bmp"));

    osg::StateSet *dstate = new osg::StateSet;

    //dstate->setTextureAttributeAndModes(0, tex, osg::StateAttribute::OFF );
    //dstate->setTextureAttribute(0, new osg::TexEnv );
    dstate->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    dstate->setMode( GL_CULL_FACE, osg::StateAttribute::ON );
    

    // clear the depth to the far plane.
    osg::Depth* depth = new osg::Depth;
    depth->setFunction(osg::Depth::ALWAYS);
    //depth->setRange(1.0,1.0);
    depth->setWriteMask(false);   
    dstate->setAttributeAndModes(depth,osg::StateAttribute::ON );
    dstate->setMode(GL_FOG, osg::StateAttribute::OFF );

    dstate->setRenderBinDetails(-2,"RenderBin");

    //for wireframe rendering
    //osg::PolygonMode *polymode = new osg::PolygonMode;
    //polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    //dstate->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

    geom->setStateSet( dstate );

    mGeode = new osg::Geode;
    mGeode ->addDrawable( geom );

    mGeode ->setName( "Sky" );

    return mGeode;
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
void dtCore::SkyDome::Repaint(const osg::Vec3& sky_color, const osg::Vec3& fog_color, 
                           double sun_angle, double sunAzimuth,
                           double vis)
{
    double diff;
    osg::Vec3 outer_param, outer_amt, outer_diff;
    osg::Vec3 middle_param, middle_amt, middle_diff;
    int i, j;

    //rotate the dome to line up with the sun's azimuth.
    mXform->mAzimuth = sunAzimuth;

    // Check for sunrise/sunset condition
    if ((sun_angle > -10.0)  && (sun_angle < 10.0) )
    {
       // 0.0 - 0.4
       outer_param.set(
          (10.0 - fabs(sun_angle)) / 20.0,
          (10.0 - fabs(sun_angle)) / 40.0,
          -(10.0 - fabs(sun_angle)) / 30.0 );

       middle_param.set(
          (10.0 - fabs(sun_angle)) / 40.0,
          (10.0 - fabs(sun_angle)) / 80.0,
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

    //
    // First, recaclulate the basic colors
    //

    osg::Vec4 center_color;
    osg::Vec4 upper_color[19];
    osg::Vec4 middle_color[19];
    osg::Vec4 lower_color[19];
    osg::Vec4 bottom_color[19];

    double vis_factor, cvf = vis;
    if (cvf > 20000.f)
       cvf = 20000.f;

    if ( vis < 3000.0 ) 
    {
       vis_factor = (vis - 1000.0) / 2000.0;
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
       diff = sky_color[j] - fog_color[j];
       center_color[j] = sky_color[j] - diff * ( 1.0 - vis_factor );
    }
    center_color[3] = 1.0;


    for ( i = 0; i < 9; i++ ) 
    {
       for ( j = 0; j < 3; j++ ) 
       {
          diff = sky_color[j] - fog_color[j];

          upper_color[i][j] = sky_color[j] - diff *
             ( 1.0 - vis_factor * (0.7 + 0.3 * cvf/20000.f) );
          middle_color[i][j] = sky_color[j] - diff *
             ( 1.0 - vis_factor * (0.1 + 0.85 * cvf/20000.f) )
             + middle_amt[j];
          lower_color[i][j] = fog_color[j] + outer_amt[j];

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
          diff = sky_color[j] - fog_color[j];

          upper_color[i][j] = sky_color[j] - diff *
             ( 1.0 - vis_factor * (0.7 + 0.3 * cvf/20000.f) );
          middle_color[i][j] = sky_color[j] - diff *
             ( 1.0 - vis_factor * (0.1 + 0.85 * cvf/20000.f) )
             + middle_amt[j];
          lower_color[i][j] = fog_color[j] + outer_amt[j];

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
       bottom_color[i].set(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
    }

//    for (i=0; i<19; i++)
//    {
//       Notify(ALWAYS, "bottom: %0.2f, %0.2f, %0.2f", bottom_color[i][0], bottom_color[i][1], bottom_color[i][2]);
//    }
//    for (i=0; i<19; i++)
//    {
//       Notify(ALWAYS, "lower: %0.2f, %0.2f, %0.2f", lower_color[i][0], lower_color[i][1], lower_color[i][2]);
//    }
//    for (i=0; i<19; i++)
//    {
//       Notify(ALWAYS, "middle: %0.2f, %0.2f, %0.2f", middle_color[i][0], middle_color[i][1], middle_color[i][2]);
//    }
//    for (i=0; i<19; i++)
//    {
//       Notify(ALWAYS, "upper: %0.2f, %0.2f, %0.2f", upper_color[i][0], upper_color[i][1], upper_color[i][2]);
//    }

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
