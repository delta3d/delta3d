#include <CEGUI/CEGUI.h>

#include <dtABC/application.h>
#include <dtCore/globals.h>
#include <dtCore/inputmapper.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/joystick.h>
#include <dtGUI/ceuidrawable.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <osgDB/FileUtils>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;

/**
* The application instance.
*/
class TestInputApp* app;

///colors to reflect the button states
CEGUI::colour kOff(0.f, 0.f, 0.f, 1.f);
CEGUI::colour kOn(1.f, 0.f, 0.f, 1.f);

/**
* The input test application.
*/
class TestInputApp : public Application,
   public InputMapperCallback
{

public:

   /**
   * Constructor.
   */
   TestInputApp( const std::string& configFile = "config.xml" )
      : Application( configFile ),
      mGUILoaded(false)
   {
      mKeyboardAxisInputDevice = new LogicalInputDevice;

      mKeyboardAxisInputDevice->AddAxis(
         "w/s",
         new ButtonsToAxis(
         GetKeyboard()->GetButton(Producer::Key_W),
         GetKeyboard()->GetButton(Producer::Key_S)
         )
         );

      mKeyboardAxisInputDevice->AddAxis(
         "a/d",
         new ButtonsToAxis(
         GetKeyboard()->GetButton(Producer::Key_A),
         GetKeyboard()->GetButton(Producer::Key_D)
         )
         );

      mApplicationInputDevice = new LogicalInputDevice;

      mApplicationInputDevice->AddButton(
         "action 1", 
         GetKeyboard()->GetButton(Producer::Key_1)
         );

      mApplicationInputDevice->AddButton(
         "action 2",
         GetKeyboard()->GetButton(Producer::Key_2)
         );

      mApplicationInputDevice->AddButton(
         "action 3",
         GetKeyboard()->GetButton(Producer::Key_3)
         );

      mApplicationInputDevice->AddAxis(
         "axis 1",
         mKeyboardAxisInputDevice->AddAxis(
         "up/down",
         new ButtonsToAxis(
         GetKeyboard()->GetButton(Producer::Key_Up),
         GetKeyboard()->GetButton(Producer::Key_Down)
         )
         )
         );

      mApplicationInputDevice->AddAxis(
         "axis 2",
         mKeyboardAxisInputDevice->AddAxis(
         "left/right",
         new ButtonsToAxis(
         GetKeyboard()->GetButton(Producer::Key_Left),
         GetKeyboard()->GetButton(Producer::Key_Right)
         )
         )
         );

      mInputMapper = new InputMapper;

      mInputMapper->AddDevice(GetKeyboard());
      mInputMapper->AddDevice(mKeyboardAxisInputDevice.get());
      mInputMapper->AddDevice(GetMouse());

      Joystick::CreateInstances();

      for(int i=0;i<Joystick::GetInstanceCount();i++)
      {
         mInputMapper->AddDevice(Joystick::GetInstance(i));
      }

      mInputMapper->SetCancelButton(GetKeyboard()->GetButton(Producer::Key_Escape));
   }

   /**
   * Configures the application.
   */
   virtual void Config()
   {
      Application::Config();

      int x, y, w, h;

      GetWindow()->GetPosition(&x, &y, &w, &h);

      mUIDrawable = new dtGUI::CEUIDrawable(w, h);

      try
      {
         std::string schemeFileName = osgDB::findDataFile("schemes/WindowsLookSkin.scheme");
         CEGUI::SchemeManager::getSingleton().loadScheme(schemeFileName);

         CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");
         CEGUI::System::getSingleton().setDefaultFont("Tahoma-12");

         CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", "root_wnd");
         CEGUI::System::getSingleton().setGUISheet(sheet);
          
         CEGUI::Window *w = CEGUI::WindowManager::getSingleton().loadWindowLayout("gui.xml");
         if (w != NULL)
         {
            mGUILoaded = true;
            sheet->addChildWindow(w);
         }

      }
      // catch to prevent exit (errors will be logged).
      catch(CEGUI::Exception &e)
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
            "CEGUI::%s", e.getMessage().c_str() );
      }


      if(!mGUILoaded)  return;
       
      CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();
      wm->getWindow("Action 1 Button")->subscribeEvent(CEGUI::PushButton::EventClicked, &CallbackHandler);
      wm->getWindow("Action 2 Button")->subscribeEvent(CEGUI::PushButton::EventClicked, &CallbackHandler);
      wm->getWindow("Action 3 Button")->subscribeEvent(CEGUI::PushButton::EventClicked, &CallbackHandler);
      wm->getWindow("Axis 1 Button")->subscribeEvent(CEGUI::PushButton::EventClicked, &CallbackHandler);
      wm->getWindow("Axis 2 Button")->subscribeEvent(CEGUI::PushButton::EventClicked, &CallbackHandler);

      GetScene()->AddDrawable( mUIDrawable.get() );
   }

   bool IsGUILoaded()
   {
      return mGUILoaded;
   }

   /**
   * Notifies the listener that the button mapping acquisition has
   * completed.
   *
   * @param mapping the newly acquired button mapping, or NULL if
   * the user canceled the acquisition
   */
   virtual void ButtonMappingAcquired(ButtonMapping* mapping)
   {
      LogicalButton* button = static_cast<LogicalButton*>(mApplicationInputDevice->GetButton(mButtonIndex));

      button->SetMapping(mapping);

      if( ButtonToButton* b2b = dynamic_cast<ButtonToButton*>(mapping) )
      {
         CEGUI::Window *b = CEGUI::WindowManager::getSingleton().getWindow(mButtonName);

         b->setText( b2b->GetSourceButton()->GetDescription() );
      }
   }

   /**
   * Notifies the listener that the axis mapping acquisition has
   * completed.
   *
   * @param mapping the newly acquired axis mapping, or NULL if
   * the user canceled the acquisition
   */
   virtual void AxisMappingAcquired(AxisMapping* mapping)
   {
      LogicalAxis* axis = static_cast<LogicalAxis*>(mApplicationInputDevice->GetAxis(mAxisIndex));

      axis->SetMapping(mapping);

      if( AxisToAxis* a2a = dynamic_cast<AxisToAxis*>(mapping) )
      {
         CEGUI::Window *b = CEGUI::WindowManager::getSingleton().getWindow(mAxisName);

         b->setText(a2a->GetSourceAxis()->GetDescription().c_str());
      }
   }


protected:

   /**
   * Pre-frame callback.
   *
   * @param deltaFrameTime the amount of time elapsed since the last frame
   */
   virtual void PreFrame(const double deltaFrameTime)
   {
      Joystick::PollInstances();

      {
         CEGUI::Static *w = static_cast<CEGUI::Static*>(CEGUI::WindowManager::getSingleton().getWindow("Checkbox6"));
         if (mApplicationInputDevice->GetButton(0)->GetState())
         {
            w->setBackgroundColours( kOn );
         }
         else
         {
            w->setBackgroundColours( kOff );
         }
      }

      {
         CEGUI::Static *w = static_cast<CEGUI::Static*>(CEGUI::WindowManager::getSingleton().getWindow("Checkbox7"));
         if (mApplicationInputDevice->GetButton(1)->GetState())
         {
            w->setBackgroundColours( kOn );
         }
         else
         {
            w->setBackgroundColours( kOff );
         }
      }

      {
         CEGUI::Static *w = static_cast<CEGUI::Static*>(CEGUI::WindowManager::getSingleton().getWindow("Checkbox8"));
         if (mApplicationInputDevice->GetButton(2)->GetState())
         {
            w->setBackgroundColours( kOn );
         }
         else
         {
            w->setBackgroundColours( kOff );
         }
      }


      CEGUI::ProgressBar *bar1 = static_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("Axis 1 Slider"));
      bar1->setProgress( (mApplicationInputDevice->GetAxis(0)->GetState()+1.f)*0.5f);


      CEGUI::ProgressBar *bar2 = static_cast<CEGUI::ProgressBar*>(CEGUI::WindowManager::getSingleton().getWindow("Axis 2 Slider"));
      bar2->setProgress( (mApplicationInputDevice->GetAxis(1)->GetState()+1.f)*0.5f );

   }

   /**
   * GUI callback handler.
   */
   static bool CallbackHandler(const CEGUI::EventArgs& e)
   {
      CEGUI::Window* w = static_cast<const CEGUI::WindowEventArgs&>(e).window;
      
      switch( w->getID() )
      {
      case 1: // Action 1

         if(app->mInputMapper->AcquireButtonMapping(app))
         {
            w->setText("");
            app->mButtonIndex = 0;
            app->mButtonName = w->getName().c_str();
         }
         break;

      case 2: // Action 2
         if(app->mInputMapper->AcquireButtonMapping(app))
         {
            w->setText("");
            app->mButtonIndex = 1;
            app->mButtonName = w->getName().c_str();
         }
         break;

      case 3: // Action 3
         if(app->mInputMapper->AcquireButtonMapping(app))
         {
            w->setText("");
            app->mButtonIndex = 2;
            app->mButtonName = w->getName().c_str();
         }
         break;

      case 4: // Axis 1
         if(app->mInputMapper->AcquireAxisMapping(app))
         {
            w->setText("");
            app->mAxisIndex = 0;
            app->mAxisName = w->getName().c_str();
         }
         break;

      case 5: // Axis 2
         if(app->mInputMapper->AcquireAxisMapping(app))
         {
            w->setText("");
            app->mAxisIndex = 1;
            app->mAxisName = w->getName().c_str();
         }
         break;
      }

      return false;
   }   



private:

   /**
   * The application input device.
   */
   RefPtr<LogicalInputDevice> mApplicationInputDevice;

   /**
   * The keyboard axis input device.
   */
   RefPtr<LogicalInputDevice> mKeyboardAxisInputDevice;

   /**
   * The input mapper.
   */
   RefPtr<InputMapper> mInputMapper;

   /**
   * The user interface.
   */
   RefPtr<dtGUI::CEUIDrawable> mUIDrawable;

   /**
   * The index of the button/axis being mapped.
   */
   int mButtonIndex, mAxisIndex;
   std::string mButtonName, mAxisName;
   bool mGUILoaded;

};

int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testInput/;" +
                        GetDeltaDataPathList() + ";" +
                        GetDeltaDataPathList()+"/gui/;" );

   app = new TestInputApp( "config.xml" );

   app->Config();

   if(!app->IsGUILoaded())
      return 0;

   app->Run();

   delete app;

   return 0;
}
