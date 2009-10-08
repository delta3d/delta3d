#include <dtGUI/hud.h>
#include <dtGUI/ceguirenderer.h>
#include <dtGUI/ceguitexture.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUIDefaultResourceProvider.h>
#include <CEGUI/CEGUIPropertySet.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIVersion.h>
#include <CEGUI/CEGUIXMLParser.h>
#include <CEGUI/CEGUITexture.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/CEGUISchemeManager.h>

#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtCore/mouse.h>
#include <dtCore/camera.h>
#include <dtCore/keyboard.h>
#include <dtCore/deltadrawable.h>
#include <dtUtil/log.h>
#include <dtUtil/macros.h>

#include <osg/GraphicsContext>
#include <osg/State>
#include <osg/Camera>
#include <osg/Texture2D>
#include <osg/Geode>

#include <sstream>

using namespace dtGUI;

//default path where to find the layout/looknfeel/../images-files
std::string HUD::FilePath=std::string("./data/gui/");

bool dtGUI::HUD::SystemAndRendererCreatedByHUD = false;

/********************************************************************************
                              DELTA-implementation
 ********************************************************************************/
IMPLEMENT_MANAGEMENT_LAYER(HUD)

////////////////////////////////////////////////////////////////////////////////
HUD::HUD(osg::Camera* pTargetCamera, dtCore::Keyboard* pObservedKeyboard, dtCore::Mouse* pObservedMouse, const std::string& sName)
   : dtCore::Base(sName)
   , m_Viewport(0,0,0,0)
   , m_pCamera(0)
   , m_pKeyboard(0)
   , m_pMouse(0)
   , mIsMouseCursorVisible(true)
{
   m_pMouseListener    = new dtGUI::CEGUIMouseListener(this);
   m_pKeyboardListener = new dtGUI::CEGUIKeyboardListener(this);

   AddSender(&dtCore::System::GetInstance());
   RegisterInstance(this);

   _SetupInternalGraph();

   SetCamera(pTargetCamera);

   SetMouse(pObservedMouse);
   SetKeyboard(pObservedKeyboard);

   _SetupSystemAndRenderer();
   _SetupDefaultUI();
}

////////////////////////////////////////////////////////////////////////////////
HUD::HUD(dtCore::Camera* pTargetCamera, dtCore::Keyboard* pObservedKeyboard, dtCore::Mouse* pObservedMouse, const std::string& sName)
   : dtCore::Base(sName)
   , m_Viewport(0,0,0,0)
   , m_pCamera(0)
   , m_pKeyboard(0)
   , m_pMouse(0)
{
   m_pMouseListener = new CEGUIMouseListener(this);
   m_pKeyboardListener = new CEGUIKeyboardListener(this);

   AddSender( &dtCore::System::GetInstance() );
   RegisterInstance(this);

   _SetupInternalGraph();

   SetCamera(pTargetCamera->GetOSGCamera());

   SetMouse(pObservedMouse);
   SetKeyboard(pObservedKeyboard);
   _SetupSystemAndRenderer();
   _SetupDefaultUI();
}

////////////////////////////////////////////////////////////////////////////////
HUD::~HUD()
{
   m_pRootsheet->destroy();

   if (m_pCamera.valid())
   {
      m_pCamera->removeChild(m_pInternalGraph);
   }

   if (CEGUI::System::getSingletonPtr() != NULL)
   {
      // We are only responsible for destroying resources we created
      if (SystemAndRendererCreatedByHUD)
      {
         CEGUI::System* system = CEGUI::System::getSingletonPtr();
         CEGUI::Renderer* renderer = system->getRenderer();

         if (renderer != NULL)
         {
            delete renderer;
         }

         delete system;
      }
   }

   DeregisterInstance(this);
   RemoveSender(&dtCore::System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
std::string GetCEGUIPrefix(CEGUI::Window* win)
{
   assert(win != NULL);
#if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 6
   return (std::string(win->getPrefix().c_str()));
#else
   return (std::string());
#endif
}

////////////////////////////////////////////////////////////////////////////////
Widget* HUD::GetWidget(const std::string& sWidgetName, const std::string& sPrefix)
{
   CEGUI::WindowManager* wm = CEGUI::WindowManager::getSingletonPtr();
   if (!wm->isWindowPresent(GetCEGUIPrefix(m_pRootsheet) + sPrefix + sWidgetName))
   {
      LOG_ERROR(sPrefix + sWidgetName + " is not available in gui \"" + this->GetName() + "\"\n");
      return 0;
   }
   return wm->getWindow(GetCEGUIPrefix(m_pRootsheet) + sPrefix + sWidgetName);
}

////////////////////////////////////////////////////////////////////////////////
Widget* HUD::CreateWidget(const std::string& sWidgetTypeName, const std::string& sWidgetName, const std::string& sPrefix)
{
   MakeCurrent();
#if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 6
   Widget* pNewWidget = CEGUI::WindowManager::getSingleton().createWindow(sWidgetTypeName, sWidgetName, GetCEGUIPrefix(m_pRootsheet) + sPrefix);
#else
   Widget* pNewWidget = CEGUI::WindowManager::getSingleton().createWindow(sWidgetTypeName, sPrefix+sWidgetName);
#endif

   m_pRootsheet->addChildWindow(pNewWidget);
   return pNewWidget;
}

////////////////////////////////////////////////////////////////////////////////
Widget* HUD::CreateWidget(Widget* pParentWidget, const std::string& sWidgetTypeName, const std::string& sWidgetName, const std::string& sPrefix)
{
   MakeCurrent();
   Widget* pNewWidget = CreateWidget(sWidgetTypeName, sWidgetName, sPrefix);
   if (pParentWidget) { pParentWidget->addChildWindow(pNewWidget); }
   return pNewWidget;
}

////////////////////////////////////////////////////////////////////////////////
Widget* HUD::LoadLayout(Widget* pParentWidget, const std::string& sFileName, const std::string& sPrefix)
{
   Widget* pNewLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout(sFileName, GetCEGUIPrefix(m_pRootsheet) + sPrefix);
   pParentWidget->addChildWindow(pNewLayout);
   return pNewLayout;
}

////////////////////////////////////////////////////////////////////////////////
Widget* HUD::LoadLayout(const std::string& sFileName, const std::string& sPrefix)
{
   return LoadLayout(m_pRootsheet, sFileName, sPrefix);
}

////////////////////////////////////////////////////////////////////////////////
const std::string HUD::GetPrefix() const
{
   return GetCEGUIPrefix(m_pRootsheet);
}

////////////////////////////////////////////////////////////////////////////////
void HUD::_SetupDefaultUI()
{
   //create&setup defaults:
   //if (! CEGUI::FontManager::getSingleton().isFontPresent("DejaVuSans-10") )
   //{
   //    CEGUI::FontManager::getSingleton().createFont("DejaVuSans-10.font");
   //    CEGUI::System::getSingletonPtr()->setDefaultFont("DejaVuSans-10");
   //    CEGUI::System::getSingletonPtr()->getDefaultFont()->setProperty("PointSize", "10");
   //}

   //generate unique root-window-name:
   std::string sGeneratedUniquePrefix="";

   //generate unqiue prefix
   for (unsigned int i = 0; i < 65000; i++)
   {
      std::stringstream ssTryName;
      ssTryName << "gui" << i;
      if (!CEGUI::WindowManager::getSingleton().isWindowPresent(ssTryName.str()+"rootsheet"))
      {
         sGeneratedUniquePrefix = ssTryName.str();
         break;
      }
   }
   if (sGeneratedUniquePrefix=="")
   {
      LOG_ERROR("cannot generate prefix");
      return;
   }

#if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 6
   m_pRootsheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", "rootsheet", sGeneratedUniquePrefix);
#else
   m_pRootsheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", std::string(sGeneratedUniquePrefix + "rootsheet").c_str() );
#endif
}

////////////////////////////////////////////////////////////////////////////////
void HUD::SetCamera(osg::Camera* pTargetCamera)
{
   // if this was already a child of another camera remove itself from there:
   if (m_pCamera.valid())
   {
      m_pCamera->removeChild(m_pInternalGraph);
   }

   // set ("parent") camera
   m_pCamera = pTargetCamera;

   // that'll force the camera to draw this gui via the HUDDrawable-object
   if (pTargetCamera)
   {
      pTargetCamera->addChild(m_pInternalGraph);
   }
}

////////////////////////////////////////////////////////////////////////////////
void HUD::_SetupInternalGraph()
{
   m_pInternalGraph = new osg::Geode();

   //m_pInternalGraph->setName("internal_GUI_Geode");
   m_pInternalGraph->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   m_pInternalGraph->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");
   m_pInternalGraph->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
   m_pInternalGraph->getOrCreateStateSet()->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
   m_pInternalGraph->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   m_pInternalGraph->addDrawable(new HUDDrawable(this));
}

////////////////////////////////////////////////////////////////////////////////
void HUD::_SetupSystemAndRenderer()
{
   if (!CEGUI::System::getSingletonPtr())
   {
      dtGUI::CEGUIRenderer* pRenderer = new dtGUI::CEGUIRenderer(1024);

      CEGUI::DefaultResourceProvider* pRP = new CEGUI::DefaultResourceProvider();

      CEGUI::Imageset::setDefaultResourceGroup("imagesets");
      pRP->setResourceGroupDirectory("imagesets", FilePath + "/imagesets/");

      CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
      pRP->setResourceGroupDirectory("looknfeels", FilePath + "/looknfeel/");

      CEGUI::WindowManager::setDefaultResourceGroup("layouts");
      pRP->setResourceGroupDirectory("layouts", FilePath + "/layouts/");

      CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
      pRP->setResourceGroupDirectory("lua_scripts", FilePath + "/lua_scripts/");

      CEGUI::Scheme::setDefaultResourceGroup("schemes");
      pRP->setResourceGroupDirectory("schemes", FilePath + "/schemes/");

      CEGUI::Font::setDefaultResourceGroup("fonts");
      pRP->setResourceGroupDirectory("fonts", FilePath + "/fonts/");

      new CEGUI::System(pRenderer, pRP);

      SystemAndRendererCreatedByHUD = true;
   }
}

////////////////////////////////////////////////////////////////////////////////
void HUD::_CheckCamera()
{
   // if we have valid setup, check if camera-viewport fits cegui-rendering-size
   if (m_pCamera.valid())
   {
      const osg::Viewport& camViewport =  *m_pCamera->getViewport();
      if (camViewport.width() != m_Viewport.iWidth ||
          camViewport.height() != m_Viewport.iHeight ||
          camViewport.x() != m_Viewport.iX ||
          camViewport.y() != m_Viewport.iY    )
      {
         m_pMouseListener->SetWindowSize(camViewport.width() , camViewport.height());
         m_Viewport.iWidth = camViewport.width();
         m_Viewport.iHeight = camViewport.height();
         m_Viewport.iX = camViewport.x();
         m_Viewport.iY = camViewport.y();
         reinterpret_cast<CEGUIRenderer *>(CEGUI::System::getSingleton().getRenderer())->setDisplaySize( CEGUI::Size(m_Viewport.iWidth , m_Viewport.iHeight));
         CEGUI::System::getSingletonPtr()->setGUISheet(GetRootSheet());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void HUD::SetMouse(dtCore::Mouse* pObservedMouse)
{
   if (m_pMouse.valid())
   {
      m_pMouse->RemoveMouseListener(m_pMouseListener.get());
   }

   m_pMouse = pObservedMouse;

   if (m_pMouse.valid())
   {
      if (m_pMouse->GetListeners().empty())
      {
         m_pMouse->AddMouseListener(m_pMouseListener.get());
      }
      else
      {
         m_pMouse->InsertMouseListener(m_pMouse->GetListeners().front() , m_pMouseListener.get());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void HUD::SetKeyboard(dtCore::Keyboard* pObservedKeyboard)
{
   if (m_pKeyboard.valid())
   {
      m_pKeyboard->RemoveKeyboardListener(m_pKeyboardListener.get());
   }

   m_pKeyboard = pObservedKeyboard;

   if (m_pKeyboard.valid())
   {
      if (m_pKeyboard->GetListeners().empty())
      {
         m_pKeyboard->AddKeyboardListener(m_pKeyboardListener.get());
      }
      else
      {
         m_pKeyboard->InsertKeyboardListener(m_pKeyboard->GetListeners().front(), m_pKeyboardListener.get());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void HUD::OnMessage(dtCore::Base::MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      _CheckCamera();

      const double dDeltaTime = *static_cast<const double*>(data->userData);

      ///\todo must we really cast here?
      CEGUI::System::getSingletonPtr()->injectTimePulse(static_cast<float>(dDeltaTime));
   }
}

////////////////////////////////////////////////////////////////////////////////
void HUD::MakeCurrent() const
{
   CEGUI::System& system = CEGUI::System::getSingleton();

   //set renderer-display-size to viewport-dimensions of the gui:
   reinterpret_cast<CEGUIRenderer *>(system.getRenderer())->setDisplaySize( CEGUI::Size(m_Viewport.iWidth , m_Viewport.iHeight));

   //set root window as current sheet:
   if (system.getGUISheet() != m_pRootsheet)
   {
      system.setGUISheet(m_pRootsheet);
   }

   //if no mouse(-input) is present do not draw a cursor for this gui:
   if (m_pMouse.valid() && mIsMouseCursorVisible)
   {
      CEGUI::MouseCursor::getSingleton().show();
   }
   else
   {
      CEGUI::MouseCursor::getSingleton().hide();
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Texture2D* HUD::GetOrCreateOSGTexture(const std::string& sWidgetName)
{
   Widget* pWidget = GetWidget(sWidgetName);

   // get osg-texture for the static image of the cegui-window
   if (!pWidget->isPropertyPresent("Image"))
   {
      LOG_ERROR( " Property \"Image\" is not available for widget \"" + std::string(pWidget->getName().c_str()) + "\" \n" );
      return 0;
   }

   const CEGUI::Image* pImage = CEGUI::PropertyHelper::stringToImage(pWidget->getProperty("Image"));
   if (!pImage)
   {
      //
      // try to create an image/texture if none present:
      //

      // generate imageset with an unique name:
      std::string sImagesetName = "GUI-StaticImageTexture";
      while ( CEGUI::ImagesetManager::getSingleton().isImagesetPresent(sImagesetName) )
          sImagesetName = sImagesetName + "X";

      // create an osg(CEGUI)-Texture:
      dtGUI::CEGUITexture *pTexture = (dtGUI::CEGUITexture *)(CEGUI::System::getSingleton().getRenderer()->createTexture(512));

      // create cegui-imageset
      CEGUI::Imageset* pImageset = CEGUI::ImagesetManager::getSingleton().createImageset(sImagesetName, pTexture );
      pImageset->defineImage("image1", CEGUI::Rect(0, 0, 512, 512), CEGUI::Vector2(0,0));
      pWidget->setProperty("Image", CEGUI::PropertyHelper::imageToString( &(pImageset->getImage("image1"))));

      // apply to window
      pImage = CEGUI::PropertyHelper::stringToImage(pWidget->getProperty("Image"));

      // sth. went wrong :-((( ... cleanup
      if (!pImage)
      {
         CEGUI::ImagesetManager::getSingleton().destroyImageset(sImagesetName);
         CEGUI::System::getSingleton().getRenderer()->destroyTexture(pTexture);
      }
   }
   if (!pImage)
   {
      LOG_ERROR( " invalid CEGUI::Window \n" );
      return 0;
   }
   dtGUI::CEGUITexture* pTexture = reinterpret_cast<dtGUI::CEGUITexture*>(pImage->getImageset()->getTexture());
   if (!pTexture)
   {
      LOG_ERROR(" invalid dtGUI::Texture \n");
      return 0;
   }

   pTexture->SetFlipHorizontal(true);
   return pTexture->GetOSGTexture();
}

/********************************************************************************
                      HUD-static-wrappers
 ********************************************************************************/

////////////////////////////////////////////////////////////////////////////////
void HUD::LoadScheme(const std::string& sFileName)
{
   _SetupSystemAndRenderer();
   CEGUI::SchemeManager::getSingleton().loadScheme(sFileName);
}

////////////////////////////////////////////////////////////////////////////////
void HUD::UnloadScheme(const std::string& sFileName)
{
   _SetupSystemAndRenderer();
   CEGUI::SchemeManager::getSingleton().unloadScheme(sFileName);
}

////////////////////////////////////////////////////////////////////////////////
void HUD::UnloadAllSchemes()
{
   _SetupSystemAndRenderer();
   CEGUI::SchemeManager::getSingleton().unloadAllSchemes();
}

////////////////////////////////////////////////////////////////////////////////
void HUD::CreateFont(const std::string& sFileName)
{
   _SetupSystemAndRenderer();
   CEGUI::FontManager::getSingleton().createFont(sFileName);
}

////////////////////////////////////////////////////////////////////////////////
void HUD::DestroyFont(const std::string& sFileName)
{
   _SetupSystemAndRenderer();
   CEGUI::FontManager::getSingleton().destroyFont(sFileName);
}

////////////////////////////////////////////////////////////////////////////////
void HUD::DestroyAllFonts()
{
   _SetupSystemAndRenderer();
   CEGUI::FontManager::getSingleton().destroyAllFonts();
}

////////////////////////////////////////////////////////////////////////////////
void HUD::SetMouseCursor(const std::string &sImagesetName, const std::string &sImageName)
{
   _SetupSystemAndRenderer();
   CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(sImagesetName, sImageName);
}

////////////////////////////////////////////////////////////////////////////////
void HUD::SetFilePath(const std::string& sPath)
{
   FilePath = sPath;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& HUD::GetFilePath()
{
   return FilePath;
}

/********************************************************************************
                      HUD::HUDDrawable-implementation
 ********************************************************************************/

////////////////////////////////////////////////////////////////////////////////
HUD::HUDDrawable::HUDDrawable(const HUD::HUDDrawable& drawable, const osg::CopyOp& copyop)
   : m_pGUI(drawable.m_pGUI)
{
}

////////////////////////////////////////////////////////////////////////////////
HUD::HUDDrawable::HUDDrawable(HUD* gui)
   : m_pGUI(gui)
{
   setSupportsDisplayList(false);
   setUseDisplayList(false);
}

////////////////////////////////////////////////////////////////////////////////
HUD::HUDDrawable::~HUDDrawable()
{
}

////////////////////////////////////////////////////////////////////////////////
osg::Object* HUD::HUDDrawable::cloneType() const
{
   return new HUDDrawable(m_pGUI);
}

////////////////////////////////////////////////////////////////////////////////
osg::Object* HUD::HUDDrawable::clone(const osg::CopyOp& copyop) const
{
   return new HUDDrawable(*this, copyop);
}

////////////////////////////////////////////////////////////////////////////////
void HUD::HUDDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
   // tell the UI to update and to render
   if (!m_pGUI) { return; }
   if (!m_pGUI->m_pCamera.valid()) { return; }

   osg::State& state = *renderInfo.getState();

   state.setClientActiveTextureUnit(0);
   glDisable(GL_TEXTURE_2D);

   state.setActiveTextureUnit(0);
   glEnable(GL_TEXTURE_2D);
   reinterpret_cast<CEGUIRenderer*>(CEGUI::System::getSingleton().getRenderer())->SetGraphicsContext(state.getGraphicsContext());
   m_pGUI->MakeCurrent();

   CEGUI::System::getSingletonPtr()->renderGUI();
}

////////////////////////////////////////////////////////////////////////////////
