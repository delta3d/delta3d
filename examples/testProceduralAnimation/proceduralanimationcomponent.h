/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * Michael Guerrero
 */

#ifndef PROCEDURAL_ANIM_COMPONENT_H
#define PROCEDURAL_ANIM_COMPONENT_H

////////////////////////////////////////////////////////////////////////////////

#include <dtCore/refptr.h>
#include <dtGame/baseinputcomponent.h>
#include <dtGame/defaultgroundclamper.h>
#include <dtAnim/posemeshdatabase.h>
#include <map>

namespace dtGame { class GameActorProxy;    }
namespace dtAnim { class Cal3dModelWrapper; }
namespace dtAnim { class PoseMeshDatabase;  }
namespace dtDAL  { class ActorProxy;        }

class CalCoreModel;
class ProceduralAnimationActor;

////////////////////////////////////////////////////////////////////////////////

class ProceduralAnimationComponent : public dtGame::BaseInputComponent
{
public:

   static const std::string& NAME;

   /// Constructor
   ProceduralAnimationComponent(const std::string& name = NAME);

   /**
    * Handles incoming messages
    */
   virtual void ProcessMessage(const dtGame::Message& message);

   /**
   * KeyboardListener call back- Called when a key is pressed.
   */
   virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);
   
   /**
   * Generate many actors to stress test the system
   */
   void InitializePerformanceTest();

protected:

   virtual ~ProceduralAnimationComponent();

private:  

   typedef dtCore::RefPtr<dtAnim::PoseMeshDatabase> IKDatabase;
  
   std::map<CalCoreModel*, IKDatabase> mPoseMeshMap;
   std::vector<ProceduralAnimationActor*> mActorList;

   dtCore::RefPtr<dtGame::BaseGroundClamper> mGroundClamper;

   void OnMapLoaded();  
   void OnMapUnloaded();
   void OnActorCreated(dtGame::GameActor* newActor);
   
   void CreateIKActorGrid(const osg::Vec3& startPos, const osg::Vec3& forwardDirection, 
      const osg::Vec3& sideDirection, int forwardCount, int sideCount, bool perturb = true);

   void CreateIKActorsForPerfTest();
   void CreateIKActorsForAesthetics();

   void InitializeIKActors();

   dtCore::Transformable* GetTerrain();

   void SetAimTarget(const dtCore::Transformable* transformable);

   dtAnim::PoseMeshDatabase* GetPoseMeshDatabaseForActor(ProceduralAnimationActor *actor);
};

////////////////////////////////////////////////////////////////////////////////

#endif  // PROCEDURAL_ANIM_COMPONENT_H
