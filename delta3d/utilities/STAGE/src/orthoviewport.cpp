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
#include <prefix/stageprefix.h>
#include <QtGui/QMouseEvent>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/stagecameramotionmodel2d.h>
#include <dtCore/exceptionenum.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(OrthoViewport::OrthoViewType);
   const OrthoViewport::OrthoViewType OrthoViewport::OrthoViewType::TOP("TOP");
   const OrthoViewport::OrthoViewType OrthoViewport::OrthoViewType::FRONT("FRONT");
   const OrthoViewport::OrthoViewType OrthoViewport::OrthoViewType::SIDE("SIDE");
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   OrthoViewport::OrthoViewport(const std::string& name, QWidget* parent,
      osg::GraphicsContext* shareWith)
      : EditorViewport(ViewportManager::ViewportType::ORTHOGRAPHIC, name, parent, shareWith)
   {
      setViewType(OrthoViewType::TOP,false);

      mObjectMotionModel->SetScale(450.0f);

      // Create our camera model.
      mDefaultCameraMotionModel = new STAGECameraMotionModel2D();
      mDefaultCameraMotionModel->SetCamera(getCamera());
      mDefaultCameraMotionModel->SetViewport(this);

      mCameraMotionModel = mDefaultCameraMotionModel;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void OrthoViewport::initializeGL()
   {
      EditorViewport::initializeGL();

      // We do not want OSG to compute our near and far clipping planes when in
      // orthographic view
      mCamera->getDeltaCamera()->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

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
         mEnabledMask = 0xFFFF1FFF;
         mDisabledMask = 0x0000100F;
      }
      else if (type == OrthoViewType::FRONT)
      {
         mViewType = &OrthoViewType::FRONT;
         getCamera()->resetRotation();
         mEnabledMask = 0xFFFF4FFF;
         mDisabledMask = 0x0000400F;
      }
      else if (type == OrthoViewType::SIDE)
      {
         mViewType = &OrthoViewType::SIDE;
         getCamera()->resetRotation();
         getCamera()->yaw(90);
         mEnabledMask = 0xFFFF2FFF;
         mDisabledMask = 0x0000200F;
      }

      if (refreshView)
      {
         if (!isInitialized())
         {
            throw dtCore::BaseException("Cannot refresh the viewport. "
               "It has not been initialized.", __FILE__, __LINE__);
         }
         refresh();
      }
   }
} // namespace dtEditQt

////////////////////////////////////////////////////////////////////////////////
