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

#include <dtAI/aiparticlemanager.h>
#include <dtAI/basenpc.h>

#include <dtUtil/log.h>

//vs thinks std::numeric_limits<>::max() is a macro
#ifdef max
#undef max
#endif

#include <limits>
#include <algorithm>


struct funcCompareAgent
{
   funcCompareAgent(const dtAI::BaseNPC* pNPC) { mRemoveElement = pNPC; }

   template<class T>
   bool operator()(T pObj)
   {
      if (pObj.second.get() == mRemoveElement)
      {
         return true;
      }

      return false;
   }

  private:
   const dtAI::BaseNPC* mRemoveElement;
};

bool funcAgentSorter(const dtAI::AIParticleManager::AgentParticle& lhs, const dtAI::AIParticleManager::AgentParticle& rhs)
{
  return lhs.first > rhs.first;
}

namespace dtAI
{
   AIParticleManager::AIParticleManager(const CloneFunctor& pClone, const FilterFunctor& pFilter)
      : mAgents()
      , mRemoveList()
      , mClone(pClone)
      , mFilter(pFilter)
      , mMaxAgents(std::numeric_limits<unsigned>::max())
   {
   }

   AIParticleManager::~AIParticleManager()
   {
      ClearAllAgents();
   }


   void AIParticleManager::SetMaxAgents(unsigned pMaxAgents)
   {
      mMaxAgents = pMaxAgents;
   }

   unsigned AIParticleManager::GetMaxAgents() const
   {
      return mMaxAgents;
   }


   void AIParticleManager::AddAgent(BaseNPC* pAgent, float pProbability)
   {
      mAgents.push_back(std::make_pair(pProbability, pAgent));
      SortAgents();
   }

   BaseNPC* AIParticleManager::CloneAgent(BaseNPC* pAgent)
   {

      ParticleList::iterator iter = mAgents.begin();
      ParticleList::iterator endOfList = mAgents.end();

      while (iter != endOfList)
      {
         if ((*iter).second == pAgent)
         {
            AgentParticle newAgent = mClone(*iter);
            mAgents.push_back(newAgent);
            SortAgents();

            return newAgent.second.get();
         }

         ++iter;
      }

      return 0;
   }

   void AIParticleManager::RemoveAgent(BaseNPC* pAgent)
   {
      if (!InRemoveList(pAgent)) { mRemoveList.push_back(pAgent); }
   }

   bool AIParticleManager::InRemoveList(const BaseNPC* pNPC) const
   {
      AgentList::const_iterator iter = std::find(mRemoveList.begin(), mRemoveList.end(), pNPC);

      return iter != mRemoveList.end();
   }

   void AIParticleManager::RemoveAgents(const AgentList& pList)
   {
      AgentList::const_iterator iter      = pList.begin();
      AgentList::const_iterator endOfList = pList.end();

      float pProbability = 0.0f;

      while (iter != endOfList)
      {
         funcCompareAgent pRemovePred((*iter));
         ParticleList::iterator agentIter = std::find_if (mAgents.begin(), mAgents.end(), pRemovePred);
         if (agentIter != mAgents.end())
         {
           pProbability += (*agentIter).first;
           mAgents.erase(agentIter);
         }

         ++iter;
      }

      pProbability += RemoveOverflowAgents();

      ReAdjustProbabilities(pProbability);

      mRemoveList.clear();
   }

   void AIParticleManager::ReAdjustProbabilities(float pProbRemoved)
   {
      ParticleList::iterator iter = mAgents.begin();
      ParticleList::iterator endOfList = mAgents.end();

      float remainder = 1.0f - pProbRemoved;

      while (iter != endOfList)
      {
         //re-normalize the probability
         float pNewProb = (*iter).first / remainder;
         pNewProb = (*iter).first + (pNewProb * pProbRemoved);
         (*iter).first = pNewProb;

         ++iter;
      }

   }

   float AIParticleManager::RemoveOverflowAgents()
   {
      float probRemoved = 0.0f;
      //since we are sorted on probability we will remove the least probable agents
      //until we are under our limit
      while (mAgents.size() > mMaxAgents && !mAgents.empty())
      {
         probRemoved += mAgents.back().first;
         mAgents.pop_back();
      }

      return probRemoved;
   }

   void AIParticleManager::ClearAllAgents()
   {
      mAgents.clear();
      mRemoveList.clear();
   }

   void AIParticleManager::UpdateAgents(double dt)
   {

      ParticleList::iterator endOfList = mAgents.end();

      //we use a for loop here and compare against the new end just an npc added to our list
      //on its update
      for (ParticleList::iterator iter = mAgents.begin(); iter != mAgents.end(); ++iter)
      {
         BaseNPC* pNPC = (*iter).second.get();

         //update each npc
         pNPC->Update(dt);

         //if filter returns true that means this npc is no longer valid
         //and we get rid of it
         if (mFilter(*iter))
         {
            mRemoveList.push_back(pNPC);
         }
      }

      RemoveAgents(mRemoveList);
   }


   AIParticleManager::AgentParticle* AIParticleManager::GetAgentParticle(BaseNPC* pNPC)
   {
      ParticleList::iterator iter = std::find_if (mAgents.begin(), mAgents.end(), funcCompareAgent(pNPC));
      if (iter == mAgents.end())
      {
         return 0;
      }
      else
      {
         return &*iter;
      }
   }

   void AIParticleManager::CombineAgents(BaseNPC* pAgentToDelete, BaseNPC* pAgentToAddTo)
   {
      AgentParticle* pParticleToAdd = GetAgentParticle(pAgentToAddTo);
      AgentParticle* pParticleToDelete = GetAgentParticle(pAgentToDelete);

      if (!pParticleToAdd || !pParticleToDelete)
      {
         LOG_ERROR("Attempting to combine unknown agents");
         return;
      }

      float prob = pParticleToDelete->first;
      RemoveAgent(pAgentToDelete);
      pParticleToAdd->first += prob;

   }

   void AIParticleManager::SortAgents()
   {
      mAgents.sort(funcAgentSorter);
   }

   const AIParticleManager::ParticleList& AIParticleManager::GetParticleList() const
   {
      return mAgents;
   }

   AIParticleManager::ParticleList& AIParticleManager::GetParticleList()
   {
      return mAgents;
   }

} // namespace dtAI
