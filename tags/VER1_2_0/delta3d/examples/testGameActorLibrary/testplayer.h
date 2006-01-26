/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Matthew W. Campbell
 */
#ifndef DELTA_TESTPLAYER
#define DELTA_TESTPLAYER

#include <osg/Vec3>
#include <string>
#include <dtGame/gameactor.h>
#include "export.h"

class DT_EXAMPLE_EXPORT TestPlayer : public dtGame::GameActor
{
   public:
   
      TestPlayer(dtGame::GameActorProxy& proxy);
      
      void SetModel(const std::string &fileName);
      std::string GetModel() const { return mModelFile; }
      
      void SetVelocity(float velocity); 
      float GetVelocity() const;
      
      void SetTurnRate(float rate); 
      float GetTurnRate() const; 
      
      virtual void TickLocal(const dtGame::Message &tickMessage);
      virtual void TickRemote(const dtGame::Message &tickMessage);  
                
      void HandleTick(const double deltaSimTime);
      
   protected:
      virtual ~TestPlayer();
            
   private:
      float mVelocity,mTurnRate;
      std::string mModelFile;
};

class DT_EXAMPLE_EXPORT TestPlayerProxy : public dtGame::GameActorProxy
{
   public:   
      TestPlayerProxy();
      
      virtual void BuildPropertyMap();
      virtual void BuildInvokables();
      
         
   protected:
      virtual ~TestPlayerProxy();
      virtual void CreateActor();
      virtual void OnEnteredWorld();

   private:
      bool mEnteredBefore;

};

#endif
