#include "dtCore/dt.h"
#include "dtABC/dtabc.h"


using namespace dtCore;
using namespace dtABC;
using namespace osg;


/**
* The application instance.
*/
class TestInputApp* app;


/**
* The input test application.
*/
class TestInputApp : public Application,
   public InputMapperCallback
{
   DECLARE_MANAGEMENT_LAYER(TestInputApp)


public:

   /**
   * Constructor.
   */
   TestInputApp( std::string configFile = "config.xml" )
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

      mUIDrawable = new UIDrawable(w, h);

      mUIDrawable->SetWindowResolution(w, h);

      mGUILoaded = mUIDrawable->LoadGUIFile("gui.xml");

      if(!mGUILoaded)
         return;

      mUIDrawable->SetCallbackFunc("main", (CUI_UI::callbackfunc)CallbackHandler);
      mUIDrawable->SetActiveRootFrame("main");

      GetScene()->AddDrawable(mUIDrawable.get());
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
      LogicalButton* button = 
         (LogicalButton*)mApplicationInputDevice->GetButton(mButtonIndex);

      button->SetMapping(mapping);

      ButtonToButton* b2b = dynamic_cast<ButtonToButton*>(mapping);

      if(b2b != NULL)
      {
         CUI_Button* b = (CUI_Button*)mUIDrawable->GetUI()->GetFrame(mButtonIndex+1);

         std::string desc = b2b->GetSourceButton()->GetDescription();

         //pad with spaces to make formatting look right
         unsigned int columns = 12;
         std::string buttonText = "";
         if( desc.length() < columns )
         {
            buttonText = desc;
            for( unsigned int i = 0; i < (columns - desc.length()); i++)
               buttonText = buttonText + ' ';
         }
         else
            buttonText = desc;

         b->SetText(buttonText.c_str());
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
      LogicalAxis* axis =
         (LogicalAxis*)mApplicationInputDevice->GetAxis(mAxisIndex);

      axis->SetMapping(mapping);

      AxisToAxis* a2a = dynamic_cast<AxisToAxis*>(mapping);

      if(a2a != NULL)
      {
         CUI_Button* b = (CUI_Button*)mUIDrawable->GetUI()->GetFrame(mAxisIndex+4);

         b->SetText(a2a->GetSourceAxis()->GetDescription().c_str());
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

      mUIDrawable->GetUI()->GetFrame(6)->SetShader(
         mUIDrawable->GetShader(
         mApplicationInputDevice->GetButton(0)->GetState() ?
         "button_red" : "button"
         )
         );

      mUIDrawable->GetUI()->GetFrame(7)->SetShader(
         mUIDrawable->GetShader(
         mApplicationInputDevice->GetButton(1)->GetState() ?
         "button_red" : "button"
         )
         );

      mUIDrawable->GetUI()->GetFrame(8)->SetShader(
         mUIDrawable->GetShader(
         mApplicationInputDevice->GetButton(2)->GetState() ?
         "button_red" : "button"
         )
         );

      CUI_SliderBar* bar;

      bar = (CUI_SliderBar*)mUIDrawable->GetUI()->GetFrame(9);

      bar->SetValue(
         (mApplicationInputDevice->GetAxis(0)->GetState()+1.0)*50.0
         );

      bar = (CUI_SliderBar*)mUIDrawable->GetUI()->GetFrame(10);

      bar->SetValue(
         (mApplicationInputDevice->GetAxis(1)->GetState()+1.0)*50.0
         );
   }

   /**
   * GUI callback handler.
   */
   static bool CallbackHandler(int id, int numParam, void* param)
   {
      CUI_Button* button;

      switch(id)
      {
      case 1: // Action 1
         button = 
            (CUI_Button*)app->mUIDrawable->GetUI()->GetFrame(1);
         if(app->mInputMapper->AcquireButtonMapping(app))
         {
            button->SetText("");
            app->mButtonIndex = 0;
         }
         break;

      case 2: // Action 2
         button = 
            (CUI_Button*)app->mUIDrawable->GetUI()->GetFrame(2);
         if(app->mInputMapper->AcquireButtonMapping(app))
         {
            button->SetText("");
            app->mButtonIndex = 1;
         }
         break;

      case 3: // Action 3
         button = 
            (CUI_Button*)app->mUIDrawable->GetUI()->GetFrame(3);
         if(app->mInputMapper->AcquireButtonMapping(app))
         {
            button->SetText("");
            app->mButtonIndex = 2;
         }
         break;

      case 4: // Axis 1
         button = 
            (CUI_Button*)app->mUIDrawable->GetUI()->GetFrame(4);
         if(app->mInputMapper->AcquireAxisMapping(app))
         {
            button->SetText("");
            app->mAxisIndex = 0;
         }
         break;

      case 5: // Axis 2
         button = 
            (CUI_Button*)app->mUIDrawable->GetUI()->GetFrame(5);
         if(app->mInputMapper->AcquireAxisMapping(app))
         {
            button->SetText("");
            app->mAxisIndex = 1;
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
   RefPtr<UIDrawable> mUIDrawable;

   /**
   * The index of the button/axis being mapped.
   */
   int mButtonIndex, mAxisIndex;
   bool mGUILoaded;

};

IMPLEMENT_MANAGEMENT_LAYER(TestInputApp)

int main( int argc, char **argv )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   app = new TestInputApp( "config.xml" );

   app->Config();

   if(!app->IsGUILoaded())
      return 0;

   app->Run();

   delete app;

   return 0;
}
