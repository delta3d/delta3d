#include <cassert>

#include <system.h>
#include <globals.h>

#include "helperstructs.h"
#include "viewer.h"



using namespace   dtCore;
using namespace   std;


const string Viewer::DEF_PATH("../../data;");
const string Viewer::DEF_FILE("cessna.osg");



IMPLEMENT_MANAGEMENT_LAYER(Viewer)



/**
 * Constructor.
 */
Viewer::Viewer()
:  Base(),
   cfg(false),
   cam(NULL),
   cmd(0L)
{
   RegisterInstance( this );
   ctor();
}



/**
 * Destructor.
 */
Viewer::~Viewer()
{
   DeregisterInstance( this );
}



/**
 * Base override to receive messages.
 *
 * @param data the message to receive
 */
void
Viewer::OnMessage( MessageData* data )
{
   assert( data );

   if( data->message == "step" )
   {
      Step();
      return;
   }

   if( data->message == "preframe" )
   {
      PreFrame( *(reinterpret_cast<const double*>(data->userData)) );
      return;
   }

   if( data->message == "frame" )
   {
      Frame( *(reinterpret_cast<const double*>(data->userData)) );
      return;
   }

   if( data->message == "postframe" )
   {
      PostFrame( *(reinterpret_cast<const double*>(data->userData)) );
      return;
   }

   if( data->message == "resize" )
   {
      rect   = *(reinterpret_cast<WinRect*>(data->userData));
      cmd   |= RESIZE;
      return;
   }

   if( data->message == "windata" )
   {
      Config( reinterpret_cast<WinData*>(data->userData) );
      return;
   }
}



/**
 * Configure dtCore to render in a given window.
 *
 * @param data the window handle and size
 */
void
Viewer::Config( const WinData* d /*= NULL*/ )
{
   if( cfg )
      // don't config twice
      return;

   cfg   = true;

   System*  sys   = System::GetSystem();
   assert( sys );

   AddSender( sys );

   if( d != NULL )
   {
      Producer::Camera*          pcam  = cam->GetCamera();
      assert( pcam );

      Producer::RenderSurface*   prs   = pcam->getRenderSurface();
      assert( prs );

      prs->setWindowRectangle( d->pos_x, d->pos_y, d->width, d->height );
      prs->setWindow( d->hwnd );
      sys->Config();
      sys->Start();

      rect  = *d;
   }
   else
   {
      sys->Config();
   }
}



/**
 * Handle a window resize event.
 */
void
Viewer::Resize( void )
{
   cmd   &= ~RESIZE;

   Producer::Camera*          pcam  = cam->GetCamera();
   assert( pcam );

   Producer::RenderSurface*   prs   = pcam->getRenderSurface();
   assert( prs );

   prs->setWindowRectangle( rect.pos_x, rect.pos_y, rect.width, rect.height, false );
}



/**
 * Advance a single frame cycle.
 */
void
Viewer::Step( void )
{
   System*  sys   = System::GetSystem();
   assert( System::GetSystem() );

   sys->Step();
   SendMessage( "redraw" );
}



/**
 * Called durring the frame cycle steps.
 *
 * @param deltaFrameTime time since last call
 */
void
Viewer::PreFrame( const double deltaFrameTime )
{
   if( cmd & RESIZE )
      Resize();
}



/**
 * Called durring the frame cycle steps.
 *
 * @param deltaFrameTime time since last call
 */
void
Viewer::Frame( const double deltaFrameTime )
{
}



/**
 * Called durring the frame cycle steps.
 *
 * @param deltaFrameTime time since last call
 */
void
Viewer::PostFrame( const double deltaFrameTime )
{
}



/**
 * Inline unified constructor.
 */
void
Viewer::ctor( void )
{
   SetDataFilePathList(DEF_PATH + GetDeltaDataPathList());


   // create the scene
   dtCore::Scene* scn   = new dtCore::Scene;
   assert( scn );

   // show the frame rate statistics
   scn->SetNextStatisticsType();


   // create the camera
   cam   = new dtCore::Camera;
   assert( cam );

   // attach camera to the scene
   cam->SetScene( scn );


   // create the one single object
   Object*  obj   = new Object( DEF_FILE );
   assert( obj );

   // load the default file into the object and add to the scene
   obj->LoadFile( DEF_FILE );
   scn->AddDrawable( obj );


   // get the bounding sphere of all objects in the scene
   osg::Node*  node  = scn->GetSceneNode();
   assert( node );

   osg::BoundingSphere  bs(node->getBound());


   // set the camera's position based on the scene's bounding sphere
   sgVec3   eye   = { bs.center().x(), bs.center().y() - bs.radius() * 2.5f, bs.center().z() + bs.radius() * 0.5f };
   sgVec3   targ  = { bs.center().x(), bs.center().y(), bs.center().z() };
   sgVec3   up    = { 0.f, 0.f, 1.f };

   Transform   pos(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
   pos.SetLookAt( eye, targ, up );

   cam->SetTransform( &pos );
}
