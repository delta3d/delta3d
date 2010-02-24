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

};


bool GUI::SystemAndRendererCreatedByHUD = false;

////////////////////////////////////////////////////////////////////////////////
GUI::GUI(dtCore::Camera* camera,
         dtCore::Keyboard* keyboard,
         dtCore::Mouse* mouse):
mRootSheet(NULL)
{
   mMouseListener    = new CEGUIMouseListener();
   mKeyboardListener = new CEGUIKeyboardListener();

   AddSender(&dtCore::System::GetInstance());
   RegisterInstance(this);

   _SetupInternalGraph();

   SetCamera(camera);
   SetKeyboard(keyboard);
   SetMouse(mouse);

   _SetupSystemAndRenderer();
   _SetupDefaultUI();
}

////////////////////////////////////////////////////////////////////////////////
GUI::~GUI()
{
   RemoveSender(&dtCore::System::GetInstance());

   if (mCamera.valid() && mInternalGraph)
   {
      mCamera->removeChild(mInternalGraph);
   }

   if (mMouse.valid())
   {
      mMouse->RemoveMouseListener(mMouseListener.get());
   }

   if (mKeyboard.valid())
   {
      mKeyboard->RemoveKeyboardListener(mKeyboardListener.get());
   }

   if (mRootSheet)
   {
      CEGUI::WindowManager::getSingletonPtr()->destroyWindow(mRootSheet);
      mRootSheet = NULL;
   }

   CEGUI::OpenGLRenderer* renderer = static_cast<CEGUI::OpenGLRenderer*>(CEGUI::System::getSingletonPtr()->getRenderer());
   CEGUI::System::destroy();
   if (renderer)
   {
      CEGUI::OpenGLRenderer::destroy(*renderer);
   }

   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void GUI::_SetupInternalGraph()
{
   mInternalGraph = new osg::Geode();

   //m_pInternalGraph->setName("internal_GUI_Geode");
   mInternalGraph->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   mInternalGraph->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");
   mInternalGraph->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
   mInternalGraph->getOrCreateStateSet()->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
   mInternalGraph->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   mInternalGraph->addDrawable(new HUDDrawable());
}

////////////////////////////////////////////////////////////////////////////////
void GUI::SetCamera(dtCore::Camera* camera)
{
   // if this was already a child of another camera remove itself from there:
   if (mCamera.valid())
   {
      mCamera->removeChild(mInternalGraph);
   }

   // set ("parent") camera
   mCamera = camera == NULL ? NULL : camera->GetOSGCamera();

   // that'll force the camera to draw this gui via the HUDDrawable-object
   if (mCamera)
   {
      mCamera->addChild(mInternalGraph);
   }

}

////////////////////////////////////////////////////////////////////////////////
void GUI::SetMouse(dtCore::Mouse* mouse)
{
   if (mMouse.valid())
   {
      mMouse->RemoveMouseListener(mMouseListener.get());
   }

   mMouse = mouse;

   if (mMouse.valid())
   {
      if (mMouse->GetListeners().empty())
      {
         mMouse->AddMouseListener(mMouseListener.get());
      }
      else
      {
         mMouse->InsertMouseListener(mMouse->GetListeners().front() , mMouseListener.get());
      }
   }

}

////////////////////////////////////////////////////////////////////////////////
void GUI::SetKeyboard(dtCore::Keyboard* keyboard)
{
   if (mKeyboard.valid())
   {
      mKeyboard->RemoveKeyboardListener(mKeyboardListener.get());
   }

   mKeyboard = keyboard;

   if (mKeyboard.valid())
   {
      if (mKeyboard->GetListeners().empty())
      {
         mKeyboard->AddKeyboardListener(mKeyboardListener.get());
      }
      else
      {
         mKeyboard->InsertKeyboardListener(mKeyboard->GetListeners().front(), mKeyboardListener.get());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void GUI::_SetupSystemAndRenderer()
{
   if (!CEGUI::System::getSingletonPtr())
   {
      CEGUI::OpenGLRenderer& renderer = CEGUI::OpenGLRenderer::create();
      renderer.enableExtraStateSettings(true);
      CEGUI::System::create(renderer);

      CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

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
void GUI::_SetupDefaultUI()
{
   //generate unique root-window-name:
   std::string generatedUniquePrefix="";

   //generate unqiue prefix
   for (unsigned int i = 0; i < 65000; i++)
   {
      std::stringstream ssTryName;
      ssTryName << "gui" << i;
      if (!CEGUI::WindowManager::getSingleton().isWindowPresent(ssTryName.str()+"rootsheet"))
      {
         generatedUniquePrefix = ssTryName.str();
         break;
      }
   }
   if (generatedUniquePrefix=="")
   {
      LOG_ERROR("cannot generate prefix");
      return;
   }

   mRootSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", std::string(generatedUniquePrefix + "rootsheet").c_str());
   CEGUI::System::getSingleton().setGUISheet(mRootSheet);
}

////////////////////////////////////////////////////////////////////////////////
void GUI::LoadScheme(const std::string& fileName, const std::string& resourceGroup)
{
   _SetupSystemAndRenderer();
   CEGUI::SchemeManager::getSingleton().create(fileName, resourceGroup);
}

////////////////////////////////////////////////////////////////////////////////
void GUI::SetMouseCursor(const std::string& imagesetName, const std::string& imageName)
{
   _SetupSystemAndRenderer();
   CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(imagesetName, imageName);
}

////////////////////////////////////////////////////////////////////////////////
CEGUI::Window* GUI::LoadLayout(const std::string& fileName, const std::string& prefix, const std::string& resourceGroup)
{
   return LoadLayout(mRootSheet, fileName, prefix, resourceGroup);
}

////////////////////////////////////////////////////////////////////////////////
CEGUI::Window* GUI::LoadLayout(Widget* parent, const std::string& fileName,
                               const std::string& prefix,
                               const std::string& resourceGroup)
{
   CEGUI::Window* layout = CEGUI::WindowManager::getSingleton().loadWindowLayout(fileName, prefix, resourceGroup);
   
   parent->addChildWindow(layout);
   
   return layout;
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::CreateWidget(Widget* parent, const std::string& typeName,
                                 const std::string& name)
{
   Widget* newWidget = CreateWidget(typeName, name);
   if (parent)
   { 
      parent->addChildWindow(newWidget); 
   }
   return newWidget;
}


////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::CreateWidget(const std::string& typeName, const std::string& name)
{
   CEGUI::Window* window = CEGUI::WindowManager::getSingleton().createWindow(typeName, name);

   mRootSheet->addChildWindow(window);
   
   return window;
}

////////////////////////////////////////////////////////////////////////////////
void GUI::OnMessage(dtCore::Base::MessageData *data)
{
   if( data->message == dtCore::System::MESSAGE_PRE_FRAME )
   {
      //_CheckCamera();

      const double deltaTime = *static_cast<const double*>(data->userData);

      CEGUI::System::getSingletonPtr()->injectTimePulse(static_cast<float>(deltaTime));
   }
}

////////////////////////////////////////////////////////////////////////////////
void GUI::SetResourceGroupDirectory(const std::string& resourceType, const std::string& directory)
{
   if (CEGUI::System::getSingletonPtr() == NULL)
   {
      return;
   }

   CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

   rp->setResourceGroupDirectory(resourceType, directory);
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::GetWidget(const std::string& name)
{
   if (!CEGUI::WindowManager::getSingleton().isWindowPresent(name))
   {
      LOG_ERROR(name + " is not available in gui \"" + this->GetName() + "\"\n");
      return 0;
   }

   return CEGUI::WindowManager::getSingleton().getWindow(name);
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::GetRootSheet()
{
   return mRootSheet;
}

////////////////////////////////////////////////////////////////////////////////
const Widget* dtGUI::GUI::GetRootSheet() const
{
   return mRootSheet;
}
////////////////////////////////////////////////////////////////////////////////
CEGUI::Event::Connection GUI::SubscribeEvent(const std::string& widgetName, 
                                             const std::string& event,
                                             GUI::Subscriber subscriber)
{
   CEGUI::Window* window = GetWidget(widgetName);
   
   if (window)
      return SubscribeEvent(*window, event, subscriber);
   
   LOG_ERROR("Could not find widget for event subscription: " + widgetName);
   return CEGUI::Event::Connection();
}

////////////////////////////////////////////////////////////////////////////////
CEGUI::Event::Connection GUI::SubscribeEvent(CEGUI::Window& window,
                                             const std::string& event,
                                             GUI::Subscriber subscriber)
{
   return window.subscribeEvent(event, subscriber);
}

////////////////////////////////////////////////////////////////////////////////
bool GUI::IsWindowPresent(const std::string& widgetName)
{
   _SetupSystemAndRenderer();
   return CEGUI::WindowManager::getSingleton().isWindowPresent(widgetName);
}

////////////////////////////////////////////////////////////////////////////////
bool GUI::IsImagesetPresent(const std::string& imagesetName)
{
   _SetupSystemAndRenderer();
   return CEGUI::ImagesetManager::getSingleton().isDefined(imagesetName);
}

////////////////////////////////////////////////////////////////////////////////
void GUI::CreateImageset(const std::string& imagesetName, 
                                const std::string& fileName,
                                const std::string& resourceGroup)
{
   _SetupSystemAndRenderer();
   CEGUI::ImagesetManager::getSingleton().createFromImageFile(imagesetName, fileName, resourceGroup);
}


