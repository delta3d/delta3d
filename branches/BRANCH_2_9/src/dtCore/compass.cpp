#include <prefix/dtcoreprefix.h>
#include <dtCore/compass.h>

#include <cassert>

#include <osg/Vec3>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Projection>
#include <osgText/Text>

#include <dtUtil/mathdefines.h>

#include <dtCore/deltawin.h>
#include <dtCore/transform.h>
#include <dtCore/pointaxis.h>
#include <dtCore/camera.h>

// This exists to fix a compile error where M_PI is undefined.
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES 1
#endif
#include <math.h>



using namespace dtCore;
IMPLEMENT_MANAGEMENT_LAYER(Compass)

// callback functor objects
class _updateCompassCallback  :  public   osg::NodeCallback
{
   public:
                     _updateCompassCallback( dtCore::Compass* compass )
                     :  mCompass(0)
                     {
                        assert( compass );
                        mCompass = compass;
                     }

      virtual  void  operator()( osg::Node* node, osg::NodeVisitor* nv )
      {

         assert( node );
         assert( nv );

         osg::MatrixTransform*   xformNode   = static_cast<osg::MatrixTransform*>(node);

         Transform   xform;
         mCompass.get()->GetTransform( xform );

         osg::Matrix mat;
         xform.Get( mat );

         if( dtCore::Camera*   cam   = mCompass->GetCamera() )
         {
            Transform   cam_xform;
            cam->GetTransform( cam_xform );

            osg::Matrix   cam_mat;
            cam_xform.GetRotation( cam_mat );

            osg::Vec3   cam_pos;
            cam_xform.GetTranslation( cam_pos );

            osg::Matrix comp_mat = osg::Matrix::identity();
            comp_mat *= osg::Matrix::inverse( cam_mat );
            comp_mat *= osg::Matrix::rotate( M_PI * -0.5, 1.0f, 0.0f, 0.0f );
            comp_mat *= osg::Matrix::translate( osg::Vec3( 20.0f, 20.0f, -100.0f ) );


            osg::Projection*  proj  = static_cast<osg::Projection*>(xformNode->getChild( 0L ));
            assert( proj );

            osg::MatrixTransform*   abs_xform  = static_cast<osg::MatrixTransform*>(proj->getChild( 0L ));
            assert( abs_xform );

            abs_xform->setMatrix( comp_mat );
         }

         xformNode->setMatrix( mat );

         traverse( node, nv );
      }

   private:
      RefPtr<dtCore::Compass> mCompass;
};



// static member variables
const float                Compass::MAX_SCREEN_X(1.0f);
const float                Compass::MAX_SCREEN_Y(1.0f);
const float                Compass::MIN_SCREEN_X(-1.0f);
const float                Compass::MIN_SCREEN_Y(-1.0f);
const float                Compass::DEF_SCREEN_X(320.0f);
const float                Compass::DEF_SCREEN_Y(240.0f);
const float                Compass::DEF_SCREEN_W(640.0f);
const float                Compass::DEF_SCREEN_H(480.0f);
const float                Compass::DEF_AXIS_SIZE(50.0f);



// default constructor
Compass::Compass( Camera* cam )
:
   mCamera(cam),
   mScreenX(DEF_SCREEN_X),
   mScreenY(DEF_SCREEN_Y),
   mScreenW(DEF_SCREEN_W),
   mScreenH(DEF_SCREEN_H)
{
   RegisterInstance( this );

   ctor();

   //hookup an update callback on this node
   GetOSGNode()->setUpdateCallback( new _updateCompassCallback( this ) );

   // set render depth so that it is not culled from scene due to occluding geometry
   osg::StateSet* state = GetOSGNode()->getOrCreateStateSet() ;
   state->setMode( GL_DEPTH_TEST , osg::StateAttribute::OFF ) ;

   // Need to make sure this geometry is draw last. RenderBins are handled
   // in numerical order so set bin number to 99
   state->setRenderBinDetails( 99, "RenderBin" ) ;
}



// destructor
Compass::~Compass()
{
   DeregisterInstance( this );
}



/** PUBLIC MEMBER FUNCTIONS */

void
Compass::GetScreenPosition( float& x, float& y )   const
{
   x  = mScreenX;
   y  = mScreenY;
}

osg::Vec2
Compass::GetScreenPosition()   const
{
   return osg::Vec2( mScreenX, mScreenY );
}

void
Compass::SetScreenPosition( float x, float y )
{
   dtUtil::Clamp<float>( x, MIN_SCREEN_X, MAX_SCREEN_X );
   dtUtil::Clamp<float>( y, MIN_SCREEN_Y, MAX_SCREEN_Y );
   mScreenX = x;
   mScreenY = y;
}


void
Compass::SetScreenPosition( const osg::Vec2& xy )
{
   SetScreenPosition( xy.x(), xy.y() );
}

dtCore::Camera*
Compass::GetCamera()
{
   return   mCamera.get();
}



void
Compass::SetCamera( dtCore::Camera* cam )
{
   mCamera  = cam;

   SetWindow( cam->GetWindow() );
}



/** PRIVATE MEMBER FUNCTIONS */
void
Compass::ctor()
{
   PointAxis*  axis = new PointAxis;
   assert( axis );

   axis->SetLength( PointAxis::X, DEF_AXIS_SIZE );
   axis->SetLength( PointAxis::Y, DEF_AXIS_SIZE );
   axis->SetLength( PointAxis::Z, DEF_AXIS_SIZE );
   axis->Enable( PointAxis::LABEL_X );
   axis->Enable( PointAxis::LABEL_Y );
   axis->Enable( PointAxis::LABEL_Z );

   osg::MatrixTransform*   modelview_abs  = new osg::MatrixTransform;
   assert( modelview_abs );

   modelview_abs->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
   modelview_abs->setMatrix( osg::Matrix::identity() );
   modelview_abs->addChild( axis->GetOSGNode() );


   osg::Projection*  projection  = new osg::Projection;
   assert( projection );

   projection->setMatrix( osg::Matrix::ortho( 0.f , mScreenW , 0.f , mScreenH , -200.0 , 200.0 ) ) ;
   projection->addChild( modelview_abs );

   GetMatrixNode()->addChild( projection );

   SetName( "Compass" );

   if( mCamera.get() )
   {
      SetWindow( mCamera->GetWindow() );
   }
}



void
Compass::SetWindow( dtCore::DeltaWin* win )
{
   if( win == 0 )
   {
      return;
   }

   int   x(0L);
   int   y(0L);
   int   w(0L);
   int   h(0L);

   win->GetPosition( x, y, w, h );
   assert( w > 0 );
   assert( h > 0 );

   mScreenW = float(w);
   mScreenH = float(h);

   mScreenX = mScreenW * 0.5f;
   mScreenY = mScreenH * 0.5f;
}
