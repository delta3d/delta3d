#include <dtGUI/ceuidrawable.h>
#include <dtCore/deltawin.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>

#include <osg/geode>
#include <osg/Projection>
#include <osg/matrixtransform>

using namespace dtCore;
using namespace dtGUI;

IMPLEMENT_MANAGEMENT_LAYER(CEUIDrawable)

/** The constructor.  Supply the width and height
* of the parent Window.  The constructor will create a new CEUI and 
* OpenGLRenderer,and create the OSG nodes.
*/
CEUIDrawable::CEUIDrawable(int width, int height):
mUI(NULL),
mButtonState(0),
mWidth(width),
mHeight(height),
mRenderer(NULL),
mMouseX(.0f),
mMouseY(.0f),
elapsedTime(0.0)
{
   RegisterInstance(this);

   mHalfWidth = .5f * mWidth;
   mHalfHeight = .5f * mHeight;

   //TODO find something better than GetInstance(0)
   dtCore::Mouse::GetInstance(0)->AddMouseListener(this); 
   dtCore::DeltaWin::GetInstance(0)->GetKeyboard()->AddKeyboardListener(this);

   AddSender( System::GetSystem() );

   mRenderer = new dtGUI::Renderer(1024, mWidth, mHeight);
   new CEGUI::System(mRenderer);
   mUI = CEGUI::System::getSingletonPtr();

   mNode = new osg::Group();
   osg::Geode *geod = new osg::Geode();

   osg::StateSet* stateset = geod->getOrCreateStateSet();
   stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

   stateset->setRenderBinDetails(11,"RenderBin");
   stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
   //stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   geod->setStateSet(stateset);

   osgCEUI = new osgCEUIDrawable(mUI);
   geod->addDrawable(osgCEUI); //add our osg node here

   osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
   modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   modelview_abs->setMatrix(osg::Matrix::identity());
   modelview_abs->addChild(geod);

   osg::Projection* projection = new osg::Projection();
   projection->setMatrix(osg::Matrix::ortho2D(0,mWidth,0,mHeight)); //fix this
   projection->addChild(modelview_abs);

   dynamic_cast<osg::Group*>(mNode.get())->addChild(projection);
}

CEUIDrawable::~CEUIDrawable(void)
{
   mNode = NULL;
}

void CEUIDrawable::OnMessage(MessageData *data)
{
}

void CEUIDrawable::MouseMoved(Mouse* mouse, float x, float y)
{
   if (!mUI)
      return;
   mMouseX = x - mMouseX;
   mMouseY = y - mMouseY;
   CEGUI::System::getSingleton().injectMouseMove(mMouseX * mHalfWidth, mMouseY * -mHalfHeight);
   mMouseX = x;
   mMouseY = y;
}

void CEUIDrawable::MouseDragged(Mouse* mouse, float x, float y)
{
   if (!mUI)
      return;
   mMouseX = x - mMouseX;
   mMouseY = y - mMouseY;
   CEGUI::System::getSingleton().injectMouseMove(mMouseX * mHalfWidth, mMouseY * -mHalfHeight);
   mMouseX = x;
   mMouseY = y;
}

void CEUIDrawable::ButtonPressed(Mouse* mouse, MouseButton button)
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

void CEUIDrawable::ButtonReleased(Mouse* mouse, MouseButton button) 
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
   
   default: CEGUI::System::getSingleton().injectChar(character); break;
   }

   
}
