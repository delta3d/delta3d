#ifndef DELTA_CEUIDRAWABLE
#define DELTA_CEUIDRAWABLE

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#pragma comment(lib, "glu32.lib")
#pragma once
#endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#include <dtGUI/renderer.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>

#include <CEGUI/CEGUISystem.h>

#include <osg/Drawable>
#include <osg/group>
#include <osg/copyop>

namespace dtGUI
{

   ///ties the OSG GUI node to a dtCore Drawable

   /** The UIDrawable class allows you to associate a glGUI User Interface with
   * a dtCore::Drawable so it can be added to the dtCore::Scene.
   * To use, just create an instance of it, then add in the UI elements using
   * AddFrame(), AddShader(), AddFont(), AddBorder(), etc.
   * Then add the UIDrawable to the Scene for it to be rendered.
   * 
   * Default Shaders will be created and used if the UI elements are added 
   * without a Shader assigned to it.
   *
   * This class will read Keyboard and Mouse events and pass them on to the
   * CUI_UI for processing.
   *
   */
   class DT_EXPORT CEUIDrawable : public dtCore::DeltaDrawable,
      public dtCore::MouseListener,
      public dtCore::KeyboardListener
   {

      ///private class that ties the GUI rendering to an OSG node

      /** This is a private class that is used by the UIDrawable class.  To use,
      * it needs to have a valid CUI_UI passed on the constructor.
      */
      class osgCEUIDrawable : public osg::Drawable
      {
      public:
         osgCEUIDrawable(const osgCEUIDrawable& drawable,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY){}

         osgCEUIDrawable(CEGUI::System *ui)
            :  mUI(ui)
         {
            this->setSupportsDisplayList(false);
            this->setUseDisplayList(false);
         }

         virtual ~osgCEUIDrawable() {delete CEGUI::System::getSingletonPtr();}  //this could cause problems

         virtual Object* cloneType() const { return new osgCEUIDrawable(mUI); }
         virtual Object* clone(const osg::CopyOp& copyop) const { return new osgCEUIDrawable(*this,copyop); }        

         virtual void drawImplementation(osg::State& state) const
         { //tell the UI to update and to render
            if (!mUI) return;
            mUI->getSingletonPtr()->renderGUI();
         }

      private:
         CEGUI::System *mUI; ///< osgCEUIDrawable's pointer to CEGUI
      };

   public:
      DECLARE_MANAGEMENT_LAYER(CEUIDrawable)

      ///Default constructor - accepts the width and height of window
      CEUIDrawable(int width=1, int height=1);

      virtual ~CEUIDrawable();

      ///Override to receive messages
      virtual void OnMessage(MessageData *data);

      ///Get a pointer to the underlying CUI_UI
      CEGUI::System* GetUI(void) {return mUI;}

      ///Get a pointer to the underlying CUI_OpenGLRender
      CEGUI::OpenGLRenderer* GetRenderer(void) const {return mRenderer;}

      virtual osg::Node* GetOSGNode() {return mNode.get();}

      virtual void MouseMoved(dtCore::Mouse* mouse, float x, float y);

      virtual void MouseDragged(dtCore::Mouse* mouse, float x, float y);

      virtual void ButtonPressed(dtCore::Mouse* mouse, dtCore::MouseButton button);

      virtual void ButtonReleased(dtCore::Mouse* mouse, dtCore::MouseButton button);

      virtual void KeyPressed(dtCore::Keyboard* keyboard, 
         Producer::KeyboardKey key,
         Producer::KeyCharacter character);

   private:

      osg::StateSet* stateset;
      osgCEUIDrawable* osgCEUI;
      //osg::ref_ptr<osg::Group> mNode; ///<Contains the node which renders the UI
      CEGUI::System *mUI; ///<Pointer to the CUI_UI
      std::string mActiveRootFrame; ///<The name of the active root frame
      unsigned short mButtonState; ///<The current mouse button state
      int mWidth; ///<the width of the Window
      int mHeight; ///<The height of the Window
      int mHalfWidth; ///<the width of the Window
      int mHalfHeight; ///<The height of the Window
      float mMouseX; ///<The current Mouse X position
      float mMouseY; ///<the current Mouse Y position
      Renderer* mRenderer; ///<The opengl renderer we're using
      double elapsedTime;
   };
}//namespace dtGUI

#endif // DELTA_CEUIDRAWABLE
