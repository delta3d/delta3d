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
#ifndef DELTA_CEUIDRAWABLE
#define DELTA_CEUIDRAWABLE

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#pragma comment(lib, "glu32.lib")
#pragma once
#endif // defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

#include <dtCore/deltadrawable.h>           // for base class
#include <dtGUI/ceguimouselistener.h>       // for member
#include <dtGUI/ceguikeyboardlistener.h>    // for member
#include <dtCore/refptr.h>                  // for members
#include <dtGUI/export.h>                   // for export symbols

#include <osg/Drawable>                     // for base class
#include <osg/CopyOp>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace CEGUI
{
   class System;
   class Renderer;
}

namespace osg
{
   class MatrixTransform;
   class Projection;
}
/// @endcond

namespace dtGUI
{
   class BaseScriptModule;
   class Renderer;

   /** 
    * A DeltaDrawable used to render the CEGUI.
    * This class is a derivative of DeltaDrawable and is used to render and 
    * manage the CEGUI system.  The CEUIDrawable is responsible for setting
    * up the CEGUI system and supplying mouse and keyboard events to the UI.
    *
    * To create a new GUI, instantiate a CEGUIDrawable and add it to the Scene
    * using Scene::AddDrawable().  You can then use the CEGUI API to create
    * CEGUI::Windows and adjust their properties.
    *
    * NOTE: The CEGUIDrawable class must be instantiated *after* the application
    * has created a valid OpenGL context (i.e., during dtABC::Application::Config()).
    */
   class DT_GUI_EXPORT CEUIDrawable : public dtCore::DeltaDrawable
   {
   public:
      DECLARE_MANAGEMENT_LAYER(CEUIDrawable)

      ///Overloaded constructor, will automatically update CEGUI when the supplied Window is resized
      CEUIDrawable(dtCore::DeltaWin *win, dtGUI::BaseScriptModule *sm=0);
   protected:
      virtual ~CEUIDrawable();
   public:
      ///Get a pointer to the underlying CEGUI::System
      CEGUI::System* GetUI() {return mUI;}

      ///Get a pointer to the underlying Renderer
      dtGUI::Renderer* GetRenderer() {return mRenderer;}
      const dtGUI::Renderer* GetRenderer() const {return mRenderer;}

      /// Attaches the Delta3D child's OSG graphics Node
      bool AddChild(dtCore::DeltaDrawable *child);

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

      const CEGUIMouseListener* GetMouseListener() const { return mMouseListener.get(); }
      const CEGUIKeyboardListener* GetKeyboardListener() const { return mKeyboardListener.get(); }

   protected: 

      void OnMessage(dtCore::Base::MessageData *data);
      
      /**
       * Determines the CEGUI scancode that corresponds to the specified Producer::KeyboardKey.
       *
       * @param key the key to map
       * @return the corresponding CEGUI key scancode
       */
      static CEGUI::Key::Scan KeyboardKeyToKeyScan( Producer::KeyboardKey key );
      
      CEGUI::System *mUI; ///<Pointer to the CUI_UI

      Renderer* mRenderer; ///<The opengl renderer we're using
      dtGUI::BaseScriptModule* mScriptModule;
      dtCore::RefPtr<osg::Projection> mProjection;
      dtCore::RefPtr<osg::MatrixTransform> mTransform;
      dtCore::RefPtr<dtCore::DeltaWin> mWindow; ///<The window this UI is being rendered in
      int mWidth;
      int mHeight;

   private:
      /** Default constructor
        * @param width is the width of the window, only relevant upon window realization.
        * @param height is the width of the window, only relevant upon window realization.
        * @param sm is a derivation of BaseScriptModule, provide an instance of this to handle CEGUI::Event triggered by CEGUI::Windows.
        */
      //CEUIDrawable(int width=1024, int height=768, dtGUI::BaseScriptModule* sm=0);
      CEUIDrawable();
      CEUIDrawable(int);
      CEUIDrawable(int,int);
      CEUIDrawable(int,int,dtGUI::BaseScriptModule*);

      /// private class that ties the GUI rendering to an OSG node
      /// This is a private class that is used by the UIDrawable class.
      /// To use, it needs to have a valid CUI_UI passed on the constructor.
      class osgCEUIDrawable : public osg::Drawable
      {
      public:
         osgCEUIDrawable(const osgCEUIDrawable& drawable,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
         osgCEUIDrawable(CEGUI::System *ui);

         virtual osg::Object* cloneType() const;
         virtual osg::Object* clone(const osg::CopyOp& copyop) const;
         virtual void drawImplementation(osg::State& state) const;

      protected:
         virtual ~osgCEUIDrawable();

      private:
         CEGUI::System *mUI; ///< osgCEUIDrawable's pointer to CEGUI
      };

      ///setup the internals
      void Config();

      bool mAutoResize; ///<Automatically tell CEGUI about DeltaWin size changes?

      dtCore::RefPtr<CEGUIKeyboardListener> mKeyboardListener;
      dtCore::RefPtr<CEGUIMouseListener> mMouseListener;
   };
}//namespace dtGUI

#endif // DELTA_CEUIDRAWABLE
