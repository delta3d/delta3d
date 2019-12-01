/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - camera (.h & .cpp) - Using 'The MIT License'
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

#ifndef DELTA_STAGE_CAMERA
#define DELTA_STAGE_CAMERA

#include <QtCore/QObject>
#include <osg/Referenced>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/Matrix>
#include <list>
#include <dtCore/refptr.h>
#include <dtCore/camera.h>
#include <dtCore/compass.h>

#include <dtCore/transformableactorproxy.h>
#include <dtEditQt/export.h>

namespace dtEditQt
{
   class Viewport;

   /**
    * The camera class provides a view into the scene.  It supports orthographic
    * and perspective views.
    */
   class DT_EDITQT_EXPORT StageCamera : public QObject, public osg::Referenced
   {
      Q_OBJECT

   public:
      /**
       * Constructs a new camera.
       * @note
       *  The default camera is set to perspective projection with aspect ratio=1.3, FovY=90,
       *  near clipping plane=1, far clipping plane=10000
       */
      StageCamera();

      /**
       * Sets the position of the camera to the position specified in world coordinates.
       * @param pos The new position.
       */
      void setPosition(const osg::Vec3& pos);

      /**
       * Set the orientation of the camera to the supplied rotation.
       * @param rot The rotation to orient the camera
       */
      void setRotation(const osg::Quat& rot);

      /**
       * Moves the camera relative to its orientation.
       * @param relPos A vector containing the relative position from the current one.
       */
      void move(const osg::Vec3& relPos);

      /**
       * Rotates the camera about its local x-axis.
       * @param degrees Amount of rotation in degrees.
       */
      void pitch(double degrees);

      /**
       * Rotates the camera about its up vector.
       * @param degrees Amount of rotation in degrees.
       */
      void yaw(double degrees);

      /**
       * Rotates the camera about its current view vector.
       * @param degrees Amount of rotation in degrees.
       */
      //void roll(double degrees);

      /**
       * Rotates the camera using the given quaternion.
       * @param q The quaternion to rotate by.
       */
      void rotate(const osg::Quat& q);

      /**
       * Zooms in camera.
       * @note
       *  This is only applicable in orthographic viewing modes.  Values greater than
       *  1.0f zoom the camera in, where as values less than 1.0f zoom the camera out.
       * @param amount A floating point value slighting less than or greater than 1.0f.
       */
      void zoom(double amount);

      /**
       * Gets the current zoom factor.
       * @return The current zoom factor.
       */
      double getZoom() const { return mZoomFactor; }

      /**
      * Sets the current zoom factor.
      * @param zoom The amount of zoom to set.
      */
      void setZoom(double zoom);

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
      void makeOrtho(double left, double right, double bottom, double top,
         double near, double far);

      /**
       * Sets the camera's perspective viewing parameters and puts this camera into
       * perspective viewing mode.
       * @param fovY
       * @param aspect
       * @param near
       * @param far
       */
      void makePerspective(double fovY, double aspect, double near, double far);

      /**
       * Sets the horizontal aspect ratio of this camera.
       * @param ratio The new aspect ratio.
       */
      void setAspectRatio(double ratio);

      /**
       * Gets the horizonal aspect ratio of this camera.
       */
      double getAspectRatio() const { return mAspectRatio; }

      /**
       * Sets the value of the near clipping plane of this camera.  Anything
       * closer than or behind this point are clipped away.
       * @param value The new value.
       */
      void setNearClipPlane(double value);

      /**
       * Sets the value of the far clipping plane of this camera.  Anything
       * beyond this point is clipped away.
       * @param value The new value.
       */
      void setFarClipPlane(double value);

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
      osg::Quat getOrientation() const;

      /**
       * Gets the current position of this camera.
       * @return Returns a 3D vector corresponding to this camera's current position
       * in world coordinates.
       */
      osg::Vec3 getPosition() const;

      void getOrthoParams(double& left, double& right, double& bottom, double& top,
         double& nearZ, double& farZ);

      /**
       * Retrieves the delta representation of the camera.
       */
      dtCore::Camera* getDeltaCamera() {return mDeltaCamera.get();}

      /**
       * Updates the camera's viewing and projection matrices.  Also updates
       * any actors currently attached to it.
       */
      void update();

      /**
       * Attaches a transformableactorproxy to the camera.
       * @param proxy The proxy to attach.
       */
      void attachActorProxy(dtCore::TransformableActorProxy* proxy);

      /**
       * Removes a transformableactorproxy that is currently attached to the camera.
       * @param proxy The proxy to remove from this camera.
       */
      void detachActorProxy(dtCore::TransformableActorProxy* proxy);

      /**
       * Removes all actor proxies currently attached to this camera.
       */
      void removeAllActorAttachments();

      /**
       * Invoked when the camera is moved or rotated.  This updates any attachments
       * on this camera.
       */
      void updateActorAttachments();

      /**
       *	Shows the compass actor to be shown within the camera's view.
       */
      dtCore::Compass* ShowCompass();

      /**
       * Returns a count of the number of actor proxies currently attached to this
       * camera.
       * @return The number of proxies.
       */
      unsigned int getNumActorAttachments() const
      {
         return mAttachedProxies.size();
      }

   signals:
      ///Signal that the camera has moved to new position
      void PositionMoved(double x, double y, double z);

   protected:
      struct ActorAttachment
      {
         dtCore::RefPtr<dtCore::TransformableActorProxy> mActor;
         osg::Vec3 mPositionOffset;
         osg::Quat mRotationOffset;
         osg::Vec3 mInitialCameraHPR;
      };

      /**
       * Empty destructor.
       * @return
       */
      virtual ~StageCamera() {}

   private:
      /**
       * Enumeration of the types of modes the camera can be in.
       */
      enum ProjectionType
      {
         ORTHOGRAPHIC,
         PERSPECTIVE
      };

      osg::Matrixd mProjectionMat;
      osg::Matrixd mWorldViewMat;

      double mFovY, mAspectRatio;
      double mOrthoLeft, mOrthoRight, mOrthoTop, mOrthoBottom;
      double mZNear, mZFar;
      double mZoomFactor;

      ProjectionType mProjType;

      dtCore::RefPtr<dtCore::Camera> mDeltaCamera;

      ///A list of transformable actor proxies currently attached to the camera.
      std::list<ActorAttachment> mAttachedProxies;

      ///An optional compass actor to show the orientation of the camera.
      dtCore::RefPtr<dtCore::Compass> mCompass;
   };

} // namespace dtEditQt

#endif // DELTA_STAGE_CAMERA
