#include <dtGUI/gui.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>

#include <osg/Drawable>
#include <osg/StateSet>
#include <osg/Geode>

#include <CEGUI/CEGUISystem.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLRenderer.h>  // for base class
#include <CEGUI/CEGUIDefaultResourceProvider.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIScriptModule.h>
#include <CEGUI/CEGUIScheme.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIFont.h>
#include <CEGUI/falagard/CEGUIFalWidgetLookManager.h>

using namespace dtGUI;
IMPLEMENT_MANAGEMENT_LAYER(GUI)

namespace dtGUI
{

   /**
   * @brief
   *     an osg::Drawable which will render a provided(defined via the constructor) gui
   */
   class HUDDrawable : public osg::Drawable
   {
   public:

      ///copy constructor
      HUDDrawable(const HUDDrawable& drawable, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
      {
         this->setSupportsDisplayList(false);
         this->setUseDisplayList(false);
      }

      ///constructs an GUIdrawable-object and assign an HUD-object
      HUDDrawable()
      {
         this->setSupportsDisplayList(false);
         this->setUseDisplayList(false);
      }

      ///implementation of osg::Object
      virtual osg::Object* cloneType() const
      {
         return new HUDDrawable();
      }

      ///implementation of osg::Object
      virtual osg::Object* clone(const osg::CopyOp& copyop) const
      {
         return new HUDDrawable(*this, copyop);
      }

      ///draw-implementation of osg::Object
      virtual void drawImplementation(osg::RenderInfo & renderInfo) const
      {
         osg::State* state = renderInfo.getState();

         const CEGUI::Size& ceguiSize = CEGUI::System::getSingleton().getRenderer()->getDisplaySize();
         if (ceguiSize.d_width != state->getCurrentViewport()->width() ||
             ceguiSize.d_height != state->getCurrentViewport()->height())
         {
            CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Size(state->getCurrentViewport()->width(), state->getCurrentViewport()->height()));
         }

         state->disableAllVertexArrays();
         state->setClientActiveTextureUnit(0);

         CEGUI::System::getSingletonPtr()->renderGUI();
      }

   protected:

      ///destructs an HUDDrawable-object
      virtual ~HUDDrawable()
      {}

   private:
   };
}


//default path where to find the layout/looknfeel/../images-files
std::string dtGUI::GUI::FilePath=std::string("./data/gui/");
bool dtGUI::GUI::SystemAndRendererCreatedByHUD = false;

////////////////////////////////////////////////////////////////////////////////
dtGUI::GUI::GUI(dtCore::Camera* pTargetCamera,
                dtCore::Keyboard* pObservedKeyboard ,
                dtCore::Mouse* pObservedMouse):
m_pRootsheet(NULL)
{
   m_pMouseListener    = new dtGUI::CEGUIMouseListener();
   m_pKeyboardListener = new dtGUI::CEGUIKeyboardListener();

   AddSender(&dtCore::System::GetInstance());
   RegisterInstance(this);

   _SetupInternalGraph();

   SetCamera(pTargetCamera);
   SetKeyboard(pObservedKeyboard);
   SetMouse(pObservedMouse);

   _SetupSystemAndRenderer();
   _SetupDefaultUI();
}

////////////////////////////////////////////////////////////////////////////////
dtGUI::GUI::~GUI()
{

}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::_SetupInternalGraph()
{
   m_pInternalGraph = new osg::Geode();

   //m_pInternalGraph->setName("internal_GUI_Geode");
   m_pInternalGraph->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   m_pInternalGraph->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");
   m_pInternalGraph->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
   m_pInternalGraph->getOrCreateStateSet()->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
   m_pInternalGraph->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   m_pInternalGraph->addDrawable(new HUDDrawable());

}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::SetCamera(dtCore::Camera* pTargetCamera)
{
   // if this was already a child of another camera remove itself from there:
   if (m_pCamera.valid())
   {
      m_pCamera->removeChild(m_pInternalGraph);
   }

   // set ("parent") camera
   m_pCamera = pTargetCamera == NULL ? NULL : pTargetCamera->GetOSGCamera();

   // that'll force the camera to draw this gui via the HUDDrawable-object
   if (m_pCamera)
   {
      m_pCamera->addChild(m_pInternalGraph);
   }

}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::SetMouse(dtCore::Mouse* pObservedMouse)
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
void dtGUI::GUI::SetKeyboard(dtCore::Keyboard* pObservedKeyboard)
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
void dtGUI::GUI::_SetupSystemAndRenderer()
{
   if (!CEGUI::System::getSingletonPtr())
   {
      CEGUI::OpenGLRenderer& pRenderer = CEGUI::OpenGLRenderer::create();
      pRenderer.enableExtraStateSettings(true);
      CEGUI::System::create(pRenderer);

      CEGUI::DefaultResourceProvider* pRP = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

      CEGUI::Imageset::setDefaultResourceGroup("imagesets");
      SetResourceGroupDirectory("imagesets", dtUtil::FindFileInPathList("imagesets"));

      CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
      SetResourceGroupDirectory("looknfeels", dtUtil::FindFileInPathList("looknfeel"));

      CEGUI::WindowManager::setDefaultResourceGroup("layouts");
      SetResourceGroupDirectory("layouts", dtUtil::FindFileInPathList("layouts"));

      CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
      SetResourceGroupDirectory("lua_scripts", dtUtil::FindFileInPathList("lua_scripts"));

      CEGUI::Scheme::setDefaultResourceGroup("schemes");
      SetResourceGroupDirectory("schemes", dtUtil::FindFileInPathList("schemes"));

      CEGUI::Font::setDefaultResourceGroup("fonts");
      SetResourceGroupDirectory("fonts", dtUtil::FindFileInPathList("fonts"));

      SystemAndRendererCreatedByHUD = true;
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::_SetupDefaultUI()
{
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

   m_pRootsheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", std::string(sGeneratedUniquePrefix + "rootsheet").c_str());
   CEGUI::System::getSingleton().setGUISheet(m_pRootsheet);
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::LoadScheme(const std::string& sFileName, const std::string& sResourceGroup)
{
   _SetupSystemAndRenderer();
   CEGUI::SchemeManager::getSingleton().create(sFileName, sResourceGroup);
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::SetMouseCursor(const std::string& sImagesetName, const std::string& sImageName)
{
   _SetupSystemAndRenderer();
   CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(sImagesetName, sImageName);
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::LoadLayout(const std::string& sFileName, const std::string& sPrefix, const std::string& sResourceGroup)
{
   return LoadLayout(m_pRootsheet, sFileName, sPrefix, sResourceGroup);
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::LoadLayout(Widget* pWidgetParent, const std::string& sFileName, const std::string& sPrefix, const std::string& sResourceGroup)
{
   Widget* pNewLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout(sFileName, sPrefix, sResourceGroup);
   pWidgetParent->addChildWindow(pNewLayout);
   return pNewLayout;
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::CreateWidget(Widget* pParentWidget, const std::string& sWidgetTypeName, const std::string& sWidgetName)
{
   Widget* pNewWidget = CreateWidget(sWidgetTypeName, sWidgetName);
   if (pParentWidget) { pParentWidget->addChildWindow(pNewWidget); }
   return pNewWidget;
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::CreateWidget(const std::string& sWidgetTypeName, const std::string& sWidgetName)
{
   Widget* pNewWidget = CEGUI::WindowManager::getSingleton().createWindow(sWidgetTypeName, sWidgetName);
   m_pRootsheet->addChildWindow(pNewWidget);
   return pNewWidget;
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::SetFilePath(const std::string& sPath)
{
   FilePath = sPath;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& dtGUI::GUI::GetFilePath()
{
   return FilePath;
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::OnMessage(dtCore::Base::MessageData *data)
{
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      //_CheckCamera();

      const double dDeltaTime = *static_cast<const double*>(data->userData);

      CEGUI::System::getSingletonPtr()->injectTimePulse(static_cast<float>(dDeltaTime));
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::SetResourceGroupDirectory(const std::string& resourceType, const std::string& directory)
{
   if (CEGUI::System::getSingletonPtr() == NULL)
   {
      return;
   }

   CEGUI::DefaultResourceProvider* pRP = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

   pRP->setResourceGroupDirectory(resourceType, directory);
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::GetWidget(const std::string& sWidgetName)
{
   if (!CEGUI::WindowManager::getSingleton().isWindowPresent(sWidgetName))
   {
      LOG_ERROR(sWidgetName + " is not available in gui \"" + this->GetName() + "\"\n");
      return 0;
   }

   return CEGUI::WindowManager::getSingleton().getWindow(sWidgetName);
}

////////////////////////////////////////////////////////////////////////////////
CEGUI::Event::Connection dtGUI::GUI::SubscribeEvent(const std::string& widgetName, 
                                const std::string& event,
                                GUI::Subscriber subscriber)
{
   CEGUI::Window* w = GetWidget(widgetName);
   if (w)
   {
      return w->subscribeEvent(event, subscriber);
   }

   
   LOG_ERROR("Could not find widget for event subscription: " + widgetName);
   return CEGUI::Event::Connection();
}
