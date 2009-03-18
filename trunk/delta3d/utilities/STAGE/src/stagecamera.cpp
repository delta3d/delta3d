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
#include <osg/Math>
#include <dtEditQt/stagecamera.h>
#include <dtCore/transformable.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/log.h>
#include <dtDAL/actorproxyicon.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   StageCamera::StageCamera()
   {
      this->updateProjectionMatrix = true;
      this->updateWorldViewMatrix = true;
      this->fovY = 60.0f;
      this->aspectRatio = 1.3333333333f;
      this->zNear = 1.0f;
      this->zFar = 10000.0f;
      this->zoomFactor = 1.0f;
      this->projType = PERSPECTIVE;
      this->camPitch = this->camYaw = this->camRoll = 0.0;
      resetRotation();
      setPosition(osg::Vec3(0,0,0));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setPosition(const osg::Vec3& pos)
   {
      this->position = pos;
      this->updateWorldViewMatrix = true;
      emit PositionMoved(pos.x(), pos.y(), pos.z());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::move(const osg::Vec3& relPos)
   {
      this->position += relPos;
      this->updateWorldViewMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::pitch(double degrees)
   {
      osg::Quat q;
      q.makeRotate(osg::DegreesToRadians(-degrees),getRightDir());
      rotate(q);

      this->camPitch += degrees;
      
      if (this->camPitch < 0.0)
      {
         this->camPitch += 360.0;
      }
      
      if (this->camPitch > 360.0)
      {
         this->camPitch -= 360.0;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::yaw(double degrees)
   {
      osg::Quat q;
      q.makeRotate(osg::DegreesToRadians(-degrees),osg::Vec3(0,0,1));
      rotate(q);

      this->camYaw += degrees;

      if (this->camYaw < 0.0)
      {
         this->camYaw += 360.0;
      }

      if (this->camYaw > 360.0)
      {
         this->camYaw -= 360.0;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::roll(double degrees)
   {
      osg::Quat q;
      q.makeRotate(osg::DegreesToRadians(-degrees),getViewDir());
      rotate(q);

      this->camRoll += degrees;

      if (this->camRoll < 0.0)
      {
         this->camRoll += 360.0;
      }

      if (this->camRoll > 360.0)
      {
         this->camRoll -= 360.0;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::makeOrtho(double left, double right, double bottom, double top,
      double nearZ, double farZ)
   {
      this->orthoLeft = left;
      this->orthoRight = right;
      this->orthoBottom = bottom;
      this->orthoTop = top;
      this->zNear = nearZ;
      this->zFar = farZ;
      this->updateProjectionMatrix = true;
      this->projType = ORTHOGRAPHIC;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::makePerspective(double fovY, double aspect, double nearZ, double farZ)
   {
      this->fovY = fovY;
      this->aspectRatio = aspect;
      this->zNear = nearZ;
      this->zFar = farZ;
      this->updateProjectionMatrix = true;
      this->projType = PERSPECTIVE;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setNearClipPlane(double value)
   {
      this->zNear = value;
      this->updateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setFarClipPlane(double value)
   {
      this->zFar = value;
      this->updateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::setAspectRatio(double ratio)
   {
      this->aspectRatio = ratio;
      this->updateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getViewDir() const
   {
      return this->orientation.conj() * osg::Vec3(0.0f, 0.0f, -1.0f);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getUpDir() const
   {
      return this->orientation.conj() * osg::Vec3(0.0f, 1.0f, 0.0f);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 StageCamera::getRightDir() const
   {
      return this->orientation.conj() * osg::Vec3(1.0f, 0.0f, 0.0f);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::rotate(const osg::Quat& q)
   {
      this->orientation = q*this->orientation;
      this->updateWorldViewMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::resetRotation()
   {
      this->orientation = osg::Quat(osg::DegreesToRadians(-90.0),osg::Vec3(1,0,0));
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Quat StageCamera::getOrientation() const
   {
      osg::Quat q;
      q.makeRotate(osg::DegreesToRadians(90.0),getRightDir());
      q *= this->orientation;
      return q;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::zoom(double amount)
   {
      this->zoomFactor *= amount;
      
      if (this->zoomFactor < 0.0001)
      {
         this->zoomFactor = 0.0001;
      }

      this->updateProjectionMatrix = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::update()
   {
      getProjectionMatrix();
      getWorldViewMatrix();
      updateActorAttachments();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const osg::Matrix& StageCamera::getProjectionMatrix()
   {
      if (this->updateProjectionMatrix) 
      {
         if (this->projType == PERSPECTIVE) 
         {
            this->projectionMat.makePerspective(this->fovY, this->aspectRatio,
               this->zNear, this->zFar);
         }
         else if (this->projType == ORTHOGRAPHIC) 
         {
            this->projectionMat.makeOrtho(this->orthoLeft / this->zoomFactor,
               this->orthoRight / this->zoomFactor,
               this->orthoBottom / this->zoomFactor,
               this->orthoTop / this->zoomFactor,
               this->zNear, this->zFar);
         }

         this->updateProjectionMatrix = false;
      }

      return this->projectionMat;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const osg::Matrix& StageCamera::getWorldViewMatrix()
   {
      if (this->updateWorldViewMatrix)
      {
         this->worldViewMat = osg::Matrix::translate(-this->position) *
            osg::Matrix::rotate(this->orientation);
         this->updateWorldViewMatrix = false;
      }

      return this->worldViewMat;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::getOrthoParams(double& left, double& right, double& bottom, double& top,
      double& nearZ, double& farZ)
   {
      left   = this->orthoLeft/this->zoomFactor;
      right  = this->orthoRight/this->zoomFactor;
      bottom = this->orthoBottom/this->zoomFactor;
      top    = this->orthoTop/this->zoomFactor;
      nearZ  = this->zNear;
      farZ   = this->zFar;
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
            toAttach.actor = proxy;
            toAttach.positionOffset = billBoardPos - this->position;
            toAttach.initialCameraHPR = osg::Vec3(this->camYaw,this->camPitch,this->camRoll);

            rotOffset.set(billBoard->GetActorRotation());
            toAttach.rotationOffset = rotOffset * getOrientation();
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
         dtCore::Transformable *transformable =
            dynamic_cast<dtCore::Transformable*>(proxy->GetActor());

         if (transformable != NULL) 
         {
            dtCore::Transform tx;
            osg::Vec3 tPos;

            transformable->GetTransform(tx);
            tx.GetRotation(rotMat);
            tx.GetTranslation(tPos);
            rotOffset.set(rotMat);

            toAttach.actor = proxy;
            toAttach.positionOffset = tPos - this->position;
            toAttach.rotationOffset = rotOffset * getOrientation();
            toAttach.initialCameraHPR = osg::Vec3(this->camYaw,this->camPitch,this->camRoll);
         }
         else 
         {
            LOG_ERROR("Unable to attach proxy to camera.  Actor is not a transformable.");
            return;
         }
      }

      this->attachedProxies.push_back(toAttach);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::detachActorProxy(dtDAL::TransformableActorProxy* proxy)
   {
      std::list<ActorAttachment>::iterator itor;
      for (itor=this->attachedProxies.begin(); itor!=this->attachedProxies.end(); ++itor)
      {
         dtDAL::TransformableActorProxy *toRemove = itor->actor.get();
         if (toRemove == proxy) 
         {
            this->attachedProxies.erase(itor);
            break;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::removeAllActorAttachments()
   {
      this->attachedProxies.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StageCamera::updateActorAttachments()
   {
      if (this->attachedProxies.empty())
      {
         return;
      }

      std::list<ActorAttachment>::iterator itor;
      osg::Quat camLookInv = getOrientation().conj();
      osg::Vec3 camHPR = osg::Vec3(this->camYaw,this->camPitch,this->camRoll);

      for (itor=this->attachedProxies.begin(); itor != this->attachedProxies.end(); ++itor)
      {
         dtDAL::TransformableActorProxy* tProxy = itor->actor.get();
         osg::Vec3 deltaHPR,tPos,actorHPR;
         osg::Quat pitchRotate,yawRotate;
         osg::Matrix newRotationMat;

         deltaHPR = camHPR - itor->initialCameraHPR;
         yawRotate.makeRotate(osg::DegreesToRadians(deltaHPR[0]),osg::Vec3(0.0f, 0.0f, 1.0f));
         pitchRotate.makeRotate(osg::DegreesToRadians(deltaHPR[1]),getRightDir());
         newRotationMat.makeRotate(itor->rotationOffset * camLookInv);
         dtUtil::MatrixUtil::MatrixToHpr(actorHPR,newRotationMat);
         tPos = (yawRotate*pitchRotate*itor->positionOffset);
         tPos += this->position;

         tProxy->SetTranslation(tPos);
         tProxy->SetRotationFromMatrix(newRotationMat);
      }
   }
}
