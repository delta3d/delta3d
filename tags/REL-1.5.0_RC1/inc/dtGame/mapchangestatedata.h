/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
#ifndef DELTA_MAPCHANGESTATEDATA
#define DELTA_MAPCHANGESTATEDATA

#include <string>
#include <osg/Referenced>

#include <dtUtil/enumeration.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtGame/export.h>

namespace dtGame
{
   class GameManager;
   class MessageType;
   
   //intentionally not exported
   class DT_GAME_EXPORT MapChangeStateData: public osg::Referenced
   {
      public:
      
         class DT_GAME_EXPORT MapChangeState : public dtUtil::Enumeration
         {
            DECLARE_ENUM(MapChangeState);
            public:
      
               ///State for unloading the old map.
               static const MapChangeState UNLOAD;

               ///State for loading the new map.
               static const MapChangeState LOAD;

               ///Not changing the map.
               static const MapChangeState IDLE;
      
            protected:
               MapChangeState(const std::string &name) : Enumeration(name)
               {
                  AddInstance(this);
               }
         };
      
         MapChangeStateData(dtGame::GameManager& gm);
                  
         const std::string& GetOldMapName() const { return mOldMapName; }        
         const std::string& GetNewMapName() const { return mNewMapName; }        
      
         const MapChangeState& GetCurrentState() const { return *mCurrentState; }    
         void ChangeState(const MapChangeState& newState) { mCurrentState = &newState; }
         
         /**
          * @throws dtUtil::Exception with ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if the GameManager has been deleted.
          */
         void BeginMapChange(const std::string& oldMapName, const std::string& newMapName, bool addBillboards, bool enableDatabasePaging);
         
          /**
          * @throws dtUtil::Exception with ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if the GameManager has been deleted.
          */
         void ContinueMapChange();
             
      private:
         dtCore::ObserverPtr<GameManager> mGameManager;
         
         std::string mOldMapName;
         std::string mNewMapName;
      
         const MapChangeState* mCurrentState;
         bool mAddBillboards, mEnableDatabasePaging;
         
         
         //disable copy constructor and operator = 
         MapChangeStateData(const MapChangeStateData& toCopy) {}
         MapChangeStateData& operator = (const MapChangeStateData& toAssign) { return *this; }
         void SendMapMessage(const MessageType& type, const std::string& name);
   };
}

#endif /*DELTA_MAPCHANGESTATEDATA*/
