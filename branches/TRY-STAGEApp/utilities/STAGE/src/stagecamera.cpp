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
#include <prefix/dtstageprefix-src.h>
#include <osg/Math>
#include <dtEditQt/stagecamera.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/log.h>
#include <dtDAL/actorproxyicon.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   StageCamera::StageCamera()
      : 
        mCamPitch(0.0)
      , mCamYaw(0.0)
      , mCamRoll(0.0)
      , mFovY(60.0f)
      , mAspectRatio(1.3333333333f)
      , mZNear(1.0f)
      , mZFar(10000.0f)
      , mZoomFactor(1.0f)
      //, mUpdateProjectionMatrix(true)
      //, mUpdateWorldViewMatrix(true)
      //, mProjType(PERSPECTIVE)
      , mDeltaCamera(new dtCore::Camera("StageCamera"))
   {
      resetRotation();
      setPosition(osg::Vec3(0, 0, 0));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setPosition(const osg::Vec3& pos)
   {
      mPosition = pos;
      //mUpdateWorldViewMatrix = true;
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.SetTranslation(mPosition);
      mDeltaCamera->SetTransform(xform);

      emit PositionMoved(pos.x(), pos.y(), pos.z());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::move(const osg::Vec3& relPos)
   {
      mPosition += relPos;
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.SetTranslation(mPosition);
      mDeltaCamera->SetTransform(xform);
      //mUpdateWorldViewMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::pitch(double degrees)
   {
      //osg::Quat q;
      //q.makeRotate(osg::DegreesToRadians(degrees), getRightDir());
      //rotate(q);

      mCamPitch += degrees;

      if (mCamPitch < 0.0)
      {
         mCamPitch += 360.0;
      }

      if (mCamPitch > 360.0)
      {
         mCamPitch -= 360.0;
      }

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      xform.SetRotation(hpr[0], mCamPitch, hpr[2]);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::yaw(double degrees)
   {
      //osg::Quat q;
      //q.makeRotate(osg::DegreesToRadians(degrees), osg::Vec3(0, 0, 1));
      //rotate(q);

      mCamYaw += degrees;

      if (mCamYaw < 0.0)
      {
         mCamYaw += 360.0;
      }

      if (mCamYaw > 360.0)
      {
         mCamYaw -= 360.0;
      }

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      xform.SetRotation(mCamYaw, hpr[1], hpr[2]);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::roll(double degrees)
   {
      //osg::Quat q;
      //q.makeRotate(osg::DegreesToRadians(-degrees), getViewDir());
      //rotate(q);

      mCamRoll += degrees;

      if (mCamRoll < 0.0)
      {
         mCamRoll += 360.0;
      }

      if (mCamRoll > 360.0)
      {
         mCamRoll -= 360.0;
      }

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 hpr;
      xform.GetRotation(hpr);
      xform.SetRotation(hpr[0], hpr[1], mCamRoll);
      mDeltaCamera->SetTransform(xform);
   }

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
      //mUpdateProjectionMatrix = true;
      //mProjType               = ORTHOGRAPHIC;
      
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
      //mUpdateProjectionMatrix = true;
      //mProjType               = PERSPECTIVE;
      
      mDeltaCamera->SetPerspectiveParams(mFovY, mAspectRatio, mZNear, mZFar);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setNearClipPlane(double value)
   {
      mZNear = value;
      //mUpdateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setFarClipPlane(double value)
   {
      mZFar = value;
      //mUpdateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setAspectRatio(double ratio)
   {
      mAspectRatio = ratio;
      //mUpdateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getViewDir() const
   {
      //return mOrientation.conj() * osg::Vec3(0.0f, 0.0f, -1.0f);

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 right, up, forward;
      xform.GetOrientation(right, up, forward);
      return forward;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getUpDir() const
   {
      //return mOrientation.conj() * osg::Vec3(0.0f, 1.0f, 0.0f);
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 right, up, forward;
      xform.GetOrientation(right, up, forward);
      return up;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getRightDir() const
   {
      //return mOrientation.conj() * osg::Vec3(1.0f, 0.0f, 0.0f);
      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Vec3 right, up, forward;
      xform.GetOrientation(right, up, forward);
      return right;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::rotate(const osg::Quat& q)
   {
      //mOrientation = q * mOrientation;
      //mUpdateWorldViewMatrix = true;

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      osg::Quat orientation;
      xform.GetRotation(orientation);

      xform.SetRotation(q * orientation);
      mDeltaCamera->SetTransform(xform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::resetRotation()
   {
      //mOrientation = osg::Quat(osg::DegreesToRadians(-90.0), osg::Vec3(1, 0, 0));
      mCamPitch = 0;
      mCamYaw = 0;
      mCamRoll = 0;

      dtCore::Transform xform;
      mDeltaCamera->GetTransform(xform);
      xform.SetRotation(mCamYaw, mCamPitch, mCamRoll);
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

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::zoom(double amount)
   {
      mZoomFactor *= amount;

      if (mZoomFactor < 0.0001)
      {
         mZoomFactor = 0.0001;
      }

      //mUpdateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::update()
   {
      //getProjectionMatrix();
      //getWorldViewMatrix();
      updateActorAttachments();
      //updateDeltaCamera();
   }

   ///////////////////////////////////////////////////////////////////////////////
   //const osg::Matrix& StageCamera::getProjectionMatrix()
   //{
   //   if (mUpdateProjectionMatrix)
   //   {
   //      if (mProjType == PERSPECTIVE)
   //      {
   //         //mProjectionMat.makePerspective(mFovY, mAspectRatio,
   //         //   mZNear, mZFar);
   //         mProjectionMat = mDeltaCamera->GetOSGCamera()->getViewMatrix();
   //      }
   //      else if (mProjType == ORTHOGRAPHIC)
   //      {
   //         //mProjectionMat.makeOrtho(mOrthoLeft / mZoomFactor,
   //         //   mOrthoRight  / mZoomFactor,
   //         //   mOrthoBottom / mZoomFactor,
   //         //   mOrthoTop    / mZoomFactor,
   //         //   mZNear, mZFar);
   //         mProjectionMat = mDeltaCamera->GetOSGCamera()->getViewMatrix();
   //      }

   //      mUpdateProjectionMatrix = false;
   //   }

   //   return mProjectionMat;
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //const osg::Matrix& StageCamera::getWorldViewMatrix()
   //{
   //   if (mUpdateWorldViewMatrix)
   //   {
   //      //mWorldViewMat = osg::Matrix::translate(-mPosition) *
   //      //   osg::Matrix::rotate(mOrientation);
   //      mUpdateWorldViewMatrix = false;

   //      dtCore::Transform xform;
   //      mDeltaCamera->GetTransform(xform);
   //      xform.Get(mWorldViewMat);
   //   }

   //   return mWorldViewMat;
   //}

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
   void StageCamera::attachActorProxy(dtDAL::TransformableActorProxy* proxy)
   {
      ActorAttachment toAttach;
      const dtDAL::ActorProxy::RenderMode& renderMode = proxy->GetRenderMode();
      osg::Matrix rotMat;
      osg::Quat rotOffset;

      if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
      {
         osg::Vec3 billBoardPos;
         dtDAL::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();

         if (billBoard != NULL)
         {
            billBoardPos = proxy->GetTranslation();
            toAttach.mActor = proxy;
            toAttach.mPositionOffset = billBoardPos - mPosition;
            toAttach.mInitialCameraHPR = osg::Vec3(mCamYaw, mCamPitch, mCamRoll);

            rotOffset.set(billBoard->GetActorRotation());
            toAttach.mRotationOffset = rotOffset * getOrientation();
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
            dynamic_cast<dtCore::Transformable*>(proxy->GetActor());

         if (transformable != NULL)
         {
            dtCore::Transform tx;
            osg::Vec3 tPos;

            transformable->GetTransform(tx);
            tx.GetRotation(rotMat);
            tx.GetTranslation(tPos);
            rotOffset.set(rotMat);

            toAttach.mActor = proxy;
            toAttach.mPositionOffset = tPos - mPosition;
            toAttach.mRotationOffset = rotOffset * getOrientation();
            toAttach.mInitialCameraHPR = osg::Vec3(mCamYaw, mCamPitch, mCamRoll);
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
   void StageCamera::detachActorProxy(dtDAL::TransformableActorProxy* proxy)
   {
      std::list<ActorAttachment>::iterator itor;
      for (itor = mAttachedProxies.begin(); itor != mAttachedProxies.end(); ++itor)
      {
         dtDAL::TransformableActorProxy* toRemove = itor->mActor.get();
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

      std::list<ActorAttachment>::iterator itor;
      osg::Quat camLookInv = getOrientation().conj();
      osg::Vec3 camHPR = osg::Vec3(mCamYaw, mCamPitch, mCamRoll);

      for (itor = mAttachedProxies.begin(); itor != mAttachedProxies.end(); ++itor)
      {
         dtDAL::TransformableActorProxy* tProxy = itor->mActor.get();
         osg::Vec3 deltaHPR, tPos, actorHPR;
         osg::Quat pitchRotate, yawRotate;
         osg::Matrix newRotationMat;

         deltaHPR = camHPR - itor->mInitialCameraHPR;
         yawRotate.makeRotate(osg::DegreesToRadians(deltaHPR[0]), osg::Vec3(0.0f, 0.0f, 1.0f));
         pitchRotate.makeRotate(osg::DegreesToRadians(deltaHPR[1]), getRightDir());
         newRotationMat.makeRotate(itor->mRotationOffset * camLookInv);
         dtUtil::MatrixUtil::MatrixToHpr(actorHPR, newRotationMat);
         tPos = (yawRotate * pitchRotate * itor->mPositionOffset);
         tPos += mPosition;

         tProxy->SetTranslation(tPos);
         tProxy->SetRotationFromMatrix(newRotationMat);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   //void StageCamera::updateDeltaCamera()
   //{
   //   if (!mDeltaCamera.valid())
   //   {
   //      return;
   //   }

   //   if (mProjType == PERSPECTIVE)
   //   {
   //      mDeltaCamera->GetOSGCamera()->setProjectionMatrixAsPerspective(mFovY, 1.0f, mZNear, mZFar);
   //   }
   //   else
   //   {
   //      mDeltaCamera->GetOSGCamera()->setProjectionMatrix(getProjectionMatrix());
   //   }

   //   mDeltaCamera->GetOSGCamera()->setViewMatrix(getWorldViewMatrix());

   //   dtCore::Transform transform;
   //   mDeltaCamera->GetTransform(transform);
   //   //transform.SetRotation(mOrientation);
   //   transform.SetRotation(getYaw(), getPitch(), getRoll());
   //   transform.SetTranslation(mPosition);
   //   mDeltaCamera->SetTransform(transform);
   //}

} // namespace dtEditQt
