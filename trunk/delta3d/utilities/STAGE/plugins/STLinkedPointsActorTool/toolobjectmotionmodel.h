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

#ifndef DELTA_TOOLOBJECTMOTIONMODEL
#define DELTA_TOOLOBJECTMOTIONMODEL

// toolobjectmotionmodel.h: Declaration of the ToolObjectMotionModel class.
//
////////////////////////////////////////////////////////////////////////////////

#include <dtCore/objectmotionmodel.h>
#include <dtDAL/transformableactorproxy.h>

#include <dtDAL/functor.h>

namespace dtActors
{
   class LinkedPointsActor;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * A motion model used to manipulate object positioning (used in STAGE Linked Points Actor Tool).
 */
class ToolObjectMotionModel : public dtCore::ObjectMotionModel
{
   DECLARE_MANAGEMENT_LAYER(ToolObjectMotionModel)

   typedef dtUtil::Functor<void, TYPELIST_3(osg::Vec2, osg::Vec3&, osg::Vec3&)> GetMouseLineFunc;
   typedef dtUtil::Functor<osg::Vec2, TYPELIST_1(osg::Vec3)> ObjectToScreenFunc;

   public:

      /**
      * Constructor.
      *
      * @param[in]  view      The View.
      */
      ToolObjectMotionModel(dtCore::View* view);

   protected:

   public:

      /**
      * Sets the Linked Points Actor point index.
      */
      void SetPointIndex(int index);

      /**
      * Sets the Active Linked Points Actor.
      */
      void SetActiveActor(dtActors::LinkedPointsActor* actor);

      /**
      * Presses the left mouse button.
      * Should only use this if Delta3D is not receiving a regular tick update.
      */
      void OnLeftMousePressed(void);

      /**
      * Releases the left mouse button.
      * Should only use this if Delta3D is not receiving a regular tick update.
      */
      void OnLeftMouseReleased(void);

      /**
      * Presses the right mouse button.
      * Should only use this if Delta3D is not receiving a regular tick update.
      */
      void OnRightMousePressed(void);

      /**
      * Releases the right mouse button.
      * Should only use this if Delta3D is not receiving a regular tick update.
      */
      void OnRightMouseReleased(void);

      /**
      * Message handler callback.
      *
      * @param[in] data  The message data
      */
      void OnMessage(MessageData* data);

      /**
      * Resets the GetMouseLine callback function.
      */
      void ResetGetMouseLineFunc(void);

      /**
      * Sets the GetMouseLine callback function.
      *
      * @param[in]  func  The functor.
      */
      void SetGetMouseLineFunc(GetMouseLineFunc func);

      /**
      * Resets the Object to screen callback function.
      */
      void ResetObjectToScreenFunc(void);

      /**
      * Sets the Object to Screen callback function.
      *
      * @param[in]  func  The functor.
      */
      void SetObjectToScreenFunc(ObjectToScreenFunc func);

   protected:

      /**
      * Calculates the 3D collision line that represents the mouse.
      *
      * @param[in]  mousePos  The position of the mouse in screen coords.
      * @param[in]  start     The start position of the line.
      * @param[in]  end       The end position of the line.
      */
      virtual void GetMouseLine(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end);

      /**
      * Calculates the screen coordinates of a 3d position in the world.
      *
      * @param[in]  objectPos  The position of the object in 3d space.
      *
      * @return                The position of the object in screen coords.
      */
      virtual osg::Vec2 GetObjectScreenCoordinates(osg::Vec3 objectPos);

   private:

      /**
      * This is the default callback for calculating the 3D collision
      * line that represents the mouse.
      *
      * @param[in]  mousePos  The position of the mouse in screen coords.
      * @param[in]  start     The start position of the line.
      * @param[in]  end       The end position of the line.
      */
      void DefaultGetMouseLineFunc(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end);

      /**
      * This is the default callback for calculating the screen
      * coordinates of a 3d position in the world.
      *
      * @param[in]  objectPos  The position of the object in 3d space.
      *
      * @return                The position of the object in screen coords.
      */
      osg::Vec2 DefaultGetObjectScreenCoordinates(osg::Vec3 objectPos);

      /**
      * This callback handles the actual translation of the target.
      *
      * @param[in]  delta  The amount of translation to be performed.
      */
      virtual void OnTranslate(osg::Vec3 delta);

      /**
      * This callback handles the actual rotation of the target.
      *
      * @param[in]  delta  The amount of rotation to apply.
      * @param[in]  axis   The axis of rotation.
      */
      virtual void OnRotate(float delta, osg::Vec3 axis);

      GetMouseLineFunc mGetMouseLineFunc;
      ObjectToScreenFunc mObjectToScreenFunc;

      int   mPointIndex;

      dtActors::LinkedPointsActor* mActiveActor;
};

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_TOOLOBJECTMOTIONMODEL
