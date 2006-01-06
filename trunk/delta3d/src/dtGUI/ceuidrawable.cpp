#include <CEGUI.h>

#include <dtGUI/ceuidrawable.h>
#include <dtGUI/basescriptmodule.h>
#include <dtCore/deltawin.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <osg/Geode>
#include <osg/Projection>
#include <osg/MatrixTransform>

using namespace dtCore;
using namespace dtGUI;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(CEUIDrawable)

/** The constructor.  Supply the width and height
* of the parent Window.  The constructor will create a new CEUI and 
* OpenGLRenderer,and create the OSG nodes.
* The user must manually notify CEGUIDrawable if the parent DeltaWin changes
* size.
* @see SetRenderingSize()
*/
CEUIDrawable::CEUIDrawable(int width, int height, dtGUI::BaseScriptModule* sm):
   mUI(0),
   mWidth(width),
   mHeight(height),
   mRenderer(new dtGUI::Renderer(1024, mWidth, mHeight)),
   mScriptModule(sm),
   mProjection(new osg::Projection(osg::Matrix::ortho2D(0,width,0,height))),
   mTransform(new osg::MatrixTransform(osg::Matrix::identity())),
   mWindow(DeltaWin::GetInstance(0)),
   mAutoResize(false)
{
   //throw exception if win == NULL
   Config();
}


/** The supplied DeltaWin will automatically be monitored for size change and pass the new
  * size onto the CEGUI Renderer.
  * @param win : The DeltaWin to monitor for size change
  * @param sm : The ScriptModule to use for CEGUI script processing
  */
CEUIDrawable::CEUIDrawable( dtCore::DeltaWin *win, dtGUI::BaseScriptModule *sm):
   mUI(0),
   mWidth(1024),
   mHeight(1280),
   mRenderer(new dtGUI::Renderer(1024, mWidth, mHeight)),
   mScriptModule(sm),
   mProjection(new osg::Projection(osg::Matrix::ortho2D(0,mWidth,0,mHeight))),
   mTransform(new osg::MatrixTransform(osg::Matrix::identity())),
   mWindow(win),
   mAutoResize(true)
{
   //throw exception if win == NULL
   Config();
}

CEUIDrawable::~CEUIDrawable(void)
{
   RemoveSender( System::Instance() );
   mNode = NULL;
   delete mUI;
   delete mRenderer;
}

void CEUIDrawable::Config()
{
   AddSender( System::Instance() );
   
   RegisterInstance(this);

   SetRenderingSize(mWidth, mHeight);

   mWindow->GetMouse()->AddMouseListener(this); 
   mWindow->GetKeyboard()->AddKeyboardListener(this);

   if( mScriptModule )
      new CEGUI::System(mRenderer,mScriptModule);
   else
      new CEGUI::System(mRenderer);

   mUI = CEGUI::System::getSingletonPtr();

   osg::Geode *geod = new osg::Geode();

   osg::StateSet* stateset = geod->getOrCreateStateSet();
   stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

   stateset->setRenderBinDetails(11,"RenderBin");
   stateset->setMode(GL_BLEND,osg::StateAttribute::ON);

   geod->setStateSet(stateset);

   osg::ref_ptr<osgCEUIDrawable> osgCEUI = new osgCEUIDrawable(mUI);
   geod->addDrawable( osgCEUI.get() ); //add our osg node here

   mTransform->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
   mTransform->addChild( geod );

   mProjection->addChild( mTransform.get() );

   mNode = mProjection.get();
}

bool CEUIDrawable::AddChild(DeltaDrawable *child)
{
   // Add the child's graphics node to our's
   if( DeltaDrawable::AddChild(child) ) 
   {
      mTransform->addChild( child->GetOSGNode() );
      return true;
   }
   return false;
}

void CEUIDrawable::MouseMoved(Mouse* mouse, float x, float y)
{
   if (!mUI) return;

   CEGUI::System::getSingleton().injectMousePosition( ((x+1)*0.5f)*mWidth, ((-y+1)*0.5f)*mHeight);
}

void CEUIDrawable::MouseDragged(Mouse* mouse, float x, float y)
{
   this->MouseMoved(mouse, x, y);
}

void CEUIDrawable::ButtonPressed(dtCore::Mouse* mouse, dtCore::MouseButton button)
{
      switch(button)
      {
      case LeftButton:
         CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
         break;
      case RightButton:
         CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
         break;
      case MiddleButton:
         CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
         break;
      }
}

void CEUIDrawable::ButtonReleased(dtCore::Mouse* mouse, dtCore::MouseButton button) 
{
   switch(button)
   {
   case LeftButton:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
      break;
   case RightButton:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
      break;
   case MiddleButton:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
      break;
   }
}

void CEUIDrawable::KeyPressed(Keyboard* keyboard, 
                               Producer::KeyboardKey key,
                               Producer::KeyCharacter character)
{
   switch(character) 
   {
	case Producer::KeyChar_BackSpace: CEGUI::System::getSingleton().injectKeyDown(0x0E);  	break;
	case Producer::KeyChar_Return: CEGUI::System::getSingleton().injectKeyDown(0x1C);  	break;
   
   default:
      {
         CEGUI::System::getSingleton().injectKeyDown( static_cast<CEGUI::uint>(key) );
         CEGUI::System::getSingleton().injectChar( static_cast<CEGUI::utf32>(character) );
      } break;
   }
}

/**  Display the properties associated with the supplied CEGUI::Window to the 
  *  console.  Useful to find all the text names of the properties and see what
  *  the current values of the properties are.
  *
  * @param window : The window to query the properties of
  * @param onlyNonDefault : Display only properties that are not default values
  *                          (default=true)
  */
void CEUIDrawable::DisplayProperties(CEGUI::Window *window, bool onlyNonDefault)
{
   // Log all its properties + values
   dtUtil::Log *log = &dtUtil::Log::GetInstance();

   CEGUI::PropertySet::PropertyIterator itr = ((CEGUI::PropertySet*)window)->getIterator();
   while (!itr.isAtEnd()) 
   {
      try 
      {
         if ( onlyNonDefault && !window->isPropertyDefault(itr.getCurrentKey()) )
         {
            {
               log->LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
                        "%s, Non Default Prop: %s, %s", window->getName().c_str(),
                        itr.getCurrentKey().c_str(),
                        window->getProperty(itr.getCurrentKey()).c_str());
            }
         }
         else if ( !onlyNonDefault )
         {
            log->LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
                     "%s, Prop: %s, %s", window->getName().c_str(),
                     itr.getCurrentKey().c_str(),
                     window->getProperty(itr.getCurrentKey()).c_str());
         }
      }
      catch (CEGUI::InvalidRequestException& exception) 
      {
         // If something goes wrong, show user
         log->LogMessage(Log::LOG_WARNING, __FUNCTION__,
            "InvalidRequestException for %s: %s", itr.getCurrentKey().c_str(), exception.getMessage().c_str());
      }
      itr++;
   }
}

void CEUIDrawable::MouseScrolled(dtCore::Mouse* mouse, int change)
{
   CEGUI::System::getSingleton().injectMouseWheelChange( (float)change );
}


void CEUIDrawable::OnMessage(dtCore::Base::MessageData *data)
{
   if( data->message == "preframe" )
   {  
      if (GetAutoResizing() == true)
      {
         if (!mWindow.valid()) return;

         int x,y,w,h;

         mWindow->GetPosition(&x, &y, &w, &h);
         
         //if window is the same size, don't do anything
         if (w == mWidth && h == mHeight) return;

         //update with the new size
         SetRenderingSize(w, h);
      }
   }
}

/** Set the width and height of the rendering area.  Typically this is just the size
 *  of the DeltaWin the GUI is being rendered in.  If AutoResizing is enabled, these values
 *  will be overwritten.  Disable AutoResizing to manually control the rendered area.
 * @see SetAutoResizing()
 * @param width : the width of the rendered area (pixels)
 * @param height : the heigh tof hte rendered area (pixels)
 */
void CEUIDrawable::SetRenderingSize( int width, int height )
{
   mWidth = width;
   mHeight = height;

   CEGUI::Size s(mWidth, mHeight);

   CEGUI::OpenGLRenderer *rend = static_cast<CEGUI::OpenGLRenderer*>(this->GetRenderer());
   rend->setDisplaySize(s);
}

