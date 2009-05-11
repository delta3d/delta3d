/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.a
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author Michael Guerrero
 */

#ifndef PROCEDURAL_ANIMATION_ACTOR_H_
#define PROCEDURAL_ANIMATION_ACTOR_H_

////////////////////////////////////////////////////////////////////////////////

#include <dtAnim/animationgameactor.h>
#include <dtAnim/posemeshutility.h>
#include <dtAnim/posemesh.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/datatype.h>
#include <dtDAL/namedparameter.h>
#include <dtCore/timer.h>
#include <dtUtil/enumeration.h>

#include "testexport.h"

namespace dtAnim 
{
   class PoseMeshDatabase; 
   class PoseMeshUtility;
}

namespace dtGame
{
   class TickMessage;
}

class ProceduralAnimationActorProxy;

////////////////////////////////////////////////////////////////////////////////

class TEST_ANIM_EXPORT ProceduralAnimationActor : public dtAnim::AnimationGameActor
{
public:

   // This class is designed to work with pose meshes
   // that cover both watching and aiming
   enum eMode
   {
      MODE_WATCH,
      MODE_AIM
   };

   ProceduralAnimationActor(ProceduralAnimationActorProxy& proxy);

   /// Set the source of IK/procedural information
   void SetPoseMeshDatabase(dtAnim::PoseMeshDatabase* poseMeshDatabase);

   void SetMode(eMode mode) { mMode = mode; }

   // Set the point of interest and an option offset from the target
   void SetTarget(const dtCore::Transformable* target, osg::Vec3* offset = NULL);

   // Set how long the procedural animations should take
   void SetBlendTime(float blendTime);

   /*virtual*/ void OnTickLocal(const dtGame::TickMessage& tickMessage);

   /* virtual */ void AddedToScene(dtCore::Scene* scene);

   /// Get the animation model
   dtAnim::Cal3DModelWrapper* GetModelWrapper();
   
protected:

   /// Destructor
   virtual ~ProceduralAnimationActor();

   virtual void OnEnteredWorld();

private:

   struct ProceduralAnimationData
   {
      // This actor is set up to work with pose meshes 
      // for each of the following parts
      enum ePoseMeshPart
      {  
         PMP_FIRST = 0,

         LEFT_EYE = PMP_FIRST,
         RIGHT_EYE, 
         HEAD, 
         TORSO, 
         GUN, 

         PMP_TOTAL
      };

      dtAnim::PoseMesh* mPoseMeshes[PMP_TOTAL];
      dtAnim::PoseMesh::TargetTriangle mTargetTriangles[PMP_TOTAL];
   };

   eMode mMode;

   // Animation IK system variables
   dtAnim::PoseMeshDatabase*                mPoseMeshDatabase;
   dtCore::RefPtr<dtAnim::PoseMeshUtility>  mPoseMeshUtil;
   float                                    mBlendTime;

   /// The source data for IK to operate on
   ProceduralAnimationData mMarinePoseData;
   const dtCore::Transformable* mCurrentTarget;

   /// The relative position of interest from the target
   osg::Vec3 mTargetOffset;

   osg::Vec3 GetPoseMeshEndEffectorDirection(const dtAnim::PoseMesh* poseMesh) const;

   void TickIK(float dt);

   void AssemblePoseData();

   // The following functions should be refactored for less repetition
   // but they serve as good example for things you might want to do

   /// Get the direction the marine is facing
   osg::Vec3 GetForwardDirection() const;

   /// Get the direction that the marine is looking in
   osg::Vec3 GetGazeDirection() const;

   /// Get the direction that marine's gun is pointing in
   osg::Vec3 GetGunDirection() const;

   /// Get the world space position of the gun barrel
   osg::Vec3 GetGunPosition() const;

   /// Get the world space position of the actor
   osg::Vec3 GetWorldPosition() const;

   /// Get the world space position of the head bone
   osg::Vec3 GetHeadPosition() const;

};

class TEST_ANIM_EXPORT ProceduralAnimationActorProxy: public dtAnim::AnimationGameActorProxy
{
public:

   /// Constructor
   ProceduralAnimationActorProxy();

   /// Adds the properties associated with this actor
   void BuildPropertyMap();

   void BuildInvokables();

   /// Creates the actor
   void CreateActor();

   /// Insert into game
   void RegisterWithGameManager(dtGame::GameManager* gm);

protected:

   /// Destructor
   virtual ~ProceduralAnimationActorProxy();

};

////////////////////////////////////////////////////////////////////////////////

#endif // PROCEDURAL_ANIMATION_ACTOR_H_
