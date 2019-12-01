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

#include <dtCore/deltadrawable.h>           // for base class
#include <dtCore/refptr.h>                  // for members
#include <dtGUI/export.h>                   // for export symbols
#include <dtUtil/deprecationmgr.h>
#include <osg/Drawable>                     // for base class
#include <osg/CopyOp>
#include <osg/Node>                         //for member

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace CEGUI
{
   class System;
   class Window;
}
/// @endcond

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class MatrixTransform;
   class Projection;
}
/// @endcond

namespace dtCore
{
   class DeltaWin;
   class Keyboard;
   class Mouse;
}

namespace dtGUI
{
   class BaseScriptModule;
   class ScriptModule;
   class CEGUIRenderer;
   class CEGUIKeyboardListener;
   class CEGUIMouseListener;

   ///A DeltaDrawable used to render CEGUI

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
    * @note CEUIDrawable has been deprecated in favor of dtGUI::GUI
    * 
    */
   class DT_GUI_EXPORT CEUIDrawable : public dtCore::DeltaDrawable
   {
   public:
      DECLARE_MANAGEMENT_LAYER(CEUIDrawable)

      ///deprecated 2/12/10
      DEPRECATE_FUNC CEUIDrawable(dtCore::DeltaWin* win,
                  dtCore::Keyboard* keyboard,
                  dtCore::Mouse* mouse,
                  dtGUI::BaseScriptModule* sm = 0);

      ///deprecated 2/12/10
      DEPRECATE_FUNC CEUIDrawable(dtCore::DeltaWin* win,
                                  dtCore::Keyboard* keyboard,
                                  dtCore::Mouse* mouse,
                                  dtGUI::ScriptModule* sm);

   protected:
      virtual ~CEUIDrawable();

   public:
      ///Get a pointer to the underlying CEGUI::System
      CEGUI::System* GetUI() { return mUI; }

      ///Get a pointer to the underlying Renderer
      dtGUI::CEGUIRenderer* GetRenderer() { return mRenderer; }
      const dtGUI::CEGUIRenderer* GetRenderer() const { return mRenderer; }

      /// Attaches the Delta3D child's OSG graphics Node
      bool AddChild(dtCore::DeltaDrawable* child);

      ///Display all the properties of the supplied CEGUI::Window
      static void DisplayProperties(CEGUI::Window* window, bool onlyNonDefault = true);

      /// Not usually needed, but this getter is provided for unusual scenarios.
      osg::Projection* GetProjectionNode() { return mProjection.get(); }

      /// Not usually needed, but this getter is provided for unusual scenarios.
      osg::MatrixTransform* GetTransformNode() { return mTransform.get(); }

      /// Manually set the size of the rendering area (in pixels).
      /// Set the width and height of the rendering area.  Typically this is just the size
      /// of the DeltaWin the GUI is being rendered in.  If AutoResizing is enabled, these values
      /// will be overwritten.  Disable AutoResizing to manually control the rendered area.
      /// @see SetAutoResizing()
      /// @param width : the width of the rendered area (pixels)
      /// @param height : the heigh tof hte rendered area (pixels)
      void SetRenderingSize(int width, int height);

      /// Automatically notify CEGUI of DeltaWin resizes (enabled by default)
      void SetAutoResizing(bool enable = true) { mAutoResize = enable; }

      bool GetAutoResizing() const { return mAutoResize; }

      const CEGUIMouseListener* GetMouseListener() const { return mMouseListener.get(); }
      const CEGUIKeyboardListener* GetKeyboardListener() const { return mKeyboardListener.get(); }

      /// Turns off the GUI System, no longer yielding GUI support.
      /// The System will do its clean up at this time.
      void ShutdownGUI();

      ///required by DeltaDrawable
      osg::Node* GetOSGNode() { return mNode.get(); }
      const osg::Node* GetOSGNode() const { return mNode.get(); }

      void SetOSGNode(osg::Node* pNode) { mNode = pNode; }

      /**
       * Set the render bin details on the contained geode's state set.
       * @param binNumber Order of the bin in relation to other bins.
       * @param binName Name of the bin.
       */
      void SetRenderBinDetails(int binNumber, const std::string& binName);


   protected: 

      void OnSystem(const dtUtil::RefString& str, double, double)
;
      
      CEGUI::System* mUI; ///<Pointer to the CUI_UI

      CEGUIRenderer* mRenderer; ///<The opengl renderer we're using
      dtGUI::ScriptModule* mScriptModule;
      dtGUI::BaseScriptModule* mBaseScriptModule;
      dtCore::RefPtr<osg::Projection> mProjection;
      dtCore::RefPtr<osg::MatrixTransform> mTransform;
      dtCore::RefPtr<dtCore::DeltaWin> mWindow; ///<The window this UI is being rendered in
      dtCore::RefPtr<dtCore::Mouse> mMouse;
      dtCore::RefPtr<dtCore::Keyboard> mKeyboard;
      dtCore::RefPtr<osg::Node> mNode;
      dtCore::RefPtr<osg::Geode> mGeode;
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
         osgCEUIDrawable(CEGUI::System* ui);

         virtual osg::Object* cloneType() const;
         virtual osg::Object* clone(const osg::CopyOp& copyop) const;
         virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

      protected:
         virtual ~osgCEUIDrawable();

      private:
         CEGUI::System* mUI; ///< osgCEUIDrawable's pointer to CEGUI
      };

      ///setup the internals
      void Config();

      bool mAutoResize; ///<Automatically tell CEGUI about DeltaWin size changes?

      dtCore::RefPtr<CEGUIKeyboardListener> mKeyboardListener;
      dtCore::RefPtr<CEGUIMouseListener> mMouseListener;
   };
} // namespace dtGUI

#endif // DELTA_CEUIDRAWABLE
