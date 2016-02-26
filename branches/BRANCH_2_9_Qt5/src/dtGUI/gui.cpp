#include <dtGUI/gui.h>
#include <dtGUI/scriptmodule.h>
#include <dtGUI/resourceprovider.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/nodemask.h>

#include <osg/Drawable>
#include <osg/StateSet>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/BlendFunc>
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
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLTexture.h>
#include <CEGUI/falagard/CEGUIFalWidgetLookManager.h>

#include <algorithm>

using namespace dtGUI;
IMPLEMENT_MANAGEMENT_LAYER(GUI)


std::string GUIVertexShader =
" "
"void main()"
"{"
"gl_Position = ftransform();"
"}";

std::string GUIFragmentShader =
"uniform sampler2D GUITexture;"
"uniform float ScreenWidth;"
"uniform float ScreenHeight;"
"void main ()"
"{"
"vec2 texCoords = vec2(gl_FragCoord.x / ScreenWidth, gl_FragCoord.y / ScreenHeight);"
"vec4 result = texture2D(GUITexture, texCoords);"
"gl_FragColor = result;"
"}";

namespace dtGUI
{
   GUI::GUITask::GUITask()
   {}

   GUI::GUITask::~GUITask()
   {}



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
         setDataVariance(osg::Object::DYNAMIC);
      }

      ///constructs an GUIdrawable-object and assign an HUD-object
      HUDDrawable()
      {
         this->setSupportsDisplayList(false);
         this->setUseDisplayList(false);
         setDataVariance(osg::Object::DYNAMIC);
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
         state->setActiveTextureUnit(0);
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



//////////////////////////////////////////////////////////////////////////
osg::Node* _CreateQuad( osg::Texture2D *tex, int renderBin )
{
   osg::Geometry* geo = new osg::Geometry;
   geo->setUseDisplayList( false );
   osg::Vec4Array* colors = new osg::Vec4Array;
   colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
   geo->setColorArray(colors);
   geo->setColorBinding(osg::Geometry::BIND_OVERALL);
   osg::Vec3Array *vx = new osg::Vec3Array;
   vx->push_back(osg::Vec3(0, 0, 0));
   vx->push_back(osg::Vec3(1, 0, 0));
   vx->push_back(osg::Vec3(1, 1, 0 ));
   vx->push_back(osg::Vec3(0, 1, 0));
   geo->setVertexArray(vx);
   osg::Vec3Array *nx = new osg::Vec3Array;
   nx->push_back(osg::Vec3(0, 0, 1));
   geo->setNormalArray(nx);
   if(tex != NULL)
   {
      osg::Vec2Array *tx = new osg::Vec2Array;
      tx->push_back(osg::Vec2(0, 0));
      tx->push_back(osg::Vec2(1, 0));
      tx->push_back(osg::Vec2(1, 1));
      tx->push_back(osg::Vec2(0, 1));
      geo->setTexCoordArray(0, tx);
      geo->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
   }

   geo->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
   osg::Geode *geode = new osg::Geode;
   geode->addDrawable(geo);
   geode->setCullingActive(false);
   osg::StateSet* ss = geode->getOrCreateStateSet();
   ss->setMode( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
   ss->setMode( GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
   //ss->setRenderBinDetails( renderBin, "RenderBin" );

   ss->setRenderBinDetails(11, "RenderBin");
   ss->setMode(GL_BLEND, osg::StateAttribute::ON);
   ss->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
   ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   osg::BlendFunc* bf = new osg::BlendFunc();
   bf->setFunction(osg::BlendFunc::SRC_ALPHA ,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
   ss->setAttributeAndModes(bf);


   return geode;
}

//////////////////////////////////////////////////////////////////////////
osg::Texture2D* _CreateTexture(int width, int height, bool mipmap)
{
   osg::Texture2D* tex = new osg::Texture2D();
   tex->setTextureSize(width, height);
   tex->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
   tex->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
   //tex->setSourceFormat( GL_RGBA );
   tex->setInternalFormat(GL_RGBA);
   if(mipmap)
   {
      tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
   }
   else
   {
      tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
   }

   tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
   return tex;
}


//////////////////////////////////////////////////////////////////////////
//CEGUI Logger
//////////////////////////////////////////////////////////////////////////
class CEGUILogger : public CEGUI::Logger
{
   void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel logLevel)
   {
      dtUtil::Log& log = dtUtil::Log::GetInstance("CEGUI");

      if(logLevel == CEGUI::Errors)
      {
         log.LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, message.c_str());
      }
      else if(logLevel == CEGUI::Warnings)
      {
         log.LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, message.c_str());
      }
      else if(logLevel == CEGUI::Standard)
      {
         log.LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, message.c_str());
      }
      else
      {
         log.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, message.c_str());
      }

   }

   void setLogFilename(const CEGUI::String& str, bool b)
   {
      //we dont need to do anything here
   }
};

//declaring this overrides the default logger for CEGUI
CEGUILogger CEGUILogger;


////////////////////////////////////////////////////////////////////////////////
// STATIC VARIABLES & OBJECTS
////////////////////////////////////////////////////////////////////////////////
dtGUI::ResourceProvider mResProvider;
bool GUI::SystemAndRendererCreatedByHUD = false;



////////////////////////////////////////////////////////////////////////////////
// GUI IMPLEMENTATION
////////////////////////////////////////////////////////////////////////////////
GUI::GUI(dtCore::Camera* camera,
         dtCore::Keyboard* keyboard,
         dtCore::Mouse* mouse)
         : mPreRenderToTexture(false)
         , mRootSheet(NULL)
{
   mMouseListener    = new CEGUIMouseListener();
   mKeyboardListener = new CEGUIKeyboardListener();

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &GUI::OnSystem);
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
   if (mCamera.valid() && mInternalGraph.valid())
   {
      mCamera->GetOSGCamera()->removeChild(mInternalGraph.get());
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

   // Need to do this BEFORE the instance count call below, otherwise it will fail to delete anything.
   DeregisterInstance(this);

   // Don't shutdown CEGUI unless all GUI instances are destroyed.
   if (GetInstanceCount() == 0)
   {
      CEGUI::OpenGLRenderer* renderer = static_cast<CEGUI::OpenGLRenderer*>(CEGUI::System::getSingletonPtr()->getRenderer());
      CEGUI::System::destroy();
      if (renderer)
      {
         CEGUI::OpenGLRenderer::destroy(*renderer);
      }
   }

}

////////////////////////////////////////////////////////////////////////////////
void GUI::_SetupInternalGraph()
{
   osg::Camera* camera = new osg::Camera();
   mInternalGraph = camera;//new osg::Group();

   //make sure this is indicated as being a post render pass
   camera->setNodeMask(dtUtil::NodeMask::FOREGROUND);

   camera->setClearMask(GL_DEPTH_BUFFER_BIT);
   camera->setRenderOrder(osg::Camera::POST_RENDER, 100);
   //// we don't want the camera to grab event focus from the viewers main camera(s).
   camera->setAllowEventFocus(false);

   osg::StateSet* states = mInternalGraph->getOrCreateStateSet();

   //m_pInternalGraph->setName("internal_GUI_Geode");
   states->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   states->setMode(GL_BLEND, osg::StateAttribute::ON);
   states->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);

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
      mCamera->GetOSGCamera()->removeChild(mInternalGraph.get());
   }

   // set ("parent") camera
   mCamera = camera;

   // that'll force the camera to draw this gui via the HUDDrawable-object
   if (mCamera.valid())
   {
      mCamera->GetOSGCamera()->addChild(mInternalGraph.get());
   }

}

////////////////////////////////////////////////////////////////////////////////
dtCore::Camera* GUI::GetCamera()
{
    return mCamera.get();
}

////////////////////////////////////////////////////////////////////////////////
const dtCore::Camera* GUI::GetCamera() const
{
    return mCamera.get();
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
      CEGUI::System::create(renderer, &mResProvider);

      //CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

      CEGUI::Imageset::setDefaultResourceGroup("imagesets");
      //      SetResourceGroupDirectory("imagesets", dtUtil::FindFileInPathList("imagesets"));
      //
      CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
      //      SetResourceGroupDirectory("looknfeels", dtUtil::FindFileInPathList("looknfeel"));
      //
      CEGUI::WindowManager::setDefaultResourceGroup("layouts");
      //      SetResourceGroupDirectory("layouts", dtUtil::FindFileInPathList("layouts"));
      //
      CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
      //      SetResourceGroupDirectory("lua_scripts", dtUtil::FindFileInPathList("lua_scripts"));
      //
      CEGUI::Scheme::setDefaultResourceGroup("schemes");
      //      SetResourceGroupDirectory("schemes", dtUtil::FindFileInPathList("schemes"));
      //
      CEGUI::Font::setDefaultResourceGroup("fonts");
      //      SetResourceGroupDirectory("fonts", dtUtil::FindFileInPathList("fonts"));

      if (CEGUI::System::getSingletonPtr() == NULL)
      {
         return;
      }

      dtGUI::ResourceProvider* rp = dynamic_cast<dtGUI::ResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

      if (rp != NULL)
      {
         rp->AddSearchSuffix("imagesets", "");
         rp->AddSearchSuffix("imagesets", "CEGUI/imagesets");
         rp->AddSearchSuffix("imagesets", "imagesets");
         rp->AddSearchSuffix("imagesets", "gui/imagesets");
         rp->AddSearchSuffix("looknfeel", "");
         rp->AddSearchSuffix("looknfeel", "CEGUI/looknfeel");
         rp->AddSearchSuffix("looknfeel", "looknfeel");
         rp->AddSearchSuffix("looknfeel", "gui/looknfeel");
         rp->AddSearchSuffix("layouts", "");
         rp->AddSearchSuffix("layouts", "CEGUI/layouts");
         rp->AddSearchSuffix("layouts", "layouts");
         rp->AddSearchSuffix("layouts", "gui/layouts");
         rp->AddSearchSuffix("lua_scripts", "");
         rp->AddSearchSuffix("lua_scripts", "CEGUI/lua_scripts");
         rp->AddSearchSuffix("lua_scripts", "lua_scripts");
         rp->AddSearchSuffix("lua_scripts", "gui/lua_scripts");
         rp->AddSearchSuffix("schemes", "");
         rp->AddSearchSuffix("schemes", "CEGUI/schemes");
         rp->AddSearchSuffix("schemes", "schemes");
         rp->AddSearchSuffix("schemes", "gui/schemes");
         rp->AddSearchSuffix("fonts", "");
         rp->AddSearchSuffix("fonts", "CEGUI/fonts");
         rp->AddSearchSuffix("fonts", "fonts");
         rp->AddSearchSuffix("fonts", "gui/fonts");
         rp->AddSearchSuffix("", "");
         rp->AddSearchSuffix("", "CEGUI");
         rp->AddSearchSuffix("", "gui");
      }

      SystemAndRendererCreatedByHUD = true;
   }
}

////////////////////////////////////////////////////////////////////////////////
void GUI::_SetupDefaultUI()
{
   dtCore::UniqueId rootSheetId;

   mRootSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", std::string(rootSheetId.ToString() + "rootsheet").c_str());
   mRootSheet->setMousePassThroughEnabled(true);
#if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR >= 7 && CEGUI_VERSION_PATCH > 1
   mRootSheet->setMouseInputPropagationEnabled(true);
#endif
   CEGUI::System::getSingleton().setGUISheet(mRootSheet);
}

////////////////////////////////////////////////////////////////////////////////
bool GUI::IsSchemePresent(const std::string& schemeName)
{
   _SetupSystemAndRenderer();
   return CEGUI::SchemeManager::getSingletonPtr()->isDefined(schemeName);
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

   if (!imagesetName.empty() && !imageName.empty())
   {
      CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(imagesetName, imageName);
   }
   else
   {
      CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(CEGUI::BlankMouseCursor);
   }
}

///////////////////////////////////////////////////////////////////////////////
void GUI::ShowCursor()
{
   _SetupSystemAndRenderer();
   CEGUI::MouseCursor::getSingleton().show();
}

///////////////////////////////////////////////////////////////////////////////
void GUI::HideCursor()
{
   _SetupSystemAndRenderer();
   CEGUI::MouseCursor::getSingleton().hide();
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

   if (layout)
   {
      mLayoutMap[fileName] = layout;
   }

   return layout;
}

////////////////////////////////////////////////////////////////////////////////
Widget* GUI::CreateWidget(Widget* parent, const std::string& typeName,
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
void GUI::DestroyWidget(Widget* widget)
{
   if(widget->getParent() != NULL)
   {
      widget->getParent()->removeChildWindow(widget);
   }
   CEGUI::WindowManager::getSingleton().destroyWindow(widget);
}

////////////////////////////////////////////////////////////////////////////////
Widget* GUI::CreateWidget(const std::string& typeName, const std::string& name)

{
   CEGUI::Window* window = CEGUI::WindowManager::getSingleton().createWindow(typeName, name);

   mRootSheet->addChildWindow(window);

   return window;
}

////////////////////////////////////////////////////////////////////////////////
void GUI::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      // deltaTime[0] is simulated time, [1] is real frame rate time

      CEGUI::System::getSingletonPtr()->injectTimePulse(static_cast<float>(deltaReal));

      UpdateTasks((float)deltaSim);
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
std::string GUI::SetResourceGroupFromResource(const std::string& resourceGroup,
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
Widget* GUI::GetWidget(const std::string& name)
{
   if (!CEGUI::WindowManager::getSingleton().isWindowPresent(name))
   {
      LOG_ERROR(name + " is not available in gui \"" + this->GetName() + "\"\n");
      return 0;
   }

   return CEGUI::WindowManager::getSingleton().getWindow(name);
}


////////////////////////////////////////////////////////////////////////////////
void GUI::FindWidgets(const std::string& subName, std::vector<Widget*>& toFill)
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
Widget* GUI::FindWidget(const std::string& subName)
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
Widget* GUI::GetRootSheet()
{
   return mRootSheet;
}

////////////////////////////////////////////////////////////////////////////////
const Widget* GUI::GetRootSheet() const
{
   return mRootSheet;
}

////////////////////////////////////////////////////////////////////////////////
void GUI::SetScriptModule(BaseScriptModule* scriptModule)
{
   if (CEGUI::System::getSingletonPtr())
   {
      CEGUI::System::getSingletonPtr()->setScriptingModule(scriptModule);
   }
}

////////////////////////////////////////////////////////////////////////////////
BaseScriptModule* GUI::GetScriptModule()
{
   if (CEGUI::System::getSingletonPtr())
   {
      return CEGUI::System::getSingletonPtr()->getScriptingModule();
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
osg::Group& GUI::GetRootNode()
{
   return *mInternalGraph;
}

////////////////////////////////////////////////////////////////////////////////
const osg::Group& GUI::GetRootNode() const
{
   return *mInternalGraph;
}

////////////////////////////////////////////////////////////////////////////////
bool GUI::SetDefaultParser(const std::string& parserName)
{
   _SetupSystemAndRenderer();
   CEGUI::System& sys = CEGUI::System::getSingleton();
   const CEGUI::String currentParserName = sys.getDefaultXMLParserName();

   bool success = false;
   try
   {
      sys.setXMLParser(parserName);
      success = true;
   }
   catch(CEGUI::Exception& e)
   {
      std::ostringstream oss;
      oss << "CEGUI system will use the original default XML parser \""
         << currentParserName.c_str() << "\" because the system could not link to use XML parser \""
         << parserName.c_str() << "\" because of the following CEGUI exception ("
         << e.getName().c_str() << "):\n"
         << e.getMessage().c_str() << "\n";
      LOG_WARNING(oss.str().c_str());
   }
   catch(...)
   {
      std::ostringstream oss;
      oss << "CEGUI system will use the original default XML parser \""
         << currentParserName.c_str() << "\" because the system could not link to use XML parser \""
         << parserName.c_str() << "\" because of some unknown exception.\n";
      LOG_ERROR(oss.str().c_str());
   }

   // If the intended parser assignment failed, ensure the last parser used is assigned.
   if( ! success)
   {
      sys.setXMLParser(currentParserName);
   }

   return success;
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
void GUI::AutoScaleImageset(const std::string& imagesetName, bool autoScale)
{
   _SetupSystemAndRenderer();
   if (IsImagesetPresent(imagesetName))
   {
      CEGUI::ImagesetManager::getSingleton().get(imagesetName).setAutoScalingEnabled(autoScale);
   }
}

////////////////////////////////////////////////////////////////////////////////
void GUI::DefineImage(const std::string& imagesetName, const std::string& image,
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
dtCore::RefPtr<osg::Texture2D> GUI::CreateRenderTargetTexture(Widget& widget,
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

   if (dims.x() < 1.0) dims.x() = 1.0;
   if (dims.y() < 1.0) dims.y() = 1.0;

   // If no image name was specified, use the image set name.
   const std::string& imageName = newImageName.empty() ? newImagesetName : newImageName;

   CEGUI::Size ceguiDims(dims.x(), dims.y());
   CEGUI::Texture& texture = CEGUI::System::getSingleton().getRenderer()->createTexture(ceguiDims);
   CEGUI::Imageset& imageset = CEGUI::ImagesetManager::getSingleton().create(newImagesetName, texture);
   imageset.defineImage(imageName, CEGUI::Point(0,texture.getSize().d_height),
      CEGUI::Size(texture.getSize().d_width, -texture.getSize().d_height), CEGUI::Point(0,0));  //note: flipped upside down

   // create/allocate/setup osg-texture-render target
   osg::Texture2D* rttTexture = new osg::Texture2D();
   rttTexture->setTextureSize(int(dims.x()), int(dims.y()));
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

dtCore::RefPtr<dtCore::Camera> GUI::CreateCameraForRenderTargetTexture(osg::Texture2D& renderTargetTexture,
                                                                       const osg::Vec2& viewDimensions)
{
   // Create a Camera to render the specified target texture.
   dtCore::Camera* rttCam = new dtCore::Camera("RTTCamera");
   rttCam->SetWindow(mCamera->GetWindow()); // Use the root camera to gain access to the main window.

   osg::Camera* osgCam = rttCam->GetOSGCamera();
   osgCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   osgCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   osgCam->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 0.0));
   osgCam->setViewport(0, 0, int(viewDimensions.x()), int(viewDimensions.y()));
   osgCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
   osgCam->setRenderOrder(osg::Camera::PRE_RENDER);
   osgCam->attach(osg::Camera::COLOR_BUFFER, &renderTargetTexture);

   return rttCam;
}

////////////////////////////////////////////////////////////////////////////////
bool GUI::AddSearchSuffix(const std::string& resourceGroup,
                          const std::string& searchSuffix)
{
   return mResProvider.AddSearchSuffix(resourceGroup, searchSuffix);
}

////////////////////////////////////////////////////////////////////////////////
bool GUI::RemoveSearchSuffix(const std::string& resourceGroup,
                             const std::string& searchSuffix)
{
   return mResProvider.RemoveSearchSuffix(resourceGroup, searchSuffix);
}

////////////////////////////////////////////////////////////////////////////////
unsigned GUI::RemoveSearchSuffixes(const std::string& resourceGroup)
{
   return mResProvider.RemoveSearchSuffixes(resourceGroup);
}

////////////////////////////////////////////////////////////////////////////////
unsigned GUI::ClearSearchSuffixes()
{
   return mResProvider.ClearSearchSuffixes();
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::GUI::SetPreRenderGUIToTexture(bool b)
{
   mPreRenderToTexture = b;

   if(mPreRenderToTexture)
   {
      if(mGUIPreRenderCamera.valid())
      {

         mCamera->GetOSGCamera()->removeChild(mGUIPreRenderCamera.get());
         mCamera->GetOSGCamera()->removeChild(mGUICameraScreen.get());
         mGUIPreRenderCamera = NULL;
         mGUICameraScreen = NULL;
         mGUITexture = NULL;
      }

      mGUIPreRenderCamera = new osg::Camera(static_cast<const osg::Camera&>(*mInternalGraph), osg::CopyOp::SHALLOW_COPY);

      mCamera->GetOSGCamera()->removeChild(mInternalGraph.get());
      mInternalGraph = NULL;

      int width = mCamera->GetOSGCamera()->getViewport()->width();
      int height = mCamera->GetOSGCamera()->getViewport()->height();

      mGUIPreRenderCamera->setRenderOrder(osg::Camera::PRE_RENDER, 1);
      mGUIPreRenderCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      mGUIPreRenderCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

      mGUITexture = _CreateTexture(width, height, false);
      mGUIPreRenderCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      mGUIPreRenderCamera->setProjectionMatrixAsOrtho2D(0.0, 1.0, 0.0, 1.0);
      mGUIPreRenderCamera->setViewport(0, 0, width, height);
      mGUIPreRenderCamera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
      mGUIPreRenderCamera->detach( osg::Camera::COLOR_BUFFER );
      mGUIPreRenderCamera->attach( osg::Camera::COLOR_BUFFER, mGUITexture.get());

      mGUICameraScreen = new osg::Camera();
      mGUICameraScreen->setRenderOrder(osg::Camera::POST_RENDER, 1);

      //don't clear the color buffer (allows the UI to be superimposed on the scene)
      mGUICameraScreen->setClearMask(GL_DEPTH_BUFFER_BIT);
      mGUIPreRenderCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      mGUIPreRenderCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

      mGUICameraScreen->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      mGUICameraScreen->setProjectionMatrixAsOrtho2D(0.0, 1.0, 0.0, 1.0);
      mGUICameraScreen->setViewport(0, 0, width, height);
      AddOrthoQuad(mGUICameraScreen.get(), mGUITexture.get(), "GUITexture");

      osg::StateSet* guiScreenStateSet = mGUICameraScreen->getStateSet();
      osg::Uniform* screenWidth = guiScreenStateSet->getOrCreateUniform("ScreenWidth", osg::Uniform::FLOAT);
      osg::Uniform* screenHeight = guiScreenStateSet->getOrCreateUniform("ScreenHeight", osg::Uniform::FLOAT);

      screenWidth->set(float(width));
      screenHeight->set(float(height));

      osg::StateSet* states = mGUIPreRenderCamera->getOrCreateStateSet();

      states->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
      states->setRenderBinDetails(11, "RenderBin");
      states->setMode(GL_BLEND, osg::StateAttribute::ON);
      states->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);
      states->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

      osg::BlendFunc* bf = new osg::BlendFunc();
      bf->setFunction(osg::BlendFunc::SRC_ALPHA ,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
      states->setAttributeAndModes(bf);

      mCamera->GetOSGCamera()->addChild(mGUIPreRenderCamera.get());
      mCamera->GetOSGCamera()->addChild(mGUICameraScreen.get());

      mInternalGraph = mGUIPreRenderCamera;
   }
   else
   {

      if(mGUIPreRenderCamera.valid())
      {
         //osg::Camera* camera = new osg::Camera(static_cast<const osg::Camera&>(*mGUIPreRenderCamera), osg::CopyOp::SHALLOW_COPY);
         osg::Camera* camera = new osg::Camera();
         for(unsigned i = 0; i < mGUIPreRenderCamera->getNumChildren(); ++i)
         {
            camera->addChild(mGUIPreRenderCamera->getChild(i));
         }

         mCamera->GetOSGCamera()->removeChild(mGUIPreRenderCamera.get());
         mCamera->GetOSGCamera()->removeChild(mGUICameraScreen.get());
         mGUIPreRenderCamera = NULL;
         mGUICameraScreen = NULL;
         mGUITexture = NULL;

         mInternalGraph = camera;

         camera->setClearMask(GL_DEPTH_BUFFER_BIT);
         camera->setRenderOrder(osg::Camera::POST_RENDER, 100);
         //// we don't want the camera to grab event focus from the viewers main camera(s).
         camera->setAllowEventFocus(false);

         osg::StateSet* states = mInternalGraph->getOrCreateStateSet();

         //m_pInternalGraph->setName("internal_GUI_Geode");
         states->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
         states->setMode(GL_BLEND, osg::StateAttribute::ON);
         states->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);

         mCamera->GetOSGCamera()->addChild(mInternalGraph.get());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool dtGUI::GUI::GetPreRenderGUIToTexture() const
{
   return mPreRenderToTexture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GUI::AddTask(GUI::GUITask& task)
{
   bool success = false;
   
   if ( ! HasTask(task))
   {
      mTasks.push_back(&task);
      success = true;
   }

   return success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GUI::RemoveTask(GUI::GUITask& task)
{
   bool success = false;
   GUITaskArray::iterator foundIter = std::find(mTasks.begin(), mTasks.end(), &task);

   if (foundIter != mTasks.end())
   {
      mTasks.erase(foundIter);
      success = true;
   }

   return success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GUI::HasTask(GUI::GUITask& task) const
{
   return mTasks.end() != std::find(mTasks.begin(), mTasks.end(), &task);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GUI::GUITaskArray& GUI::GetTasks()
{
   return mTasks;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const GUI::GUITaskArray& GUI::GetTasks() const
{
   return mTasks;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GUI::GetTaskCount() const
{
   return (int)mTasks.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GUI::UpdateTasks(float dt)
{
   int results = 0;

   GUITaskArray::iterator curIter = mTasks.begin();
   GUITaskArray::iterator endIter = mTasks.end();
   for (; curIter != endIter; ++curIter)
   {
      (*curIter)->Update(dt);
      ++results;
   }

   return results;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GUI::AddOrthoQuad(osg::Camera* cn, osg::Texture2D* tx, const std::string& texUniform)
{
   osg::Node* quad = _CreateQuad(tx, 11);
   cn->addChild(quad);

   osg::ref_ptr<osg::Shader> vertShader = new osg::Shader(osg::Shader::VERTEX, GUIVertexShader);
   osg::ref_ptr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT, GUIFragmentShader);

   osg::ref_ptr<osg::Program> shaderProgram = new osg::Program();
   shaderProgram->addShader(vertShader.get());
   shaderProgram->addShader(fragShader.get());

   osg::StateSet* ss = quad->getOrCreateStateSet();
   ss->setAttribute(shaderProgram.get(), osg::StateAttribute::ON);

   if(tx != NULL)
   {
      osg::StateSet* ss = quad->getOrCreateStateSet();
      osg::Uniform* uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D, texUniform);
      uniform->set(int(0));
      ss->addUniform(uniform);
   }
}

