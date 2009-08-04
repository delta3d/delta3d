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

#ifndef DELTA_PERSPECTIVE_VIEWPORT
#define DELTA_PERSPECTIVE_VIEWPORT

#include <dtEditQt/editorviewport.h>

namespace dtEditQt
{

   /**
    * This class provides a 3D first person style view into the scene.  Its camera
    * mode allows the user to fly around a scene and view it from any angle.
    */
   class PerspectiveViewport : public EditorViewport
   {
      Q_OBJECT

   public:
      /**
       * Enumerates the specific types of interactions a perspective viewport
       * supports.  These extend the interactions of the base viewport.  For example,
       * when the overall mode is camera mode, the perspective viewport supports
       * more specific behavior.
       */
      class CameraMode : public dtUtil::Enumeration
      {
         DECLARE_ENUM(CameraMode);
      public:
         static const CameraMode CAMERA_TRANSLATE;
         static const CameraMode CAMERA_NAVIGATE;
         static const CameraMode CAMERA_LOOK;
         static const CameraMode NOTHING;

      private:
         CameraMode(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      /**
       * Moves the camera.
       * @par
       *  The camera's movement is determined by the current mode of the
       *  perspective viewport:<br>
       *      CAMERA_TRANSLATE - The camera is moved by up/down (dy) and
       *          left/right (dx). <br>
       *      CAMERA_NAVIGATE - The camera is moved forward/backward (dy) and
       *          can be rotated about its up axis. (dx). <br>
       *      CAMERA_LOOK - The camera is stationary and can look in any direction. <br>
       * @param dx
       * @param dy
       */
      bool moveCamera(float dx, float dy);

      /**
       * Sets the scene to be rendered by this viewport.
       * @param scene The new scene to be rendered.
       */
      //virtual void setScene(dtCore::Scene* scene);

      /**
       * Initializes the viewport.  This just sets the render style to be
       * textured and un-lit.
       */
      virtual void initializeGL();

      /**
       * Sets the perspective projection viewing parameters of this viewport's
       * camera.
       * @param width The new width of the viewport.
       * @param height The new height of the viewport.
       */
      void resizeGL(int width, int height);

   public slots:

   protected:
      /**
       * Constructs the perspective viewport.
       * @param name
       * @param parent
       * @param shareWith
       */
      PerspectiveViewport(const std::string& name, QWidget* parent = NULL,
            osg::GraphicsContext* shareWith = NULL);

      /**
       * Destroys the viewport.
       */
      virtual ~PerspectiveViewport() { }

      /**
       * Called when the user presses a key on the keyboard in the viewport.
       * Based on the combination of keys pressed, the viewport's current
       * mode will be set.
       * @param e
       */
      void keyPressEvent(QKeyEvent* e);

      /**
       * Called when the user releases a key on the keyboard in the viewport.
       * Based on the keys released, the viewport's current mode is
       * updated accordingly.
       * @param e
       */
      //void keyReleaseEvent(QKeyEvent* e);

      /**
       * Called when the user presses a mouse button in the viewport.  Based on
       * the combination of buttons pressed, the viewport's current mode will
       * be set.
       * @param e
       * @see ModeType
       */
      //void mousePressEvent(QMouseEvent* e);

      /**
       * Called when the user releases a mouse button in the viewport.  Based on
       * the buttons released, the viewport's current mode is updated
       * accordingly.
       * @param e
       */
      //void mouseReleaseEvent(QMouseEvent* e);

      /**
       * Called when the user moves the mouse while pressing any combination of
       * mouse buttons.  Based on the current mode, the camera is updated.
       * @param dx the adjusted change in x that the mouse moved.
       * @param dy the adjusted change in y that the mouse moved.
       */
      //virtual void onMouseMoveEvent(QMouseEvent* e, float dx, float dy);

      /**
       * Called from the mousePressEvent handler.  This sets the viewport state
       * to properly respond to mouse movement events when in camera mode.
       * @param e
       */
      bool beginCameraMode(QMouseEvent* e);

      /**
       * Called from the mouseReleaseEvent handler.  This restores the state of
       * the viewport as it was before camera mode was entered.
       * @param e
       */
      bool endCameraMode(QMouseEvent* e);

      /**
       * Called from the mousePressEvent handler.  Depending on what modifier
       * key is pressed, this puts the viewport state into a mode that enables
       * actor manipulation.
       * @param e
       */
      bool beginActorMode(QMouseEvent* e);

      /**
       * Called from the mouseReleaseEvent handler.  This restores the state of
       * the viewport as it was before actor mode was entered.
       * @param e
       */
      bool endActorMode(QMouseEvent* e);

      /**
       * Takes the current selection of actors and attaches them to the camera.
       * Therefore, as the camera moves, the actors move along with it.
       */
      void attachCurrentSelectionToCamera();

   private:
      ///Allow the ViewportManager access to it can create perspective viewports.
      friend class ViewportManager;

      const CameraMode* mCameraMode;
   };

} // namespace dtEditQt

#endif // DELTA_PERSPECTIVE_VIEWPORT
