#include "dt.h"
#include "camera.h"
#include "globals.h"
#include "notify.h"

#include "joystick.h"
#include "keyboard.h"
#include "logicalinputdevice.h"
#include "mouse.h"
#include "tracker.h"

#include "guimgr.h"
#include "system.h"

using namespace dtCore;


//Read the input device and reposition itself
//based on the System "frame" message
class MouseControl : public Transformable
{
public:
   DECLARE_MANAGEMENT_LAYER(MouseControl)
   
   MouseControl()
   {
      SetName("MouseControl");
      AddSender( System::GetSystem() );
      Config();
   }

   void Config(void)
   {
      Joystick::CreateInstances();
      
      keyboard = Window::GetInstance(0)->GetKeyboard();
      mouse = Window::GetInstance(0)->GetMouse();
           
      quitButton = lid.AddButton(
         "quit",
         new ButtonToButton(
            keyboard->GetButton(Producer::Key_Escape)
         )
      );
      
      objectXAxis = lid.AddAxis(
         "object x",
         new AxisToAxis(
         mouse->GetAxis(0),
         10
         )
         );
      
      objectYAxis = lid.AddAxis(
         "object y",
         new AxisToAxis(
         mouse->GetAxis(1),
         10
         )
         );

         /*
         objectXAxis->SetMapping(
         new ButtonsToAxis(
         keyboard->GetButton(Producer::Key_Left),
         keyboard->GetButton(Producer::Key_Right),
         -10, 10
         )
         );
         
           objectYAxis->SetMapping(
           new ButtonsToAxis(
           keyboard->GetButton(Producer::Key_Up),
           keyboard->GetButton(Producer::Key_Down),
           10, -10
           )
           );
      */
   }
   virtual ~MouseControl() {};
   
private:

   virtual void OnMessage(MessageData *data) //overrided from Base
   {
      if (data->message == "frame")
      {
         Transform position;
         
         if (quitButton->GetState())
         {
            //quit app
            System::GetSystem()->Stop();
         }
         
         Joystick::PollInstances();
         // Tracker::PollInstances();
         
         position.Set(
            objectXAxis->GetState(), 
            objectYAxis->GetState(),
            0, 0, 0, 0
            );
         
         this->SetTransform(&position);
         
      }
   }

   Keyboard *keyboard;
   Mouse *mouse;
   LogicalButton* quitButton;
   LogicalAxis* objectXAxis;
   LogicalAxis* objectYAxis;
   LogicalInputDevice lid;
   
};

IMPLEMENT_MANAGEMENT_LAYER(MouseControl)



int main( int argc, char **argv )
{
   Window *win = new Window();
   Scene *scene = new Scene();
   System *sys = System::GetSystem();

   Camera *cam = new Camera();
   cam->SetWindow( win );
   cam->SetScene( scene );
   Transform position;
   position.Set(0.f, -50.f, 5.f, 0.f, -10.f, 0.f);
   cam->SetTransform( &position );
   
   //This is where we'll find our files to load
   SetDataFilePathList("../../data/");

   Object *obj1 = new Object("cube");
   Object *obj2 = new Object("cessna");
   obj1->LoadFile("box.flt");
   obj2->LoadFile("cessna.osg");
   obj1->AddChild( obj2 ); //make obj2 a child of obj1 just for goofs

   position.SetTranslation( 0.f, 0.f, -2.f );
   obj2->SetTransform( &position, Transformable::REL_CS );

   //Add the Objects to the Scene to be rendered
   scene->AddDrawable( obj1 );
   scene->AddDrawable( obj2 );

   MouseControl *mot = new MouseControl();
   mot->AddChild( obj1 ); //MouseControl will move obj1
   
   GUI *gui = new GUI();

   sys->Config();
   sys->Run();

   return 0;
}
