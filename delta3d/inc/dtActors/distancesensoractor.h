/*
* Delta3D Open Source Game and Simulation Engine
* Copyright 2008, Alion Science and Technology
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
* David Guthrie
*/
#ifndef DELTA_DISTANCE_SENSOR_ACTOR
#define DELTA_DISTANCE_SENSOR_ACTOR

#include <dtDAL/plugin_export.h>
#include <dtGame/gameactor.h>
#include <dtAI/sensor.h>
#include <dtAI/distancesensor.h>
#include <string>
#include <map>
#include <vector>

namespace dtGame
{
   class Message;
}

namespace dtActors
{
   class DT_PLUGIN_EXPORT DistanceSensorActorProxy: public dtGame::GameActorProxy
   {
      public:
         static const dtUtil::RefString PROPERTY_TRIGGER_DISTANCE;
         static const dtUtil::RefString PROPERTY_ATTACH_TO_ACTOR;

         DistanceSensorActorProxy();

         /**
          * Creates the properties associated with this proxy
          */
         virtual void BuildPropertyMap();
         virtual void BuildInvokables();

         void SetAttachToProxy(dtDAL::ActorProxy* newAttachToProxy);

      protected:
         virtual ~DistanceSensorActorProxy();

         virtual void CreateActor();
   };

   class DT_PLUGIN_EXPORT DistanceSensorActor: public dtGame::GameActor
   {
      public:
         typedef std::map<std::string, dtCore::RefPtr<dtAI::SensorBase<float> > > SensorMap;
         typedef dtGame::GameActor BaseClass;

         DistanceSensorActor(dtGame::GameActorProxy& parent);

         template <typename DistanceCallback>
         bool RegisterWithSensor(const std::string& keyName, dtCore::Transformable& referenceActor,
                  DistanceCallback func)
         {
            typedef dtAI::DistanceSensor<dtAI::EvaluateTransformablePosition<>,
               dtAI::EvaluateTransformablePosition<>, DistanceCallback> TransformableDistanceSensor;

            dtAI::EvaluateTransformablePosition<> etpThis(*this, dtCore::Transformable::ABS_CS);
            dtAI::EvaluateTransformablePosition<> etpRefActor(referenceActor, dtCore::Transformable::ABS_CS);

            dtCore::RefPtr<TransformableDistanceSensor> distSensor =
               new TransformableDistanceSensor(etpThis, etpRefActor, mTriggerDistance, func);

            if (!mSensors.insert(std::make_pair(keyName, distSensor.get())).second)
            {
               return false;
            }
            return true;
         }

         template <typename DistanceCallback>
         bool RegisterWithSensor(const std::string& keyName, osg::MatrixTransform& referenceNode,
                  DistanceCallback func)
         {
            typedef dtAI::DistanceSensor<dtAI::EvaluateTransformablePosition<>,
               dtAI::EvaluateMatrixPosition<>, DistanceCallback> MatrixTransformDistanceSensor;

            dtAI::EvaluateTransformablePosition<> etpThis(*this, dtCore::Transformable::ABS_CS);
            dtAI::EvaluateMatrixPosition<> etpRefMatrix(referenceNode);


            dtCore::RefPtr<MatrixTransformDistanceSensor> distSensor =
               new MatrixTransformDistanceSensor(etpThis, etpRefMatrix, mTriggerDistance, func);

            if (!mSensors.insert(std::make_pair(keyName, dtCore::RefPtr<dtAI::SensorBase<float> >(distSensor.get()))).second)
            {
               return false;
            }
            return true;
         }

         bool HasRegistration(const std::string& name) const;
         void RemoveSensorRegistration(const std::string& name);

         virtual void OnEnteredWorld();

         void SetAttachToActor(dtCore::DeltaDrawable* attachTo);
         dtCore::DeltaDrawable* GetAttachToActor();

         void SetTriggerDistance(float distance);
         float GetTriggerDistance() const;

         ///Overridden to handle evaluating callbacks.
         virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);

         ///Overridden to handle evaluating callbacks.
         virtual void OnTickRemote(const dtGame::TickMessage& tickMessage);

      protected:

         virtual ~DistanceSensorActor();
         SensorMap mSensors;
         std::vector<std::string> mRemoveList;
         dtCore::RefPtr<dtCore::DeltaDrawable> mAttachToActor;
         float mTriggerDistance;
   };


}

#endif /*DELTA_DISTANCE_SENSOR_ACTOR*/
