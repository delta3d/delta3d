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
#include <vector>
#include <osg/Referenced>

#include <dtUtil/enumeration.h>
#include <dtCore/observerptr.h>
#include <dtGame/export.h> 
#include <dtGame/gamemanager.h>

namespace dtGame
{
   class MessageType;
   
   /**
    * A helper class for changing the map on the GM.  In the future, it would be nice
    * to allow swapping out this class on the GM so people could override/add to the process to 
    * do loading screens and such.
    * 
    * @see dtGame::GameManager::ChangeMapSet for more information on the process.
    */
   class DT_GAME_EXPORT MapChangeStateData: public osg::Referenced
   {
      public:
         typedef std::vector<std::string> NameVector;
         
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
                  
         const NameVector& GetOldMapNames() const { return mOldMapNames; }
         const NameVector& GetNewMapNames() const { return mNewMapNames; }
      
         const MapChangeState& GetCurrentState() const { return *mCurrentState; }
         void ChangeState(const MapChangeState& newState) { mCurrentState = &newState; }

         /**
         * @throws dtUtil::Exception with ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if the GameManager has been deleted.
         */
         void BeginMapChange(const NameVector& oldMapNames, const NameVector& newMapNames, bool addBillboards);
         
          /**
          * @throws dtUtil::Exception with ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if the GameManager has been deleted.
          */
         void ContinueMapChange();

         
         /// Takes a map name and loads all the actors into the GM
         /**
          This utility might be used either during a whole Change Map routine,
          or to load an additional map to the current set.
          \param: mapName the name of the map to be loaded          
          */
         void LoadSingleMapIntoGM(const std::string& mapName);

         /// Closes a single map from the Project, remove the map's gameEvent
         /**
          This utility might be used either during a whole Change Map routine,
          or to load an additional map to the current set.
          \param: mapName the name of the map to be loaded
          \param: deleteLibraries flag to delete dynamic libraries used by the map to be closed          
          \see dtDal::Project::CloseMap()
          */
         void CloseSingleMap(const std::string& mapName, bool deleteLibraries = true);


      protected:         

         // Opens all of the new maps in the new map vector. Returns true if successful
         bool OpenNewMaps();

         // Closes all of the old maps in the old map vector.
         void CloseOldMaps();         

      private:
         dtCore::ObserverPtr<GameManager> mGameManager;

         NameVector mOldMapNames;
         NameVector mNewMapNames;

         const MapChangeState* mCurrentState;
         bool mAddBillboards;

         //disable copy constructor and operator = 
         MapChangeStateData(const MapChangeStateData&) {}
         MapChangeStateData& operator = (const MapChangeStateData&) { return *this; }
         void SendMapMessage(const MessageType& type, const NameVector& names);
   };
}

#endif /*DELTA_MAPCHANGESTATEDATA*/
