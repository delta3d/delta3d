#ifdef None
#undef None
#endif
#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_ALL_START
#include <CEGUI/CEGUI.h>
DT_DISABLE_WARNING_END

#include <dtABC/application.h>
#include <dtCore/inputmapper.h>
#include <dtCore/logicalinputdevice.h>
#include <dtGUI/gui.h>
#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtInputPLIB/joystick.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;

///colors to reflect the button states
CEGUI::colour kOff(0.0f, 0.0f, 0.0f, 1.0f);
CEGUI::colour kOn(1.0f, 0.0f, 0.0f, 1.0f);

static int KBUTTON1 = 0xAAAA;
static int KBUTTON2 = 0xAAAB;
static int KBUTTON3 = 0xAAAC;

class TestInputApp;
dtCore::RefPtr<TestInputApp> app;

/**
 * The input test application.
 */
class TestInputApp : public Application, public InputMapperCallback
{

public:

   /**
   * Constructor.
   */
   TestInputApp(const std::string& configFile = "config.xml")
      : Application(configFile)
      , mGUILoaded(false)
   {
      mKeyboardAxisInputDevice = new LogicalInputDevice;

      mKeyboardAxisInputDevice->AddAxis(
         "w/s",
         new ButtonsToAxis(
            GetKeyboard()->GetButton('w'),
            GetKeyboard()->GetButton('s')
         )
      );

      mKeyboardAxisInputDevice->AddAxis(
         "a/d",
         new ButtonsToAxis(
            GetKeyboard()->GetButton('a'),
            GetKeyboard()->GetButton('d')
         )
      );

      mApplicationInputDevice = new LogicalInputDevice;

      mApplicationInputDevice->AddButton(
         "action 1",
         GetKeyboard()->GetButton('1'),
         KBUTTON1
      );

      mApplicationInputDevice->AddButton(
         "action 2",
         GetKeyboard()->GetButton('2'),
         KBUTTON2
      );

      mApplicationInputDevice->AddButton(
         "action 3",
         GetKeyboard()->GetButton('3'),
         KBUTTON3
      );

      mApplicationInputDevice->AddAxis(
         "axis 1",
         mKeyboardAxisInputDevice->AddAxis(
            "up/down",
            new ButtonsToAxis(
               GetKeyboard()->GetButton(osgGA::GUIEventAdapter::KEY_Up),
               GetKeyboard()->GetButton(osgGA::GUIEventAdapter::KEY_Down)
            )
         )
      );

      mApplicationInputDevice->AddAxis(
         "axis 2",
         mKeyboardAxisInputDevice->AddAxis(
            "left/right",
            new ButtonsToAxis(
               GetKeyboard()->GetButton(osgGA::GUIEventAdapter::KEY_Left),
               GetKeyboard()->GetButton(osgGA::GUIEventAdapter::KEY_Right)
            )
         )
      );

      mInputMapper = new InputMapper;

      mInputMapper->AddDevice(GetKeyboard());
      mInputMapper->AddDevice(mKeyboardAxisInputDevice.get());
      mInputMapper->AddDevice(GetMouse());

      dtInputPLIB::Joystick::CreateInstances();

      for (int i = 0; i < dtInputPLIB::Joystick::GetInstanceCount(); i++)
      {
         mInputMapper->AddDevice(dtInputPLIB::Joystick::GetInstance(i));
      }

      mInputMapper->SetCancelButton(GetKeyboard()->GetButton(osgGA::GUIEventAdapter::KEY_Escape));
   }

protected:

   virtual ~TestInputApp()
   {
      mConnections.clear();
   }

public:
   /**
    * Configures the application.
    */
   virtual void Config()
   {
      Application::Config();

      mUIDrawable = new dtGUI::GUI(GetCamera(),GetKeyboard(), GetMouse());

      try
      {
         mUIDrawable->LoadScheme("WindowsLook.scheme");

         //overwrite the default search path, to found our local layout file.
         mUIDrawable->SetResourceGroupDirectory("layouts", dtUtil::GetDeltaRootPath() + "/examples/testInput/");

         CEGUI::Window* w = mUIDrawable->LoadLayout("gui.xml");
         if (w != NULL)
         {
            mGUILoaded = true;
         }
      }
      // catch to prevent exit (errors will be logged).
      catch(CEGUI::Exception& e)
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
            "CEGUI::%s", e.getMessage().c_str() );
      }


      if (!mGUILoaded) { return; }

      mConnections.push_back(mUIDrawable->SubscribeEvent("Action 1 Button", CEGUI::PushButton::EventClicked.c_str(), &CallbackHandler));
      mConnections.push_back(mUIDrawable->SubscribeEvent("Action 2 Button", CEGUI::PushButton::EventClicked.c_str(), &CallbackHandler));
      mConnections.push_back(mUIDrawable->SubscribeEvent("Action 3 Button", CEGUI::PushButton::EventClicked.c_str(), &CallbackHandler));
      mConnections.push_back(mUIDrawable->SubscribeEvent("Axis 1 Button", CEGUI::PushButton::EventClicked.c_str(), &CallbackHandler));
      mConnections.push_back(mUIDrawable->SubscribeEvent("Axis 2 Button", CEGUI::PushButton::EventClicked.c_str(), &CallbackHandler));
   }

   bool IsGUILoaded()
   {
      return mGUILoaded;
   }

   /**
    * Notifies the observer that the button mapping acquisition has
    * completed.
    *
    * @param mapping the newly acquired button mapping, or NULL if
    * the user canceled the acquisition
    */
   virtual void ButtonMappingAcquired(ButtonMapping* mapping)
   {
      LogicalButton* button = static_cast<LogicalButton*>(mApplicationInputDevice->GetButton(mButtonIndex));

      button->SetMapping(mapping);

      if ( ButtonToButton* b2b = dynamic_cast<ButtonToButton*>(mapping) )
      {
         CEGUI::Window *b = mUIDrawable->GetWidget(mButtonName);

         b->setText( b2b->GetSourceButton()->GetDescription() );
      }
   }

   /**
    * Notifies the observer that the axis mapping acquisition has
    * completed.
    *
    * @param mapping the newly acquired axis mapping, or NULL if
    * the user canceled the acquisition
    */
   virtual void AxisMappingAcquired(AxisMapping* mapping)
   {
      LogicalAxis* axis = static_cast<LogicalAxis*>(mApplicationInputDevice->GetAxis(mAxisIndex));

      axis->SetMapping(mapping);

      if (AxisToAxis* a2a = dynamic_cast<AxisToAxis*>(mapping))
      {
         CEGUI::Window* b = mUIDrawable->GetWidget(mAxisName);

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
      dtInputPLIB::Joystick::PollInstances();

      {
         CEGUI::Window* w = mUIDrawable->GetWidget("Checkbox6");
         if (mApplicationInputDevice->GetButton(KBUTTON1)->GetState())
         {
            w->setProperty("BackgroundColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000" );
         }
         else
         {
            w->setProperty("BackgroundColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000" );
         }
      }

      {
         CEGUI::Window* w = mUIDrawable->GetWidget("Checkbox7");
         if (mApplicationInputDevice->GetButton(KBUTTON2)->GetState())
         {
            w->setProperty("BackgroundColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000" );
         }
         else
         {
            w->setProperty("BackgroundColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000" );
         }
      }

      {
         CEGUI::Window* w = mUIDrawable->GetWidget("Checkbox8");
         if (mApplicationInputDevice->GetButton(KBUTTON3)->GetState())
         {
            w->setProperty("BackgroundColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000" );
         }
         else
         {
            w->setProperty("BackgroundColours", "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000" );
         }
      }


      CEGUI::ProgressBar* bar1 = static_cast<CEGUI::ProgressBar*>(mUIDrawable->GetWidget("Axis 1 Slider"));
      bar1->setProgress((mApplicationInputDevice->GetAxis(0)->GetState() + 1.0f) * 0.5f);


      CEGUI::ProgressBar* bar2 = static_cast<CEGUI::ProgressBar*>(mUIDrawable->GetWidget("Axis 2 Slider"));
      bar2->setProgress((mApplicationInputDevice->GetAxis(1)->GetState() + 1.0f) * 0.5f);

   }

   /**
    * GUI callback handler.
    */
   static bool CallbackHandler(const CEGUI::EventArgs& e)
   {
      CEGUI::Window* w = static_cast<const CEGUI::WindowEventArgs&>(e).window;

      switch (w->getID())
      {
      case 1: // Action 1

         if (app->mInputMapper->AcquireButtonMapping(app))
         {
            w->setText("waiting for input");
            app->mButtonIndex = KBUTTON1;
            app->mButtonName = w->getName().c_str();
         }
         break;

      case 2: // Action 2
         if (app->mInputMapper->AcquireButtonMapping(app))
         {
            w->setText("waiting for input");
            app->mButtonIndex = KBUTTON2;
            app->mButtonName = w->getName().c_str();
         }
         break;

      case 3: // Action 3
         if (app->mInputMapper->AcquireButtonMapping(app))
         {
            w->setText("waiting for input");
            app->mButtonIndex = KBUTTON3;
            app->mButtonName = w->getName().c_str();
         }
         break;

      case 4: // Axis 1
         if (app->mInputMapper->AcquireAxisMapping(app))
         {
            w->setText("waiting for input");
            app->mAxisIndex = 0;
            app->mAxisName = w->getName().c_str();
         }
         break;

      case 5: // Axis 2
         if (app->mInputMapper->AcquireAxisMapping(app))
         {
            w->setText("waiting for input");
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
   RefPtr<dtGUI::GUI> mUIDrawable;

   /**
    * The index of the button/axis being mapped.
    */
   int mButtonIndex, mAxisIndex;
   std::string mButtonName, mAxisName;
   bool mGUILoaded;

   std::vector<CEGUI::Event::Connection> mConnections;

};

int main(int argc, char** argv)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaRootPath() + "/examples/data/;" +
                               dtUtil::GetDeltaRootPath() + "/examples/data/gui/;" +
                               dtUtil::GetDeltaRootPath() + "/examples/testInput/;" +
                               dataPath + ";");

   app = new TestInputApp("config.xml");

   app->Config();

   if (!app->IsGUILoaded())
   {
      return 0;
   }

   app->Run();

   return 0;
}
