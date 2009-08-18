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

#ifndef DELTA_STAGEOBJECTMOTIONMODEL
#define DELTA_STAGEOBJECTMOTIONMODEL

// stageobjectmotionmodel.h: Declaration of the STAGEObjectMotionModel class.
//
////////////////////////////////////////////////////////////////////////////////

#include <dtCore/objectmotionmodel.h>
#include <dtDAL/transformableactorproxy.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * A motion model used to manipulate object positioning (used in STAGE).
 */
class STAGEObjectMotionModel : public dtCore::ObjectMotionModel
{
   DECLARE_MANAGEMENT_LAYER(STAGEObjectMotionModel)

   public:

      /**
      * Constructor.
      *
      * @param[in]  view      The View.
      */
      STAGEObjectMotionModel(dtCore::View* view);

   protected:

   public:

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
      //void OnMessage(MessageData* data);

      /**
      * Adds a target to the target list.
      *
      * @param[in]  target  The target to add.
      */
      void AddTarget(dtDAL::TransformableActorProxy* target);

      /**
      * Removes a target from the subtarget list.
      *
      * @param[in]  target  The target to remove.
      */
      void RemoveTarget(dtDAL::TransformableActorProxy* target);

      /**
      * Removes all targets from the sub target list.
      */
      void ClearTargets(void);

      /**
      * Gets the total number of sub targets.
      *
      * @return  Total number of sub targets.
      */
      int GetNumTargets(void) {return (int)mTargets.size();}

      /**
      * Gets the current sub target at an index.
      *
      * @param[in]  index  The index
      *
      * @return            The sub target at the index.
      */
      dtDAL::TransformableActorProxy* GetTarget(int index) {return mTargets[index];}

   protected:

   private:

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

      /**
      * This callback handles the action scale of the target.
      *
      * @param[in]  delta  The amount of scale to apply.
      */
      virtual void OnScale(osg::Vec3 delta);

      std::vector<dtDAL::TransformableActorProxy*> mTargets;
};

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_STAGEOBJECTMOTIONMODEL
