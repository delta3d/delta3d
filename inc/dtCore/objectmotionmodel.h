/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeffrey P Houde
 */

#ifndef DELTA_OBJECTMOTIONMODEL
#define DELTA_OBJECTMOTIONMODEL

// objectmotionmodel.h: Declaration of the ObjectMotionModel class.
//
////////////////////////////////////////////////////////////////////////////////

#include <dtCore/view.h>
#include <dtCore/transformable.h>
#include <dtCore/motionmodel.h>

#include <osg/Group>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>

////////////////////////////////////////////////////////////////////////////////

namespace dtCore
{
   class Mouse;

   /**
    * A motion model used to manipulate object positioning (used in editors).
    */
   class DT_CORE_EXPORT ObjectMotionModel : public MotionModel
   {
      DECLARE_MANAGEMENT_LAYER(ObjectMotionModel)

   public:
      // The coordinate space.
      enum CoordinateSpace
      {
         LOCAL_SPACE,
         WORLD_SPACE,
      };

      // Used to determine how the object will be manipulated.
      enum MotionType
      {
         MOTION_TYPE_TRANSLATION,
         MOTION_TYPE_ROTATION,
         MOTION_TYPE_SCALE,
         MOTION_TYPE_PLANE_TRANSLATION,
         MOTION_TYPE_MAX,
      };

      // The motion arrow types.
      enum ArrowType
      {
         ARROW_TYPE_AT,          // green (Y)
         ARROW_TYPE_RIGHT,       // red   (X)
         ARROW_TYPE_UP,          // blue  (Z)
         ARROW_TYPE_MAX,
         ARROW_TYPE_ALL,         // white (XYZ)
      };

      /**
       * Constructor.
       *
       * @param[in]  view      The View.
       */
      ObjectMotionModel(dtCore::View* view);

   protected:
      /**
       * Destructor.
       */
      virtual ~ObjectMotionModel();

   public:
      /**
       * Sets the target of this motion model.
       *
       * @param target the new target
       */
      virtual void SetTarget(Transformable* target);

      /**
       * Sets the current view.
       *
       * @param[in]  view  The View.
       */
      virtual void SetView(dtCore::View* view = NULL);

      /**
       * Sets the current scene node.
       */
      virtual void SetSceneNode(osg::Group* sceneNode = NULL);

      /**
       * Sets the current camera.
       *
       * @param[in]  camera  The camera.
       */
      virtual void SetCamera(dtCore::Camera* camera = NULL);

      /**
       * Enables or disables this motion model.
       *
       * @param[in]  enabled  True to enable this motion model, false
       *                      to disable it
       */
      virtual void SetEnabled(bool enabled);

      /**
       * Enables the interaction of the motion model.
       *
       * @param[in]  enabled  True to enable interaction.
       */
      virtual void SetInteractionEnabled(bool enabled);

      /**
       * Enables or disables the scale gizmos.
       *
       * @param[in]  enabled  True to enable the scale gizmos.
       */
      virtual void SetScaleEnabled(bool enabled);

      /**
       * Sets the scale of the arrows displayed.
       *
       * @param[in]  scale  The scale value.
       */
      virtual void SetScale(float scale);

      /**
       * Gets the current scale of the model.
       */
      virtual float GetScale(void) {return mScale;}

      /**
       * Calculates the desired scale based on auto scale.
       */
      float GetAutoScaleSize(void);

      /**
       * Retrieves the distance of the camera to the target.
       */
      float GetCameraDistanceToTarget(void);

      /**
       * Sets whether the motion model should be scaled
       * based on distance from the camera.
       */
      virtual void SetAutoScaleEnabled(bool enabled);

      /**
       * Gets the current coordinate space.
       *
       * @return  The current coordinate space.
       */
      CoordinateSpace GetCoordinateSpace(void);

      /**
       * Sets the maximum translation distance of the object based
       * on the camera position.
       *
       * @param[in]  distance  The maximum distance from the camera.
       *                       Use -1 for infinite distance.
       */
      void SetMaxObjectDistanceFromCamera(float distance = -1);

      /**
       * Sets the current coordinate space.
       *
       * @param[in]  coordinateSpace  The new coordinate space.
       */
      void SetCoordinateSpace(CoordinateSpace coordinateSpace);

      /**
       * Sets the snap translation increment.
       *
       * @param[in]  increment  The increment length per snap position.
       */
      void SetSnapTranslation(float increment);

      /**
       * Sets the snap rotational degree increment.
       *
       * @param[in]  degrees  The number of degrees between snap positions.
       */
      void SetSnapRotation(float degrees);

      /**
       * Sets the snap scale increment.
       *
       * @param[in]  increment  The increment length per snap position.
       */
      void SetSnapScale(float increment);

      /**
       * Sets snap mode enabled.
       *
       * @param[in]  enabled  True to enable.
       */
      void SetSnapEnabled(bool translation, bool rotation, bool scale);

      /**
       * Gets the current position of the mouse.
       *
       * @return    The position of the mouse.
       */
      osg::Vec2 GetMousePosition(void);

      /**
       * Retrieves the current motion type.
       */
      MotionType GetMotionType() { return mMotionType; }

      /**
       * Presses the left mouse button.
       * Should only use this if Delta3D is not receiving a regular tick update.
       */
      virtual void OnLeftMousePressed(void);

      /**
       * Releases the left mouse button.
       * Should only use this if Delta3D is not receiving a regular tick update.
       */
      virtual void OnLeftMouseReleased(void);

      /**
       * Presses the right mouse button.
       * Should only use this if Delta3D is not receiving a regular tick update.
       */
      virtual void OnRightMousePressed(void);

      /**
       * Releases the right mouse button.
       * Should only use this if Delta3D is not receiving a regular tick update.
       */
      virtual void OnRightMouseReleased(void);

      /**
       * Message handler callback.
       *
       * @param[in] data  The message data
       */
      void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

      /**
       * Updates the motion model.
       * Should only use this if Delta3D is not receiving a regular tick update.
       *
       * @param[in]  pos  The current position of the mouse.
       *
       * @return          Returns the current motion type activated.
       */
      MotionType Update(const osg::Vec2& pos);

      /**
       * Updates position the motion model widgets.
       */
      void UpdateWidgets(void);

   protected:
      /**
       * Initialize our three Axes arrows.
       */
      virtual void InitArrows(void);

      /**
       * Updates the visibility of the motion model widgets.
       */
      void UpdateVisibility();

      /**
       * Generates a triangle mesh used for the rotation rings.
       *
       * @param[in]   minRadius  The inner radius of the ring.
       * @param[in]   maxRadius  The outer radius of the ring.
       * @param[in]   segments   The number of vertices around the ring.
       *
       * @return                 The generated TriangleMesh.
       */
      virtual osg::TriangleMesh* GenerateRing(float minRadius, float maxRadius, int segments);

      /**
       *	Generates a triangle mesh used for the planar translation.
       *
       * @param[in]  inner  The length from the origin in which the plane will start.
       * @param[in]  outer  The length from the origin in which the plane will end.
       */
      virtual osg::TriangleMesh* GeneratePlane(float inner, float outer);

      /**
       * Does a collision test to see if the mouse has picked
       * one of the widgets of this motion model.
       * mMotionType and mHoverArrow will be set to
       * the proper types based on collision.
       *
       * @return  The drawable widget that was picked.
       */
      dtCore::DeltaDrawable* MousePick(void);

      /**
       * Calculates the 3D collision line that represents the mouse.
       *
       * @param[in]  mousePos  The position of the mouse in screen coords.
       * @param[in]  start     The start position of the line.
       * @param[in]  end       The end position of the line.
       */
      virtual void GetMouseLine(const osg::Vec2& mousePos, osg::Vec3& start, osg::Vec3& end);

      /**
       * Calculates the screen coordinates of a 3d position in the world.
       *
       * @param[in]  objectPos  The position of the object in 3d space.
       *
       * @return                The position of the object in screen coords.
       */
      virtual osg::Vec2 GetObjectScreenCoordinates(const osg::Vec3& objectPos);

      /**
       * Tests if a given delta drawable belongs to this motion model.
       *
       * @param[in]  drawable  The drawable to test.
       *
       * @return               True if the drawable belongs to this motion model.
       */
      bool HighlightWidgets(dtCore::DeltaDrawable* drawable);

      /**
       * Sets an arrow as highlighted or not.
       *
       * @param[in]  arrowType  The arrow to highlight
       *                        (ARROW_TYPE_MAX to un-highlight all).
       *                        (ARROW_TYPE_ALL to highlight the scale all gizmo).
       */
      void SetArrowHighlight(ArrowType arrowType);

      /**
       * Updates Translation, Rotation, and Scale of the target object.
       */
      void UpdateTranslation();
      void UpdateRotation(void);
      void UpdateScale(void);
      void UpdatePlanarTranslation(void);

      /**
       * This callback handles the actual translation of the target.
       *
       * @param[in]  delta  The amount of translation to be performed.
       */
      virtual void OnTranslate(const osg::Vec3& delta);

      /**
       * Can be used by derivative classes to know when a Translation event
       * begins.
       */
      virtual void OnTranslateBegin() {};

      /**
       * Can be used by derivative classes to know when a Translation event
       * ends.
       */
      virtual void OnTranslateEnd() {};

      /**
       * This callback handles the actual rotation of the target.
       *
       * @param[in]  delta  The amount of rotation to apply.
       * @param[in]  axis   The axis of rotation.
       */
      virtual void OnRotate(float delta, const osg::Vec3& axis);

      /**
       * Can be used by derivative classes to know when a Rotation event
       * begins.
       */
      virtual void OnRotateBegin() {};

      /**
       * Can be used by derivative classes to know when a Rotation event
       * ends.
       */
      virtual void OnRotateEnd() {};

      /**
       * This callback handles the action scale of the target.
       *
       * @param[in]  delta  The amount of scale to apply.
       */
      virtual void OnScale(const osg::Vec3& delta);

      /**
       * Can be used by derivative classes to know when a Scale event
       * begins.
       */
      virtual void OnScaleBegin() {};

      /**
       * Can be used by derivative classes to know when a Scale event
       * ends.
       */
      virtual void OnScaleEnd() {};

      struct ArrowData
      {
         dtCore::RefPtr<dtCore::Transformable>  translationTransform;
         dtCore::RefPtr<dtCore::Transformable>  rotationTransform;
         dtCore::RefPtr<dtCore::Transformable>  scaleTransform;
         dtCore::RefPtr<dtCore::Transformable>  planarTransform;

         osg::ref_ptr<osg::Geode>               arrowGeode;
         osg::ref_ptr<osg::ShapeDrawable>       arrowCylinder;
         osg::ref_ptr<osg::ShapeDrawable>       arrowCone;

         osg::ref_ptr<osg::Geode>               rotationGeode;
         osg::ref_ptr<osg::Geode>               rotationSelectionGeode;
         osg::ref_ptr<osg::ShapeDrawable>       rotationRing;

         osg::ref_ptr<osg::Geode>               scaleGeode;
         osg::ref_ptr<osg::ShapeDrawable>       scaleBox;

         osg::ref_ptr<osg::Geode>               planarGeode;
         osg::ref_ptr<osg::Geode>               planarSelectionGeode;
         osg::ref_ptr<osg::ShapeDrawable>       planarGeom;

         osg::Vec4                              arrowCylinderColor;
         osg::Vec4                              arrowConeColor;
         osg::Vec4                              planarColor;
      };

      dtCore::RefPtr<dtCore::Transformable>     mScaleTransform;
      osg::ref_ptr<osg::Geode>                  mScaleGeode;
      osg::ref_ptr<osg::ShapeDrawable>          mScaleOrb;

      dtCore::RefPtr<dtCore::Transformable>     mAngleOriginTransform;
      osg::ref_ptr<osg::Geode>                  mAngleOriginGeode;
      osg::ref_ptr<osg::Cylinder>               mAngleOriginCylinder;
      osg::ref_ptr<osg::ShapeDrawable>          mAngleOriginDrawable;

      dtCore::RefPtr<dtCore::Transformable>     mAngleTransform;
      osg::ref_ptr<osg::Geode>                  mAngleGeode;
      osg::ref_ptr<osg::Cylinder>               mAngleCylinder;
      osg::ref_ptr<osg::ShapeDrawable>          mAngleDrawable;

      osg::ref_ptr<osg::AutoTransform>          mTargetTransform;

      ArrowData       mArrows[ARROW_TYPE_MAX];
      float           mScale;

      bool            mVisible;
      bool            mInteractionEnabled;

      float           mCurScale;
      osg::Quat       mCurQuat;

      dtCore::View*   mView;
      osg::Group*     mSceneNode;
      Mouse*          mMouse;

      dtCore::Camera* mCamera;
      float           mMaxDistance;

      osg::Vec2       mMousePos;
      bool            mIsPixelMousePos;

      CoordinateSpace mCoordinateSpace;
      MotionType      mMotionType;
      ArrowType       mHoverArrow;

      bool            mLeftMouse;
      bool            mRightMouse;

      bool            mMouseDown;
      bool            mMouseLocked;
      osg::Vec2       mMouseOffset;
      osg::Vec2       mMouseOrigin;
      float           mOriginAngle;

      bool            mAutoScale;
      bool            mAllowScaleGizmo;

      osg::Vec3       mOriginalTargetPos;

      float           mSnapTranslation;
      float           mSnapRotation;
      float           mSnapScale;
      bool            mSnap;

      bool            mSnapTranslationEnabled;
      bool            mSnapRotationEnabled;
      bool            mSnapScaleEnabled;
   };
} // namespace dtCore

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_OBJECTMOTIONMODEL
