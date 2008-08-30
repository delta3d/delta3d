// mouse.cpp: Implementation of the Mouse class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/mouse.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>//due to include of scene.h
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h
#include <dtCore/deltawin.h>

#include <dtUtil/log.h>

#include <osgViewer/View>
#ifdef DELTA_WIN32
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#elif defined(__APPLE__)
#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
#else
#include <osgViewer/api/X11/GraphicsWindowX11>
#endif
#include <algorithm>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Mouse)

Mouse::Mouse(const std::string& name) : InputDevice(name)
{
   RegisterInstance(this);

   AddFeature(new Axis(this, "mouse x axis"));
   AddFeature(new Axis(this, "mouse y axis"));
   AddFeature(new Axis(this, "mouse scroll axis"));
   AddFeature(new Button(this, LeftButton, "left mouse button"));
   AddFeature(new Button(this, MiddleButton, "middle mouse button"));
   AddFeature(new Button(this, RightButton, "right mouse button"));
}

Mouse::Mouse(dtCore::View * view, const std::string& name) : InputDevice(name), mView(view)
{
   RegisterInstance(this);

   AddFeature(new Axis(this, "mouse x axis"));
   AddFeature(new Axis(this, "mouse y axis"));
   AddFeature(new Axis(this, "mouse scroll axis"));
   AddFeature(new Button(this, LeftButton, "left mouse button"));
   AddFeature(new Button(this, MiddleButton, "middle mouse button"));
   AddFeature(new Button(this, RightButton, "right mouse button"));
}

Mouse::~Mouse()
{
   DeregisterInstance(this);
}


void Mouse::GetPosition(float& x, float& y) const
{
   x = GetAxis(0)->GetState();
   y = GetAxis(1)->GetState();
}

osg::Vec2 Mouse::GetPosition() const
{
   osg::Vec2 xy;
   GetPosition(xy.x(), xy.y());
   return xy;
}

void Mouse::SetPosition(float x, float y)
{
   if( mView.valid() && 
      mView->GetOsgViewerView() &&
      mView->GetOsgViewerView()->getCamera() &&
      mView->GetOsgViewerView()->getCamera()->getGraphicsContext() &&
      mView->GetOsgViewerView()->getCamera()->getGraphicsContext()->getTraits())
   {
      const float w2 = mView->GetOsgViewerView()->getCamera()->getGraphicsContext()->getTraits()->width/2.f;
      const float h2 = mView->GetOsgViewerView()->getCamera()->getGraphicsContext()->getTraits()->height/2.f;

      //we're converting from (-1..1) to (0..width) and (0..height)
      mView->GetOsgViewerView()->requestWarpPointer((x*w2) + w2, (y*h2) + h2);
   }
   else
   {
      LOG_ERROR("Can not set the Mouse position: missing View, osg::View, osg::Camera, or graphics context");
   }
}

void Mouse::SetPosition(const osg::Vec2& xy)
{
   SetPosition(xy.x(), xy.y());
}

bool Mouse::GetButtonState(MouseButton button) const
{
   return GetButton(button)->GetState();
}

void Mouse::AddMouseListener(MouseListener* mouseListener)
{
   mMouseListeners.push_back(mouseListener);
}

void Mouse::InsertMouseListener(const MouseListenerList::value_type& pos, MouseListener* ml)
{
   MouseListenerList::iterator iter = std::find( mMouseListeners.begin() , mMouseListeners.end() , pos );
   mMouseListeners.insert(iter,ml);
}

void Mouse::RemoveMouseListener(MouseListener* mouseListener)
{
   mMouseListeners.remove(mouseListener);
}

bool Mouse::MouseScroll( osgGA::GUIEventAdapter::ScrollingMotion sm )
{
   int delta(0);

   switch(sm)
   {
      case osgGA::GUIEventAdapter::SCROLL_UP:
          delta = +1;
          break;
      case osgGA::GUIEventAdapter::SCROLL_DOWN:
          delta = -1;
          break;
      case osgGA::GUIEventAdapter::SCROLL_NONE:
      case osgGA::GUIEventAdapter::SCROLL_LEFT:
      case osgGA::GUIEventAdapter::SCROLL_RIGHT:
      case osgGA::GUIEventAdapter::SCROLL_2D:
      default:
          delta = 0;
          break;
   }

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseScrolled(this, delta);
      ++iter;
   }

   // a workaround for classes that listen to input device features
   dtCore::RefPtr<Axis> scrollAxis = GetAxis(2);
   if( !handled )
   {
      handled = scrollAxis->SetState( scrollAxis->GetState() + delta, delta );
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      scrollAxis->SetState( scrollAxis->GetState() + delta, delta );
   }

   return handled;
}

bool Mouse::MouseMotion(float x, float y)
{
   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseDragged(this, x, y);
      ++iter;
   }

   // a workaround for classes that listen to input device features
   Axis* zero = GetAxis(0);
   Axis* one = GetAxis(1);
   if( !handled )
   {
      bool zero_handled = zero->SetState(x, x - zero->GetState());
      bool one_handled = one->SetState(y, y - one->GetState());
      handled = (one_handled || zero_handled);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      zero->SetState(x, x - zero->GetState());
      one->SetState(y, y - one->GetState());
   }

   return handled;
}

bool Mouse::PassiveMouseMotion(float x, float y)
{
   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleMouseMoved(this, x, y);
      ++iter;
   }

   // a workaround for classes that listen to input device features
   Axis* zero = GetAxis(0);
   Axis* one = GetAxis(1);
   if( !handled )  // affect the return value
   {
      bool zero_handled = zero->SetState(x, x - zero->GetState());
      bool one_handled = one->SetState(y, y - one->GetState());
      handled = (one_handled || zero_handled);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      zero->SetState(x, x - zero->GetState());
      one->SetState(y, y - one->GetState());
   }

   return handled;
}

bool Mouse::ButtonDown(float x, float y, MouseButton button)
{
   if (int(button) > GetButtonCount() ) return false;

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonPressed(this, button);
      ++iter;
   }

   // a workaround for classes that listen to input device features
   if( !handled )  // affect the return value
   {
      handled = GetButton(button)->SetState(true);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(button)->SetState(true);
   }

   return handled;
}

bool Mouse::DoubleButtonDown(float x, float y, MouseButton button)
{
   if (int(button) > GetButtonCount() ) return false;

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonClicked(this, button, 2);
      ++iter;
   }

   if( !handled )  // affect the return value
   {
      handled = GetButton(button)->SetState(true);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(button)->SetState(true);
   }

   return handled;
}

bool Mouse::ButtonUp(float x, float y, MouseButton button)
{
   if (int(button) > GetButtonCount() ) return false;

   bool handled(false);
   MouseListenerList::iterator iter = mMouseListeners.begin();
   MouseListenerList::iterator enditer = mMouseListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleButtonReleased(this, button);
      ++iter;
   }

   if( !handled )   // affect the return value
   {
      handled = GetButton(button)->SetState(false);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(button)->SetState(false);
   }

   return handled;
}

bool Mouse::GetHasFocus()
{
   DeltaWin *win = mView->GetCamera()->GetWindow();
#if defined(__APPLE__)
   
   osgViewer::GraphicsWindowCarbon *carbon = 
      dynamic_cast<osgViewer::GraphicsWindowCarbon*>(win->GetOsgViewerGraphicsWindow());
   if (carbon != NULL)
      return IsWindowActive(carbon->getNativeWindowRef());
   
#elif defined(DELTA_WIN32)
   
   osgViewer::GraphicsWindowWin32 *win32 = 
      dynamic_cast<osgViewer::GraphicsWindowWin32*>(win->GetOsgViewerGraphicsWindow());
   if(win32 != NULL)
   {
      return win32->getHWND() == GetForegroundWindow();
   }
   
#else
   
   osgViewer::GraphicsWindowX11 *x11 = 
      dynamic_cast<osgViewer::GraphicsWindowX11*>(win->GetOsgViewerGraphicsWindow());
   if (x11 != NULL)
   {
      Display* display = x11->getDisplay();
      Window windowId = 0;
      int focusType = 0;
      XGetInputFocus(display, &windowId, &focusType);
      return x11->getWindow() == windowId;
   }
#endif
   LOG_DEBUG("The GraphicsWindow is not an known type, unable to tell if the window has mouse focus");
   return true;
}
