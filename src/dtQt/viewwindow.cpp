#include <dtQt/viewwindow.h>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>
#include <osgGA/GUIEventAdapter>

#include <dtUtil/log.h>

using namespace dtQt;

//////////////////////////////////////////////////////////////////////////////////
class QtKeyboardMap
{

public:
   QtKeyboardMap()
   {
      mKeyMap[Qt::Key_Escape   ] = osgGA::GUIEventAdapter::KEY_Escape;
      mKeyMap[Qt::Key_Insert   ] = osgGA::GUIEventAdapter::KEY_KP_Insert;
      mKeyMap[Qt::Key_Delete   ] = osgGA::GUIEventAdapter::KEY_Delete;
      mKeyMap[Qt::Key_Home     ] = osgGA::GUIEventAdapter::KEY_Home;
      mKeyMap[Qt::Key_End      ] = osgGA::GUIEventAdapter::KEY_End;
      mKeyMap[Qt::Key_PageUp   ] = osgGA::GUIEventAdapter::KEY_Page_Up;
      mKeyMap[Qt::Key_PageDown ] = osgGA::GUIEventAdapter::KEY_Page_Down;
      mKeyMap[Qt::Key_Enter    ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
      mKeyMap[Qt::Key_Return   ] = osgGA::GUIEventAdapter::KEY_Return;
      mKeyMap[Qt::Key_Left     ] = osgGA::GUIEventAdapter::KEY_Left;
      mKeyMap[Qt::Key_Right    ] = osgGA::GUIEventAdapter::KEY_Right;
      mKeyMap[Qt::Key_Up       ] = osgGA::GUIEventAdapter::KEY_Up;
      mKeyMap[Qt::Key_Down     ] = osgGA::GUIEventAdapter::KEY_Down;
      mKeyMap[Qt::Key_Backspace] = osgGA::GUIEventAdapter::KEY_BackSpace;
      mKeyMap[Qt::Key_Tab      ] = osgGA::GUIEventAdapter::KEY_Tab;
      mKeyMap[Qt::Key_Space    ] = osgGA::GUIEventAdapter::KEY_Space;
      mKeyMap[Qt::Key_Alt      ] = osgGA::GUIEventAdapter::KEY_Alt_L;
      mKeyMap[Qt::Key_Shift    ] = osgGA::GUIEventAdapter::KEY_Shift_L;
      mKeyMap[Qt::Key_Control  ] = osgGA::GUIEventAdapter::KEY_Control_L;

      mKeyMap[Qt::Key_F1 ] = osgGA::GUIEventAdapter::KEY_F1;
      mKeyMap[Qt::Key_F2 ] = osgGA::GUIEventAdapter::KEY_F2;
      mKeyMap[Qt::Key_F3 ] = osgGA::GUIEventAdapter::KEY_F3;
      mKeyMap[Qt::Key_F4 ] = osgGA::GUIEventAdapter::KEY_F4;
      mKeyMap[Qt::Key_F5 ] = osgGA::GUIEventAdapter::KEY_F5;
      mKeyMap[Qt::Key_F6 ] = osgGA::GUIEventAdapter::KEY_F6;
      mKeyMap[Qt::Key_F7 ] = osgGA::GUIEventAdapter::KEY_F7;
      mKeyMap[Qt::Key_F8 ] = osgGA::GUIEventAdapter::KEY_F8;
      mKeyMap[Qt::Key_F9 ] = osgGA::GUIEventAdapter::KEY_F9;
      mKeyMap[Qt::Key_F10] = osgGA::GUIEventAdapter::KEY_F10;
      mKeyMap[Qt::Key_F11] = osgGA::GUIEventAdapter::KEY_F11;
      mKeyMap[Qt::Key_F12] = osgGA::GUIEventAdapter::KEY_F12;
      mKeyMap[Qt::Key_F13] = osgGA::GUIEventAdapter::KEY_F13;
      mKeyMap[Qt::Key_F14] = osgGA::GUIEventAdapter::KEY_F14;
      mKeyMap[Qt::Key_F15] = osgGA::GUIEventAdapter::KEY_F15;
      mKeyMap[Qt::Key_F16] = osgGA::GUIEventAdapter::KEY_F16;
      mKeyMap[Qt::Key_F17] = osgGA::GUIEventAdapter::KEY_F17;
      mKeyMap[Qt::Key_F18] = osgGA::GUIEventAdapter::KEY_F18;
      mKeyMap[Qt::Key_F19] = osgGA::GUIEventAdapter::KEY_F19;
      mKeyMap[Qt::Key_F20] = osgGA::GUIEventAdapter::KEY_F20;

      mKeyMap[Qt::Key_hyphen] = '-';
      mKeyMap[Qt::Key_Equal ] = '=';

      mKeyMap[Qt::Key_division] = osgGA::GUIEventAdapter::KEY_KP_Divide;
      mKeyMap[Qt::Key_multiply] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
      mKeyMap[Qt::Key_Minus   ] = '-';
      mKeyMap[Qt::Key_Plus    ] = '+';

   }

   ~QtKeyboardMap()
   {
   }

   int remapKey(QKeyEvent* event)
   {
      KeyMap::iterator itr = mKeyMap.find(event->key());
      if (itr == mKeyMap.end())
      {
         return int(*(event->text().toAscii().data()));
      }
      else
      {
         return itr->second;
      }
   }

private:
   typedef std::map<unsigned int, int> KeyMap;
   KeyMap mKeyMap;
};

//////////////////////////////////////////////////////////////////////////////////
static QtKeyboardMap STATIC_KEY_MAP;

//////////////////////////////////////////////////////////////////////////////////
ViewWindow::ViewWindow(bool drawOnSeparateThread, QWidget* parent,
   const QGLWidget* shareWidget, Qt::WindowFlags f)
   : QGLWidget(parent, shareWidget, f)
   , mThreadGLContext(NULL)
   , mDrawOnSeparateThread(drawOnSeparateThread)
{
   mTimer.setInterval(0);
   setAutoBufferSwap(!drawOnSeparateThread);

   // This enables us to track mouse movement even when
   // no button is pressed.  The motion models depend
   // on tracking the mouse location to work properly.
   setMouseTracking(true);

   //allow keyboard input to come through this widget (via user click or tab)
   setFocusPolicy(Qt::StrongFocus);

   AddSender(&dtCore::System::GetInstance());
}

//////////////////////////////////////////////////////////////////////////////////
ViewWindow::~ViewWindow()
{
}

//////////////////////////////////////////////////////////////////////////////////
osgViewer::GraphicsWindow& ViewWindow::GetGraphicsWindow()
{
   return *mGraphicsWindow;
}

//////////////////////////////////////////////////////////////////////////////////
const osgViewer::GraphicsWindow& ViewWindow::GetGraphicsWindow() const
{
   return *mGraphicsWindow;
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::initializeGL()
{
   if (!mDrawOnSeparateThread)
   {
      connect(&mTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
      mTimer.start();
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::SetGraphicsWindow(osgViewer::GraphicsWindow& newWindow)
{
   mGraphicsWindow = &newWindow;
   mGraphicsWindow->resized(0,0, width(), height());
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::ThreadedInitializeGL()
{
   connect(&mTimer, SIGNAL(timeout()), this, SLOT(ThreadedUpdateGL()));
   mTimer.start();

   //share the context on the other thread.
   mThreadGLContext = new QGLContext(QGLFormat::defaultFormat(), this);
   if (!mThreadGLContext->create(context()))
   {
      LOG_ERROR("Thread Failed to initialize");
   }

   setContext(mThreadGLContext, context(), false);
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::ThreadedMakeCurrent()
{
   mThreadGLContext->makeCurrent();
}

//////////////////////////////////////////////////////////////////////////
void ViewWindow::OnMessage(MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_FRAME)
   {
      if (doubleBuffer())
      {
         swapBuffers();
      }
      else
      {
         glFlush();
      }
   }
}
//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::ThreadedUpdateGL()
{
   if (mDoResize)
   {
      mDoResize = false;
      resizeGLImpl(width(), height());
   }

   ThreadedMakeCurrent();
   paintGLImpl();
   mThreadGLContext->swapBuffers();
}


//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::glDraw()
{
   if (!mDrawOnSeparateThread)
   {
      if (!isValid())
         return;
      makeCurrent();
      paintGL();
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::paintGL()
{
   if (!mDrawOnSeparateThread)
   {
      paintGLImpl();
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::paintGLImpl()
{
   dtCore::System& system = dtCore::System::GetInstance();

   if (system.IsRunning())
   {
      system.StepWindow();
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::resizeGLImpl(int width, int height)
{
   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->windowResize(0, 0, width, height);
      mGraphicsWindow->resized(0,0,width,height);
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::resizeGL(int width, int height)
{
   if (!mDrawOnSeparateThread)
   {
      resizeGLImpl(width, height);
   }
   else
   {
      ///set message to resize.
      /// this is actually a race condition.  It needs to be locked.
      mDoResize = true;
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::keyPressEvent(QKeyEvent* event)
{
   if (mGraphicsWindow.valid())
   {
      int value = STATIC_KEY_MAP.remapKey(event);
      mGraphicsWindow->getEventQueue()->keyPress(value);
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::keyReleaseEvent(QKeyEvent* event)
{
   if (mGraphicsWindow.valid())
   {
      int value = STATIC_KEY_MAP.remapKey(event);
      mGraphicsWindow->getEventQueue()->keyRelease(value);
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::mousePressEvent(QMouseEvent* event)
{
   int button = 0;
   switch (event->button())
   {
   case(Qt::LeftButton):  button = 1; break;
   case(Qt::MidButton):   button = 2; break;
   case(Qt::RightButton): button = 3; break;
   case(Qt::NoButton):    button = 0; break;
   default:               button = 0; break;
   }
   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::mouseReleaseEvent(QMouseEvent* event)
{
   int button = 0;
   switch (event->button())
   {
   case(Qt::LeftButton):   button = 1; break;
   case(Qt::MidButton):    button = 2; break;
   case(Qt::RightButton):  button = 3; break;
   case(Qt::NoButton):     button = 0; break;
   default:                button = 0; break;
   }

   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::mouseMoveEvent(QMouseEvent* event)
{
   if (mGraphicsWindow.valid() && hasFocus())
   {
      mGraphicsWindow->getEventQueue()->mouseMotion(event->x(), event->y());
   }
}

//////////////////////////////////////////////////////////////////////////////////
void ViewWindow::wheelEvent(QWheelEvent* event)
{
   if (mGraphicsWindow.valid())
   {
      if (event->orientation() == Qt::Horizontal)
      {
         if (event->delta() > 0)
         {
            mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_LEFT);
         }
         else
         {
            mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_RIGHT);
         }
      }
      else
      {
         if (event->delta() > 0)
         {
            mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
         }
         else
         {
            mGraphicsWindow->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////
