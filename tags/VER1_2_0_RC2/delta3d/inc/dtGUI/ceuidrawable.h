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
#include <osg/Group>
#include <osg/CopyOp>

namespace dtGUI
{
   class BaseScriptModule;

   ///A DeltaDrawable used to render the CEGUI

   /** This class is a derivative of DeltaDrawable and is used to render and 
    *  manage the CEGUI system.  The CEUIDrawable is responsible for setting
    *  up the CEGUI system and supplying mouse and keyboard events to the UI.
    *
    *  To create a new GUI, instantiate a CEGUIDrawable and add it to the Scene
    *  using Scene::AddDrawable().  You can then use the CEGUI API to create
    *  CEGUI::Windows and adjust their properties.
    *
    *  NOTE: The CEGUIDrawable class must be instantiated *after* the application
    *  has created a valid OpenGL context (i.e., during dtABC::Application::Config()).
    * 
    */
   class DT_GUI_EXPORT CEUIDrawable : public dtCore::DeltaDrawable,
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

         virtual ~osgCEUIDrawable() {}

         virtual Object* cloneType() const { return new osgCEUIDrawable(mUI); }
         virtual Object* clone(const osg::CopyOp& copyop) const { return new osgCEUIDrawable(*this,copyop); }        

         virtual void drawImplementation(osg::State& state) const
         { //tell the UI to update and to render
            if (!mUI) return;
            state.setActiveTextureUnit(0); //makes it work with other multitexture Objects
            mUI->getSingletonPtr()->renderGUI();
         }

      private:
         CEGUI::System *mUI; ///< osgCEUIDrawable's pointer to CEGUI
      };

   public:
      DECLARE_MANAGEMENT_LAYER(CEUIDrawable)

      /** Default constructor
        * @param width is the width of the window, only relevant upon window realization.
        * @param height is the width of the window, only relevant upon window realization.
        * @param sm is a derivation of BaseScriptModule, provide an instance of this to handle CEGUI::Event triggered by CEGUI::Windows.
        */
      CEUIDrawable(int width=1024, int height=768, dtGUI::BaseScriptModule* sm=0);

      ///Overloaded constructor, will automatically update CEGUI when the supplied Window is resized
      CEUIDrawable(dtCore::DeltaWin *win, dtGUI::BaseScriptModule *sm=0);

      virtual ~CEUIDrawable();

      ///Get a pointer to the underlying CEGUI::System
      CEGUI::System* GetUI(void) {return mUI;}

      ///Get a pointer to the underlying CEGUI::Renderer
      CEGUI::Renderer* GetRenderer(void) const {return mRenderer;}

      /// Attaches the Delta3D child's OSG graphics Node
      bool AddChild(DeltaDrawable *child);

      ///Display all the properties of the supplied CEGUI::Window
      static void DisplayProperties(CEGUI::Window *window, bool onlyNonDefault=true);

      /// Not usually needed, but this getter is provided for unusual scenarios.
      osg::Projection* GetProjectionNode() { return mProjection.get(); }

      /// Not usually needed, but this getter is provided for unusual scenarios.
      osg::MatrixTransform* GetTransformNode() { return mTransform.get(); }

      /// Manually set the size of the rendering area (in pixels)
      void SetRenderingSize( int width, int height );

      /// Automatically notify CEGUI of DeltaWin resizes (enabled by default)
      void SetAutoResizing(bool enable=true) {mAutoResize=enable;}
      
      bool GetAutoResizing() const {return mAutoResize;}


   protected: 

      void OnMessage(dtCore::Base::MessageData *data);

      ///pass the mouse moved events to CEGUI
      virtual void MouseMoved(dtCore::Mouse* mouse, float x, float y);

      ///pass the mouse dragged events to CEGUI
      virtual void MouseDragged(dtCore::Mouse* mouse, float x, float y);

      ///pass the button pressed events to CEGUI
      virtual void ButtonPressed(dtCore::Mouse* mouse, dtCore::MouseButton button);

      ///pass the button released events to CEGUI
      virtual void ButtonReleased(dtCore::Mouse* mouse, dtCore::MouseButton button);

      ///pass the key pressed events to CEGUI
      virtual void KeyPressed(dtCore::Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character);

      virtual void MouseScrolled(dtCore::Mouse* mouse, int change);
      
      /**
       * Determines the CEGUI scancode that corresponds to the specified Producer::KeyboardKey.
       *
       * @param key the key to map
       * @return the corresponding CEGUI key scancode
       */
      static CEGUI::Key::Scan KeyboardKeyToKeyScan( Producer::KeyboardKey key );
      
      CEGUI::System *mUI; ///<Pointer to the CUI_UI
      int mWidth; ///<the width of the Window
      int mHeight; ///<The height of the Window
      int mHalfWidth; ///<the width of the Window
      int mHalfHeight; ///<The height of the Window
      float mMouseX; ///<The current Mouse X position
      float mMouseY; ///<the current Mouse Y position

      Renderer* mRenderer; ///<The opengl renderer we're using
      dtGUI::BaseScriptModule* mScriptModule;
      osg::ref_ptr<osg::Projection> mProjection;
      osg::ref_ptr<osg::MatrixTransform> mTransform;
      dtCore::RefPtr<dtCore::DeltaWin> mWindow; ///<The window this UI is being rendered in

   private:
      ///setup the internals
      void Config();

      bool mAutoResize; ///<Automatically tell CEGUI about DeltaWin size changes?
   };
}//namespace dtGUI

#endif // DELTA_CEUIDRAWABLE
