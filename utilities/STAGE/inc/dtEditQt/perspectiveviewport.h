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
#include <dtEditQt/export.h>

namespace dtEditQt
{

   /**
    * This class provides a 3D first person style view into the scene.  Its camera
    * mode allows the user to fly around a scene and view it from any angle.
    */
   class DT_EDITQT_EXPORT PerspectiveViewport : public EditorViewport
   {
      Q_OBJECT

   public:

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

      /**
      * Takes the current selection of actors and attaches them to the camera.
      * Therefore, as the camera moves, the actors move along with it.
      */
      void attachCurrentSelectionToCamera();

      /**
      * Detaches the current selection from the camera.
      */
      void detachCurrentSelectionFromCamera();

      /**
       * Spins through the actor proxies currently in the map and if they
       * are using billboards to represent themselves, orient the billboards
       * to be inline with the camera.
       * @note The billboard behavior of OpenSceneGraph was not suitable to
       *  to the needs of the actor proxy billboards, therefore, a different
       *  implementation is required.
       */
      virtual void updateActorProxyBillboards();

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
      void keyReleaseEvent(QKeyEvent* e);

   private:
      ///Allow the ViewportManager access to it can create perspective viewports.
      friend class ViewportManager;
   };

} // namespace dtEditQt

#endif // DELTA_PERSPECTIVE_VIEWPORT
