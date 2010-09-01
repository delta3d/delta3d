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
#ifndef DELTA_VIEWPORT_CONTAINER
#define DELTA_VIEWPORT_CONTAINER

#include <QtGui/QWidget>
#include "dtEditQt/viewport.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QContextMenuEvent;
class QBoxLayout;
class QFrame;
class QLineEdit;

namespace dtEditQt
{

   /**
    * The viewport is a higher level container widget used to house viewport widgets and
    * control their behavior.  The container has a toolbar in addition to the viewport
    * which contains actions that determine for example, how the viewport displays its
    * contents.
    * @note
    *  The viewports themselves have methods to control their behavior, therefore, this
    *  class acts as a UI interface to that functionality.
    */
   class ViewportContainer : public QWidget
   {
      Q_OBJECT

   public:
      /**
       * Constructs the viewport container.  Note, there is no viewport currently assigned
       * to it.
       * @param parent The parent widget to assign to this container.
       * @return
       * @see setViewport
       */
      ViewportContainer(Viewport* vp = NULL, QWidget* parent = NULL);

      /**
       * Assigns a viewport to this container.
       * @param viewPort The viewport to put inside this container.
       */
      void setViewport(Viewport* viewPort);

   public slots:
      /**
       * Tells the current viewport object to render its contents in wireframe mode.
       * If the current viewport is invalid, this method does nothing.
       * @see Viewport::RenderStyle
       */
      void setWireFrameView()
      {
         if (mViewPort != NULL)
         {
            mViewPort->setRenderStyle(Viewport::RenderStyle::WIREFRAME);
         }
      }

      /**
       * Tells the current viewport object to render its contents in textured mode.
       * If the current viewport is invalid, this method does nothing.
       * @see Viewport::RenderStyle
       */
      void setTexturesOnlyView()
      {
         if (mViewPort != NULL)
         {
            mViewPort->setRenderStyle(Viewport::RenderStyle::TEXTURED);
         }
      }

      /**
       * Tells the current viewport object to render its contents with lighting
       * enabled. If the current viewport is invalid, this method does nothing.
       * @see Viewport::RenderStyle
       */
      void setLightingOnlyView()
      {
         if (mViewPort != NULL)
         {
            mViewPort->setRenderStyle(Viewport::RenderStyle::LIT);
         }
      }

      /**
       * Tells the current viewport object to render its contents in textured mode
       * as well as with lighting enabled.  If the current viewport is invalid,
       * this method does nothing.
       * @see Viewport::RenderStyle
       */
      void setTexturesAndLightingView()
      {
         if (mViewPort != NULL)
         {
            mViewPort->setRenderStyle(Viewport::RenderStyle::LIT_AND_TEXTURED);
         }
      }

      /**
       * This method is called when the render style of the viewport owned by this
       * container has changed.
       */
      void onViewportRenderStyleChanged();

      ///Sets the camera speed to the slowest setting.
      void setCameraSpeedSlowest();

      ///Sets the camera speed to a slow setting.
      void setCameraSpeedSlow();

      ///Sets the camera speed to the default.
      void setCameraSpeedNormal();

      ///Sets the camera speed to a fast setting.
      void setCameraSpeedFast();

      ///Sets the camera speed to the fastest setting.
      void setCameraSpeedFastest();

      void OnCameraMoved(double x, double y, double z);

      void OnNewPositionEntered();

   protected:
      ///Creates the toolbar action objects.
      void createActions();

      ///Adds the action objects to this containers toolbar.
      void createToolBar();

      ///Creates a right-click menu for the toolbar of this container.
      void createContextMenu();

      /**
       * Overridden to ensure the right-click menu only appears when right clicking
       * on the toolbar and not on the viewport widget.
       */
      void contextMenuEvent(QContextMenuEvent* e);

      /**
       * Creates a widget for controlling the camera speed on this viewport.
       * @note Only gets called if the viewport in this container is a perspective viewport.
       */
      void addCameraControlWidget();

   private:
      Viewport* mViewPort;

      QBoxLayout*       mLayout;
      QBoxLayout*       mButtonLayout;
      QLabel*           mViewportTitle;
      QMenu*            mContextMenu;
      QMenu*            mCameraMovementMenu;
      QFrame*           mToolBar;
      QList<QLineEdit*> mPositionEditWidgets;

      QActionGroup* mRenderStyleActionGroup;
      QActionGroup* mCameraSpeedGroup;

      // Action objects.
      QAction* mSetWireFrameAction;
      QAction* mSetTexturesOnlyAction;
      QAction* mSetLightingOnlyAction;
      QAction* mSetTexturesAndLightingAction;

      QAction* mCameraSpeedSlowest;
      QAction* mCameraSpeedSlow;
      QAction* mCameraSpeedNormal;
      QAction* mCameraSpeedFast;
      QAction* mCameraSpeedFastest;

      void SetupPositionWidgets(QBoxLayout* layout);
   };

} // namespace dtEditQt

#endif // DELTA_VIEWPORT_CONTAINER
