#include "dt.h"
#include "dtabc.h"

/*
#include "application.h"
#include "globals.h"
#include "inputmapper.h"
#include "joystick.h"
#include "logicalinputdevice.h"
#include "notify.h"
#include "uidrawable.h"*/


using namespace dtCore;
using namespace dtABC;
using namespace osg;


/**
 * The application instance.
 */
class TestInputApplication* testInputApp;


/**
 * The input test application.
 */
class TestInputApplication : public Application,
                             public InputMapperCallback
{
   DECLARE_MANAGEMENT_LAYER(TestInputApplication)
   
   
   public:
   
      /**
       * Constructor.
       */
      TestInputApplication()
         : Application("config.xml")
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
         
         
         mUIDrawable->LoadGUIFile("gui.xml");
         
         mUIDrawable->SetCallbackFunc("main", (CUI_UI::callbackfunc)CallbackHandler);
         mUIDrawable->SetActiveRootFrame("main");
         
         GetScene()->AddDrawable(mUIDrawable.get());
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
            
            b->SetText(b2b->GetSourceButton()->GetDescription().c_str());
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
                  "green" : "black"
            )
         );
         
         mUIDrawable->GetUI()->GetFrame(7)->SetShader(
            mUIDrawable->GetShader(
               mApplicationInputDevice->GetButton(1)->GetState() ?
                  "green" : "black"
            )
         );
         
         mUIDrawable->GetUI()->GetFrame(8)->SetShader(
            mUIDrawable->GetShader(
               mApplicationInputDevice->GetButton(2)->GetState() ?
                  "green" : "black"
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
                  (CUI_Button*)testInputApp->mUIDrawable->GetUI()->GetFrame(1);
               if(testInputApp->mInputMapper->AcquireButtonMapping(testInputApp))
               {
                  button->SetText("");
                  testInputApp->mButtonIndex = 0;
               }
               break;
               
            case 2: // Action 2
               button = 
                  (CUI_Button*)testInputApp->mUIDrawable->GetUI()->GetFrame(2);
               if(testInputApp->mInputMapper->AcquireButtonMapping(testInputApp))
               {
                  button->SetText("");
                  testInputApp->mButtonIndex = 1;
               }
               break;
               
            case 3: // Action 3
               button = 
                  (CUI_Button*)testInputApp->mUIDrawable->GetUI()->GetFrame(3);
               if(testInputApp->mInputMapper->AcquireButtonMapping(testInputApp))
               {
                  button->SetText("");
                  testInputApp->mButtonIndex = 2;
               }
               break;
               
            case 4: // Axis 1
               button = 
                  (CUI_Button*)testInputApp->mUIDrawable->GetUI()->GetFrame(4);
               if(testInputApp->mInputMapper->AcquireAxisMapping(testInputApp))
               {
                  button->SetText("");
                  testInputApp->mAxisIndex = 0;
               }
               break;
               
            case 5: // Axis 2
               button = 
                  (CUI_Button*)testInputApp->mUIDrawable->GetUI()->GetFrame(5);
               if(testInputApp->mInputMapper->AcquireAxisMapping(testInputApp))
               {
                  button->SetText("");
                  testInputApp->mAxisIndex = 1;
               }
               break;
         }
         
         return false;
      }   
      
      
   private:
      
      /**
       * The application input device.
       */
      ref_ptr<LogicalInputDevice> mApplicationInputDevice;
      
      /**
       * The keyboard axis input device.
       */
      ref_ptr<LogicalInputDevice> mKeyboardAxisInputDevice;
      
      /**
       * The input mapper.
       */
      ref_ptr<InputMapper> mInputMapper;
      
      /**
       * The user interface.
       */
      ref_ptr<UIDrawable> mUIDrawable;
      
      /**
       * The index of the button/axis being mapped.
       */
      int mButtonIndex, mAxisIndex;
};

IMPLEMENT_MANAGEMENT_LAYER(TestInputApplication)

int main( int argc, char **argv )
{
   SetDataFilePathList("..;../../data;" + GetDeltaDataPathList());
 
   testInputApp = new TestInputApplication;
   
   testInputApp->Config();
   testInputApp->Run();

   return 0;
}
