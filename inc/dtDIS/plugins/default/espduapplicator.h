/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_PLUGIN_ESPDU_APPLICATOR_H__
#define __DELTA_PLUGIN_ESPDU_APPLICATOR_H__

#include <string>                      // for parameter type
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>         // for export symbols
#include <osg/Vec3>

///@cond DOXYGEN_SHOULD_SKIP_THIS
namespace DIS
{
   class EntityStatePdu;
   class EntityID;
   class ArticulationParameter;
}
///@endcond

namespace dtDAL
{
   class NamedGroupParameter;
}

namespace dtGame
{
   class ActorUpdateMessage;
}

namespace dtDIS
{
   class SharedState;
   struct ActiveEntityControl;

   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {
   /// The model of how to convert <i>all attributes</i> between an EntityStatePdu and an ActorUpdateMessage, and visa versa.
   struct DT_DIS_DEFAULT_EXPORT FullApplicator
   {
      /// for converting an incoming Pdu into a Message
      /// @param source the known data.
      /// @param dest the structure that needs the known data.
      /// @param config The knowledge needed to get the job done.
      void operator ()(const DIS::EntityStatePdu& source,
                       dtGame::ActorUpdateMessage& dest,
                       SharedState* config) const;

      /// for converting an outgoing Message into a Pdu
      ///\todo this should take a dtGame::Message, but something is broke, causing this function to require the dtGame::ActorUpdateMessage
      /// @param active a model of the known entities being published.
      void operator ()(const dtGame::ActorUpdateMessage& source,
                       const DIS::EntityID& eid,
                       DIS::EntityStatePdu& dest,
                       dtDIS::SharedState* active) const;
   };

   /// The model of how to convert <i>frame-changing attributes</i> between an EntityStatePdu and an ActorUpdateMessage, and visa versa.
   struct DT_DIS_DEFAULT_EXPORT PartialApplicator
   {
   public:
      /// converts an incoming Pdu into a Message
      void operator ()(const DIS::EntityStatePdu& source,
                       dtGame::ActorUpdateMessage& dest,
                       dtDIS::SharedState* config);

#if 0
      osg::Vec3 mAcceleration ;
      osg::Vec3 mLastVelocity ;

      // system time of last EntityState pdu received
      float mTimePDU ;

#endif

   private:
      /// builds the dtDAL::NamedGroupParameter by adding dtDAL::NamedParameters
      /// to the parent group parameter from the data within the DIS::ArticulationParameter.
      /// @param source the DIS data to be read.
      /// @param topgroup the valid dtDAL::NamedGroupParameter instance to be built.
      void AddArticulationMessageParameters(const DIS::ArticulationParameter& source,
                                            dtDAL::NamedGroupParameter* topgroup,
                                            unsigned int param_index) const;
      void AddPartParameter(unsigned int partclass, dtDAL::NamedGroupParameter* parent) const;
      void AddMotionParameter(unsigned int motionclass, double motionvalue, dtDAL::NamedGroupParameter* parent) const;
      void AddFloatParam(const std::string& name, float value, dtDAL::NamedGroupParameter* parent) const;
      void AddStringParam(const std::string& name, const std::string& value, dtDAL::NamedGroupParameter* parent) const;
//      void UpdateAcceleration( osg::Vec3& currentVelocity ) ;
   };

   } // end namespace details
   ///@endcond
}

#endif  // __DELTA_PLUGIN_ESPDU_APPLICATOR_H__
