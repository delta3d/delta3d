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

namespace dtEditQt 
{

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
        this->updateWorldViewMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::move(const osg::Vec3 &relPos)
    {
        this->position += relPos;
        this->updateWorldViewMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::pitch(float degrees)
    {
        osg::Vec3 xAxis;
        osg::Quat q;

        xAxis = this->orientation.conj()*osg::Vec3(1,0,0);
        q.makeRotate(osg::DegreesToRadians(-degrees),xAxis);
        rotate(q);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::yaw(float degrees)
    {
        osg::Vec3 yAxis;
        osg::Quat q;

        yAxis = osg::Vec3(0,0,1);
        q.makeRotate(osg::DegreesToRadians(-degrees),yAxis);
        rotate(q);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::roll(float degrees)
    {
        osg::Vec3 zAxis;
        osg::Quat q;

        zAxis = this->orientation.conj()*osg::Vec3(0,1,0);
        q.makeRotate(osg::DegreesToRadians(-degrees),zAxis);
        rotate(q);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::makeOrtho(float left, float right, float bottom, float top,
                        float near, float far)
    {
        this->orthoLeft = left;
        this->orthoRight = right;
        this->orthoBottom = bottom;
        this->orthoTop = top;
        this->zNear = near;
        this->zFar = far;
        this->updateProjectionMatrix = true;
        this->projType = ORTHOGRAPHIC;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::makePerspective(float fovY, float aspect, float near, float far)
    {
        this->fovY = fovY;
        this->aspectRatio = aspect;
        this->zNear = near;
        this->zFar = far;
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
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::resetRotation()
    {
        this->orientation.set(0,0,0,1);
        pitch(90);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::zoom(float amount)
    {
        this->zoomFactor *= amount;
        if (this->zoomFactor < 0.5f)
            this->zoomFactor = 0.5f;
        this->updateProjectionMatrix = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Camera::update()
    {
        this->updateProjectionMatrix = true;
        this->updateWorldViewMatrix = true;
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
            osg::Matrix tx;
            osg::Matrix rotate;

            tx.makeTranslate(-this->position);
            this->orientation.get(rotate);
            this->worldViewMat = tx*rotate;
            this->updateWorldViewMatrix = false;
        }

        return this->worldViewMat;
    }
}
