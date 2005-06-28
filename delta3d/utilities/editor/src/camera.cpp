/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Matthew W. Campbell
*/
#include <osg/Math>
#include "dtEditQt/camera.h"

#include <iostream>

namespace dtEditQt
{
    std::ostream &operator<<(std::ostream &o, osg::Vec3 v)
    {
        o << "[" << v.x() << " , " << v.y() << " , " << v.z() << "]";
        return o;
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::ostream &operator<<(std::ostream &o, Camera &c)
    {
        o << "Camera: " << std::endl << '\t' << "Position: " <<
            c.getPosition().x() << "," << c.getPosition().y() << "," << c.getPosition().z() << std::endl <<
            '\t' << "ViewDir: " << c.getViewDir().x() << "," << c.getViewDir().y() << "," << c.getViewDir().z() << std::endl <<
            '\t' << "UpDir: " << c.getUpDir().x() << "," << c.getUpDir().y() << "," << c.getUpDir().z() << std::endl <<
            '\t' << "RightDir: " << c.getRightDir().x() << "," << c.getRightDir().y() << "," << c.getRightDir().z();

        return o;
    }

    ///////////////////////////////////////////////////////////////////////////////
    Camera::Camera()
    {
        this->updateProjectionMatrix = true;
        this->updateWorldViewMatrix = true;
        this->fovY = 60.0f;
        this->aspectRatio = 1.3333333333f;
        this->zNear = 1.0f;
        this->zFar = 10000.0f;
        this->zoomFactor = 1.0f;
        this->projType = PERSPECTIVE;
        resetRotation();
        setPosition(osg::Vec3(0,0,0));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::setPosition(const osg::Vec3 &pos)
    {
        this->position = pos;
        updateActorAttachments();
        this->updateWorldViewMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::move(const osg::Vec3 &relPos)
    {
        this->position += relPos;
        updateActorAttachments();
        this->updateWorldViewMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::pitch(float degrees)
    {
        osg::Quat q;
        q.makeRotate(osg::DegreesToRadians(-degrees),getRightDir());
        rotate(q);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::yaw(float degrees)
    {
        osg::Quat q;
        q.makeRotate(osg::DegreesToRadians(-degrees),osg::Vec3(0,0,1));
        rotate(q);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::roll(float degrees)
    {
        osg::Quat q;
        q.makeRotate(osg::DegreesToRadians(-degrees),getViewDir());
        rotate(q);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::makeOrtho(float left, float right, float bottom, float top,
                        float nearZ, float farZ)
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
    void Camera::makePerspective(float fovY, float aspect, float nearZ, float farZ)
    {
        this->fovY = fovY;
        this->aspectRatio = aspect;
        this->zNear = nearZ;
        this->zFar = farZ;
        this->updateProjectionMatrix = true;
        this->projType = PERSPECTIVE;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::setNearClipPlane(float value)
    {
        this->zNear = value;
        this->updateProjectionMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::setFarClipPlane(float value)
    {
        this->zFar = value;
        this->updateProjectionMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::setAspectRatio(float ratio)
    {
        this->aspectRatio = ratio;
        this->updateProjectionMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 Camera::getViewDir() const
    {
        return this->orientation.conj() * osg::Vec3(0,0,-1);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 Camera::getUpDir() const
    {
        return this->orientation.conj() * osg::Vec3(0,1,0);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 Camera::getRightDir() const
    {
        return this->orientation.conj() * osg::Vec3(1,0,0);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::rotate(const osg::Quat &q)
    {
        this->orientation = q*this->orientation;
        this->updateWorldViewMatrix = true;
        updateActorAttachments();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::resetRotation()
    {
        this->orientation = osg::Quat(osg::DegreesToRadians(-90.0f),osg::Vec3(1,0,0));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::zoom(float amount)
    {
        this->zoomFactor *= amount;
        if (this->zoomFactor < 0.2f)
            this->zoomFactor = 0.2f;
        this->updateProjectionMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::update()
    {
        this->updateProjectionMatrix = true;
        this->updateWorldViewMatrix = true;
        updateActorAttachments();
        getProjectionMatrix();
        getWorldViewMatrix();
    }

    ///////////////////////////////////////////////////////////////////////////////
    const osg::Matrix &Camera::getProjectionMatrix()
    {
        if (this->updateProjectionMatrix) {
            if (this->projType == PERSPECTIVE) {
                this->projectionMat.makePerspective(this->fovY,this->aspectRatio,
                    this->zNear,this->zFar);
            }
            else if (this->projType == ORTHOGRAPHIC) {
                this->projectionMat.makeOrtho(this->orthoLeft/this->zoomFactor,
                                                        this->orthoRight/this->zoomFactor,
                                                        this->orthoBottom/this->zoomFactor,
                                                        this->orthoTop/this->zoomFactor,
                                                        this->zNear,this->zFar);
            }

            this->updateProjectionMatrix = false;
        }

        return this->projectionMat;
    }

    ///////////////////////////////////////////////////////////////////////////////
    const osg::Matrix &Camera::getWorldViewMatrix()
    {
        if (this->updateWorldViewMatrix) {
            this->worldViewMat = osg::Matrix::translate(-this->position) *
                osg::Matrix::rotate(this->orientation);
            this->updateWorldViewMatrix = false;
        }

        return this->worldViewMat;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::attachActorProxy(dtDAL::TransformableActorProxy *proxy)
    {
         dtCore::Transformable *transformable =
             dynamic_cast<dtCore::Transformable *>(proxy->GetActor());
         dtCore::Transform tx;
         osg::Matrix orig;
         osg::Vec3 trans;

         transformable->GetTransform(&tx);
         tx.Get(orig);
         tx.GetTranslation(trans);
         std::cout << "Before: " << trans << std::endl;

         orig.postMult(osg::Matrix::inverse(getWorldViewMatrix()));

         tx.Set(orig);
         tx.GetTranslation(trans);
         std::cout << "After: " << trans << std::endl;
         //transformable->SetTransform(&tx);

        this->attachedProxies.push_back(proxy);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::detachActorProxy(dtDAL::TransformableActorProxy *proxy)
    {
        AttachmentList::iterator itor;
        for (itor=this->attachedProxies.begin(); itor!=this->attachedProxies.end(); ++itor)
        {
            dtDAL::TransformableActorProxy *toRemove = itor->get();
            if (toRemove == proxy) {
                this->attachedProxies.erase(itor);
                break;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::removeAllActorAttachments()
    {
        this->attachedProxies.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::updateActorAttachments()
    {
        if (this->attachedProxies.empty())
            return;

        AttachmentList::iterator itor;
        const osg::Matrix camMatrix = osg::Matrix::inverse(getWorldViewMatrix());
        osg::Matrix proxyMat;
        dtCore::Transform tx;

        for (itor=this->attachedProxies.begin(); itor!=this->attachedProxies.end(); ++itor)
        {
            dtDAL::TransformableActorProxy *tProxy = itor->get();
            dtCore::Transformable *transformable =
                dynamic_cast<dtCore::Transformable *>(tProxy->GetActor());

            if (transformable != NULL)
            {
                osg::Vec3 trans;
                osg::Vec3 hpr;
                osg::Matrix relProxyMat;

                //trans = tProxy->getTranslation();
                //trans -= this->position;


                //First move the actor into camera space.
//                 transformable->GetTransform(&tx);
//                 tx.Get(proxyMat);
//                 relProxyMat = proxyMat * camMatrix;
//
//                 proxyMat = getWorldViewMatrix() * relProxyMat;
//                 tx.Set(proxyMat);
//                 transformable->SetTransform(&tx);

                //tx.GetTranslation(trans);
                //tx.GetRotation(hpr);

//                 std::cout << "Before: Trans: " << trans.x() << "," << trans.y() <<
//                     "," << trans.z() << " Look: " << hpr.x() << "," << hpr.y() <<
//                     "," << hpr.z() << std::endl;


                //proxyMat.postMult(camMatrixInv);
                //tx.Set(proxyMat);

//                 tx.GetTranslation(trans);
//                 tx.GetRotation(hpr);
//                 std::cout << "After: Trans: " << trans.x() << "," << trans.y() <<
//                     "," << trans.z() << " Look: " << hpr.x() << "," << hpr.y() <<
//                     "," << hpr.z() << std::endl;
//
//                 transformable->SetTransform(&tx);
            }
        }
    }
}
