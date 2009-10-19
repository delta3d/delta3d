/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Matthew W. Campbell
 */

#ifndef DELTA_ORTHO_VIEWPORT
#define DELTA_ORTHO_VIEWPORT

#include <dtUtil/enumeration.h>
#include "dtEditQt/editorviewport.h"
#include <dtEditQt/export.h>

namespace dtEditQt
{

   /**
    * The orthographic viewport renders a 2D view of the scene.  The 2D view can be
    * along each of the 3D axis.
    * @see OrthoViewType
    */
   class DT_EDITQT_EXPORT OrthoViewport : public EditorViewport
   {
      Q_OBJECT

   public:
      /**
       * An enumeration of the different types of views into the scene an
       * orthographic viewport can render.
       */
      class OrthoViewType : public dtUtil::Enumeration
      {
         DECLARE_ENUM(OrthoViewType);

      public:
         /**
          * Top or birds eye view.  This renders the scene along the XY plane looking
          * down the -Z axis.
          */
         static const OrthoViewType TOP;

         /**
          * Front view.  This renders the scene along the XZ plane looking down the
          * +Y axis.
          */
         static const OrthoViewType FRONT;

         /**
          * Side view.  This renders the scene along the XY plane looking down the
          * -X axis.
          */
         static const OrthoViewType SIDE;

      private:
         OrthoViewType(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      /**
       * Sets this orthographic viewport's current view type.
       * @param type The new view type.
       */
      void setViewType(const OrthoViewType& type, bool refreshView = true);

      /**
       * Gets the type of view currently in use by the viewport.
       * @return
       */
      const OrthoViewType& getViewType() const { return *mViewType; }

      /**
      * Initializes the viewport.  This just sets the current render style
      * to be wireframe and the view type to be OrthoViewType::TOP.
      */
      virtual void initializeGL();

      /**
      * Sets the orthographic projection parameters of the current camera.
      * @param width The width of the viewport.
      * @param height The height of the viewport.
      */
      void resizeGL(int width, int height);

   protected:
      /**
       * Constructs the orthographic viewport.
       */
      OrthoViewport(const std::string& name, QWidget* parent = NULL,
            osg::GraphicsContext* shareWith = NULL);

      /**
       * Destroys the viewport.
       */
      virtual ~OrthoViewport() { }

   private:
      ///Allow the ViewportManager access to it can create perspective viewports.
      friend class ViewportManager;

      const OrthoViewType* mViewType;
      osg::Vec3            mZoomToPosition;
   };

} // namespace dtEditQt

#endif // DELTA_ORTHO_VIEWPORT
