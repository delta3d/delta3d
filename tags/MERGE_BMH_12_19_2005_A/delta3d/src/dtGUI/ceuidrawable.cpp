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
*/
CEUIDrawable::CEUIDrawable(int width, int height, dtGUI::BaseScriptModule* sm):
   mUI(0),
   mWidth(width),
   mHeight(height),
   mMouseX(.0f),
   mMouseY(.0f),
   mRenderer(new dtGUI::Renderer(1024, mWidth, mHeight)),
   mScriptModule(sm),
   mProjection(new osg::Projection(osg::Matrix::ortho2D(0,width,0,height))),
   mTransform(new osg::MatrixTransform(osg::Matrix::identity()))
{
   AddSender( System::Instance() );
   
   RegisterInstance(this);

   mHalfWidth = mWidth / 2;
   mHalfHeight = mHeight / 2;

   //TODO find something better than GetInstance(0)
   dtCore::Mouse::GetInstance(0)->AddMouseListener(this); 
   dtCore::DeltaWin::GetInstance(0)->GetKeyboard()->AddKeyboardListener(this);

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

CEUIDrawable::~CEUIDrawable(void)
{
   RemoveSender( System::Instance() );
   mNode = NULL;
   delete mUI;
   delete mRenderer;
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
}

void CEUIDrawable::MouseDragged(Mouse* mouse, float x, float y)
{
   if (!mUI) return;
     
   mMouseX = x - mMouseX;
   mMouseY = y - mMouseY;
   CEGUI::System::getSingleton().injectMouseMove(mMouseX * mHalfWidth, mMouseY * -mHalfHeight);
   mMouseX = x;
   mMouseY = y;
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
   
   default: CEGUI::System::getSingleton().injectChar(character); break;
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
