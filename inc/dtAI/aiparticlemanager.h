/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 Bradley Anderegg
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
 * @author Bradley Anderegg 11/01/2006
 */

#ifndef __DELTA_AIPARTICLEMANAGER_H__
#define __DELTA_AIPARTICLEMANAGER_H__

#include <dtAI/export.h>

#include <dtCore/refptr.h>

#include <dtUtil/functor.h>

#include <osg/Referenced>

#include <list>
#include <utility> //for std::pair

namespace dtAI
{

   class BaseNPC;

   /**
    * The AIParticleManager is almost like a particle system for agents,
    * it is designed to support predicting another agents location
    * using probability distribution analysis.
    */
   class DT_AI_EXPORT AIParticleManager: public osg::Referenced
   {
   public:
      typedef std::pair<float, dtCore::RefPtr<BaseNPC> > AgentParticle;
      typedef std::list<AgentParticle> ParticleList;
      typedef std::list<BaseNPC*> AgentList;

      typedef dtUtil::Functor<AgentParticle, TYPELIST_1(AgentParticle&)> CloneFunctor;
      typedef dtUtil::Functor<bool, TYPELIST_1(AgentParticle&)> FilterFunctor;

   public:
      AIParticleManager(const CloneFunctor& pClone, const FilterFunctor& pFilter);

   protected:
      virtual ~AIParticleManager();

   public:
     /**
      * Adds a new agent to the particle list with the assigned probability
      */
      void AddAgent(BaseNPC* pAgent, float pProbability);

     /**
      * Clones a given agent and adds it to the particle list, uses the CloneFunctor
      * assigned in the constructor
      */
      BaseNPC* CloneAgent(BaseNPC* pAgent);

     /**
      * Adds the given agent to the remove list, it is important to note
      * that the agent won't actually be removed until update.
      * once the agents are removed the probabilities are renormalized
      */
      void RemoveAgent(BaseNPC* pAgent);

     /**
      * Removes all the agents in the list from the particle list
      * this function will remove the agents immediately and re-normalize
      * the probability pool
      */
      void RemoveAgents(const AgentList& pList);

     /**
      * Adds one agent to the remove list and adds its probability to the
      * other agent instead of equally re-normalizing probabilities which
      * would happen if the agent was simply just removed
      */
      void CombineAgents(BaseNPC* pAgentToDelete, BaseNPC* pAgentToAddTo);

     /**
      * Removes all agents from the list, happens immediately
      */
      void ClearAllAgents();

     /**
      * Updates all agents, filters them using the filter function assigned in
      * the constructor, pops off all excess particles of least probability
      * (use SetMaxAgents() to adjust this number, removes all agents that are
      * on the remove list and the re-normalizes the remaining particles probabilities
      */
      void UpdateAgents(double dt);

     /**
      * Returns a const version of the particle list
      */
      const ParticleList& GetParticleList() const;

     /**
      * Returns a non const version of the particle list
      * NOTE: unexpected Behavior will result if agent particles
      * are removed from this list, please use RemoveAgent() for that
      */
      ParticleList& GetParticleList();

     /**
      * Returns the max number of agents allowed, when the agent list gets too big
      * the agents of least probability are removed from the list
      */
      unsigned GetMaxAgents() const;

     /**
      * Sets the maximum number of allowable agents, additional agents are removed
      * on update starting with the least probable
      */
      void SetMaxAgents(unsigned pMaxAgents);

     /**
      * Given a pointer to a BaseNPC, this returns the associated AgentParticle
      * in the list
      */
      AgentParticle* GetAgentParticle(BaseNPC* pNPC);

     /**
      * Returns true if an NPC has been added to the remove list, and will be removed
      * on update
      */
      bool InRemoveList(const BaseNPC* pNPC) const;

   private:
      /// Sorts agents based on their probability
      void SortAgents();
      //re-normalizes probabilities when agents are removed
      void ReAdjustProbabilities(float pProbRemoved);
      // removes excess agents starting with the least probable
      float RemoveOverflowAgents();

      ParticleList mAgents;
      AgentList mRemoveList;

      CloneFunctor mClone;
      FilterFunctor mFilter;

      unsigned mMaxAgents;
   };

} // namespace dtAI

#endif // __DELTA_AIPARTICLEMANAGER_H__
