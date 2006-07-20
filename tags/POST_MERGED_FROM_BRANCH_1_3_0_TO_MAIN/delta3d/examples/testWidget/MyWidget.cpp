#include "MyWidget.h"
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4244) // for warning C4244: '=' : conversion from 'short' to 'uchar', possible loss of data
#endif

#include "TestWidget.h"

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

using namespace dtCore;
using namespace dtABC;

MyWidget::EscapeListener::EscapeListener(const MyWidget::QuitFunctor& quitFunctor) :
   mQuitFunctor(quitFunctor)
{
}

MyWidget::EscapeListener::~EscapeListener()
{
}
   
bool MyWidget::EscapeListener::HandleKeyPressed( const Keyboard* keyboard, 
                                                 Producer::KeyboardKey key,
                                                 Producer::KeyCharacter character )
{
   switch(key)
   {
      case Producer::Key_Escape:
      {
         mQuitFunctor();
         return true;
      }
      default:
      {
      }
   }

   return false;
}

bool MyWidget::EscapeListener::HandleKeyReleased( const Keyboard* keyboard, 
                                                  Producer::KeyboardKey key,
                                                  Producer::KeyCharacter character )
{
   return false;
}

bool MyWidget::EscapeListener::HandleKeyTyped( const Keyboard* keyboard, 
                                               Producer::KeyboardKey key,
                                               Producer::KeyCharacter character )
{
   return false;
}
    
MyWidget::MyWidget( const std::string& name /*= "MyWidget"*/ )
:  Widget(name),
   mMotionModel(NULL)
{
   
}

MyWidget::~MyWidget()
{
}

void MyWidget::OnMessage( MessageData* data )
{
   if( data->message == "loadfile")
   {
      assert( data->userData );

      std::string filename = *(static_cast<std::string*>(data->userData));
      assert( !filename.empty() );

      Object* obj = new Object(filename);
      assert( obj );

      obj->LoadFile( filename );

      ObjectLoaded( obj );
      return;
   }
   else if( data->message == "fileloaded" )
   {
      ObjectLoaded( static_cast<dtCore::Object*>(data->userData) );
      return;
   }

   Widget::OnMessage( data );
}

void MyWidget::Config( const WinData* d /*= NULL*/ )
{
   Widget::Config( d );

   InitInputDevices();
}

void MyWidget::ObjectLoaded( dtCore::Object* obj )
{
   assert( obj );

   AddDrawable( obj );

   osg::Node* node = obj->GetOSGNode();
   assert( node );

   osg::BoundingSphere bs(node->getBound());

   // set the camera's position based on the scene's bounding sphere
   osg::Vec3 eye(bs.center().x(), bs.center().y() - bs.radius() * 3.0f, bs.center().z() + bs.radius() * 0.5f );
   osg::Vec3 targ(bs.center().x(), bs.center().y(), bs.center().z() );
   osg::Vec3 up(0.f, 0.f, 1.f );

   Transform pos(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
   pos.SetLookAt( eye, targ, up );

   GetCamera()->SetTransform( &pos );
   mMotionModel->SetDistance( (targ - eye).length() );
}

void MyWidget::InitInputDevices()
{
   GetKeyboard()->AddKeyboardListener( new EscapeListener( MyWidget::QuitFunctor(this, &MyWidget::Quit) ) );

   mMotionModel = new OrbitMotionModel( GetKeyboard(), GetMouse() );
   mMotionModel->SetTarget( GetCamera() );
   mMotionModel->SetEnabled( true );
}

int main()
{
   TestWidget* app = new TestWidget;

   Fl::visual( FL_DOUBLE | FL_RGB8 );
   app->make_window();
   Fl::run();

   delete app;
   return 0;
}
