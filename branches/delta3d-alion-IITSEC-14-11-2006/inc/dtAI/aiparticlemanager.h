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

         void AddAgent(BaseNPC* pAgent, float pProbability);
         BaseNPC* CloneAgent(BaseNPC* pAgent);
         void RemoveAgent(BaseNPC* pAgent);

         void RemoveAgents(const AgentList& pList);

         void ClearAllAgents();

         void UpdateAgents(double dt);

         const ParticleList& GetParticleList() const;

   private:

      void SortAgents();
      void ReAdjustProbabilities(float pProbRemoved);

      ParticleList mAgents;
      AgentList mRemoveList;

      CloneFunctor mClone;
      FilterFunctor mFilter;
   
   };
}//namespace 

#endif // __DELTA_AIPARTICLEMANAGER_H__
