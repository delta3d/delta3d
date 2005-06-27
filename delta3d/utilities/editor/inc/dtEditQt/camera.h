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
#ifndef __Camera__h
#define __Camera__h

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/Matrix>

namespace dtEditQt 
{

    /**
     * The camera class provides a view into the scene.  It supports orthographic
     * and perspective views.
     */
	class Camera : public osg::Referenced 
    {
	public:

        /**
         * Constructs a new camera.
         * @note
         *  The default camera is set to perspective projection with aspect ratio=1.3, FovY=90,
         *  near clipping plane=1, far clipping plane=10000
         */
		Camera();

        /**
         * Sets the position of the camera to the position specfied in world coordinates.
         * @param pos The new position.
         */
        void setPosition(const osg::Vec3 &pos);

        /**
         * Moves the camera relative to its current position. (currentPos = currentPos + relPos).
         * @param relPos A vector containing the relative position from the current one.
         */
        void move(const osg::Vec3 &relPos);

        /**
         * Rotates the camera about its local x-axis.
         * @param degrees Amount of rotation in degrees.
         */
        void pitch(float degrees);
   
        /**
         * Rotates the camera about its up vector.
         * @param degrees Amount of rotation in degrees.
         */
		void yaw(float degrees);

        /**
         * Rotates the camera about its current view vector.
         * @param degrees Amount of rotation in degrees.
         */
		void roll(float degrees);
   
        /**
         * Rotates the camera using the given quaternion.
         * @param q The quaternion to rotate by.
         */
		void rotate(const osg::Quat &q);

        /**
         * Zooms in camera.
         * @note
         *  This is only applicable in orthographic viewing modes.  Values greater than
         *  1.0f zoom the camera in, where as values less than 1.0f zoom the camera out.
         * @param amount A floating point value slighting less than or greater than 1.0f.
         */
        void zoom(float amount);
		
        /**
         * Gets the current zoom factor.
         * @return The current zoom factor.
         */
		float getZoom() const {
			return this->zoomFactor;
		}		
		
        /**
         * Resets the camera's rotation quaternion to the identity where the positive Z
         * axis is up, the positive Y axis is forward, and the positive X axis is right.
         */
		void resetRotation();		
		
        /**
         * Sets the camera's orthographic viewing parameters and puts this camera into
         * orthographic viewing mode.
         * @param left 
         * @param right 
         * @param bottom 
         * @param top 
         * @param near 
         * @param far 
         */
		void makeOrtho(float left, float right, float bottom, float top,
			float near, float far);
			
        /**
         * Sets the camera's perspective viewing parameters and puts this camera into
         * perspective viewing mode.
         * @param fovY 
         * @param aspect 
         * @param near 
         * @param far 
         */
		void makePerspective(float fovY, float aspect, float near, float far);

        /**
         * Sets the horizontal aspect ratio of this camera.
         * @param ratio The new aspect ratio.
         */
        void setAspectRatio(float ratio);

        /**
         * Sets the value of the near clipping plane of this camera.  Anything
         * closer than or behind this point are clipped away.
         * @param value The new value.
         */
        void setNearClipPlane(float value);

        /**
         * Sets the value of the far clipping plane of this camera.  Anything
         * beyond this point is clipped away.
         * @param value The new value.
         */
        void setFarClipPlane(float value);
		
        /**
         * Gets this camera's current view direction.
         * @return A 3D vector pointing in the direction the camera is facing.
         */
		osg::Vec3 getViewDir() const;

        /**
         * Gets this camera's current up direction.
         * @return A 3D vector perpendicular to the camera's view direction.
         */
        osg::Vec3 getUpDir() const;
        
        /**
         * Gets this camera's current right direction.
         * @return A 3D vector which is the cross product of the camera's current viewing
         * direction and the current up direction.
         */
		osg::Vec3 getRightDir() const;

        /**
         * Gets this camera's orientation in a quaternion.
         * @return A quaternion representation of this camera's orientation.
         */
		const osg::Quat &getOrientation() const {
			return this->orientation;
		}

        /**
         * Gets the current position of this camera.
         * @return Returns a 3D vector corresponding to this camera's current position
         * in world coordinates.
         */
		const osg::Vec3 &getPosition() const {
			return this->position;
		}
		
        /**
         * Gets this camera's current projection matrix.
         * @return The current projection matrix.  This is constructed based on the
         * the orthographic or perspective projection settings.
         * @see makePerspective
         * @see makeOrtho
         */
		const osg::Matrix &getProjectionMatrix();
        
        /**
         * Gets the camera's current view matrix.
         * @return The view matrix is generated from its current position and its
         * current orientation.
         */
		const osg::Matrix &getWorldViewMatrix();

        /**
         * Forces an update of this camera's projection matrix and modelview
         * matrix.
         */
        void update();

	protected:
        
        /**
         * Empty destructor.
         * @return 
         */
		virtual ~Camera() { }
		
	private:
       /**
        * Enumeration of the types of modes the camera can be in.
        */
	   enum ProjectionType {
			ORTHOGRAPHIC,
			PERSPECTIVE
		};
				
		osg::Vec3 position;
		osg::Quat orientation;
		
		osg::Matrix projectionMat;
		osg::Matrix worldViewMat;
		
		float fovY, aspectRatio;
		float orthoLeft, orthoRight, orthoTop, orthoBottom;
		float zNear, zFar;
		float zoomFactor;
		
		bool updateProjectionMatrix;
		bool updateWorldViewMatrix;
	
		ProjectionType projType;
	};
}

#endif
