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
#include <dtCore/scene.h>
#include <dtCore/object.h>
#include <dtCore/transformable.h>
#include <dtCore/motionmodel.h>

#include <osg/Group>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>

////////////////////////////////////////////////////////////////////////////////

namespace dtCore
{
   class Keyboard;
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
            MOTION_TYPE_MAX,
         };

         // The motion arrow types.
         enum ArrowType
         {
            ARROW_TYPE_AT,          // green (Y)
            ARROW_TYPE_RIGHT,       // red   (X)
            ARROW_TYPE_UP,          // blue  (Z)
            //ARROW_TYPE_ALL,         // white (XYZ)
            ARROW_TYPE_MAX,
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
         * Sets the current view.
         *
         * @param[in]  view  The View.
         */
         virtual void SetView(dtCore::View* view = NULL);

         /**
         * Enables or disables this motion model.
         *
         * @param[in] enabled  True to enable this motion model, false
         *                      to disable it
         */
         virtual void SetEnabled(bool enabled);

         /**
         * Sets the scale of the arrows displayed.
         *
         * @param[in]  scale  The scale value.
         */
         virtual void SetScale(float scale);

         /**
         * Gets the current motion type of the motion model.
         *
         * @return    The current motion type.
         */
         MotionType GetMotionType(void);

         /**
         * Sets the current motion type of the motion model.
         *
         * @param[in]  motionType  The new motion type.
         */
         void SetMotionType(MotionType motionType);

         /**
         * Gets the current coordinate space.
         *
         * @return  The current coordinate space.
         */
         CoordinateSpace GetCoordinateSpace(void);

         /**
         * Sets the current coordinate space.
         *
         * @param[in]  coordinateSpace  The new coordinate space.
         */
         void SetCoordinateSpace(CoordinateSpace coordinateSpace);

         /**
          * Message handler callback.
          *
          * @param[in] data  The message data
          */
         virtual void OnMessage(MessageData* data);

      protected:

      private:

         /**
         * Initialize our three Axes arrows.
         */
         virtual void InitArrows(void);

         /**
         * Converts the current mouse position to a 3D vector in the world.
         *
         * @param[in]  mousePos  The position of the mouse.
         *
         * @return  The mouse vector.
         */
         osg::Vec3 GetMouseVector(osg::Vec2 mousePos);

         /**
         * Converts a 3D object position to window screen coordinates.
         */
         osg::Vec2 ObjectToScreenCoords(osg::Vec3 objectPosition);

         /**
         * Sets an arrow as highlighted or not.
         *
         * @param[in]  arrowType  The arrow to highlight (ARROW_TYPE_MAX to unhighlight all).
         */
         void SetArrowHighlight(ArrowType arrowType);

         /**
         * Updates Translation, Rotation, and Scale of the target object.
         */
         void UpdateTranslation(void);
         void UpdateRotation(void);
         void UpdateScale(void);

         struct ArrowData
         {
            dtCore::RefPtr<dtCore::Transformable>  transformable;
            osg::ref_ptr<osg::Geode>               arrowGeode;
            osg::ref_ptr<osg::ShapeDrawable>       arrowCylinder;
            osg::ref_ptr<osg::ShapeDrawable>       arrowCone;

            osg::ref_ptr<osg::ShapeDrawable>       rotationRing;

            osg::Vec4                              arrowCylinderColor;
            osg::Vec4                              arrowConeColor;
         };

         dtCore::RefPtr<dtCore::Transformable>     mAngleOriginTransform;
         osg::ref_ptr<osg::Geode>                  mAngleOriginGeode;
         osg::ref_ptr<osg::Cylinder>               mAngleOriginCylinder;
         osg::ref_ptr<osg::ShapeDrawable>          mAngleOriginDrawable;

         dtCore::RefPtr<dtCore::Transformable>     mAngleTransform;
         osg::ref_ptr<osg::Geode>                  mAngleGeode;
         osg::ref_ptr<osg::Cylinder>               mAngleCylinder;
         osg::ref_ptr<osg::ShapeDrawable>          mAngleDrawable;

         dtCore::RefPtr<dtCore::Transformable>     mTargetTransform;

         ArrowData   mArrows[ARROW_TYPE_MAX];
         float       mScale;

         dtCore::View*  mView;
         dtCore::Scene* mScene;
         Keyboard*      mKeyboard;
         Mouse*         mMouse;

         CoordinateSpace mCoordinateSpace;
         MotionType      mMotionType;
         ArrowType       mHoverArrow;
         ArrowType       mCurrentArrow;
         bool            mMouseDown;
         bool            mMouseLocked;
         osg::Vec2       mMouseOffset;
         osg::Vec2       mMouseOrigin;
         float           mOriginAngle;
   };
} // namespace dtCore

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_OBJECTMOTIONMODEL
