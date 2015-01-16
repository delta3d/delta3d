/* -*-c++-*-
 * TutorialLibrary - KillableTargetActor (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */
#ifndef __GM_TUTORIAL_KILLABLE_TARGET_ACTOR__
#define __GM_TUTORIAL_KILLABLE_TARGET_ACTOR__

#include "export.h"
#include <dtCore/refptr.h>
#include <dtActors/gamemeshactor.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shaderparamfloat.h>
#include <dtCore/particlesystem.h>
#include <dtCore/transform.h>
#include <dtCore/resourcedescriptor.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TUTORIAL_TANK_EXPORT KillableTargetActor : public dtActors::GameMeshDrawable
{
   class SwitchVisitor : public osg::NodeVisitor
   {
   public:
      SwitchVisitor(const std::string& state);
      void apply(osg::Switch& switchNode);
      void SetSwitchState(const std::string& state);
      const std::string& GetSwitchState() const;

   private:
      std::string mSwitchState;
   };

public:
   // Constructs the tank actor.
   KillableTargetActor(dtGame::GameActorProxy& parent);

   /**
    * This method is an invokable called when an object is local and
    * receives a tick.
    * @param tickMessage A message containing tick related information.
    */
   virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);

   /**
    * This method is an invokable called when an object is remote and
    * receives a tick.
    * @param tickMessage A message containing tick related information.
    */
   virtual void OnTickRemote(const dtGame::TickMessage& tickMessage);

   /**
    * Generic handler (Invokable) for messages. Overridden from base class.
    * This is the default invokable on GameActorProxy.
    */
   virtual void ProcessMessage(const dtGame::Message& message);

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
   virtual ~KillableTargetActor() {}

private:
   void ApplyMyShader();
   void ResetState();

   // Exposed Properties
   std::string mShaderEffect;
   int mMaxHealth;

   // Private State
   int mCurrentHealth;
   bool mIsTargeted;

   dtCore::RefPtr<dtCore::ShaderProgram> mCurrentShader;
   std::string mCurrentShaderName;

   dtCore::RefPtr<dtCore::ParticleSystem> mSmallExplosion;
   dtCore::RefPtr<dtCore::ParticleSystem> mLargeExplosion;
   dtCore::Transform mOriginalPosition;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TUTORIAL_TANK_EXPORT KillableTargetActorProxy : public dtActors::GameMeshActor
{
public:
   // Constructs the proxy.
   KillableTargetActorProxy();

   // Creates the properties that are custom to the hover tank proxy.
   virtual void BuildPropertyMap();

protected:
   virtual ~KillableTargetActorProxy() {}

   // Creates an instance of our hover tank actor
   virtual void CreateDrawable();

   // Called when this proxy is added to the game manager (ie, the "world")
   // You can respond to OnEnteredWorld on either the proxy or actor or both.
   virtual void OnEnteredWorld();
};

#endif // KILLABLE_TARGET_ACTOR
