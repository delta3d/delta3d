/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2006, Alion Science and Technology, BMH Operation & MOVES Institute
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
* @author Curtiss Murphy
* @author Chris Osborn
*/
#ifndef __GM_TUTORIAL_KILLABLE_TARGET_ACTOR__
#define __GM_TUTORIAL_KILLABLE_TARGET_ACTOR__

#include "export.h"
#include <dtCore/refptr.h>
#include <dtActors/gamemeshactor.h>
#include <dtCore/shader.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/particlesystem.h>
#include <dtDAL/resourcedescriptor.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TUTORIAL_TANK_EXPORT KillableTargetActor : public dtActors::GameMeshActor
{
   class SwitchVisitor : public osg::NodeVisitor
   {
      public:
         SwitchVisitor( const std::string& state );
         void apply( osg::Switch& switchNode );
         void SetSwitchState( const std::string& state );
         const std::string& GetSwitchState() const;

      private:

         std::string mSwitchState;
   };

   public:

      // Constructs the tank actor.
      KillableTargetActor(dtGame::GameActorProxy &proxy);

      /**
       * This method is an invokable called when an object is local and
       * receives a tick.
       * @param tickMessage A message containing tick related information.
       */
      virtual void TickLocal(const dtGame::Message &tickMessage);

      /**
       * This method is an invokable called when an object is remote and
       * receives a tick.
       * @param tickMessage A message containing tick related information.
       */
      virtual void TickRemote(const dtGame::Message &tickMessage);
      
      /**
       * Generic handler (Invokable) for messages. Overridden from base class.
       * This is the default invokable on GameActorProxy.
       */
      virtual void ProcessMessage(const dtGame::Message &message);

      void SetShaderEffect(const std::string& shaderEffect);
      const std::string& GetShaderEffect() const { return mShaderEffect; }

      void SetMaxHealth(int maxHealth);
      int GetMaxHealth() const { return mMaxHealth; }

      void SetCurrentHealth(int currentHealth);
      int GetCurrentHealth() const { return mCurrentHealth; }

      // Called when the actor has been added to the game manager.
      // You can respond to OnEnteredWorld on either the proxy or actor or both.
      virtual void OnEnteredWorld();

      //void UpdateShaderParams();

   protected:
      virtual ~KillableTargetActor() { };

   private:
      void ApplyMyShader();
      void ResetState();

      // Exposed Properties
      std::string mShaderEffect;
      int mMaxHealth;

      // Private State
      int mCurrentHealth;
      bool mIsTargeted;

      dtCore::RefPtr<dtCore::Shader> mCurrentShader;
      std::string mCurrentShaderName;

      dtCore::RefPtr<dtCore::ParticleSystem> mSmallExplosion;
      dtCore::RefPtr<dtCore::ParticleSystem> mLargeExplosion;
      dtCore::Transform mOriginalPosition;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TUTORIAL_TANK_EXPORT KillableTargetActorProxy : public dtActors::GameMeshActorProxy
{
   public:

      // Constructs the proxy.
      KillableTargetActorProxy();
      
      // Creates the properties that are custom to the hover tank proxy.
      virtual void BuildPropertyMap();
      
   protected:
      virtual ~KillableTargetActorProxy() { };

      // Creates an instance of our hover tank actor 
      virtual void CreateActor();

      // Called when this proxy is added to the game manager (ie, the "world")
      // You can respond to OnEnteredWorld on either the proxy or actor or both.
      virtual void OnEnteredWorld();
};

#endif //KILLABLE_TARGET_ACTOR
