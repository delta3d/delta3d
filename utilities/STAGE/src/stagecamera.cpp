/* -*-c++-*
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
#include <osg/Math>
#include <dtEditQt/stagecamera.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/log.h>
#include <dtCore/actorproxyicon.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   StageCamera::StageCamera()
      : mFovY(60.0f)
      , mAspectRatio(1.3333333333f)
      , mZNear(1.0f)
      , mZFar(10000.0f)
      , mZoomFactor(1.0f)
      , mProjType(PERSPECTIVE)
      , mDeltaCamera(new dtCore::Camera("StageCamera"))
   {
      mDeltaCamera->SetupBackwardCompatibleStateset();
      resetRotation();
      setPosition(osg::Vec3(0, 0, 0));
      mDeltaCamera->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setPosition(const osg::Vec3& pos)
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.SetTranslation(pos);
      mDeltaCamera->SetTransform(xform);

      emit PositionMoved(pos.x(), pos.y(), pos.z());
   }


   //////////////////////////////////////////////////////////////////////////
   void StageCamera::setRotation(const osg::Quat& rot)
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.SetRotation(rot);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::move(const osg::Vec3& relPos)
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.SetTranslation(xform.GetTranslation() + relPos);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::pitch(double degrees)
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      xform.SetRotation(hpr[0], hpr[1] + degrees, hpr[2]);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::yaw(double degrees)
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      xform.SetRotation(hpr[0] + degrees, hpr[1], hpr[2]);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   //void StageCamera::roll(double degrees)
   //{
   //   dtCore::Transform xform;
   //   mDeltaCamera->GetTransform(xform);
   //   osg::Vec3 hpr;
   //   xform.GetRotation(hpr);
   //   xform.SetRotation(hpr[0], hpr[1], hpr[2] + degrees);
   //   mDeltaCamera->SetTransform(xform);
   //}

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::makeOrtho(double left, double right, double bottom, double top,
      double nearZ, double farZ)
   {
      mOrthoLeft              = left;
      mOrthoRight             = right;
      mOrthoBottom            = bottom;
      mOrthoTop               = top;
      mZNear                  = nearZ;
      mZFar                   = farZ;
      mProjType               = ORTHOGRAPHIC;

      mDeltaCamera->SetOrtho(mOrthoLeft, mOrthoRight, mOrthoBottom, mOrthoTop,
                             mZNear, mZFar);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::makePerspective(double fovY, double aspect, double nearZ, double farZ)
   {
      mFovY                   = fovY;
      mAspectRatio            = aspect;
      mZNear                  = nearZ;
      mZFar                   = farZ;
      mProjType               = PERSPECTIVE;

      mDeltaCamera->SetPerspectiveParams(mFovY, mAspectRatio, mZNear, mZFar);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setNearClipPlane(double value)
   {
      mZNear = value;

      if (mProjType == PERSPECTIVE)
      {
         makePerspective(mFovY, mAspectRatio, mZNear, mZFar);
      }
      else
      {
         makeOrtho(mOrthoLeft, mOrthoRight, mOrthoBottom, mOrthoTop,
            mZNear, mZFar);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setFarClipPlane(double value)
   {
      mZFar = value;

      if (mProjType == PERSPECTIVE)
      {
         makePerspective(mFovY, mAspectRatio, mZNear, mZFar);
      }
      else
      {
         makeOrtho(mOrthoLeft, mOrthoRight, mOrthoBottom, mOrthoTop,
            mZNear, mZFar);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setAspectRatio(double ratio)
   {
      mAspectRatio = ratio;

      if (mProjType == PERSPECTIVE)
      {
         makePerspective(mFovY, mAspectRatio, mZNear, mZFar);
      }
      else
      {
         makeOrtho(mOrthoLeft, mOrthoRight, mOrthoBottom, mOrthoTop,
            mZNear, mZFar);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getViewDir() const
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      return xform.GetForwardVector();
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getUpDir() const
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      return xform.GetUpVector();
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getRightDir() const
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      return xform.GetRightVector();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::rotate(const osg::Quat& q)
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Quat orientation;
      xform.GetRotation(orientation);

      xform.SetRotation(q * orientation);

      // Now make sure we remove the roll.
      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      hpr.z() = 0.0f;
      xform.SetRotation(hpr);

      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::resetRotation()
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.SetRotation(0, 0, 0);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Quat StageCamera::getOrientation() const
   {
      osg::Quat q;
      //q.makeRotate(osg::DegreesToRadians(90.0), getRightDir());
      //q *= mOrientation;

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.GetRotation(q);

      return q;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getPosition() const
   {
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      return xform.GetTranslation();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::zoom(double amount)
   {
      mZoomFactor *= amount;

      if (mZoomFactor < 0.0001)
      {
         mZoomFactor = 0.0001;
      }

      mDeltaCamera->SetOrtho(mOrthoLeft / mZoomFactor,
                            mOrthoRight  / mZoomFactor,
                            mOrthoBottom / mZoomFactor,
                            mOrthoTop    / mZoomFactor,
                            mZNear, mZFar);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StageCamera::setZoom(double zoom)
   {
      mZoomFactor = zoom;

      if (mZoomFactor < 0.0001)
      {
         mZoomFactor = 0.0001;
      }

      mDeltaCamera->SetOrtho(mOrthoLeft / mZoomFactor,
         mOrthoRight  / mZoomFactor,
         mOrthoBottom / mZoomFactor,
         mOrthoTop    / mZoomFactor,
         mZNear, mZFar);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::update()
   {
      updateActorAttachments();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::getOrthoParams(double& left, double& right, double& bottom, double& top,
      double& nearZ, double& farZ)
   {
      left   = mOrthoLeft   / mZoomFactor;
      right  = mOrthoRight  / mZoomFactor;
      bottom = mOrthoBottom / mZoomFactor;
      top    = mOrthoTop    / mZoomFactor;
      nearZ  = mZNear;
      farZ   = mZFar;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::attachActorProxy(dtCore::TransformableActorProxy* proxy)
   {
      ActorAttachment toAttach;
      const dtCore::BaseActorObject::RenderMode& renderMode = proxy->GetRenderMode();
      osg::Matrix rotMat;
      osg::Quat rotOffset;

      if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         osg::Vec3 billBoardPos;
         dtCore::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();

         if (billBoard != NULL)
         {
            dtCore::Transform xform;
            mDeltaCamera->GetTransform(xform);

            billBoardPos = proxy->GetTranslation();
            toAttach.mActor = proxy;
            toAttach.mPositionOffset = billBoardPos - xform.GetTranslation();
            xform.GetRotation(toAttach.mInitialCameraHPR);

            rotOffset.set(billBoard->GetActorRotation());
            toAttach.mRotationOffset = rotOffset * getOrientation().conj();
         }
         else
         {
            LOG_ERROR("Proxy " + proxy->GetName() + " is using billboard render"
               " mode but does not have a valid billboard.");
            return;
         }
      }
      else
      {
         dtCore::Transformable* transformable =
            dynamic_cast<dtCore::Transformable*>(proxy->GetDrawable());

         if (transformable != NULL)
         {
            dtCore::Transform tx, xform;
            osg::Vec3 tPos;

            transformable->GetTransform(tx);
            tx.GetRotation(rotMat);
            tx.GetTranslation(tPos);
            rotOffset.set(rotMat);

            mDeltaCamera->GetTransform(xform);

            toAttach.mActor = proxy;
            toAttach.mPositionOffset = tPos - xform.GetTranslation();
            toAttach.mRotationOffset = rotOffset * getOrientation().conj();
            xform.GetRotation(toAttach.mInitialCameraHPR);
         }
         else
         {
            LOG_ERROR("Unable to attach proxy to camera.  Actor is not a transformable.");
            return;
         }
      }

      mAttachedProxies.push_back(toAttach);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::detachActorProxy(dtCore::TransformableActorProxy* proxy)
   {
      std::list<ActorAttachment>::iterator itor;
      for (itor = mAttachedProxies.begin(); itor != mAttachedProxies.end(); ++itor)
      {
         dtCore::TransformableActorProxy* toRemove = itor->mActor.get();
         if (toRemove == proxy)
         {
            mAttachedProxies.erase(itor);
            break;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::removeAllActorAttachments()
   {
      mAttachedProxies.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::updateActorAttachments()
   {
      if (mAttachedProxies.empty())
      {
         return;
      }

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);

      std::list<ActorAttachment>::iterator itor;
      osg::Quat camLookInv = getOrientation();
      osg::Vec3 camHPR;
      xform.GetRotation(camHPR);

      for (itor = mAttachedProxies.begin(); itor != mAttachedProxies.end(); ++itor)
      {
         dtCore::TransformableActorProxy* tProxy = itor->mActor.get();
         osg::Vec3 deltaHPR, tPos, actorHPR;
         osg::Quat pitchRotate, yawRotate;
         osg::Matrix newRotationMat;

         deltaHPR = camHPR - itor->mInitialCameraHPR;
         yawRotate.makeRotate(osg::DegreesToRadians(deltaHPR[0]), osg::Vec3(0.0f, 0.0f, 1.0f));
         pitchRotate.makeRotate(osg::DegreesToRadians(deltaHPR[1]), getRightDir());
         newRotationMat.makeRotate(itor->mRotationOffset * camLookInv);
         dtUtil::MatrixUtil::MatrixToHpr(actorHPR, newRotationMat);
         tPos = (yawRotate * pitchRotate * itor->mPositionOffset);
         tPos += getPosition();

         tProxy->SetTranslation(tPos);
         tProxy->SetRotationFromMatrix(newRotationMat);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Compass* StageCamera::ShowCompass()
   {
      if (!mCompass.valid())
      {
         mCompass = new dtCore::Compass(getDeltaCamera());
      }

      return mCompass;
   }
} // namespace dtEditQt
