#include "MyWidget.h"


using namespace   dtCore;
using namespace   dtABC;



MyWidget::MyWidget( std::string name /*= "MyWidget"*/ )
:  Widget(name),
   mInputDevice(NULL),
   mMotionModel(NULL)
{
}



MyWidget::~MyWidget( void )
{
}



void
MyWidget::OnMessage( MessageData* data )
{
   if( data->message == "loadfile")
   {
      assert( data->userData );

      std::string filename = *(reinterpret_cast<std::string*>(data->userData));
      assert( filename != "" );

      Object*  obj   = new Object(filename);
      assert( obj );

      obj->LoadFile( filename );

      //SendMessage( "fileloaded", obj );
      ObjectLoaded( obj );
      return;
   }

   if( data->message == "fileloaded" )
   {
      ObjectLoaded( reinterpret_cast<dtCore::Object*>(data->userData) );
      return;
   }

   Widget::OnMessage( data );
}



void
MyWidget::Config( const WinData* d /*= NULL*/ )
{
   Widget::Config( d );

   InitInputDevices();

   GetScene()->SetNextStatisticsType();
}



void
MyWidget::ObjectLoaded( dtCore::Object* obj )
{
   assert( obj );

   AddDrawable( obj );


   osg::Node*  node  = obj->GetOSGNode();
   assert( node );


   osg::BoundingSphere  bs(node->getBound());


   // set the camera's position based on the scene's bounding sphere
   sgVec3   eye   = { bs.center().x(), bs.center().y() - bs.radius() * 3.0f, bs.center().z() + bs.radius() * 0.5f };
   sgVec3   targ  = { bs.center().x(), bs.center().y(), bs.center().z() };
   sgVec3   up    = { 0.f, 0.f, 1.f };

   Transform   pos(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
   pos.SetLookAt( eye, targ, up );

   GetCamera()->SetTransform( &pos );
   static_cast<OrbitMotionModel*>(mMotionModel.get())->SetDistance( sgDistanceVec3( targ, eye ) );
}



void
MyWidget::InitInputDevices( void )
{
   mInputDevice   = new LogicalInputDevice;
   assert( mInputDevice.get() );

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


   OrbitMotionModel* omm   = new OrbitMotionModel;
   assert( omm );

   omm->SetAzimuthAxis( primaryLeftAndRight );
   omm->SetElevationAxis( primaryUpAndDown );
   omm->SetDistanceAxis( middleButtonUpAndDown );
   omm->SetLeftRightTranslationAxis( secondaryLeftAndRight );
   omm->SetUpDownTranslationAxis( secondaryUpAndDown );

   mMotionModel   = omm;
   mMotionModel->SetTarget( GetCamera() );
   mMotionModel->SetEnabled( true );

   //float angRate = omm->GetAngularRate();
   //omm->SetAngularRate(1.0f);
   
   //float linRate = omm->GetLinearRate();
   //omm->SetLinearRate(0.05f);

   //Notify(ALWAYS,"angRate = %f, linRate = %f\n",angRate,linRate);
}
