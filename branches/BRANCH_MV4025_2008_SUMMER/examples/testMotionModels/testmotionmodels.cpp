#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

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
      FPS,
      COLLISION,
      RTS
   };

   /// a small helper class that will set up and modify the GUI
   class QuickMenuManager : public osg::Referenced
   {
   public:
      typedef dtUtil::Functor<void,TYPELIST_1(MotionModelType)> ForwardingFunctor;

      QuickMenuManager(const ForwardingFunctor& ff) : 
         mWalk(NULL), 
         mFly(NULL), 
         mUFO(NULL), 
         mOrbit(NULL), 
         mFPS(NULL), 
         mCollision(NULL),
         mRTS(NULL),
         mForwardingFunctor(ff)
      {
      }

      void SetSelected(const MotionModelType mt)
      {
         switch( mt )
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

         if( mWalk == wea.window )       { mForwardingFunctor( WALK ); }
         else if( mFly == wea.window )   { mForwardingFunctor( FLY ); }
         else if( mUFO == wea.window )   { mForwardingFunctor( UFO ); }
         else if( mOrbit == wea.window ) { mForwardingFunctor( ORBIT ); }
         else if( mFPS == wea.window )   { mForwardingFunctor( FPS ); }
         else if( mCollision == wea.window )   { mForwardingFunctor( COLLISION ); }
         else if( mRTS == wea.window )   { mForwardingFunctor( RTS ); }

         return true;
      }

      void BuildMenu()
      {
         try
         {
            std::string schemefile( dtCore::FindFileInPathList("gui/schemes/WindowsLook.scheme") );
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
            frame->setArea(CEGUI::UVector2(cegui_reldim(0.f),cegui_reldim(0.f)),
                           CEGUI::UVector2(cegui_reldim(1.f),cegui_reldim(0.2f)));
            frame->setProperty("BackgroundEnabled","True");
            frame->setProperty("BackgroundColours","tl:FFDFDFDF tr:FFDFDFDF bl:FFDFDFDF br:FFDFDFDF");
            frame->setText("");

            CEGUI::Window* menu = CreateWidget("DefaultWindow","SelectionBar");
            menu->setArea(CEGUI::UVector2(cegui_reldim(0),cegui_reldim(0)),
                          CEGUI::UVector2(cegui_reldim(1.0),cegui_reldim(1.0)));
            frame->addChildWindow( menu );

            CEGUI::Window* walk = CreateWidget("WindowsLook/RadioButton","WALK");
            walk->setArea(CEGUI::UVector2(cegui_reldim(0.05),cegui_reldim(0)),
                          CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            walk->setText("WALK (1)");
            walk->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( walk );

            CEGUI::Window* fly = CreateWidget("WindowsLook/RadioButton","FLY");
            fly->setArea(CEGUI::UVector2(cegui_reldim(0.25),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            fly->setText("FLY (2)");
            fly->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( fly );

            CEGUI::Window* ufo = CreateWidget("WindowsLook/RadioButton","UFO");
            ufo->setArea(CEGUI::UVector2(cegui_reldim(0.45),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            ufo->setText("UFO (3)");
            ufo->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( ufo );

            CEGUI::Window* orb = CreateWidget("WindowsLook/RadioButton","ORBIT");
            orb->setArea(CEGUI::UVector2(cegui_reldim(0.65),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            orb->setText("ORBIT (4)");
            orb->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( orb );

            CEGUI::Window* fps = CreateWidget("WindowsLook/RadioButton","FPS");
            fps->setArea(CEGUI::UVector2(cegui_reldim(0.85),cegui_reldim(0)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            fps->setText("FPS (5)");
            fps->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( fps );

            CEGUI::Window* col = CreateWidget("WindowsLook/RadioButton","COLLISION");
            col->setArea(CEGUI::UVector2(cegui_reldim(0.05),cegui_reldim(0.25)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            col->setText("COLLISION (6)");
            col->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( col );

            CEGUI::Window* rts = CreateWidget("WindowsLook/RadioButton","RTS");
            rts->setArea(CEGUI::UVector2(cegui_reldim(0.25),cegui_reldim(0.25)),
                         CEGUI::UVector2(cegui_reldim(0.2),cegui_reldim(1.0)));
            rts->setText("RTS (7)");
            rts->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&QuickMenuManager::ChangeMotionModelCB,this));
            menu->addChildWindow( rts );


            CEGUI::System::getSingleton().setGUISheet( menu );

            InitializeWidgets(static_cast<CEGUI::RadioButton*>(walk),
                              static_cast<CEGUI::RadioButton*>(fly),
                              static_cast<CEGUI::RadioButton*>(ufo),
                              static_cast<CEGUI::RadioButton*>(orb),
                              static_cast<CEGUI::RadioButton*>(fps),
                              static_cast<CEGUI::RadioButton*>(col),
                              static_cast<CEGUI::RadioButton*>(rts)
                              );

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

      void InitializeWidgets(CEGUI::RadioButton* walk, CEGUI::RadioButton* fly,
                             CEGUI::RadioButton* ufo, CEGUI::RadioButton* orb,
                             CEGUI::RadioButton* fps, CEGUI::RadioButton* col,
                             CEGUI::RadioButton* rts)
      {
         mWalk = walk;
         mFly = fly;
         mUFO = ufo;
         mOrbit = orb;
         mFPS = fps;
         mCollision = col;
         mRTS = rts;
      }

      CEGUI::Window* CreateWidget(const std::string &wtype, const std::string &wname)
      {
         CEGUI::WindowManager *wmgr = CEGUI::WindowManager::getSingletonPtr();
         CEGUI::Window *w = NULL;

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
      CEGUI::RadioButton* mCollision;
      CEGUI::RadioButton* mRTS;

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
      mTown(NULL),
      mGUIDrawable(NULL),
      mMenuManager(NULL)
   {
      dtExample::QuickMenuManager::ForwardingFunctor ff(this,&TestMotionModelsApp::SetMotionModel);
      mMenuManager = new dtExample::QuickMenuManager(ff);
   }

   void Config()
   {
      //make sure to call the Base Config() as well.
      Application::Config();

      mTown = new Object("Town");
      mTown->LoadFile("/demoMap/StaticMeshes/TestTownLt.ive");
      mTown->SetCollisionMesh();
      AddDrawable(mTown.get());
      GetScene()->GetSceneNode()->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

      RefPtr<WalkMotionModel> wmm = new WalkMotionModel( GetKeyboard(), GetMouse() );
      wmm->SetScene( GetScene() );
      mMotionModels.push_back( wmm.get() );

      mMotionModels.push_back( new FlyMotionModel( GetKeyboard(), GetMouse() ) );
      mMotionModels.push_back( new UFOMotionModel( GetKeyboard(), GetMouse() ) );
      mMotionModels.push_back( new OrbitMotionModel( GetKeyboard(), GetMouse() ) );

      RefPtr<FPSMotionModel> fmm = new FPSMotionModel( GetKeyboard(), GetMouse() );
      fmm->SetScene( GetScene() );
      mMotionModels.push_back( fmm.get() );

      osg::Vec3 gravity;
      GetScene()->GetGravity(gravity);
      mMotionModels.push_back( new CollisionMotionModel(1.5f, 0.4f, 0.25f, 0.1f, GetScene(), GetKeyboard(), GetMouse()) );

      mMotionModels.push_back( new RTSMotionModel(GetKeyboard(), GetMouse()) );

      for( unsigned int i = 0; i < mMotionModels.size(); i++ )
      {  
         mMotionModels[i]->SetTarget( GetCamera() );
      }

      // show a HUD
      mGUIDrawable = new dtGUI::CEUIDrawable( GetWindow(), GetKeyboard(), GetMouse() );
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
   virtual bool KeyPressed(const dtCore::Keyboard* kb, int key)
   {
      bool verdict = Application::KeyPressed(kb, key);
      if(verdict)
      {
         return verdict;
      }

      switch(key)
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
      bool show_cursor = (index != dtExample::FPS) &&
                         (index != dtExample::COLLISION);
      GetWindow()->ShowCursor(show_cursor);
   }

   /// The Town.
   RefPtr<Object> mTown;

   /// The five motion models.
   std::vector< RefPtr<MotionModel> > mMotionModels;

   /// Tool for rendering GUI.
   dtCore::RefPtr<dtGUI::CEUIDrawable> mGUIDrawable;

   dtCore::RefPtr<dtExample::QuickMenuManager> mMenuManager;
};

IMPLEMENT_MANAGEMENT_LAYER( TestMotionModelsApp )

int main( int argc, char **argv )
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" + 
                               dtCore::GetDeltaRootPath() + "/examples/data/;" +
                               dtCore::GetDeltaRootPath() + "/examples/data/gui/imagesets;" +
                               dtCore::GetDeltaRootPath() + "/examples/data/gui/schemes;" +
                               dtCore::GetDeltaRootPath() + "/examples/data/gui/fonts;" +
                               dtCore::GetDeltaRootPath() + "/examples/data/gui/looknfeel;" +
                               dtCore::GetDeltaRootPath() + "/examples/testMotionModels/;");

   RefPtr<TestMotionModelsApp> app = new TestMotionModelsApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}