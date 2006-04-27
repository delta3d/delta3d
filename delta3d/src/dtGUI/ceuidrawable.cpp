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
   DeltaDrawable("CEUIDrawable"),
   mUI(0),
   mWidth(width),
   mHeight(height),
   mMouseX(.0f),
   mMouseY(.0f),
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
   DeltaDrawable("CEUIDrawable"),
   mUI(0),
   mWidth(1024),
   mHeight(1280),
   mMouseX(.0f),
   mMouseY(.0f),
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
   DeregisterInstance(this);

   mWindow->GetMouse()->RemoveMouseListener(this); 
   mWindow->GetKeyboard()->RemoveKeyboardListener(this);
   
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

   stateset->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);

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

   mMouseX = x - mMouseX;
   mMouseY = y - mMouseY;
   CEGUI::System::getSingleton().injectMouseMove(mMouseX * mHalfWidth, mMouseY * -mHalfHeight);
   mMouseX = x;
   mMouseY = y;

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
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      CEGUI::System::getSingleton().injectKeyDown(scanKey);
   }

   CEGUI::System::getSingleton().injectChar( static_cast<CEGUI::utf32>(character) );   
}

void CEUIDrawable::KeyReleased(Keyboard* keyboard, 
                               Producer::KeyboardKey key,
                               Producer::KeyCharacter character)
{
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      CEGUI::System::getSingleton().injectKeyDown(scanKey);
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

      const double deltaTime = *static_cast<const double*>(data->userData);
      GetUI()->injectTimePulse( static_cast<float>(deltaTime) );
   }
}

/**
 * Determines the CEGUI scancode that corresponds to the specified Producer::KeyboardKey.
 *
 * @param key the key to map
 * @return the corresponding CEGUI key scancode
 */
CEGUI::Key::Scan CEUIDrawable::KeyboardKeyToKeyScan( Producer::KeyboardKey key )
{
   switch(key)
   {
      case Producer::Key_Unknown:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_Escape:
      {
         return CEGUI::Key::Escape;
      }
      case Producer::Key_F1:
      {
         return CEGUI::Key::F1;
      }
      case Producer::Key_F2:
      {
         return CEGUI::Key::F2;
      }
      case Producer::Key_F3:
      {
         return CEGUI::Key::F3;
      }
      case Producer::Key_F4:
      {
         return CEGUI::Key::F4;
      }
      case Producer::Key_F5:
      {
         return CEGUI::Key::F5;
      }
      case Producer::Key_F6:
      {
         return CEGUI::Key::F6;
      }
      case Producer::Key_F7:
      {
         return CEGUI::Key::F7;
      }
      case Producer::Key_F8:
      {
         return CEGUI::Key::F8;
      }
      case Producer::Key_F9:
      {
         return CEGUI::Key::F9;
      }
      case Producer::Key_F10:
      {
         return CEGUI::Key::F10;
      }
      case Producer::Key_F11:
      {
         return CEGUI::Key::F11;
      }
      case Producer::Key_F12:
      {
         return CEGUI::Key::F12;
      }
      case Producer::Key_quoteleft:
      {
         return CEGUI::Key::Apostrophe;
      }
      case Producer::Key_1:
      {
         return CEGUI::Key::One;
      }
      case Producer::Key_2:
      {
         return CEGUI::Key::Two;
      }
      case Producer::Key_3:
      {
         return CEGUI::Key::Three;
      }
      case Producer::Key_4:
      {
         return CEGUI::Key::Four;
      }
      case Producer::Key_5:
      {
         return CEGUI::Key::Five;
      }
      case Producer::Key_6:
      {
         return CEGUI::Key::Six;
      }
      case Producer::Key_7:
      {
         return CEGUI::Key::Seven;
      }
      case Producer::Key_8:
      {
         return CEGUI::Key::Eight;
      }
      case Producer::Key_9:
      {
         return CEGUI::Key::Nine;
      }
      case Producer::Key_0:
      {
         return CEGUI::Key::Zero;
      }
      case Producer::Key_minus:
      {
         return CEGUI::Key::Minus;
      }
      case Producer::Key_equal:
      {
         return CEGUI::Key::Equals;
      }
      case Producer::Key_BackSpace:
      {
         return CEGUI::Key::Backspace;
      }
      case Producer::Key_Tab:
      {
         return CEGUI::Key::Tab;
      }
      case Producer::Key_A:
      {
         return CEGUI::Key::A;
      }
      case Producer::Key_B:
      {
         return CEGUI::Key::B;
      }
      case Producer::Key_C:
      {
         return CEGUI::Key::C;
      }
      case Producer::Key_D:
      {
         return CEGUI::Key::D;
      }
      case Producer::Key_E:
      {
         return CEGUI::Key::E;
      }
      case Producer::Key_F:
      {
         return CEGUI::Key::F;
      }
      case Producer::Key_G:
      {
         return CEGUI::Key::G;
      }
      case Producer::Key_H:
      {
         return CEGUI::Key::H;
      }
      case Producer::Key_I:
      {
         return CEGUI::Key::I;
      }
      case Producer::Key_J:
      {
         return CEGUI::Key::J;
      }
      case Producer::Key_K:
      {
         return CEGUI::Key::K;
      }
      case Producer::Key_L:
      {
         return CEGUI::Key::L;
      }
      case Producer::Key_M:
      {
         return CEGUI::Key::M;
      }
      case Producer::Key_N:
      {
         return CEGUI::Key::N;
      }
      case Producer::Key_O:
      {
         return CEGUI::Key::O;
      }
      case Producer::Key_P:
      {
         return CEGUI::Key::P;
      }
      case Producer::Key_Q:
      {
         return CEGUI::Key::Q;
      }
      case Producer::Key_R:
      {
         return CEGUI::Key::R;
      }
      case Producer::Key_S:
      {
         return CEGUI::Key::S;
      }
      case Producer::Key_T:
      {
         return CEGUI::Key::T;
      }
      case Producer::Key_U:
      {
         return CEGUI::Key::U;
      }
      case Producer::Key_V:
      {
         return CEGUI::Key::V;
      }
      case Producer::Key_W:
      {
         return CEGUI::Key::W;
      }
      case Producer::Key_X:
      {
         return CEGUI::Key::X;
      }
      case Producer::Key_Y:
      {
         return CEGUI::Key::Y;
      }
      case Producer::Key_Z:
      {
         return CEGUI::Key::Z;
      }
      case Producer::Key_bracketleft:
      {
         return CEGUI::Key::LeftBracket;
      }
      case Producer::Key_bracketright:
      {
         return CEGUI::Key::RightBracket;
      }
      case Producer::Key_backslash:
      {
         return CEGUI::Key::Backslash;
      }
      case Producer::Key_Caps_Lock:
      {
         return CEGUI::Key::Capital;
      }
      case Producer::Key_semicolon:
      {
         return CEGUI::Key::Semicolon;
      }
      case Producer::Key_apostrophe:
      {
         return CEGUI::Key::Apostrophe;
      }
      case Producer::Key_Return:
      {
         return CEGUI::Key::Return;
      }
      case Producer::Key_Shift_L:
      {
         return CEGUI::Key::LeftShift;
      }
      case Producer::Key_comma:
      {
         return CEGUI::Key::Comma;
      }
      case Producer::Key_period:
      {
         return CEGUI::Key::Period;
      }
      case Producer::Key_slash:
      {
         return CEGUI::Key::Slash;
      }
      case Producer::Key_Shift_R:
      {
         return CEGUI::Key::RightShift;
      }
      case Producer::Key_Control_L:
      {
         return CEGUI::Key::LeftControl;
      }
      case Producer::Key_Super_L:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_space:
      {
         return CEGUI::Key::Space;
      }
      case Producer::Key_Alt_L:
      {
         return CEGUI::Key::LeftAlt;
      }
      case Producer::Key_Alt_R:
      {
         return CEGUI::Key::RightAlt;
      }
      case Producer::Key_Super_R:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_Menu:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_Control_R:
      {
         return CEGUI::Key::RightControl;
      }
      case Producer::Key_Print:
      {
         return CEGUI::Key::SysRq;
      }
      case Producer::Key_Scroll_Lock:
      {
         return CEGUI::Key::ScrollLock;
      }
      case Producer::Key_Pause:
      {
         return CEGUI::Key::Pause;
      }
      case Producer::Key_Home:
      {
         return CEGUI::Key::Home;
      }
      case Producer::Key_Page_Up:
      {
         return CEGUI::Key::PageUp;
      }
      case Producer::Key_End:
      {
         return CEGUI::Key::End;
      }
      case Producer::Key_Page_Down:
      {
         return CEGUI::Key::PageDown;
      }
      case Producer::Key_Delete:
      {
         return CEGUI::Key::Delete;
      }
      case Producer::Key_Insert:
      {
         return CEGUI::Key::Insert;
      }
      case Producer::Key_Left:
      {
         return CEGUI::Key::ArrowLeft;
      }
      case Producer::Key_Up:
      {
         return CEGUI::Key::ArrowUp;
      }
      case Producer::Key_Right:
      {
         return CEGUI::Key::ArrowRight;
      }
      case Producer::Key_Down:
      {
         return CEGUI::Key::ArrowDown;
      }
      case Producer::Key_Num_Lock:
      {
         return CEGUI::Key::NumLock;
      }
      case Producer::Key_KP_Divide:
      {
         return CEGUI::Key::Divide;
      }
      case Producer::Key_KP_Multiply:
      {
         return CEGUI::Key::Multiply;
      }
      case Producer::Key_KP_Subtract:
      {
         return CEGUI::Key::Subtract;
      }
      case Producer::Key_KP_Add:
      {
         return CEGUI::Key::Add;
      }
      case Producer::Key_KP_Home:
      {
         return CEGUI::Key::Numpad7;
      }
      case Producer::Key_KP_Up:
      {
         return CEGUI::Key::Numpad8;
      }
      case Producer::Key_KP_Page_Up:
      {
         return CEGUI::Key::Numpad9;
      }
      case Producer::Key_KP_Left:
      {
         return CEGUI::Key::Numpad4;
      }
      case Producer::Key_KP_Begin:
      {
         return CEGUI::Key::Scan(0); //?
      }
      case Producer::Key_KP_Right:
      {
         return CEGUI::Key::Numpad6;
      }
      case Producer::Key_KP_End:
      {
         return CEGUI::Key::Numpad1;
      }
      case Producer::Key_KP_Down:
      {
         return CEGUI::Key::Numpad2;
      }
      case Producer::Key_KP_Page_Down:
      {
         return CEGUI::Key::Numpad3;
      }
      case Producer::Key_KP_Insert:
      {
         return CEGUI::Key::Numpad0;
      }
      case Producer::Key_KP_Delete:
      {
         return CEGUI::Key::Decimal;
      }
      case Producer::Key_KP_Enter:
      {
         return CEGUI::Key::NumpadEnter;
      }
      default:
      {
         return CEGUI::Key::Scan(0);
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

   mHalfWidth = mWidth / 2;
   mHalfHeight = mHeight / 2;

   CEGUI::Size s(mWidth, mHeight);

   CEGUI::OpenGLRenderer *rend = static_cast<CEGUI::OpenGLRenderer*>(this->GetRenderer());
   rend->setDisplaySize(s);
}

bool CEUIDrawable::HandleMouseMoved(dtCore::Mouse* mouse, float x, float y)
{
   mMouseX = x - mMouseX;
   mMouseY = y - mMouseY;
   CEGUI::System::getSingleton().injectMouseMove(mMouseX * mHalfWidth, mMouseY * -mHalfHeight);
   mMouseX = x;
   mMouseY = y;

   return CEGUI::System::getSingleton().injectMousePosition( ((x+1)*0.5f)*mWidth, ((-y+1)*0.5f)*mHeight);
}

bool CEUIDrawable::HandleMouseDragged(dtCore::Mouse* mouse, float x, float y)
{
   return HandleMouseMoved(mouse, x, y);
}

bool CEUIDrawable::HandleButtonPressed(Mouse* mouse, MouseButton button)
{
   switch( button )
   {
   case LeftButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
      break;
   case RightButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
      break;
   case MiddleButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
      break;
   }

   return false;
}

bool CEUIDrawable::HandleButtonReleased(Mouse* mouse, MouseButton button)
{
   switch(button)
   {
   case LeftButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
      }  break;

   case RightButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
      }  break;

   case MiddleButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
      }  break;
   }

   return false;
}

bool CEUIDrawable::HandleKeyPressed(dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter kchar)
{
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      CEGUI::System::getSingleton().injectKeyDown(scanKey);
   }

   return CEGUI::System::getSingleton().injectChar( static_cast<CEGUI::utf32>(kchar) );   
}

bool CEUIDrawable::HandleKeyReleased(dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter kchar)
{
   bool handled(false);
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      handled = CEGUI::System::getSingleton().injectKeyUp(scanKey);
   }

   return handled;
}

bool CEUIDrawable::HandleMouseScrolled(dtCore::Mouse* mouse, int delta)
{
   return CEGUI::System::getSingleton().injectMouseWheelChange( (float)delta );
}

