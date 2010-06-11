#include <dtGUI/gui.h>
#include <dtGUI/scriptmodule.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>

#include <osg/Drawable>
#include <osg/StateSet>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/FileNameUtils>
#include <osgViewer/GraphicsWindow>

#include <CEGUI/CEGUISystem.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLRenderer.h>  // for base class
#include <CEGUI/CEGUIDefaultResourceProvider.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIScriptModule.h>
#include <CEGUI/CEGUIScheme.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIFont.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/CEGUITexture.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLTexture.h>
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
      mCamera->GetOSGCamera()->removeChild(mInternalGraph);
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
   mInternalGraph = new osg::Group();
   osg::StateSet* states = mInternalGraph->getOrCreateStateSet();

   //m_pInternalGraph->setName("internal_GUI_Geode");
   states->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   states->setRenderBinDetails(11, "RenderBin");
   states->setMode(GL_BLEND, osg::StateAttribute::ON);
   states->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
   states->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   osg::Geode* geode = new osg::Geode;
   geode->addDrawable(new HUDDrawable());
   mInternalGraph->addChild(geode);
}

////////////////////////////////////////////////////////////////////////////////
void GUI::SetCamera(dtCore::Camera* camera)
{
   // if this was already a child of another camera remove itself from there:
   if (mCamera.valid())
   {
      mCamera->GetOSGCamera()->removeChild(mInternalGraph);
   }

   // set ("parent") camera
   mCamera = camera;

   // that'll force the camera to draw this gui via the HUDDrawable-object
   if (mCamera)
   {
      mCamera->GetOSGCamera()->addChild(mInternalGraph);
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

      //CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

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
void dtGUI::GUI::DestroyWidget(Widget* widget)
{
   if(widget->getParent() != NULL)
   {
      widget->getParent()->removeChildWindow(widget);
   }
   CEGUI::WindowManager::getSingleton().destroyWindow(widget);
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
std::string dtGUI::GUI::SetResourceGroupFromResource(const std::string& resourceGroup,
                                                     const std::string& resourceToFind)
{
   //using data file search paths, find the resource and set the resourceGroup to that path
   const std::string fullPath = dtUtil::FindFileInPathList(resourceToFind);
   if (fullPath.empty())
   {
      //file not found
      return std::string();
   }

   const std::string path = osgDB::getFilePath(fullPath);

   SetResourceGroupDirectory(resourceGroup, path);
   return path;
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
void dtGUI::GUI::FindWidgets(const std::string& subName, std::vector<Widget*>& toFill)
{
   for (CEGUI::WindowManager::WindowIterator iter = CEGUI::WindowManager::getSingleton().getIterator(); !iter.isAtEnd(); ++iter)
   {
      Widget* win = iter.getCurrentValue();

      if (win->getName().find(subName) != CEGUI::String::npos)
      {
         toFill.push_back(win);
      }
   }

   if (!toFill.empty())
   {
      return;
   }

   LOG_ERROR(subName + " is not available in gui \"" + this->GetName() + "\"\n");
   return;
}

////////////////////////////////////////////////////////////////////////////////
Widget* dtGUI::GUI::FindWidget(const std::string& subName)
{
   std::vector<Widget*> toFill;
   FindWidgets(subName, toFill);

   if (!toFill.empty())
   {
      return toFill.front();
   }

   LOG_ERROR(subName + " is not available in gui \"" + this->GetName() + "\"\n"); 
   return NULL;
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
void dtGUI::GUI::SetScriptModule(BaseScriptModule* scriptModule)
{
   if (CEGUI::System::getSingletonPtr())
   {
      CEGUI::System::getSingletonPtr()->setScriptingModule(scriptModule);
   }
}

////////////////////////////////////////////////////////////////////////////////
BaseScriptModule* dtGUI::GUI::GetScriptModule()
{
   if (CEGUI::System::getSingletonPtr())
   {
      return CEGUI::System::getSingletonPtr()->getScriptingModule();
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
osg::Group& dtGUI::GUI::GetRootNode()
{
   return *mInternalGraph;
}

////////////////////////////////////////////////////////////////////////////////
const osg::Group& dtGUI::GUI::GetRootNode() const
{
   return *mInternalGraph;
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

////////////////////////////////////////////////////////////////////////////////
void GUI::DestroyImageset(const std::string& imagesetName)
{
   _SetupSystemAndRenderer();
   if (IsImagesetPresent(imagesetName))
   {
      CEGUI::ImagesetManager::getSingleton().destroy(imagesetName);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::AutoScaleImageset(const std::string& imagesetName, bool autoScale)
{
   _SetupSystemAndRenderer();
   if (IsImagesetPresent(imagesetName))
   {
      CEGUI::ImagesetManager::getSingleton().get(imagesetName).setAutoScalingEnabled(autoScale);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::DefineImage(const std::string& imagesetName, const std::string& image,
   osg::Vec2 position, osg::Vec2 size, osg::Vec2 offset)
{
   _SetupSystemAndRenderer();
   if (IsImagesetPresent(imagesetName))
   {
      CEGUI::Point ceguiPosition(position.x(), position.y());
      CEGUI::Size ceguiSize(size.x(), size.y());
      CEGUI::Point ceguiOffset(offset.x(), offset.y());
      CEGUI::ImagesetManager::getSingleton().get(imagesetName).defineImage(image, ceguiPosition, ceguiSize, ceguiOffset);
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Texture2D> dtGUI::GUI::CreateRenderTargetTexture(Widget& widget,
                                                                     const osg::Vec2* dimensions,
                                                                     const std::string& newImagesetName,
                                                                     const std::string& propertyName,
                                                                     const std::string& newImageName)
{
   
   if (!widget.isPropertyPresent(propertyName))
   {
      std::ostringstream oss;
      oss << "Widget \"" << widget.getName().c_str() << "\" does not have the \""
         << propertyName << "\" property. Cannot create render target texture.";
      LOG_WARNING(oss.str());
      return NULL;
   }

   // Determine the size of the texture.
   osg::Vec2 dims;
   if (dimensions != NULL)
   {
      dims.set(dimensions->x(), dimensions->y());
   }
   else
   {
      CEGUI::Size ceguiDims = widget.getPixelSize();
      dims.set(ceguiDims.d_width, ceguiDims.d_height);
   }

   // If no image name was specified, use the image set name.
   const std::string& imageName = newImageName.empty() ? newImagesetName : newImageName;

   CEGUI::Size ceguiDims(dims.x(), dims.y());
   CEGUI::Texture& texture = CEGUI::System::getSingleton().getRenderer()->createTexture(ceguiDims);
   CEGUI::Imageset& imageset = CEGUI::ImagesetManager::getSingleton().create(newImagesetName, texture);
   imageset.defineImage(imageName, CEGUI::Point(0,texture.getSize().d_height),
      CEGUI::Size(texture.getSize().d_width, -texture.getSize().d_height), CEGUI::Point(0,0));  //note: flipped upside down

   // create/allocate/setup osg-texture-render target
   osg::Texture2D* rttTexture = new osg::Texture2D();
   rttTexture->setTextureSize(dims.x(), dims.y());
   rttTexture->setInternalFormat(GL_RGBA);
   rttTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
   rttTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

   osgViewer::GraphicsWindow* gc = mCamera->GetWindow()->GetOsgViewerGraphicsWindow();
   rttTexture->apply(*gc->getState());

   //tell the CEGUI texture to use our OSG Texture
   GLuint textureID = rttTexture->getTextureObject(gc->getState()->getContextID())->_id;
   static_cast<CEGUI::OpenGLTexture&>(texture).setOpenGLTexture(textureID, ceguiDims);

   CEGUI::String ceguiSetImage = CEGUI::PropertyHelper::imageToString(&imageset.getImage(imageName));
   widget.setProperty(propertyName, ceguiSetImage);

   return rttTexture;
}

dtCore::RefPtr<dtCore::Camera> dtGUI::GUI::CreateCameraForRenderTargetTexture(osg::Texture2D& renderTargetTexture,
                                                                              const osg::Vec2& viewDimensions)
{
   // Create a Camera to render the specified target texture.
   dtCore::Camera* rttCam = new dtCore::Camera("RTTCamera");
   rttCam->SetWindow(mCamera->GetWindow()); // Use the root camera to gain access to the main window.

   osg::Camera* osgCam = rttCam->GetOSGCamera();
   osgCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   osgCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   osgCam->setViewport(0, 0, viewDimensions.x(), viewDimensions.y());
   osgCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
   osgCam->setRenderOrder(osg::Camera::PRE_RENDER);
   osgCam->attach(osg::Camera::COLOR_BUFFER, &renderTargetTexture);

   return rttCam;
}
