/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 */
#include <CEGUI/CEGUIPropertySet.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIVersion.h>
//#if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
//#   include <CEGUI/XMLParserModules/XercesParser/CEGUIXercesParser.h>
//#endif

#include <dtGUI/ceuidrawable.h>
#include <dtGUI/ceguimouselistener.h>       // for member
#include <dtGUI/ceguikeyboardlistener.h>    // for member
#include <dtGUI/renderer.h>
#include <dtGUI/basescriptmodule.h>
#include <dtGUI/guiexceptionenum.h>
#include <dtCore/deltawin.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/State>


using namespace dtCore;
using namespace dtGUI;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(CEUIDrawable)


/** The supplied DeltaWin will automatically be monitored for size change and pass the new
  * size onto the CEGUI Renderer.
  * @param win : The DeltaWin to monitor for size change
  * @param sm : The ScriptModule to use for CEGUI script processing
  * @exception dtUtil::Exception Gets thrown if CEGUI cannot be initialized
  */
CEUIDrawable::CEUIDrawable( dtCore::DeltaWin *win, dtGUI::BaseScriptModule *sm):
   DeltaDrawable("CEUIDrawable"),
   mUI(NULL),
   mRenderer(new dtGUI::Renderer()),
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
   AddSender( &dtCore::System::GetInstance() );

   RegisterInstance(this);

   mProjection->setName("CEUIDrawable_Projection");
   mTransform->setName("CEUIDrawable_MatrixTransform");

   Config();
}

CEUIDrawable::~CEUIDrawable()
{
   RemoveSender( &dtCore::System::GetInstance() );
   DeregisterInstance(this);
   
   SetOSGNode(NULL);

   delete mRenderer;

   //ShutdownGUI();
}

/** 
*  \exception dtUtil::Exception Gets thrown if CEGUI cannot be initialized
*/
void CEUIDrawable::Config()
{
   if(CEGUI::System::getSingletonPtr() == NULL)
   {
      try
      {
         if(mScriptModule)
         {
            #if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
            // CEGUI 0.5.0 introduces a "unified" constructor. 
            // The new 0 here is for using the default ResourceProvider as well as the default XML parser.
               new CEGUI::System(mRenderer, NULL, NULL, mScriptModule);          
            #else
               new CEGUI::System(mRenderer,mScriptModule);
            #endif // CEGUI 0.5.0
         }
         else
         {
            new CEGUI::System(mRenderer);
         }
      }
      catch(CEGUI::Exception &e)
      {
         dtUtil::Log::GetInstance().LogMessage(Log::LOG_ERROR, __FUNCTION__,
            "CEGUI says: %s", e.getMessage().c_str());

         throw dtUtil::Exception(ExceptionEnum::GenericCEGUIException, 
            "Can't initialize dtGUI system. UI operations will fail!", __FILE__, __LINE__);
      }
   }

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


   int x(0), y(0), w(0), h(0);
   mWindow->GetPosition(x, y, w, h);
   SetRenderingSize(w, h);

   mUI = CEGUI::System::getSingletonPtr();

   osg::Geode *geod = new osg::Geode();
   geod->setName("CEUIDrawable_Geode");

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

   SetOSGNode( mProjection.get() );
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

   #if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
   CEGUI::PropertySet::Iterator itr = window->getPropertyIterator();
   #else
   CEGUI::PropertySet::PropertyIterator itr = static_cast<CEGUI::PropertySet*>(window)->getIterator();
   #endif // CEGUI 0.5.0
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
         mWindow->GetPosition(x, y, w, h);

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

void CEUIDrawable::SetRenderingSize(int width, int height)
{
   try 
   {
      mWidth = width;
      mHeight = height;
      mRenderer->setDisplaySize( CEGUI::Size(width, height) );
      mMouseListener->SetWindowSize( width , height );
   }
   catch (const CEGUI::Exception& ex)
   {
      const std::string error = std::string("Problem setting the CEGUI Rendering size:") + ex.getMessage().c_str();
      throw dtUtil::Exception(error, __FILE__, __LINE__);
   }
}

void CEUIDrawable::ShutdownGUI()
{
   mWindow->GetMouse()->RemoveMouseListener( mMouseListener.get() );
   mWindow->GetKeyboard()->RemoveKeyboardListener( mKeyboardListener.get() );

   delete mUI;
   mUI = NULL;
}

// implementation details for private class
CEUIDrawable::osgCEUIDrawable::osgCEUIDrawable(const CEUIDrawable::osgCEUIDrawable& drawable,
                                               const osg::CopyOp& copyop) : mUI(NULL)
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
{
   //tell the UI to update and to render
   if(!mUI) 
      return;
 
   //we must disable the client active texture unit because it may have been used and not disabled
   //this will cause our GUI to disappear
   state.setClientActiveTextureUnit(0);
   glDisable(GL_TEXTURE_2D);

   state.setActiveTextureUnit(0);
   glEnable(GL_TEXTURE_2D);

   mUI->getSingletonPtr()->renderGUI();
}
