#include <osg/PolygonMode>
#include <osg/Group>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osgUtil/Optimizer>
#include <osg/Depth>
#include <osgFX/Scribe>
#include <osgDB/WriteFile>
#include <osgDB/ReaderWriter>

#include <dtCore/flymotionmodel.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/walkmotionmodel.h>
#include <dtCore/compass.h>

#include "viewstate.h"
#include "viewer.h"



using namespace   dtCore;
using namespace   dtABC;


/** static variables */
static   const char* _DefName = "default";
static   ViewState   _CurState( _DefName );



/** static member variables */
const float                Viewer::DEF_X(0.0f);
const float                Viewer::DEF_Y(-100.0f);
const float                Viewer::DEF_Z(30.0f);
const float                Viewer::DEF_H(0.0f);
const float                Viewer::DEF_P(-15.0f);
const float                Viewer::DEF_R(0.0f);
const float                Viewer::MUL_Y(3.0f);
const float                Viewer::MUL_Z(0.5f);
const osg::Node::NodeMask  Viewer::NODEMASK_ON(0xFFFFFFFF);
const osg::Node::NodeMask  Viewer::NODEMASK_OFF(0x00000000);
const int                  Viewer::GRID_LINE_COUNT(49);
const float                Viewer::GRID_LINE_SPACING(2.f);
const unsigned int         Viewer::SCENE_INDX((unsigned int)(-1L));



const char*                Viewer::msgLoadFile("loadfile");
const char*                Viewer::msgGetState("getstate");
const char*                Viewer::msgSaveFileAs("savefileas");

const char*                Viewer::msgSetState("setstate");
const char*                Viewer::msgResetCam("resetcam");



IMPLEMENT_MANAGEMENT_LAYER(Viewer)



Viewer::Viewer( std::string name /*= "Viewer"*/ )
:  Widget(name),
   mInputDevice(NULL),
   mCurState(_CurState)
{
   RegisterInstance( this );
   memset( mMotionModel, 0L, sizeof(mMotionModel) );
   memset( mDispXform, 0L, sizeof(mDispXform) );
}



Viewer::~Viewer( void )
{
   DeregisterInstance( this );
}



void
Viewer::Config( const WinData* d /*= NULL*/ )
{
   System*  sys   = System::GetSystem();
   assert( sys );

   if( sys->IsRunning() )
      // don't configure twice
      return;

   Widget::Config( d );

   SetNotifyLevel(DEBUG_INFO, WARN);

   Scene*   scene = GetScene();
   osg::Group* sceneRoot  = scene->GetSceneNode();
   mViewerNode = new osg::Group();
   sceneRoot->addChild(mViewerNode.get());

   InitInputDevices();
   InitGridPlanes();
   InitCompass();
   InitObjects();

   GetDefaultState( &mCurState );

   Transform   cam;

   mCurState.GetCamPosition( cam, true );

   GetCamera()->SetTransform( &cam );


   // make sure that the global color mask exists.
   osg::ColorMask*   rootColorMask  = new osg::ColorMask;
   rootColorMask->setMask( true, true, true, true);        

   // set up depth to be inherited by the rest of the scene unless
   // overridden. this is overridden in bin 3.
   osg::Depth* rootDepth   = new osg::Depth;
   rootDepth->setFunction( osg::Depth::LESS );
   rootDepth->setRange( 0.0, 1.0 );

   osg::StateSet* rootStateSet = mViewerNode->getOrCreateStateSet();
   rootStateSet->setAttribute( rootColorMask );
   rootStateSet->setAttribute( rootDepth );

   // force default static settings for ViewState
   ViewState vs;
   vs.SetDisplayFlag( ViewState::COMPASS );
   vs.SetDisplayFlag( ViewState::XY_PLANE );
   vs.SetDisplayFlag( ViewState::SCENEPOLY );
   vs.SetDisplayFlag( ViewState::SCENETXT );
   vs.SetDisplayFlag( ViewState::SCENELIGHT );
}



void
Viewer::OnMessage( MessageData* data )
{
   
   if( data->message == msgGetState )
   {
      GetState( reinterpret_cast<ViewState*>(data->userData) );
      return;
   }

   if( data->message == msgSetState )
   {
      SetState( reinterpret_cast<ViewState*>(data->userData) );
      return;
   }

   if( data->message == msgLoadFile )
   {
      LoadFile( reinterpret_cast<ViewState*>(data->userData) );
      return;
   }

   if (data->message == msgSaveFileAs)
   {
      SaveFileAs(reinterpret_cast<char*>(data->userData) );
      return;
   }

   if( data->message == msgResetCam )
   {
      ResetCam();
      return;
   }

   Widget::OnMessage( data );
}



/**Protected */
void Viewer::GetState( ViewState* vs )
{
   assert( vs );

   Transform   cam;
   GetCamera()->GetTransform( &cam );
   mCurState.SetCamPosition( cam );

   OrbitMotionModel* omm   = static_cast<OrbitMotionModel*>(mMotionModel[ORBIT]);
   assert( omm );

   mCurState.SetCamOrbitDist( omm->GetDistance() );

   *vs   = mCurState;
}


//protected
void Viewer::SetState( ViewState* vs )
{
   assert( vs );

   if( mCurState.GetIndex() != vs->GetIndex() )
   {
      // toggle transparency of the geometry
      EnableFile( false, mCurState.GetIndex() );
      EnableFile( true, vs->GetIndex() );

      // set the cam position
      Transform   cam;
      vs->GetCamPosition( cam );
      GetCamera()->SetTransform( &cam );

      OrbitMotionModel* omm   = static_cast<OrbitMotionModel*>(mMotionModel[ORBIT]);
      assert( omm );

      omm->SetDistance( vs->GetCamOrbitDist() );
   }


   // set each of the display items
   if( vs->GetDisplayFlag( ViewState::DISPLAY ) )
   {
      // toggle the display items
      EnableDisplay( vs->GetDisplayFlag( ViewState::COMPASS ), COMPASS );
      EnableDisplay( vs->GetDisplayFlag( ViewState::XY_PLANE ), XY_PLANE );
      EnableDisplay( vs->GetDisplayFlag( ViewState::YZ_PLANE ), YZ_PLANE );
      EnableDisplay( vs->GetDisplayFlag( ViewState::ZX_PLANE ), ZX_PLANE );

      // set the scene view modes
      EnablePolygonMode( FILL, vs->GetDisplayFlag( ViewState::SCENEPOLY ) );
      EnablePolygonMode( WIRE, vs->GetDisplayFlag( ViewState::SCENEWIRE ) );
      EnableTexture( vs->GetDisplayFlag( ViewState::SCENETXT ) );
      EnableLighting( vs->GetDisplayFlag( ViewState::SCENELIGHT ) );
   }

   // set the model view modes
   if(   mCurState.GetDisplayFlag( ViewState::POLYGON ) !=
         vs->GetDisplayFlag( ViewState::POLYGON )       )
   {
      EnablePolygonMode( FILL, vs->GetDisplayFlag( ViewState::POLYGON ), vs->GetIndex() );
   }

   if(   mCurState.GetDisplayFlag( ViewState::WIREFRAME ) !=
         vs->GetDisplayFlag( ViewState::WIREFRAME )       )
   {
      EnablePolygonMode( WIRE, vs->GetDisplayFlag( ViewState::WIREFRAME ), vs->GetIndex() );
   }

   if(   mCurState.GetDisplayFlag( ViewState::TEXTURE ) !=
         vs->GetDisplayFlag( ViewState::TEXTURE )       )
   {
      EnableTexture( vs->GetDisplayFlag( ViewState::TEXTURE ), vs->GetIndex() );
   }

   if(   mCurState.GetDisplayFlag( ViewState::LIGHTING ) !=
         vs->GetDisplayFlag( ViewState::LIGHTING )       )
   {
      EnableLighting( vs->GetDisplayFlag( ViewState::LIGHTING ), vs->GetIndex() );
   }


   // set the motion model
   if(   mCurState.GetMotionFlag( ViewState::FLY ) !=
         vs->GetMotionFlag( ViewState::FLY )       )
   {
      if( vs->GetMotionFlag( ViewState::FLY ) )
         EnableMotionModel( FLY );
   }

   if(   mCurState.GetMotionFlag( ViewState::ORBIT )  !=
         vs->GetMotionFlag( ViewState::ORBIT )        )
   {
      if( vs->GetMotionFlag( ViewState::ORBIT ) )
      {
         EnableMotionModel( ORBIT );

         OrbitMotionModel* omm   = static_cast<OrbitMotionModel*>(mMotionModel[ORBIT]);
         assert( omm );

         omm->SetDistance( vs->GetCamOrbitDist() );
      }
   }

   if(   mCurState.GetMotionFlag( ViewState::UFO ) !=
         vs->GetMotionFlag( ViewState::UFO )       )
   {
      if( vs->GetMotionFlag( ViewState::UFO ) )
         EnableMotionModel( UFO );
   }

   if(   mCurState.GetMotionFlag( ViewState::WALK )   !=
         vs->GetMotionFlag( ViewState::WALK )         )
   {
      if( vs->GetMotionFlag( ViewState::WALK ) )
         EnableMotionModel( WALK );
   }


   // set the joysticks
   if(   mCurState.GetJoystickFlag( ViewState::JOY_1 )   !=
         vs->GetJoystickFlag( ViewState::JOY_1 )         )
   {
      EnableJoystick( vs->GetJoystickFlag( ViewState::JOY_1 ), JOY_1 );
   }

   if(   mCurState.GetJoystickFlag( ViewState::JOY_2 )   !=
         vs->GetJoystickFlag( ViewState::JOY_2 )         )
   {
      EnableJoystick( vs->GetJoystickFlag( ViewState::JOY_2 ), JOY_2 );
   }

   mCurState = *vs;
}



void
Viewer::GetDefaultState( ViewState* vs )
{
   assert( vs );

   sgVec3   pos      = { DEF_X, DEF_Y, DEF_Z };
   sgVec3   lookat   = { 0.f, 0.f, 0.f };
   sgVec3   up       = { 0.f, 0.f, 1.f };

   Transform   cam;
   cam.SetLookAt( pos, lookat, up );

   float dist(sgDistanceVec3( lookat, pos ));

   vs->SetCamPosition( cam, true );
   vs->SetCamPosition( cam, false );
   vs->SetCamOrbitDist( dist, true );
   vs->SetCamOrbitDist( dist, false );
   vs->SetDisplayFlag( ViewState::POLYGON, true );
   vs->SetDisplayFlag( ViewState::WIREFRAME, false );
   vs->SetDisplayFlag( ViewState::TEXTURE, true );
   vs->SetDisplayFlag( ViewState::LIGHTING, true );
   vs->SetMotionFlag( ViewState::ORBIT );
   vs->SetJoystickFlag( ViewState::JOY_1, false );
   vs->SetJoystickFlag( ViewState::JOY_2, false );
}


//protected
void Viewer::LoadFile( ViewState* vs )
{
   assert( vs );

   // get the filename
   std::string filename = vs->GetFilename();
   assert( filename != "" );


   // generate default states for this object
   GetDefaultState( vs );


   // create a new p51 object
   Object*  fileobj  = new Object;
   assert( fileobj );

   // load the graphics file from disk
   bool fileLoaded = false;
   fileLoaded = fileobj->LoadFile( filename );

   if (!fileLoaded) 
   {
      //tell the GUI the file didn't load
      //SendMessage("fileNotLoaded", (void*)(filename.c_str()));
      FileLoaded( false, filename.c_str() );
      return;
   }
   else 
   {
      //Tell the GUI the file loaded
      //SendMessage("fileLoaded", (void*)(filename.c_str()));
      FileLoaded( true, filename.c_str() );
   }

   osg::Node*  filenode = fileobj->GetOSGNode();
   assert( filenode );


   // set up the scribe node (turned off) then attach the file object
   osgFX::Scribe* scribe   = new osgFX::Scribe;
   assert( scribe );
   scribe->setName("fileScribe");

   scribe->setEnabled( false );
   scribe->addChild( filenode );

   // set default cam position for this object based on it's bounding sphere
   osg::BoundingSphere  bs(scribe->getBound());

   sgVec3   pos      = { bs.center().x(), bs.center().y() - bs.radius() * MUL_Y, bs.center().z() + bs.radius() * MUL_Z };
   sgVec3   lookat   = { bs.center().x(), bs.center().y(), bs.center().z() };
   sgVec3   up       = { 0.f, 0.f, 1.f };

   Transform   cam;
   cam.SetLookAt( pos, lookat, up );
    
   float dist(sgDistanceVec3( lookat, pos ));

   vs->SetCamPosition( cam, true );
   vs->SetCamPosition( cam, false );
   vs->SetCamOrbitDist( dist, true );
   vs->SetCamOrbitDist( dist, false );

   // turn off node visibility
   scribe->setNodeMask( NODEMASK_OFF );
   
   // add the object to the scene
   osg::Group* scenenode  = GetDisplayObj( FILEOBJS );
   assert( scenenode );

   scenenode->addChild( scribe );
}

//protected
void Viewer::SaveFileAs( char *filename )
{
   assert(filename!=NULL);

   osg::Group *root = GetFileObj(mCurState.GetIndex());
   assert(root!=NULL);

   osg::Group *child = static_cast<osg::Group*>(root->getChild( FILL ));
   assert( child );

   if (osgDB::writeNodeFile(*child, filename))
   {
      Notify(ALWAYS,"File %s Saved", filename);
   }
}


//Protected
void Viewer::ResetCam( void )
{
   Transform   cam;
   mCurState.GetCamPosition( cam, true );
   mCurState.SetCamPosition( cam );

   if( mCurState.GetMotionFlag( ViewState::ORBIT ) )
   {
      float dist(mCurState.GetCamOrbitDist( true ));
      mCurState.SetCamOrbitDist( dist );
      static_cast<OrbitMotionModel*>(mMotionModel[ORBIT])->SetDistance( dist );
   }

   GetCamera()->SetTransform( &cam );
}



void
Viewer::EnableFile( bool on, unsigned int indx )
{
   osg::Group*  node  = GetFileObj( indx );

   if( node == NULL )
      return;

   if( on )
      node->setNodeMask( NODEMASK_ON );
   else
      node->setNodeMask( NODEMASK_OFF );
}



void
Viewer::EnableDisplay( bool on, DISPLAYITEM di )
{
   osg::Group*  node  = GetDisplayObj( di );
   assert( node );

   if( on )
   {
      node->setNodeMask( NODEMASK_ON );
   }
   else
   {
      node->setNodeMask( NODEMASK_OFF );
   }
}



void
Viewer::EnablePolygonMode( POLYGONMODE mode, bool on, unsigned int indx /*= SCENE_INDX*/ )
{
   osgFX::Scribe* node  = NULL;

   if( indx == SCENE_INDX )
   {
      node  = static_cast<osgFX::Scribe*>(GetDisplayObj( FILEOBJS ));
      assert( node );
   }
   else
   {
      node  = static_cast<osgFX::Scribe*>(GetFileObj( indx ));
      assert( node );
   }

   switch( mode )
   {
      case  FILL:
         {
            osg::StateSet* ss = node->getOrCreateStateSet();
            assert( ss );

            osg::PolygonMode* pm =
            static_cast<osg::PolygonMode*>(ss->getAttribute( osg::StateAttribute::POLYGONMODE ));

            if( pm == NULL )
            {
               pm = new osg::PolygonMode;
               assert( pm );

               ss->setAttribute( pm );
            }

            if( on )
            {
               pm->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
               ss->setAttribute( pm, osg::StateAttribute::OFF );
            }
            else
            {
               pm->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
               ss->setAttribute( pm, osg::StateAttribute::OVERRIDE );
            }
         }
         break;

      case  WIRE:
         {
            // toggling scribe's effect on/off
            node->setEnabled( on );
         }
         break;

      default:
         assert( false );
         break;
   }
}



void
Viewer::EnableTexture( bool on, unsigned int indx /*= SCENE_INDX*/ )
{
   osg::Group*  node  = NULL;
   osg::Group* child  = NULL;

   if( indx == SCENE_INDX )
   {
      child = GetDisplayObj( FILEOBJS );
   }
   else
   {
      node  = GetFileObj( indx );
      assert( node );

      child  = static_cast<osg::Group*>(node->getChild( FILL ));
      assert( child );
   }

   osg::StateSet* ss = child->getOrCreateStateSet();
   assert( ss );

   if( on )
   {
      ss->setTextureMode( 0, GL_TEXTURE_2D, osg::StateAttribute::ON );
   }
   else
   {
      ss->setTextureMode( 0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
   }
}



void
Viewer::EnableLighting( bool on, unsigned int indx /*= SCENE_INDX*/ )
{
   osg::Group*  node  = NULL;
   osg::Group* child  = NULL;

   if( indx == SCENE_INDX )
   {
      child = GetDisplayObj( FILEOBJS );
   }
   else
   {
      node  = GetFileObj( indx );
      assert( node );

      child  = static_cast<osg::Group*>(node->getChild( FILL ));
      assert( child );
   }

   osg::StateSet* ss = child->getOrCreateStateSet();
   assert( ss );

   if( on )
   {
      ss->setMode( GL_LIGHTING, osg::StateAttribute::ON );
   }
   else
   {
      ss->setMode( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
   }
}



void
Viewer::EnableMotionModel( MOTIONMODEL mm )
{
   assert( mm <= NUMMOTIONMODELS );

   for( int ii(WALK); ii < NUMMOTIONMODELS; ii++ )
   {
      mMotionModel[ii]->SetEnabled( ii == mm );
   }
}



void
Viewer::EnableJoystick( bool on, JOYSTICKID jy )
{
}



void
Viewer::InitInputDevices( void )
{
   mInputDevice   = new LogicalInputDevice;

   dtCore::Keyboard* k     = GetKeyboard();
   assert( k );

   dtCore::Mouse* m     = GetMouse();
   assert( m );

   Axis* leftButtonUpAndDown  =
         mInputDevice->AddAxis(
                                 "left mouse button up/down",
                                 new ButtonAxisToAxis(
                                                         m->GetButton( LeftButton ),
                                                         m->GetAxis( 1 )
                                                     )
                              );


   Axis* leftButtonLeftAndRight  =
         mInputDevice->AddAxis(
                                 "left mouse button left/right",
                                 new ButtonAxisToAxis(
                                                         m->GetButton( LeftButton ),
                                                         m->GetAxis( 0 )
                                                     )
                              );


   Axis* middleButtonUpAndDown   =
         mInputDevice->AddAxis(
                                 "middle mouse button up/down",
                                 new ButtonAxisToAxis(
                                                         m->GetButton( MiddleButton ),
                                                         m->GetAxis( 1 )
                                                     )
                              );


   Axis* rightButtonUpAndDown    =
         mInputDevice->AddAxis(
                                 "right mouse button up/down",
                                 new ButtonAxisToAxis(
                                                         m->GetButton( RightButton ),
                                                         m->GetAxis( 1 )
                                                     )
                              );


   Axis* rightButtonLeftAndRight =
         mInputDevice->AddAxis(
                                 "right mouse button left/right",
                                 new ButtonAxisToAxis(
                                                         m->GetButton( RightButton ),
                                                         m->GetAxis( 0 )
                                                     )
                              );


   Axis* arrowKeysUpAndDown      =
         mInputDevice->AddAxis(
                                 "arrow keys up/down",
                                 new ButtonsToAxis(
                                                      k->GetButton( Producer::Key_Down ),
                                                      k->GetButton( Producer::Key_Up )
                                                  )
                              );


   Axis* arrowKeysLeftAndRight   =
         mInputDevice->AddAxis(
                                 "arrow keys left/right",
                                 new ButtonsToAxis(
                                                      k->GetButton( Producer::Key_Left ),
                                                      k->GetButton( Producer::Key_Right )
                                                  )
                              );


   Axis* wsKeysUpAndDown         =
         mInputDevice->AddAxis(
                                 "w/s keys up/down",
                                 new ButtonsToAxis(
                                                      k->GetButton( Producer::Key_S ),
                                                      k->GetButton( Producer::Key_W )
                                                  )
                              );


   Axis* adKeysLeftAndRight      =
         mInputDevice->AddAxis(
                                 "a/d keys left/right",
                                 new ButtonsToAxis(
                                                      k->GetButton( Producer::Key_A ),
                                                      k->GetButton( Producer::Key_D )
                                                  )
                              );


   Axis* primaryUpAndDown        =
         mInputDevice->AddAxis(
                                 "primary up/down",
                                 new AxesToAxis(
                                                   arrowKeysUpAndDown,
                                                   leftButtonUpAndDown
                                               )
                              );


   Axis* secondaryUpAndDown      =
         mInputDevice->AddAxis(
                                 "secondary up/down",
                                 new AxesToAxis(
                                                   wsKeysUpAndDown,
                                                   rightButtonUpAndDown
                                               )
                              );


   Axis* primaryLeftAndRight     =
         mInputDevice->AddAxis(
                                 "primary left/right",
                                 new AxesToAxis(
                                                   arrowKeysLeftAndRight,
                                                   leftButtonLeftAndRight
                                               )
                              );


   Axis* secondaryLeftAndRight   =
         mInputDevice->AddAxis(
                                 "secondary left/right",
                                 new AxesToAxis(
                                                   adKeysLeftAndRight,
                                                   rightButtonLeftAndRight
                                               )
                              );



   WalkMotionModel*  wmm   = new WalkMotionModel;
   assert( wmm );

   wmm->SetScene( GetScene() );
   wmm->SetWalkForwardBackwardAxis( primaryUpAndDown );
   wmm->SetTurnLeftRightAxis( primaryLeftAndRight );
   wmm->SetSidestepLeftRightAxis( secondaryLeftAndRight );
   mMotionModel[WALK]  = wmm;


   FlyMotionModel*   fmm   = new FlyMotionModel;
   assert( fmm );

   fmm->SetTurnUpDownAxis( primaryUpAndDown );
   fmm->SetTurnLeftRightAxis( primaryLeftAndRight );
   fmm->SetFlyForwardBackwardAxis( secondaryUpAndDown );
   mMotionModel[FLY]   = fmm;


   UFOMotionModel*   umm   = new UFOMotionModel;
   assert( umm );

   umm->SetFlyForwardBackwardAxis( primaryUpAndDown );
   umm->SetFlyLeftRightAxis( primaryLeftAndRight );
   umm->SetFlyUpDownAxis( secondaryUpAndDown );
   umm->SetTurnLeftRightAxis( secondaryLeftAndRight );
   mMotionModel[UFO]   = umm;


   OrbitMotionModel* omm   = new OrbitMotionModel;
   assert( omm );

   omm->SetAzimuthAxis( primaryLeftAndRight );
   omm->SetElevationAxis( primaryUpAndDown );
   omm->SetDistanceAxis( middleButtonUpAndDown );
   omm->SetLeftRightTranslationAxis( secondaryLeftAndRight );
   omm->SetUpDownTranslationAxis( secondaryUpAndDown );

   #if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
   omm->SetAngularRate(0.75);
   omm->SetLinearRate(0.025f);
   #endif
   
   mMotionModel[ORBIT]  = omm;


   for( int ii(WALK); ii < NUMMOTIONMODELS; ii++ )
   {  
      mMotionModel[ii]->SetTarget( GetCamera() );
   }

   EnableMotionModel( ORBIT );
}



void
Viewer::InitObjects( void )
{
   Scene*   scene = GetScene();
   assert( scene );

   //osg::Group*  root  = scene->GetSceneNode();
   //assert( root );

   osgFX::Scribe* scribe   = new osgFX::Scribe;
   assert( scribe );
   scribe->setName("HeadScribe");

   mViewerNode->addChild( scribe );

   scribe->setEnabled( false );
}



void
Viewer::InitCompass( void )
{
   dtCore::Camera*  cam   = GetCamera();
   assert( cam );

   Compass* compass  = new Compass( cam );
   assert( compass );

   //AddDrawable( compass );
   mViewerNode->addChild(compass->GetOSGNode());
}



void
Viewer::InitGridPlanes()
{
   const int   numVerts(2 * 2 * GRID_LINE_COUNT);
   const float length(( ( GRID_LINE_COUNT - 1 ) * GRID_LINE_SPACING ) / 2.f);

   osg::Vec3   verts[numVerts];
   int         indx(0L);
   for( int ii(0L); ii < GRID_LINE_COUNT; ii++ )
   {
      verts[indx++].set( -length + ii * GRID_LINE_SPACING,  length, 0.0f );
      verts[indx++].set( -length + ii * GRID_LINE_SPACING, -length, 0.0f );
   }

   for( int ii(0L); ii < GRID_LINE_COUNT; ii++ )
   {
      verts[indx++].set(  length, -length + ii * GRID_LINE_SPACING, 0.0f );
      verts[indx++].set( -length, -length + ii * GRID_LINE_SPACING, 0.0f );
   }


   osg::Geometry* geome = new osg::Geometry;
   assert( geome );

   geome->setVertexArray( new osg::Vec3Array(numVerts, verts) );
   geome->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numVerts) );


   osg::Geode* geode = new osg::Geode;
   assert( geode );

   geode->addDrawable( geome );
   geode->getOrCreateStateSet()->setMode( GL_LIGHTING, 0 );


   mDispXform[XY_PLANE] = new osg::MatrixTransform;
   assert( mDispXform[XY_PLANE] != NULL );

   mDispXform[XY_PLANE]->addChild( geode );
   mDispXform[XY_PLANE]->setNodeMask( NODEMASK_ON );


   mDispXform[YZ_PLANE] = new osg::MatrixTransform;
   assert( mDispXform[YZ_PLANE] != NULL );
   
   mDispXform[YZ_PLANE]->setMatrix( osg::Matrix::rotate( osg::PI_2, 0, 1, 0 ) );
   mDispXform[YZ_PLANE]->addChild( geode );
   mDispXform[YZ_PLANE]->setNodeMask( NODEMASK_OFF );


   mDispXform[ZX_PLANE] = new osg::MatrixTransform;
   assert( mDispXform[ZX_PLANE] != NULL );
   
   mDispXform[ZX_PLANE]->setMatrix( osg::Matrix::rotate( osg::PI_2, 1, 0, 0 ) );
   mDispXform[ZX_PLANE]->addChild( geode );
   mDispXform[ZX_PLANE]->setNodeMask( NODEMASK_OFF );


   //Scene* scene   = GetScene();
   //assert( scene );

   //osg::Group* root = scene->GetSceneNode();
   //assert( root != NULL );

   mViewerNode->addChild( mDispXform[XY_PLANE] );
   mViewerNode->addChild( mDispXform[YZ_PLANE] );
   mViewerNode->addChild( mDispXform[ZX_PLANE] );
}



osg::Group*
Viewer::GetFileObj( unsigned int indx )
{
   //Scene*   scene = GetScene();
   //assert( scene );

   //osg::Group* root  = scene->GetSceneNode();
   //( root );


   osg::Group* objs  = static_cast<osg::Group*>(mViewerNode->getChild( FILEOBJS ));
   assert( objs );

   if( indx >= objs->getNumChildren() )
      return   NULL;

   return   static_cast<osg::Group*>(objs->getChild( indx ));
}



osg::Group*
Viewer::GetDisplayObj( unsigned int indx )
{
   //Scene*   scene = GetScene();
   //assert( scene );

   //osg::Group* root  = scene->GetSceneNode();
   //assert( root );

   if( indx >= NUMDISPLAYITEMS )
      return   NULL;

   return   static_cast<osg::Group*>(mViewerNode->getChild( indx ));
}
