#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

#include <CEGUI/elements/CEGUIStaticProperties.h>
#include <CEGUI/elements/CEGUIRadioButton.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIEventArgs.h>
#include <dtGUI/ceuidrawable.h>

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
      FPS
   };

   /// a small helper class that will set up and modify the GUI
   class QuickMenuManager : public osg::Referenced
   {
   public:
      typedef dtUtil::Functor<void,TYPELIST_1(MotionModelType)> ForwardingFunctor;

      QuickMenuManager(const ForwardingFunctor& ff): mWalk(0), mFly(0), mUFO(0), mOrbit(0), mFPS(0), mForwardingFunctor(ff)
      {
      }

      void SetSelected(MotionModelType mt)
      {
         switch( mt )
         {
         case WALK:
            {
               mWalk->setSelected(true);
            } break;

         case FLY:
            {
               mFly->setSelected(true);
            } break;

         case UFO:
            {
               mUFO->setSelected(true);
            } break;

         case ORBIT:
            {
               mOrbit->setSelected(true);
            } break;

         case FPS:
            {
               mFPS->setSelected(true);
            } break;
         }
      }

      bool ChangeMotionModelCB(const CEGUI::EventArgs& ea)
      {
         const CEGUI::WindowEventArgs& wea = static_cast<const CEGUI::WindowEventArgs&>( ea );

         if( mWalk == wea.window )       { mForwardingFunctor( WALK ); }
         else if( mFly == wea.window )   { mForwardingFunctor( FLY ); }
         else if( mUFO == wea.window )   { mForwardingFunctor( UFO ); }
         else if( mOrbit == wea.window ) { mForwardingFunctor( ORBIT ); }
         else if( mFPS == wea.window )   { mForwardingFunctor( FPS ); }

         return true;
      }

      void BuildMenu()
      {
         try
         {
            std::string schemefile( osgDB::findDataFile("schemes/WindowsLookSkin.scheme") );
            CEGUI::SchemeManager* sm = CEGUI::SchemeManager::getSingletonPtr();
            sm->loadScheme( schemefile );  ///< populate the window factories
         }
         catch(CEGUI::Exception& )
         {
            LOG_ERROR("A problem occurred loading the scheme.")
         }

         try
         {
            CEGUI::Window* frame = CreateWidget("WindowsLook/StaticText","frame");
            frame->setAreaRect(CEGUI::Rect(CEGUI::Point(0,0),CEGUI::Size(1.0,0.2)));
            frame->setProperty("BackgroundEnabled","True");
            frame->setProperty("BackgroundColours","tl:FFDFDFDF tr:FFDFDFDF bl:FFDFDFDF br:FFDFDFDF");
            frame->setText("");

            CEGUI::Window* menu = CreateWidget("DefaultWindow","SelectionBar");
            menu->setAreaRect(CEGUI::Rect(CEGUI::Point(0,0),CEGUI::Size(1.0,1.0)));
            frame->addChildWindow( menu );

            CEGUI::Window* walk = CreateWidget("WindowsLook/RadioButton","WALK");
            walk->setAreaRect(CEGUI::Rect(CEGUI::Point(0,0),CEGUI::Size(0.2,1.0)));
            walk->setText("WALK (1)");
            walk->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( walk );

            CEGUI::Window* fly = CreateWidget("WindowsLook/RadioButton","FLY");
            fly->setAreaRect(CEGUI::Rect(CEGUI::Point(0.2,0),CEGUI::Size(0.2,1.0)));
            fly->setText("FLY (2)");
            fly->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( fly );

            CEGUI::Window* ufo = CreateWidget("WindowsLook/RadioButton","UFO");
            ufo->setAreaRect(CEGUI::Rect(CEGUI::Point(0.4,0),CEGUI::Size(0.2,1.0)));
            ufo->setText("UFO (3)");
            ufo->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( ufo );

            CEGUI::Window* orb = CreateWidget("WindowsLook/RadioButton","ORBIT");
            orb->setAreaRect(CEGUI::Rect(CEGUI::Point(0.6,0),CEGUI::Size(0.2,1.0)));
            orb->setText("ORBIT (4)");
            orb->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( orb );

            CEGUI::Window* fps = CreateWidget("WindowsLook/RadioButton","FPS");
            fps->setAreaRect(CEGUI::Rect(CEGUI::Point(0.8,0),CEGUI::Size(0.2,1.0)));
            fps->setText("FPS (5)");
            fps->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( fps );

            CEGUI::System::getSingleton().setGUISheet( menu );

            InitializeWidgets(static_cast<CEGUI::RadioButton*>(walk),
                              static_cast<CEGUI::RadioButton*>(fly),
                              static_cast<CEGUI::RadioButton*>(ufo),
                              static_cast<CEGUI::RadioButton*>(orb),
                              static_cast<CEGUI::RadioButton*>(fps));

            //RegisterCallbacks( sm );
         }
         catch(CEGUI::Exception& )
         {
            LOG_ERROR("A problem occurred creating the layout.")
         }
      }

   protected:
      ~QuickMenuManager()
      {
      }

      void InitializeWidgets(CEGUI::RadioButton* walk, CEGUI::RadioButton* fly, CEGUI::RadioButton* ufo, CEGUI::RadioButton* orb, CEGUI::RadioButton* fps)
      {
         mWalk = walk;
         mFly = fly;
         mUFO = ufo;
         mOrbit = orb;
         mFPS = fps;
      }

      CEGUI::Window* CreateWidget(const std::string& wtype, const std::string& wname)
      {
         CEGUI::WindowManager* wmgr = CEGUI::WindowManager::getSingletonPtr();
         CEGUI::Window* w = 0;

         try
         {
            w = wmgr->createWindow(wtype,wname);
         }
         catch (CEGUI::Exception &e)
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

   private:
      CEGUI::RadioButton* mWalk;
      CEGUI::RadioButton* mFly;
      CEGUI::RadioButton* mUFO;
      CEGUI::RadioButton* mOrbit;
      CEGUI::RadioButton* mFPS;

      ForwardingFunctor mForwardingFunctor;
   };
}

/// The motion model test application.
class TestMotionModelsApp : public Application
{
   DECLARE_MANAGEMENT_LAYER( TestMotionModelsApp )

public:

   /// Constructor.
   TestMotionModelsApp( const std::string& configFile = "config.xml" ) : Application( configFile ),
      mTerrain(0),
      mGUIDrawable(0),
      mMenuManager(0)
   {
      dtExample::QuickMenuManager::ForwardingFunctor ff(this,&TestMotionModelsApp::SetMotionModel);
      mMenuManager = new dtExample::QuickMenuManager(ff);
   }

   void TestMotionModelsApp::Config()
   {
      //make sure to call the Base Config() as well.
      Application::Config();

      mTerrain = new InfiniteTerrain();
      mTerrain->SetBuildDistance(1500.f);
      mTerrain->SetSegmentDivisions(64);

      AddDrawable( mTerrain.get() );

      RefPtr<WalkMotionModel> wmm = new WalkMotionModel( GetKeyboard(), GetMouse() );
      wmm->SetScene( GetScene() );
      mMotionModels.push_back( wmm.get() );

      mMotionModels.push_back( new FlyMotionModel( GetKeyboard(), GetMouse() ) );
      mMotionModels.push_back( new UFOMotionModel( GetKeyboard(), GetMouse() ) );
      mMotionModels.push_back( new OrbitMotionModel( GetKeyboard(), GetMouse() ) );

      RefPtr<FPSMotionModel> fmm = new FPSMotionModel( GetKeyboard(), GetMouse() );
      fmm->SetScene( GetScene() );
      mMotionModels.push_back( fmm.get() );

      Transform xform( 0.0f, 0.0f, mTerrain->GetVerticalScale() + 15.0f );
      GetCamera()->SetTransform( &xform );

      for( unsigned int i = 0; i < mMotionModels.size(); i++ )
      {  
         mMotionModels[i]->SetTarget( GetCamera() );
      }

      // show a HUD
      mGUIDrawable = new dtGUI::CEUIDrawable( GetWindow() );
      AddDrawable( mGUIDrawable.get() );
      mMenuManager->BuildMenu();
      mMenuManager->SetSelected( dtExample::WALK );
   }

   /**
   * Key press callback.
   *
   * @param keyboard the keyboard object generating the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual bool KeyPressed(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
   {
      bool verdict = Application::KeyPressed(kb, key, kc);
      if( verdict == true )
      {
         return verdict;
      }

      switch(key)
      {
      case Producer::Key_1:
         mMenuManager->SetSelected(dtExample::WALK);
         verdict = true;
         break;
      case Producer::Key_2:
         mMenuManager->SetSelected(dtExample::FLY);
         verdict = true;
         break;
      case Producer::Key_3:
         mMenuManager->SetSelected(dtExample::UFO);
         verdict = true;
         break;
      case Producer::Key_4:
         mMenuManager->SetSelected(dtExample::ORBIT);
         verdict = true;
         break;
      case Producer::Key_5:
         mMenuManager->SetSelected(dtExample::FPS);
         verdict = true;
         break;
      default:
         break;
      }

      return verdict;
   }

   protected:
      ~TestMotionModelsApp()
      {
         mGUIDrawable->ShutdownGUI();
      }

private:

   /**
   * Enables one of the four motion models and disables
   * the rest.
   *
   * @param index the index of the motion model to enable
   */
   void SetMotionModel( unsigned int index )
   {
      for( unsigned int i = 0; i < mMotionModels.size(); i++ )
      {
         mMotionModels[i]->SetEnabled(i == index);
      }

      //turn off cursor for FPS motion model
      GetWindow()->ShowCursor(index != dtExample::FPS); 
   }

   /// The terrain object.
   RefPtr<InfiniteTerrain> mTerrain;

   /// The five motion models.
   std::vector< RefPtr<MotionModel> > mMotionModels;

   /// Tool for rendering GUI.
   dtCore::RefPtr<dtGUI::CEUIDrawable> mGUIDrawable;

   dtCore::RefPtr<dtExample::QuickMenuManager> mMenuManager;
};

IMPLEMENT_MANAGEMENT_LAYER( TestMotionModelsApp )

int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testMotionModels/;" +
                        GetDeltaDataPathList() + ";" + GetDeltaDataPathList()+"/gui" );

   RefPtr<TestMotionModelsApp> app = new TestMotionModelsApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
