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

struct funcRemoveAgent
{
   funcRemoveAgent(const dtAI::BaseNPC* pNPC){mRemoveElement = pNPC;}

   template<class T>
   bool operator()(T pObj)
   {
      if(pObj.second.get() == mRemoveElement)
      {
         mProbability = pObj.first;
         return true;
      }
      
      return false;
   }

   float GetProbability()
   {
      return mProbability;
   }

  private:
   float mProbability;
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
   {
   }
   
   AIParticleManager::~AIParticleManager()
   {
      ClearAllAgents();
   }



   void AIParticleManager::AddAgent(BaseNPC* pAgent, float pProbability)
   {
      mAgents.push_back(std::make_pair(pProbability, pAgent));
      SortAgents();
   }
   
   void AIParticleManager::CloneAgent(BaseNPC* pAgent)
   {
      mAgents.push_back(mClone(pAgent));
      SortAgents();
   }

   void AIParticleManager::RemoveAgent(BaseNPC* pAgent)
   {
      mRemoveList.push_back(pAgent);
   }

   void AIParticleManager::RemoveAgents(const AgentList& pList)
   {
      AgentList::const_iterator iter = pList.begin();
      AgentList::const_iterator endOfList = pList.end();

      float pProbability = 0.0f;

      while(iter != endOfList)
      {
         funcRemoveAgent pRemovePred((*iter));
         mAgents.remove_if(pRemovePred);
         pProbability += pRemovePred.GetProbability();
         ++iter;
      }

      ReAdjustProbabilities(pProbability);

      mRemoveList.clear();
   }

   void AIParticleManager::ReAdjustProbabilities(float pProbRemoved)
   {
      ParticleList::iterator iter = mAgents.begin();
      ParticleList::iterator endOfList = mAgents.end();

      float remainder = 1.0f - pProbRemoved;

      while(iter != endOfList)
      {
         //re-normalize the probability
         float pNewProb = (*iter).first / remainder;
         pNewProb = (*iter).first + (pNewProb * pProbRemoved);
         (*iter).first = pNewProb;

         ++iter;
      }

   }

   void AIParticleManager::ClearAllAgents()
   {
      mAgents.clear();
      mRemoveList.clear();
   }

   void AIParticleManager::UpdateAgents(double dt)
   {
      ParticleList::iterator iter = mAgents.begin();
      ParticleList::iterator endOfList = mAgents.end();

      while(iter != endOfList)
      {
         BaseNPC* pNPC = (*iter).second.get();
         
         //update each npc
         pNPC->Update(dt);

         //if filter returns true that means this npc is no longer valid
         //and we get rid of it
         if(mFilter(pNPC))
         {
            mRemoveList.push_back(pNPC);
         }

         ++iter;
      }

      RemoveAgents(mRemoveList);
   }

   void AIParticleManager::SortAgents()
   {
      mAgents.sort(funcAgentSorter);
   }

   const AIParticleManager::ParticleList& AIParticleManager::GetParticleList() const
   {
      return mAgents;
   }

}//namespace 
