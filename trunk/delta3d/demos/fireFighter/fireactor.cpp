/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#include <fireFighter/fireactor.h>
#include <fireFighter/messagetype.h>
#include <fireFighter/hatchactor.h>
#include <fireFighter/messages.h>
#include <fireFighter/gamestate.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>
#include <dtGame/basemessages.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/gamemanager.h>
#include <dtCore/particlesystem.h>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>
#include <osg/Referenced>
#include <osgDB/ReadFile>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/Operator>
#include <osgParticle/RadialShooter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/VariableRateCounter>

/**
 * A visitor class that finds matrix transforms and sets their
 * transforms to the given value.
 */
class TransformVisitor : public osg::NodeVisitor
{
public:

   TransformVisitor(osg::Matrix matrix)
      : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
      mMatrix(matrix)
   {}

   virtual void apply(osg::MatrixTransform& node)
   {
      node.setMatrix(mMatrix);
   }

   virtual void apply(osg::LightSource& node)
   {
      osg::Light* light = dynamic_cast<osg::Light*>(node.getLight());

      osg::Vec3 trans = mMatrix.getTrans();

      light->setPosition(
         osg::Vec4(trans[0], trans[1], trans[2], 1.0)
         );
   }

private:

   osg::Matrix mMatrix;
};


/**
* A transformation callback.
*/
class TransformCallback : public osg::NodeCallback
{
public:

   TransformCallback(dtCore::Transformable* transformable)
   {
      mTransformable = transformable;
   }

   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   { 
      dtCore::Transform transform;

      mTransformable->GetTransform(transform);

      osg::Matrix matrix;

      transform.Get(matrix);

      TransformVisitor transformVisitor(matrix);
      node->accept(transformVisitor);

      traverse(node,nv);
   }

private:

   dtCore::Transformable* mTransformable;
};

/**
* The stored state of a VariableRateCounter.
*/
class VariableRateCounterUserData : public osg::Referenced
{
public:

   osgParticle::rangef mRateRange;
};

/**
* The stored state of a SectorPlacer.
*/
class SectorPlacerUserData : public osg::Referenced
{
public:

   osgParticle::rangef mRadiusRange;
};

/**
* The stored state of a RadialShooter.
*/
class RadialShooterUserData : public osg::Referenced
{
public:

   osgParticle::rangef mInitialSpeedRange;

   osgParticle::rangev3 mInitialRotationalSpeedRange;
};

/**
* An operator that keeps particles between a number of planes.
*/
class BoundaryPlaneOperator : public osgParticle::Operator
{
public:

   BoundaryPlaneOperator()
      : Operator()
   {}

   BoundaryPlaneOperator(const BoundaryPlaneOperator &copy,
      const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY)
      : Operator(copy, copyop),
      mPlanes(copy.mPlanes)
   {}

   META_Object(dtCore, BoundaryPlaneOperator)

      virtual void operate(osgParticle::Particle* p, double dt)
   {
      osg::Vec3 pos = p->getPosition();

      for(std::vector<osg::Vec4>::iterator it = mPlanes.begin();
         it != mPlanes.end();
         it++)
      {
         float dist = (*it)[0] * pos[0] +
            (*it)[1] * pos[1] +
            (*it)[2] * pos[2] +
            (*it)[3];

         if(dist < 0.0f)
         {
            pos[0] -= ((*it)[0] * dist);
            pos[1] -= ((*it)[1] * dist);
            pos[2] -= ((*it)[2] * dist);
         }
      }

      p->setPosition(pos);
   }

   std::vector<osg::Vec4> mPlanes;
};


/**
* A visitor class that finds various particle system nodes and
* adjusts their states to reflect fire parameters.
*/
class FireParameterVisitor : public osg::NodeVisitor
{
public:

   FireParameterVisitor(float radius, float intensity, std::vector<osg::Vec4> boundaryPlanes) : 
      osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
         mRadius(radius),
         mIntensity(intensity),
         mBoundaryPlanes(boundaryPlanes)
      {}

      virtual void apply(osg::Node& node)
      {
         osg::Node* nodePtr = &node;

         if(IS_A(nodePtr, osgParticle::ModularEmitter*))
         {
            osgParticle::ModularEmitter* me =
               (osgParticle::ModularEmitter*)nodePtr;

            if(IS_A(me->getCounter(), osgParticle::VariableRateCounter*))
            {
               osgParticle::VariableRateCounter* vrc = 
                  (osgParticle::VariableRateCounter*)me->getCounter();

               VariableRateCounterUserData* vrcud = 
                  (VariableRateCounterUserData*)vrc->getUserData();

               if(vrcud == NULL)
               {
                  vrcud = new VariableRateCounterUserData;

                  vrcud->mRateRange = vrc->getRateRange();

                  vrc->setUserData(vrcud);
               }

               vrc->setRateRange(
                  vrcud->mRateRange.minimum*mIntensity*mRadius*mRadius,
                  vrcud->mRateRange.maximum*mIntensity*mRadius*mRadius
                  );
            }

            if(IS_A(me->getPlacer(), osgParticle::SectorPlacer*))
            {
               osgParticle::SectorPlacer* sp =
                  (osgParticle::SectorPlacer*)me->getPlacer();

               SectorPlacerUserData* spud = 
                  (SectorPlacerUserData*)sp->getUserData();

               if(spud == NULL)
               {
                  spud = new SectorPlacerUserData;

                  spud->mRadiusRange = sp->getRadiusRange();

                  sp->setUserData(spud);
               }

               sp->setRadiusRange(
                  spud->mRadiusRange.minimum*mRadius,
                  spud->mRadiusRange.maximum*mRadius
                  );
            }

            if(IS_A(me->getShooter(), osgParticle::RadialShooter*))
            {
               osgParticle::RadialShooter* rs =
                  (osgParticle::RadialShooter*)me->getShooter();

               RadialShooterUserData* rsud = 
                  (RadialShooterUserData*)rs->getUserData();

               if(rsud == NULL)
               {
                  rsud = new RadialShooterUserData;

                  rsud->mInitialSpeedRange = rs->getInitialSpeedRange();

                  rsud->mInitialRotationalSpeedRange = 
                     rs->getInitialRotationalSpeedRange();

                  rs->setUserData(rsud);
               }

               rs->setInitialSpeedRange(
                  rsud->mInitialSpeedRange.minimum*mIntensity,
                  rsud->mInitialSpeedRange.maximum*mIntensity
                  );

               rs->setInitialRotationalSpeedRange(
                  rsud->mInitialRotationalSpeedRange.minimum*mIntensity,
                  rsud->mInitialRotationalSpeedRange.maximum*mIntensity
                  );
            }
         }
         else if(IS_A(nodePtr, osgParticle::ModularProgram*))
         {
            osgParticle::ModularProgram* mp =
               (osgParticle::ModularProgram*)nodePtr;

            BoundaryPlaneOperator* bpo;

            int i = 0;
            for(;i<mp->numOperators();i++)
            {
               if(IS_A(mp->getOperator(i), BoundaryPlaneOperator*))
               {
                  bpo = (BoundaryPlaneOperator*)mp->getOperator(i);

                  break;
               }
            }

            if(i == mp->numOperators())
            {
               bpo = new BoundaryPlaneOperator;

               mp->addOperator(bpo);
            }

            bpo->mPlanes = mBoundaryPlanes;
         }

         traverse(node);
      }

private:

   float mRadius;
   float mIntensity;
   std::vector<osg::Vec4> mBoundaryPlanes;
};


/////////////////////////////////////////////////
FireActorProxy::FireActorProxy()
{

}

FireActorProxy::~FireActorProxy()
{

}

void FireActorProxy::BuildPropertyMap()
{
   GameItemActorProxy::BuildPropertyMap();

   FireActor &fa = static_cast<FireActor&>(GetGameActor());

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM, 
      "FlameFileName", "FlameFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetFlameFilename),  
      "Sets the flame file name"));

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM,
      "SparkFileName", "SparkFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetSparkFilename),  
      "Sets the spark file name"));

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM,
      "SmokeFileName", "SmokeFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetSmokeFilename),  
      "Sets the smoke file name"));

   AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::PARTICLE_SYSTEM,
      "SmokeCeilingFileName", "SmokeCeilingFileName", 
      dtDAL::MakeFunctor(fa, &FireActor::SetSmokeCeilingFilename), 
      "Sets the smoke ceiling file name"));

   AddProperty(new dtDAL::FloatActorProperty("Radius", "Radius", 
      dtDAL::MakeFunctor(fa, &FireActor::SetRadius), 
      dtDAL::MakeFunctorRet(fa, &FireActor::GetRadius), 
      "Sets the fire radius"));

   AddProperty(new dtDAL::FloatActorProperty("Intensity", "Intensity", 
      dtDAL::MakeFunctor(fa, &FireActor::SetIntensity), 
      dtDAL::MakeFunctorRet(fa, &FireActor::GetIntensity), 
      "Sets the fire intensity"));
}

void FireActorProxy::BuildInvokables()
{
   dtGame::GameActorProxy::BuildInvokables();
}

dtDAL::ActorProxyIcon* FireActorProxy::GetBillBoardIcon()
{
   if(!mBillBoardIcon.valid())
   {
      mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::GENERIC);
   }
   return mBillBoardIcon.get();
}

void FireActorProxy::OnEnteredWorld()
{
   FireActor &fa = static_cast<FireActor&>(GetGameActor());

   dtGame::Invokable *playSoundInvoke = new dtGame::Invokable("PlaySound", 
      dtDAL::MakeFunctor(fa, &FireActor::PlayFireSound));

   dtGame::Invokable *stopSoundInvoke = new dtGame::Invokable("StopSounds", 
      dtDAL::MakeFunctor(fa, &FireActor::StopSounds));

   AddInvokable(*playSoundInvoke);
   AddInvokable(*stopSoundInvoke);

   RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
   RegisterForMessages(MessageType::ITEM_ACTIVATED,   "PlaySound");
   RegisterForMessages(MessageType::ITEM_DEACTIVATED, "PlaySound");
   RegisterForMessages(MessageType::GAME_STATE_CHANGED, "StopSounds");
}

/////////////////////////////////////////////////
FireActor::FireActor(dtGame::GameActorProxy &proxy) :
   GameItemActor(proxy),
   mFlameSystem(new dtCore::ParticleSystem),
   mSmokeSystem(new dtCore::ParticleSystem),
   mCeilingSystem(new dtCore::ParticleSystem),
   mSparkSystem(new dtCore::ParticleSystem),
   mLightSource(new osg::LightSource),
   mLight(new osg::Light), 
   mRadius(1.0f),
   mIntensity(1.0f)
{
   mLight->setLightNum(1);
   mLight->setQuadraticAttenuation(1.0);
   mLightSource->setLight(mLight.get());
   GetOSGNode()->asGroup()->addChild(mLightSource.get());
   mItemUseSnd->SetLooping(true);
   GetOSGNode()->setUpdateCallback(new TransformCallback(this));
   
   AddChild(mFlameSystem.get());
   AddChild(mSmokeSystem.get());
   AddChild(mSparkSystem.get());
   AddChild(mCeilingSystem.get());
}

FireActor::~FireActor()
{

}

void FireActor::PlayFireSound(const dtGame::Message &msg)
{
   // Check to see if the actor is the door to the fire room
   dtGame::GameActorProxy *proxy = GetGameActorProxy().GetGameManager()->FindGameActorById(msg.GetAboutActorId());
   if(proxy == NULL)
      return; 

   HatchActor *ha = dynamic_cast<HatchActor*>(proxy->GetActor());
   if(ha == NULL)
      return;

   // Hatch item was activated/deactivated, play/stop the sound
   ha->IsActivated() ? mItemUseSnd->Play() : mItemUseSnd->Stop();
}

void FireActor::OnEnteredWorld()
{
   GameItemActor::OnEnteredWorld();

   osg::Vec4 leftWall(0, -1, 0, 0.337), rightWall(-1, 0, 0, 18.271),
             ceiling(0, 0, -1, 2.459), back(1, 0, 0, -5.31),
             side(0, 1, 0, 5.96);

   AddBoundaryPlane(leftWall);
   AddBoundaryPlane(rightWall);
   AddBoundaryPlane(ceiling);
   AddBoundaryPlane(back);
   AddBoundaryPlane(side);
}

void FireActor::SetFlameFilename(const std::string &filename)
{
   mFlameSystem->LoadFile(filename);

   FireParameterVisitor fireParameterVisitor(mRadius, mIntensity, mBoundaryPlanes);
   GetOSGNode()->accept(fireParameterVisitor);
}

void FireActor::SetSparkFilename(const std::string &filename)
{
   mSparkSystem->LoadFile(filename);

   FireParameterVisitor fireParameterVisitor(mRadius, mIntensity, mBoundaryPlanes);
   GetOSGNode()->accept(fireParameterVisitor);
}

void FireActor::SetSmokeFilename(const std::string &filename)
{
   mSmokeSystem->LoadFile(filename);

   FireParameterVisitor fireParameterVisitor(mRadius, mIntensity, mBoundaryPlanes);
   GetOSGNode()->accept(fireParameterVisitor);
}

void FireActor::SetSmokeCeilingFilename(const std::string &filename)
{
   mCeilingSystem->LoadFile(filename);

   FireParameterVisitor fireParameterVisitor(mRadius, mIntensity, mBoundaryPlanes);
   GetOSGNode()->accept(fireParameterVisitor);
}

void FireActor::SetRadius(float radius)
{
   if(mRadius != radius)
   {
      mRadius = radius;

      FireParameterVisitor fireParameterVisitor(mRadius, mIntensity, mBoundaryPlanes);
      GetOSGNode()->accept(fireParameterVisitor);
   }
}

float FireActor::GetRadius() const
{
   return mRadius;
}

void FireActor::SetIntensity(float intensity)
{
   if(mIntensity != intensity)
   {
      mIntensity = intensity;

      FireParameterVisitor fireParameterVisitor(mRadius, mIntensity, mBoundaryPlanes);
      GetOSGNode()->accept(fireParameterVisitor);
   }
}

float FireActor::GetIntensity() const
{
   return mIntensity;
}

void FireActor::AddBoundaryPlane(const osg::Vec4 plane)
{
   mBoundaryPlanes.push_back(osg::Vec4(plane[0], plane[1], plane[2], plane[3]));;
}

void FireActor::RemoveBoundaryPlane(int index)
{
   mBoundaryPlanes.erase(mBoundaryPlanes.begin() + index);
}

int FireActor::GetBoundaryPlaneCount() const
{
   return mBoundaryPlanes.size();
}

void FireActor::GetBoundaryPlane(osg::Vec4 dest, int index) const
{
   dest.set(mBoundaryPlanes[index][0],mBoundaryPlanes[index][1],mBoundaryPlanes[index][2],mBoundaryPlanes[index][3]);
}

void FireActor::TickLocal(const dtGame::Message &msg)
{
   const dtGame::TickMessage &tickMsg = static_cast<const dtGame::TickMessage&>(msg);

   double delta = tickMsg.GetDeltaSimTime();

   static double randomWalk = 0.5;

   randomWalk += (delta * (((float)rand()/RAND_MAX)*5-2.5));

   if(randomWalk < 0.0)
   {
      randomWalk = 0.0;
   }
   else if(randomWalk > 1.0)
   {
      randomWalk = 1.0;
   }
   const float r = 0.9f * 10.f;
   const float g = 0.7f * 10.f;
   const float b = 0.35f * 10.f;
   mLight->setDiffuse(osg::Vec4(mIntensity * r * (0.5 + 0.5*randomWalk), mIntensity * g * (0.5 + 0.5*randomWalk), mIntensity * b * (0.5 + 0.5*randomWalk), 1));
}

void FireActor::StopSounds(const dtGame::Message &msg)
{
   const GameStateChangedMessage &gscm = static_cast<const GameStateChangedMessage&>(msg);
   if(gscm.GetNewState() == GameState::STATE_DEBRIEF ||
      gscm.GetNewState() == GameState::STATE_MENU)
   {
      StopItemUseSnd();
   }
}
