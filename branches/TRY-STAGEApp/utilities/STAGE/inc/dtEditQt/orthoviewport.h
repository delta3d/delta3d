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

#include <QtGui/QCursor>
#include <dtUtil/enumeration.h>
#include "dtEditQt/editorviewport.h"

namespace dtEditQt
{

   /**
    * The orthographic viewport renders a 2D view of the scene.  The 2D view can be
    * along each of the 3D axis.
    * @see OrthoViewType
    */
   class OrthoViewport : public EditorViewport
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
       * Enumerates the specific types of interactions an orthographic viewport
       * supports.  These extend the interactions of the base viewport.  For example,
       * when the overall mode is camera mode, the orthographic viewport supports
       * more specific behavior.
       */
      class CameraMode : public dtUtil::Enumeration
      {
         DECLARE_ENUM(CameraMode);

      public:
         static const CameraMode CAMERA_PAN;
         static const CameraMode CAMERA_ZOOM;
         static const CameraMode NOTHING;

      private:
         CameraMode(const std::string& name)
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
       * Moves the camera.
       * @par
       *  The camera's movement is based on the current camera mode.<br>
       *      CAMERA_PAN - Pans the camera along the plane the
       *          viewport is looking at.<br>
       *      CAMERA_ZOOM - Zooms the camera in and out.
       *
       * @param dx
       * @param dy
       */
      void moveCamera(float dx, float dy);

      /**
      * Initializes the viewport.  This just sets the current render style
      * to be wireframe and the view type to be OrthoViewType::TOP.
      */
      virtual void initializeGL();


   protected:
      /**
       * Constructs the orthographic viewport.
       */
      OrthoViewport(const std::string& name, QWidget* parent = NULL,
            QGLWidget* shareWith = NULL);

      /**
       * Destroys the viewport.
       */
      virtual ~OrthoViewport() { }


      /**
       * Sets the orthographic projection parameters of the current camera.
       * @param width The width of the viewport.
       * @param height The height of the viewport.
       */
      //void resizeGL(int width, int height);

      /**
       * Called when the user presses a key on the keyboard in the viewport.
       * Based on the combination of keys pressed, the viewport's current
       * mode will be set.
       * @param e
       */
      //void keyPressEvent(QKeyEvent* e);

      /**
       * Called when the user releases a key on the keyboard in the viewport.
       * Based on the keys released, the viewport's current mode is
       * updated accordingly.
       * @param e
       */
      //void keyReleaseEvent(QKeyEvent* e);

      /**
       * Called when the user releases a mouse button in the viewport.  Based on
       * the buttons released, the viewport's current mode is updated
       * accordingly.
       * @param e
       */
      //void mouseReleaseEvent(QMouseEvent* e);

      /**
       * Called when the user presses a mouse button in the viewport.  Based on
       * the combination of buttons pressed, the viewport's current mode will
       * be set.
       * @param e
       * @see ModeType
       */
      //void mousePressEvent(QMouseEvent*);

      /**
       * Called when the user moves the mouse while pressing any combination of
       * mouse buttons.  Based on the current mode, the camera is updated.
       * @param dx the adjusted change in x that the mouse moved.
       * @param dy the adjusted change in y that the mouse moved.
       */
      virtual void onMouseMoveEvent(QMouseEvent* e, float dx, float dy);

      /**
       * Called when the user moves the wheel on a mouse containing a scroll wheel.
       * This causes the scene to be zoomed in and out.
       * @param e
       */
      void wheelEvent(QWheelEvent* e);

      /**
       * Called from the mousePressEvent handler.  This sets the viewport state
       * to properly respond to mouse movement events when in camera mode.
       * @param e
       */
      void beginCameraMode(QMouseEvent* e);

      /**
       * Called from the mouseReleaseEvent handler.  This restores the state of
       * the viewport to it was before camera mode was entered.
       * @param e
       */
      void endCameraMode(QMouseEvent* e);

      /**
       * Called from the mousePressEvent handler.  Depending on what modifier
       * key is pressed, this puts the viewport state into a mode that enables
       * actor manipulation.
       * @param e
       */
      void beginActorMode(QMouseEvent* e);

      /**
       * Called from the mouseReleaseEvent handler.  This restores the state of
       * the viewport as it was before actor mode was entered.
       * @param e
       */
      void endActorMode(QMouseEvent* e);

      void warpWorldCamera(int x, int y);

   private:
      ///Allow the ViewportManager access to it can create perspective viewports.
      friend class ViewportManager;

      const CameraMode*    mCameraMode;
      const OrthoViewType* mViewType;
      osg::Vec3            mZoomToPosition;
   };

} // namespace dtEditQt

#endif // DELTA_ORTHO_VIEWPORT
