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
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QAction>
#include <osg/Math>
#include <osg/CullSettings>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editoractions.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/enginepropertytypes.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(OrthoViewport::OrthoViewType);
   const OrthoViewport::OrthoViewType OrthoViewport::OrthoViewType::TOP("TOP");
   const OrthoViewport::OrthoViewType OrthoViewport::OrthoViewType::FRONT("FRONT");
   const OrthoViewport::OrthoViewType OrthoViewport::OrthoViewType::SIDE("SIDE");
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(OrthoViewport::CameraMode);
   const OrthoViewport::CameraMode
      OrthoViewport::CameraMode::CAMERA_PAN("CAMERA_PAN");
   const OrthoViewport::CameraMode
      OrthoViewport::CameraMode::CAMERA_ZOOM("CAMERA_ZOOM");
   const OrthoViewport::CameraMode
      OrthoViewport::CameraMode::NOTHING("NOTHING");
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   OrthoViewport::OrthoViewport(const std::string& name, QWidget* parent,
      QGLWidget* shareWith)
      : EditorViewport(ViewportManager::ViewportType::ORTHOGRAPHIC, name, parent, shareWith)
   {
      mCamera = new StageCamera();
      mCameraMode = &OrthoViewport::CameraMode::NOTHING;
      setViewType(OrthoViewType::TOP,false);

      mObjectMotionModel->SetAutoScaleEnabled(false);
      mObjectMotionModel->SetScale(1.0f);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::initializeGL()
   {
      EditorViewport::initializeGL();

      // We do not want OSG to compute our near and far clipping planes when in
      // orthographic view
      getSceneView()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

      // Default to wireframe view.
      setRenderStyle(Viewport::RenderStyle::WIREFRAME,false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::resizeGL(int width, int height)
   {
      double xDim = (double)width * 0.5;
      double yDim = (double)height * 0.5;

      getCamera()->makeOrtho(-xDim, xDim, -yDim, yDim, -5000.0, 5000.0);
      EditorViewport::resizeGL(width, height);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::setViewType(const OrthoViewType& type, bool refreshView)
   {
      if (type == OrthoViewType::TOP)
      {
         mViewType = &OrthoViewType::TOP;
         getCamera()->resetRotation();
         getCamera()->pitch(-90);
      }
      else if (type == OrthoViewType::FRONT)
      {
         mViewType = &OrthoViewType::FRONT;
         getCamera()->resetRotation();
      }
      else if (type == OrthoViewType::SIDE)
      {
         mViewType = &OrthoViewType::SIDE;
         getCamera()->resetRotation();
         getCamera()->yaw(90);
      }

      if (refreshView)
      {
         if (!isInitialized())
         {
            throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,"Cannot refresh the viewport. "
               "It has not been initialized.", __FILE__, __LINE__);
         }
         refresh();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::keyPressEvent(QKeyEvent* e)
   {
      EditorViewport::keyPressEvent(e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::keyReleaseEvent(QKeyEvent* e)
   {
      EditorViewport::keyReleaseEvent(e);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::mousePressEvent(QMouseEvent* e)
   {
      EditorViewport::mousePressEvent(e);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::mouseReleaseEvent(QMouseEvent* e)
   {
      EditorViewport::mouseReleaseEvent(e);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::onMouseMoveEvent(QMouseEvent* e, float dx, float dy)
   {
      EditorViewport::onMouseMoveEvent(e, dx, dy);

      mObjectMotionModel->SetScale(450.0f / getCamera()->getZoom());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::moveCamera(float dx, float dy)
   {
      if (*mCameraMode == OrthoViewport::CameraMode::NOTHING || getCamera() == NULL)
      {
         return;
      }

      float xAmount = (-dx/getMouseSensitivity()*4.0f) / getCamera()->getZoom();
      float yAmount = (dy/getMouseSensitivity()*4.0f) / getCamera()->getZoom();

      if (*mCameraMode == OrthoViewport::CameraMode::CAMERA_PAN)
      {
         getCamera()->move(getCamera()->getRightDir() * xAmount);
         getCamera()->move(getCamera()->getUpDir() * yAmount);
      }
      else if (*mCameraMode == OrthoViewport::CameraMode::CAMERA_ZOOM)
      {
         osg::Vec3 moveVec = mZoomToPosition-getCamera()->getPosition();

         moveVec.normalize();
         if (dy <= -1.0f)
         {
            getCamera()->zoom(1.1f);
         }
         else if (dy >= 1.0f)
         {
            getCamera()->zoom(0.9f);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::wheelEvent(QWheelEvent* e)
   {
      if (e->delta() > 0)
      {
         getCamera()->zoom(1.3f);
      }
      else
      {
         getCamera()->zoom(0.7f);
      }

      refresh();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::beginCameraMode(QMouseEvent* e)
   {
      EditorViewport::beginCameraMode(e);

      if (mMouseButton == Qt::LeftButton)
      {
         mCameraMode = &OrthoViewport::CameraMode::CAMERA_PAN;
      }
      else if (mMouseButton == Qt::RightButton)
      {
         osg::Vec3 nearPoint,farPoint;
         int xLoc = e->pos().x();
         int yLoc = int(getSceneView()->getViewport()->height()-e->pos().y());
         getSceneView()->projectWindowXYIntoObject(xLoc, yLoc, nearPoint, farPoint);
         mZoomToPosition = nearPoint;
         mCameraMode = &OrthoViewport::CameraMode::CAMERA_ZOOM;
      }

      setInteractionMode(Viewport::InteractionMode::CAMERA);
      trapMouseCursor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::endCameraMode(QMouseEvent* e)
   {
      EditorViewport::endCameraMode(e);

      mCameraMode = &OrthoViewport::CameraMode::NOTHING;
      setInteractionMode(Viewport::InteractionMode::NOTHING);
      releaseMouseCursor();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::beginActorMode(QMouseEvent* e)
   {
      EditorViewport::beginActorMode(e);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::endActorMode(QMouseEvent* e)
   {
      EditorViewport::endActorMode(e);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::warpWorldCamera(int x, int y)
   {
   }

} // namespace dtEditQt
