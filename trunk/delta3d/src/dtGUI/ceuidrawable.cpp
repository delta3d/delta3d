#include <CEGUI.h>

#include <dtGUI/ceuidrawable.h>
#include <dtGUI/renderer.h>
#include <dtGUI/basescriptmodule.h>
#include <dtCore/deltawin.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/Projection>
#include <osg/MatrixTransform>

using namespace dtCore;
using namespace dtGUI;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(CEUIDrawable)

/** The supplied DeltaWin will automatically be monitored for size change and pass the new
  * size onto the CEGUI Renderer.
  * @param win : The DeltaWin to monitor for size change
  * @param sm : The ScriptModule to use for CEGUI script processing
  */
CEUIDrawable::CEUIDrawable( dtCore::DeltaWin *win, dtGUI::BaseScriptModule *sm):
   DeltaDrawable("CEUIDrawable"),
   mUI(0),
   mRenderer(new dtGUI::Renderer(0)),
   mScriptModule(sm),
   mProjection(new osg::Projection()),
   mTransform(new osg::MatrixTransform(osg::Matrix::identity())),
   mWindow(win),
   mWidth(0),
   mHeight(0),
   mAutoResize(true),
   mKeyboardListener(new CEGUIKeyboardListener()),
   mMouseListener(new CEGUIMouseListener())
{
   // make the listener the first in the list
   dtCore::Mouse* ms = mWindow->GetMouse();
   if( ms->GetListeners().empty() )
   {
      ms->AddMouseListener( mMouseListener.get() );
   }
   else
   {
      ms->InsertMouseListener( ms->GetListeners().front() , mMouseListener.get() );
   }

   dtCore::Keyboard* kb = mWindow->GetKeyboard();
   if( kb->GetListeners().empty() )
   {
      kb->AddKeyboardListener( mKeyboardListener.get() );
   }
   else
   {
      kb->InsertKeyboardListener( kb->GetListeners().front() , mKeyboardListener.get() );
   }

   Config();
}

CEUIDrawable::~CEUIDrawable(void)
{
   RemoveSender( System::Instance() );
   DeregisterInstance(this);

   mWindow->GetMouse()->RemoveMouseListener( mMouseListener.get() );
   mWindow->GetKeyboard()->RemoveKeyboardListener( mKeyboardListener.get() );
   
   mNode = NULL;
   delete mUI;
   delete mRenderer;
}

void CEUIDrawable::Config()
{
   AddSender( System::Instance() );
   
   RegisterInstance(this);

   int x(0), y(0), w(0), h(0);
   mWindow->GetPosition(&x, &y, &w, &h);
   SetRenderingSize(w, h);

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

      ///\todo must we really cast here?
      mUI->injectTimePulse( static_cast<float>(deltaTime) );
   }
}

/** Set the width and height of the rendering area.  Typically this is just the size
 *  of the DeltaWin the GUI is being rendered in.  If AutoResizing is enabled, these values
 *  will be overwritten.  Disable AutoResizing to manually control the rendered area.
 * @see SetAutoResizing()
 * @param width : the width of the rendered area (pixels)
 * @param height : the heigh tof hte rendered area (pixels)
 */
void CEUIDrawable::SetRenderingSize(int width, int height)
{
   mWidth = width;
   mHeight = height;
   mRenderer->setDisplaySize( CEGUI::Size(width, height) );
   mMouseListener->SetWindowSize( width , height );
}

// implementation details for private class
CEUIDrawable::osgCEUIDrawable::osgCEUIDrawable(const CEUIDrawable::osgCEUIDrawable& drawable,const osg::CopyOp& copyop)
{
}

CEUIDrawable::osgCEUIDrawable::osgCEUIDrawable(CEGUI::System *ui) :  mUI(ui)
{
   this->setSupportsDisplayList(false);
   this->setUseDisplayList(false);
}

CEUIDrawable::osgCEUIDrawable::~osgCEUIDrawable() {}

osg::Object* CEUIDrawable::osgCEUIDrawable::cloneType() const { return new osgCEUIDrawable(mUI); }
osg::Object* CEUIDrawable::osgCEUIDrawable::clone(const osg::CopyOp& copyop) const { return new osgCEUIDrawable(*this,copyop); }        

void CEUIDrawable::osgCEUIDrawable::drawImplementation(osg::State& state) const
{ //tell the UI to update and to render
   if (!mUI) return;       
   mUI->getSingletonPtr()->renderGUI();
}
