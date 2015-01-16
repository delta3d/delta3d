/* -*-c++-*-
 * testMotionModels - testmotionmodels(.h & .cpp) - Using 'The MIT License'
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/deltawin.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/motionmodel.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/refptr.h>
#include <dtCore/rtsmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/walkmotionmodel.h>
#include <dtCore/system.h>

#include <dtABC/application.h>

#include <CEGUI/elements/CEGUIRadioButton.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIEventArgs.h>
#include <dtGUI/gui.h>

#include <iostream>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;

namespace dtExample
{
   /// a quick way to manage the types of motion models we want to show off
   enum MotionModelType
   {
      WALK = 0L,
      FLY,
      UFO,
      ORBIT,
      FPS,
      COLLISION,
      RTS
   };

   /// a small helper class that will set up and modify the GUI
   class QuickMenuManager : public osg::Referenced
   {
   public:
      typedef dtUtil::Functor<void,TYPELIST_1(MotionModelType)> ForwardingFunctor;

      QuickMenuManager(const ForwardingFunctor& ff, dtGUI::GUI& gui)
         :mPausedUI(NULL)
         , mUseSimTimeUI(NULL)
         , mWalk(NULL)
         , mFly(NULL)
         , mUFO(NULL)
         , mOrbit(NULL)
         , mFPS(NULL)
         , mCollision(NULL)
         , mRTS(NULL)
         , mForwardingFunctor(ff)
         , mGUI(&gui)
      {
      }

      void SetSelected(const MotionModelType mt)
      {
         switch (mt)
         {
            case WALK:
            {
               mWalk->setSelected(true);
            }
            break;

            case FLY:
            {
               mFly->setSelected(true);
            }
            break;

            case UFO:
            {
               mUFO->setSelected(true);
            }
            break;

            case ORBIT:
            {
               mOrbit->setSelected(true);
            }
            break;

            case FPS:
            {
               mFPS->setSelected(true);
            }
            break;

            case COLLISION:
            {
               mCollision->setSelected(true);
            }
            break;

            case RTS:
            {
               mRTS->setSelected(true);
            }
            break;
         }
      }

      bool ChangeMotionModelCB(const CEGUI::EventArgs& ea)
      {
         const CEGUI::WindowEventArgs& wea = static_cast<const CEGUI::WindowEventArgs&>( ea );

         if (mWalk == wea.window)           { mForwardingFunctor( WALK ); }
         else if (mFly == wea.window)       { mForwardingFunctor( FLY ); }
         else if (mUFO == wea.window)       { mForwardingFunctor( UFO ); }
         else if (mOrbit == wea.window)     { mForwardingFunctor( ORBIT ); }
         else if (mFPS == wea.window)       { mForwardingFunctor( FPS ); }
         else if (mCollision == wea.window) { mForwardingFunctor( COLLISION ); }
         else if (mRTS == wea.window)       { mForwardingFunctor( RTS ); }

         return true;
      }

      void BuildMenu()
      {
         try
         {
            mGUI->LoadScheme("WindowsLook.scheme");  ///< populate the window factories
         }
         catch(CEGUI::Exception& e)
         {
            LOG_ERROR("A problem occurred loading the scheme." + std::string(e.getMessage().c_str()))
         }

         try
         {
            //CEGUI::Window* rootWindow = CreateWidget("DefaultGUISheet", "root");
            //rootWindow->setMousePassThroughEnabled(true);
            //rootWindow->setArea(CEGUI::UVector2(cegui_reldim(0),cegui_reldim(0)),
            //   CEGUI::UVector2(cegui_reldim(1.0),cegui_reldim(1.0)));

            CEGUI::Window* frame = CreateWidget("WindowsLook/StaticText","frame");
            frame->setArea(CEGUI::UVector2(cegui_reldim(0.0f),cegui_reldim(0.08f /*0.f*/)),
                           CEGUI::UVector2(cegui_reldim(1.0f),cegui_reldim(0.12f /*0.2f*/)));
            //frame->setProperty("FrameEnabled", "false");
            frame->setProperty("BackgroundEnabled","True");
            frame->setProperty("BackgroundColours","tl:FFDFDFDF tr:FFDFDFDF bl:FFDFDFDF br:FFDFDFDF");
            //frame->setProperty("BackgroundEnabled", "false");
            frame->setText("");
            //rootWindow->addChildWindow(frame);

            CEGUI::Window* menu = CreateWidget("DefaultWindow","SelectionBar");
            menu->setArea(CEGUI::UVector2(cegui_reldim(0),cegui_reldim(0)),
                          CEGUI::UVector2(cegui_reldim(1.0),cegui_reldim(1.0)));
            frame->addChildWindow( menu );

            CEGUI::Window* walk = CreateWidget("WindowsLook/RadioButton","WALK");
            walk->setArea(CEGUI::UVector2(cegui_reldim(0.05),cegui_reldim(0)),
                          CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            walk->setText("WALK (1)");
            mGUI->SubscribeEvent(*walk, CEGUI::RadioButton::EventSelectStateChanged.c_str(),
                                 dtGUI::GUI::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( walk );

            CEGUI::Window* fly = CreateWidget("WindowsLook/RadioButton","FLY");
            fly->setArea(CEGUI::UVector2(cegui_reldim(0.25),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            fly->setText("FLY (2)");
            mGUI->SubscribeEvent(*fly, CEGUI::RadioButton::EventSelectStateChanged.c_str(),
                                 dtGUI::GUI::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( fly );

            CEGUI::Window* ufo = CreateWidget("WindowsLook/RadioButton","UFO");
            ufo->setArea(CEGUI::UVector2(cegui_reldim(0.45),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            ufo->setText("UFO (3)");
            mGUI->SubscribeEvent(*ufo, CEGUI::RadioButton::EventSelectStateChanged.c_str(),
                                 dtGUI::GUI::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( ufo );

            CEGUI::Window* orb = CreateWidget("WindowsLook/RadioButton","ORBIT");
            orb->setArea(CEGUI::UVector2(cegui_reldim(0.65),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            orb->setText("ORBIT (4)");
            mGUI->SubscribeEvent(*orb, CEGUI::RadioButton::EventSelectStateChanged.c_str(),
                                 dtGUI::GUI::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( orb );

            CEGUI::Window* fps = CreateWidget("WindowsLook/RadioButton","FPS");
            fps->setArea(CEGUI::UVector2(cegui_reldim(0.85),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            fps->setText("FPS (5)");
            mGUI->SubscribeEvent(*fps, CEGUI::RadioButton::EventSelectStateChanged.c_str(),
                                 dtGUI::GUI::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( fps );

            CEGUI::Window* col = CreateWidget("WindowsLook/RadioButton","COLLISION");
            col->setArea(CEGUI::UVector2(cegui_reldim(0.05),cegui_reldim(0.25)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            col->setText("COLLISION (6)");
            mGUI->SubscribeEvent(*col, CEGUI::RadioButton::EventSelectStateChanged.c_str(),
                                 dtGUI::GUI::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( col );

            CEGUI::Window* rts = CreateWidget("WindowsLook/RadioButton","RTS");
            rts->setArea(CEGUI::UVector2(cegui_reldim(0.25),cegui_reldim(0.25)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            rts->setText("RTS (7)");
            mGUI->SubscribeEvent(*rts, CEGUI::RadioButton::EventSelectStateChanged.c_str(),
                                 dtGUI::GUI::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( rts );


            mPausedUI = CreateWidget("WindowsLook/StaticText", "PausedUI");
            mPausedUI->setText("(F2) Not Paused");
            mPausedUI->setPosition(CEGUI::UVector2(cegui_absdim(10), cegui_absdim(400)));
            mPausedUI->setSize(CEGUI::UVector2(cegui_absdim(155), cegui_absdim(20)));
            //rootWindow->addChildWindow(mPausedUI);

            mUseSimTimeUI = CreateWidget("WindowsLook/StaticText", "UseSimTimeUI");
            mUseSimTimeUI->setText("(F3) REAL Time (if applies)");
            mUseSimTimeUI->setPosition(CEGUI::UVector2(cegui_absdim(10), cegui_absdim(421)));
            mUseSimTimeUI->setSize(CEGUI::UVector2(cegui_absdim(155), cegui_absdim(20)));
            //rootWindow->addChildWindow(mUseSimTimeUI);


            //CEGUI::System::getSingleton().setGUISheet(rootWindow/*menu*/);

            InitializeWidgets(static_cast<CEGUI::RadioButton*>(walk),
                              static_cast<CEGUI::RadioButton*>(fly),
                              static_cast<CEGUI::RadioButton*>(ufo),
                              static_cast<CEGUI::RadioButton*>(orb),
                              static_cast<CEGUI::RadioButton*>(fps),
                              static_cast<CEGUI::RadioButton*>(col),
                              static_cast<CEGUI::RadioButton*>(rts)
                              );

         }
         catch(CEGUI::Exception&)
         {
            LOG_ERROR("A problem occurred creating the layout.")
         }
      }

   protected:
      ~QuickMenuManager()
      {
      }

      void InitializeWidgets(CEGUI::RadioButton* walk, CEGUI::RadioButton* fly,
                             CEGUI::RadioButton* ufo, CEGUI::RadioButton* orb,
                             CEGUI::RadioButton* fps, CEGUI::RadioButton* col,
                             CEGUI::RadioButton* rts)
      {
         mWalk      = walk;
         mFly       = fly;
         mUFO       = ufo;
         mOrbit     = orb;
         mFPS       = fps;
         mCollision = col;
         mRTS       = rts;
      }

      CEGUI::Window* CreateWidget(const std::string& wtype, const std::string& wname)
      {
         CEGUI::Window* w = NULL;

         try
         {
            w = mGUI->CreateWidget(wtype, wname);
         }
         catch (CEGUI::Exception& e)
         {
            std::string err;
            err += std::string("Can't create CEGUI Window of type ");
            err += wtype;
            err += std::string(".");
            err += std::string(e.getMessage().c_str());

            LOG_ERROR(err);
         }
         return w;
      }

   public:
      CEGUI::Window* mPausedUI;
      CEGUI::Window* mUseSimTimeUI;


   private:
      CEGUI::RadioButton* mWalk;
      CEGUI::RadioButton* mFly;
      CEGUI::RadioButton* mUFO;
      CEGUI::RadioButton* mOrbit;
      CEGUI::RadioButton* mFPS;
      CEGUI::RadioButton* mCollision;
      CEGUI::RadioButton* mRTS;

      ForwardingFunctor mForwardingFunctor;
      dtCore::RefPtr<dtGUI::GUI> mGUI;
   };
}

/// The motion model test application.
class TestMotionModelsApp : public Application
{
   DECLARE_MANAGEMENT_LAYER(TestMotionModelsApp)

public:

   /// Constructor.
   TestMotionModelsApp(const std::string& configFile = "config.xml")
      : Application(configFile)
      , mTown(NULL)
      , mGUIDrawable(NULL)
      , mMenuManager(NULL)
      , mUseSimTime(false)
      , mCurrentMotionModelIndex(0)
   {
   }

   void Config()
   {
      //make sure to call the Base Config() as well.
      Application::Config();

      mTown = new Object("Town");

      if (!mOptionalFilename.empty())
      {
         mTown->LoadFile(mOptionalFilename);
      }
      else
      {
         mTown->LoadFile("/StaticMeshes/TestTownLt.ive");
      }


      AddDrawable(mTown.get());
      GetScene()->GetSceneNode()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

      RefPtr<WalkMotionModel> wmm = new WalkMotionModel(GetKeyboard(), GetMouse());
      wmm->SetScene(GetScene());
      mMotionModels.push_back(wmm.get());

      mMotionModels.push_back(new FlyMotionModel(GetKeyboard(), GetMouse()));
      mMotionModels.push_back(new UFOMotionModel(GetKeyboard(), GetMouse()));
      mMotionModels.push_back(new OrbitMotionModel(GetKeyboard(), GetMouse()));

      RefPtr<FPSMotionModel> fmm = new FPSMotionModel(GetKeyboard(), GetMouse());
      fmm->SetScene(GetScene());
      mMotionModels.push_back(fmm.get());

      mMotionModels.push_back(new RTSMotionModel(GetKeyboard(), GetMouse()));

      for (unsigned int i = 0; i < mMotionModels.size(); i++)
      {
         mMotionModels[i]->SetTarget(GetCamera());
      }

      // show a HUD
      mGUIDrawable = new dtGUI::GUI(GetCamera(), GetKeyboard(), GetMouse());

      dtExample::QuickMenuManager::ForwardingFunctor ff(this, &TestMotionModelsApp::SetMotionModel);
      mMenuManager = new dtExample::QuickMenuManager(ff, *mGUIDrawable);
      mMenuManager->BuildMenu();
      mMenuManager->SetSelected(dtExample::WALK);

   }

   /**
    * Key press callback.
    *
    * @param keyboard the keyboard object generating the event
    * @param key the key pressed
    * @param character the corresponding character
    */
   virtual bool KeyPressed(const dtCore::Keyboard* kb, int key)
   {
      bool verdict = Application::KeyPressed(kb, key);
      if (verdict)
      {
         return verdict;
      }

      switch (key)
      {
      case '1':
         mMenuManager->SetSelected(dtExample::WALK);
         verdict = true;
         break;
      case '2':
         mMenuManager->SetSelected(dtExample::FLY);
         verdict = true;
         break;
      case '3':
         mMenuManager->SetSelected(dtExample::UFO);
         verdict = true;
         break;
      case '4':
         mMenuManager->SetSelected(dtExample::ORBIT);
         verdict = true;
         break;
      case '5':
         mMenuManager->SetSelected(dtExample::FPS);
         verdict = true;
         break;
      case '6':
         mMenuManager->SetSelected(dtExample::COLLISION);
         verdict = true;
         break;
      case '7':
         mMenuManager->SetSelected(dtExample::RTS);
         verdict = true;
         break;

      case osgGA::GUIEventAdapter::KEY_F2:
         {
            bool bPausedState = !dtCore::System::GetInstance().GetPause();
            dtCore::System::GetInstance().SetPause(bPausedState);
            if (bPausedState)
               mMenuManager->mPausedUI->setText("(F2) PAUSED!");
            else
               mMenuManager->mPausedUI->setText("(F3) Not Paused");
         }
         break;

      case osgGA::GUIEventAdapter::KEY_F3:
         {
            mUseSimTime = !mUseSimTime;
            if (mUseSimTime)
               mMenuManager->mUseSimTimeUI->setText("(F3) SIM Time (if applies)");
            else
               mMenuManager->mUseSimTimeUI->setText("(F3) REAL Time (if applies)");
            std::cout << "Note - only a few motion models support Sim time vs Real Time including::  FlyMotionModel." << std::endl;

            // resetting the MM will update the sim time flag.
            SetMotionModel(mCurrentMotionModelIndex);
         }
         break;

      default:
         break;
      }

      return verdict;
   }



   public:
      // Allow command settings to override the default filename. Allows this app to be used to test external models.
      std::string mOptionalFilename;


   protected:
      ~TestMotionModelsApp()
      {
      }


private:

   /**
   * Enables one of the four motion models and disables
   * the rest.
   *
   * @param index the index of the motion model to enable
   */
   void SetMotionModel(unsigned int index)
   {
      mCurrentMotionModelIndex = index;

      for (unsigned int i = 0; i < mMotionModels.size(); i++)
      {
         mMotionModels[i]->SetEnabled(i == index);

         if (i == index)
         {
            // figure out which MM we have so we can set the option flags appropriately.
            // Currently, only Fly supports this
            FlyMotionModel *flyMM = dynamic_cast<FlyMotionModel*> (mMotionModels[i].get());
            if (flyMM != NULL)
               flyMM->SetUseSimTimeForSpeed(mUseSimTime);
         }
      }

      //turn off cursor for FPS motion model
      bool show_cursor = (index != dtExample::FPS) &&
                         (index != dtExample::COLLISION);
      GetWindow()->SetShowCursor(show_cursor);
   }

   /// The Town.
   RefPtr<Object> mTown;

   /// The five motion models.
   std::vector< RefPtr<MotionModel> > mMotionModels;

   /// Tool for rendering GUI.
   dtCore::RefPtr<dtGUI::GUI> mGUIDrawable;

   dtCore::RefPtr<dtExample::QuickMenuManager> mMenuManager;

   bool mUseSimTime; // ie sim time versus real time. Only supported by some motion models.
   unsigned int mCurrentMotionModelIndex;

};

IMPLEMENT_MANAGEMENT_LAYER(TestMotionModelsApp)

int main(int argc, char** argv)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaRootPath() + "/examples/data/;" +
                               dtUtil::GetDeltaRootPath() + "/examples/data/gui/;" +
                               dtUtil::GetDeltaRootPath() + "/examples/testMotionModels/;" +
                               dataPath + ";");

   RefPtr<TestMotionModelsApp> app = new TestMotionModelsApp( "config.xml" );

   // Pull the optional file name off of the command line.
   if (argc > 1)
   {
      std::string temp = argv[1];
      if (!temp.empty())
         app->mOptionalFilename = temp;
      std::cout << "Attempting to load file [" << temp << "] instead of the default model." << std::endl;
   }

   app->Config();
   app->Run();

   return 0;
}
