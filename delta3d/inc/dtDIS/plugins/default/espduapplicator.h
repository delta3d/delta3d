#ifndef _plugin_espdu_applicator_h_
#define _plugin_espdu_applicator_h_

#include <string>                      // for parameter type
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>         // for export symbols

namespace DIS
{
   class EntityStatePdu;
   class EntityID;
   class ArticulationParameter;
}

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
                       const SharedState* config) const;

      /// for converting an outgoing Message into a Pdu
      ///\todo this should take a dtGame::Message, but something is broke, causing this function to require the dtGame::ActorUpdateMessage
      /// @param active a model of the known entities being published.
      void operator ()(const dtGame::ActorUpdateMessage& source,
                       const DIS::EntityID& eid,
                       DIS::EntityStatePdu& dest,
                       const dtDIS::SharedState* active) const;
   };

   /// The model of how to convert <i>frame-changing attributes</i> between an EntityStatePdu and an ActorUpdateMessage, and visa versa.
   struct DT_DIS_DEFAULT_EXPORT PartialApplicator
   {
   public:
      /// converts an incoming Pdu into a Message
      void operator ()(const DIS::EntityStatePdu& source, dtGame::ActorUpdateMessage& dest) const;

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
   };

   } // end namespace details
}

#endif  // _plugin_espdu_applicator_h_
