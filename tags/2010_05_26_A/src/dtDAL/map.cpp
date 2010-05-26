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
 * David Guthrie
 */
#include <prefix/dtdalprefix.h>
#include <osgDB/FileNameUtils>

#include <dtUtil/stringutils.h>
#include <dtDAL/map.h>
#include <dtDAL/actortype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/environmentactor.h>
#include <cstring>

namespace dtDAL 
{
   const std::string Map::MAP_FILE_EXTENSION(".dtmap");
   
   Map::Map(const std::string& mFileName, const std::string& name)
      : mModified(true)
      , mName(name)
   {
      //mFileName requires some processing.
      SetFileName(mFileName);
      mEventManager = new MapGameEvents(*this);
      mPresetCameras.resize(10);
   }
   
   Map::~Map() 
   {
   }
   
   Map::MapGameEvents::MapGameEvents(Map& parent): GameEventManager(), mParent(parent)
   {}

   void Map::MapGameEvents::AddEvent(GameEvent& event)
   {
      GameEventManager::AddEvent(event);
      mParent.SetModified(true);
   }

   void Map::MapGameEvents::RemoveEvent(GameEvent& event)
   {
      GameEventManager::RemoveEvent(event);
      mParent.SetModified(true);
   }

   void Map::MapGameEvents::RemoveEvent(const dtCore::UniqueId& id)
   {
      GameEventManager::RemoveEvent(id);
      mParent.SetModified(true);
   }

   void Map::MapGameEvents::ClearAllEvents()
   {
      GameEventManager::ClearAllEvents();
      mParent.SetModified(true);
   }

   ActorProxy* Map::GetProxyById(const dtCore::UniqueId& id) 
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::iterator i = mProxyMap.find(id);
      if (i != mProxyMap.end())
      {
         return i->second.get();
      }
      return NULL;
   }
   
   const ActorProxy* Map::GetProxyById(const dtCore::UniqueId& id) const 
   {
      std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::const_iterator i = mProxyMap.find(id);
      if (i != mProxyMap.end()) 
      {     
         return i->second.get();
      }
      return NULL;
   }
   
   void Map::SetFileName(const std::string& newFileName) 
   {
      //if "" is passed into the constructor is SetFileName
      //then it should be ignored.
      mFileName = newFileName;
      
      if (mFileName.empty())
         return;
      
      //see if the file already has an extension. If it does, just use it. If
      //not, tack on the officially sanctioned extension.
      if (osgDB::getFileExtension(mFileName).empty())
      {
         mFileName += MAP_FILE_EXTENSION;
      }
   }
   
   void Map::FindProxies(std::vector<dtCore::RefPtr<ActorProxy> >& container,
                         const std::string& name,
                         const std::string& category,
                         const std::string& typeName,
                         const std::string& className,
                         PlaceableFilter placeable) 
   {
      container.clear();
      
      if (name != "" || category != "" || typeName != "" || className != "" || placeable != Either ) 
      {
         for (std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::iterator i = mProxyMap.begin();
              i != mProxyMap.end(); ++i) 
         {
            ActorProxy* ap = i->second.get();
            
            if (name == "" || WildMatch(name, ap->GetName()))
               if (MatchesSearch(*ap, category, typeName, className, placeable))
                  container.push_back(dtCore::RefPtr<ActorProxy>(ap));
         }
      } 
      else 
      {
         //return everything.
         container.reserve(mProxyMap.size());
         for (std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::iterator i = mProxyMap.begin();
              i != mProxyMap.end(); ++i) 
         {
            ActorProxy* ap = i->second.get();
            container.push_back(ap);
         }
      }
      
   }
   
   void Map::FindProxies(std::vector<dtCore::RefPtr<const ActorProxy> >& container,
                         const std::string& name,
                         const std::string& category,
                         const std::string& typeName,
                         const std::string& className,
                         PlaceableFilter placeable) const 
   {
      container.clear();
      
      if (name != "" || category != "" || typeName != "" || className != "" || placeable != Either) 
      {
         for (std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::const_iterator i = mProxyMap.begin();
              i != mProxyMap.end(); ++i) 
         {
            const ActorProxy* ap = i->second.get();

            if (name == "" || WildMatch(name, ap->GetName()))
               if (MatchesSearch(*ap, category, typeName, className, placeable))
                  container.push_back(ap);
         }
      } 
      else 
      {
         //return everything.
         container.reserve(mProxyMap.size());
         for (std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::const_iterator i = mProxyMap.begin();
              i != mProxyMap.end(); ++i) 
         {
            const ActorProxy* ap = i->second.get();
            container.push_back(ap);
         }
      }
   }

   bool Map::MatchesSearch(const ActorProxy& actorProxy,
                           const std::string& category,
                           const std::string& typeName,
                           const std::string& className,
                           PlaceableFilter placeable) const
   {
      if (!className.empty() && !actorProxy.IsInstanceOf(className))
         return false;

      if (!typeName.empty() || !category.empty())
      {
         const ActorType* actorType = &actorProxy.GetActorType();         
         bool matches = false;
         
         while (!matches && actorType != NULL)
         {
            bool nameMatches = typeName.empty() || actorType->GetName() == typeName;       
            const std::string& actualCategory = actorType->GetCategory();      
            bool catMatches = false; 
            
            if (actualCategory.size() >= category.size())
            {
               //The category needs to be either the whole name or be a substring up to a '.'
               catMatches = category.empty() ||
                            (actualCategory.substr(0, category.size()) == category
                            && (actualCategory.size() == category.size() || actualCategory[category.size()] == '.'));
            }
            
            matches = catMatches && nameMatches;
            actorType = actorType->GetParentActorType();
         }
         
         if (!matches)
            return false;
      }
      
      if (placeable == Placeable && !actorProxy.IsPlaceable())
         return false;
      else if (placeable == NotPlaceable && actorProxy.IsPlaceable())
         return false;

      return true;

   }

   void Map::AddProxy(ActorProxy& proxy, bool reNumber) 
   {
      // Check if this proxy already has a number associated with it.
      std::string proxyName = proxy.GetName();
      std::string name, number;
      SplitProxyName(proxyName, name, number);

      // Re-number the proxy with a unique value.
      if (reNumber)
      {
         if (mProxyNumberMap.find(name) != mProxyNumberMap.end())
         {
            number = NumberToString(mProxyNumberMap[name] + 1);
         }
         else
         {
            number = "01";
         }

         proxyName = name + "_" + number;
         proxy.SetName(proxyName);
      }

      // Keep track of our highest number value.
      if (!number.empty())
      {
         int num = atoi(number.c_str());
         int highNum = 0;

         if (mProxyNumberMap.find(name) != mProxyNumberMap.end())
         {
            highNum = mProxyNumberMap[name];
         }

         if (highNum < num)
         {
            mProxyNumberMap[name] = num;
         }
      }

      if (mProxyMap.insert(std::make_pair(proxy.GetId(), dtCore::RefPtr<ActorProxy>(&proxy))).second) 
      {
         const std::set<std::string>& hierarchy = proxy.GetClassHierarchy();
         mProxyActorClasses.insert(proxy.GetClassName());
         mProxyActorClasses.insert(hierarchy.begin(), hierarchy.end());
         mModified = true;
      }
   }

   bool Map::RemoveProxy(const ActorProxy& proxy)
   {
      //This needs to be faster.
      std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::iterator i = mProxyMap.find(proxy.GetId());
      if (i != mProxyMap.end()) 
      {
         mModified = true;

         //notify proxy it is being removed from map
         proxy.OnRemove();
         std::vector<dtCore::RefPtr<ActorProxy> > proxies;

         GetAllProxies(proxies);
         for(unsigned int j = 0; j < proxies.size(); j++)
         {
            std::vector<ActorProperty*> props;
            proxies[j]->GetPropertyList(props);
            for(unsigned int k = 0; k < props.size(); k++)
            {
               if(props[k]->GetDataType() == DataType::ACTOR)
               {
                  ActorActorProperty* aap = dynamic_cast<ActorActorProperty*>(props[k]);
                  if (aap != NULL)
                  {
                     if(aap->GetValue() == &proxy)
                        aap->SetValue(NULL);
                  }
                  else
                  {
                     ActorIDActorProperty* aidap = dynamic_cast<ActorIDActorProperty*>(props[k]);
                     if (aidap != NULL)
                     {
                        if (aidap->GetValue() == proxy.GetId())
                        {
                           aidap->SetValue(dtCore::UniqueId(""));
                        }
                     }
                  }
               }
            }
         }
         mProxyMap.erase(i);
         return true;
      }
      return false;
   }

   void Map::OnProxyRenamed(ActorProxy& proxy)
   {
      std::string proxyName = proxy.GetName();
      std::string name, number;
      SplitProxyName(proxyName, name, number);

      // Keep track of our highest number value.
      if (!number.empty())
      {
         int num = atoi(number.c_str());
         int highNum = 0;

         if (mProxyNumberMap.find(name) != mProxyNumberMap.end())
         {
            highNum = mProxyNumberMap[name];
         }

         if (highNum < num)
         {
            mProxyNumberMap[name] = num;
         }
      }
   }

   void Map::ClearProxies() 
   {
      mProxyMap.clear();
      mProxyActorClasses.clear();
   }

   void Map::RebuildProxyActorClassSet() const 
   {
      mProxyActorClasses.clear();
      for (std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >::const_iterator i = mProxyMap.begin(); i != mProxyMap.end(); ++i) 
      {
         const ActorProxy& proxy = *(i->second);
         const std::set<std::string>& hierarchy = proxy.GetClassHierarchy();
         mProxyActorClasses.insert(proxy.GetClassName());
         mProxyActorClasses.insert(hierarchy.begin(), hierarchy.end());
      }
   }

   void Map::InsertLibrary(unsigned pos, const std::string& name, const std::string& version) 
   {
      std::map<std::string,std::string>::iterator old = mLibraryVersionMap.find(name);

      bool alreadyExists;
      if (old != mLibraryVersionMap.end()) 
      {
         old->second = version;
         alreadyExists = true;
      } 
      else 
      {
         mLibraryVersionMap.insert(make_pair(name,version));
         alreadyExists = false;
      }

      for (std::vector<std::string>::iterator i = mLibraryOrder.begin(); i != mLibraryOrder.end(); ++i) 
      {
         if (*i == name) 
         {
            mLibraryOrder.erase(i);
            break;
         }
      }

      if (pos < mLibraryOrder.size())
         mLibraryOrder.insert(mLibraryOrder.begin() + pos, name);
      else
         mLibraryOrder.push_back(name);

      mModified = true;
   }

   void Map::AddLibrary(const std::string& name, const std::string& version) 
   {
      InsertLibrary(mLibraryOrder.size(), name, version);
   }

   bool Map::RemoveLibrary(const std::string& name) 
   {
      std::map<std::string, std::string>::iterator oldMap = mLibraryVersionMap.find(name);

      if (oldMap != mLibraryVersionMap.end())
         mLibraryVersionMap.erase(oldMap);
      else
         return false;

      for (std::vector<std::string>::iterator i = mLibraryOrder.begin(); i != mLibraryOrder.end(); ++i) 
      {
         if (*i == name) 
         {
            mLibraryOrder.erase(i);
            break;
         }
      }

      mModified = true;
      return true;
   }

   void Map::ClearModified() 
   {
      mModified = false;
      mSavedName = mName;
      mMissingActorTypes.clear();
      mMissingLibraries.clear();
   }

   void Map::AddMissingActorTypes(const std::set<std::string>& types) 
   {
      mMissingActorTypes.insert(types.begin(), types.end());
   }

   void Map::AddMissingLibraries(const std::vector<std::string>& libs) 
   {
      mMissingLibraries.insert(mMissingLibraries.end(), libs.begin(), libs.end());
   }

   //osg::Matrix Map::GetPresetCameraMatrix()
   //{
   //   return osg::Matrix();
   //}

   bool Map::WildMatch(const std::string& sWild, const std::string& sString) 
   {
      char* WildChars = new char[sWild.size() + 1];
      char* str = new char[sString.size() + 1];
      strcpy(WildChars, sWild.c_str());
      strcpy(str, sString.c_str());
      bool result = dtUtil::Match(WildChars, str);
      delete[] WildChars;
      delete[] str;
      return result;
   }
    
   GameEventManager& Map::GetEventManager()
   {
      return *mEventManager;
   }
   
   const GameEventManager& Map::GetEventManager() const
   {
      return *mEventManager;
   }
   
   void Map::SetEnvironmentActor(ActorProxy *envActor)
   {
      if(envActor == NULL)
      {
         RemoveProxy(*mEnvActor);
         mEnvActor = NULL;
         return;
      }

      IEnvironmentActor *ea = dynamic_cast<IEnvironmentActor*>(envActor->GetActor());
      if(ea == NULL)
      {
         LOG_ERROR("The actor specified is not an EnvironmentActor. Ignoring.");
         return;
      }
      mEnvActor = envActor;
      AddProxy(*mEnvActor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Map::GetGroupCount()
   {
      return (int)mActorGroups.size();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Map::GetGroupActorCount(int groupIndex)
   {
      if (groupIndex >= 0 && groupIndex < (int)mActorGroups.size())
      {
         return (int)mActorGroups[groupIndex].actorList.size();
      }

      return -1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Map::AddActorToGroup(int groupIndex, dtDAL::ActorProxy* actor)
   {
      if (actor == NULL)
      {
         LOG_ERROR("Failed to add an actor to a group because the actor is NULL");
         return;
      }

      if (groupIndex >= 0 && groupIndex < (int)mActorGroups.size())
      {
         mActorGroups[groupIndex].actorList.push_back(actor);
      }
      else
      {
         // If the group does not exist, create one.
         MapGroupData group;
         group.actorList.push_back(actor);
         mActorGroups.push_back(group);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Map::RemoveActorFromGroups(dtDAL::ActorProxy* actor)
   {
      // Iterate through each group.
      bool found = false;
      for (int groupIndex = 0; groupIndex < (int)mActorGroups.size(); groupIndex++)
      {
         MapGroupData& group = mActorGroups[groupIndex];

         // Iterate through each actor in the group.
         for (int actorIndex = 0; actorIndex < (int)group.actorList.size(); actorIndex++)
         {
            if (group.actorList[actorIndex] == actor)
            {
               group.actorList.erase(group.actorList.begin() + actorIndex);
               found = true;
            }
         }
      }

      if (found)
      {
         // Remove any groups that are now empty.
         for (int groupIndex = 0; groupIndex < (int)mActorGroups.size(); groupIndex++)
         {
            MapGroupData& group = mActorGroups[groupIndex];

            if (group.actorList.size() == 0)
            {
               mActorGroups.erase(mActorGroups.begin() + groupIndex);
            }
         }
      }
   
      return found;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Map::FindGroupForActor(dtDAL::ActorProxy* actor)
   {
      // Iterate through each group.
      for (int groupIndex = 0; groupIndex < (int)mActorGroups.size(); groupIndex++)
      {
         MapGroupData& group = mActorGroups[groupIndex];

         // Iterate through each actor in the group.
         for (int actorIndex = 0; actorIndex < (int)group.actorList.size(); actorIndex++)
         {
            if (group.actorList[actorIndex] == actor)
            {
               return groupIndex;
            }
         }
      }

      return -1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxy* Map::GetActorFromGroup(int groupIndex, int actorIndex)
   {
      if (groupIndex >= 0 && groupIndex < (int)mActorGroups.size())
      {
         if (actorIndex >= 0 && actorIndex < (int)mActorGroups[groupIndex].actorList.size())
         {
            return mActorGroups[groupIndex].actorList[actorIndex];
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Map::PresetCameraData Map::GetPresetCameraData(int index)
   {
      if (index < 0 || index >= (int)mPresetCameras.size())
      {
         return PresetCameraData();
      }

      return mPresetCameras[index];
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Map::SetPresetCameraData(int index, Map::PresetCameraData presetData)
   {
      if (index < 0 || index >= (int)mPresetCameras.size())
      {
         return;
      }

      mPresetCameras[index] = presetData;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::vector<std::string>& Map::GetMissingLibraries() const
   {
      return mMissingLibraries;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::set<std::string>& Map::GetMissingActorTypes() const
   {
      return mMissingActorTypes;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetName() const
   {
      return mName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetSavedName() const
   {
      return mSavedName;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetName(const std::string& newName)
   {
      mModified = true; mName = newName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetFileName() const
   {
      return mFileName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetDescription() const
   {
      return mDescription;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetDescription(const std::string& newDescription)
   {
      mModified = true;
      mDescription = newDescription;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetAuthor() const
   {
      return mAuthor;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetAuthor(const std::string& newAuthor)
   {
      mModified = true;
      mAuthor = newAuthor;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetComment() const
   {
      return mComment;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetComment(const std::string& newComment)
   {
      mModified = true;
      mComment = newComment;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetCopyright() const
   {
      return mCopyright;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetCopyright(const std::string& newCopyright)
   {
      mModified = true;
      mCopyright = newCopyright;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Map::GetCreateDateTime() const
   {
      return mCreateDateTime;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetCreateDateTime(const std::string& newCreateDateTime)
   {
      mModified = true;
      mCreateDateTime = newCreateDateTime;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::map<dtCore::UniqueId, dtCore::RefPtr<ActorProxy> >& Map::GetAllProxies() const
   {
      return mProxyMap;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::GetAllProxies(std::vector<dtCore::RefPtr<ActorProxy> >& container)
   {
      FindProxies(container, "");
   }

   //////////////////////////////////////////////////////////////////////////
   const std::set<std::string>& Map::GetProxyActorClasses() const
   {
      return mProxyActorClasses;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Map::IsModified() const
   {
      return mModified;
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetModified(bool val)
   {
      mModified = val;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Map::HasLoadingErrors() const
   {
      return mMissingLibraries.size() > 0 || mMissingActorTypes.size() > 0;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::map<std::string, std::string>& Map::GetLibraryVersionMap() const
   {
      return mLibraryVersionMap;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::vector<std::string>& Map::GetAllLibraries() const
   {
      return mLibraryOrder;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Map::HasLibrary(const std::string& mName) const
   {
      return mLibraryVersionMap.find(mName) != mLibraryVersionMap.end();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string Map::GetLibraryVersion(const std::string& mName) const
   {
      std::map<std::string, std::string>::const_iterator i = mLibraryVersionMap.find(mName);
      if (i == mLibraryVersionMap.end())
         return std::string("");

      return i->second;
   }

   //////////////////////////////////////////////////////////////////////////
   ActorProxy* Map::GetEnvironmentActor()
   {
      return mEnvActor.get();
   }

   //////////////////////////////////////////////////////////////////////////
   const ActorProxy* Map::GetEnvironmentActor() const
   {
      return mEnvActor.get();
   }

   //////////////////////////////////////////////////////////////////////////
   void Map::SetSavedName(const std::string& newSavedName)
   {
      mSavedName = newSavedName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Map::SplitProxyName(const std::string& fullName, std::string& name, std::string& number)
   {
      name = fullName;
      number = "";
      int letterCount = fullName.length();

      bool foundNumber = false;
      for (int letterIndex = letterCount - 1; letterIndex >= 0; letterIndex--)
      {
         char letter = fullName[letterIndex];

         // Check if this letter is a number value.
         if (letter == '_')
         {
            name.resize(letterIndex);
            if (letterIndex < letterCount)
               number = &fullName[letterIndex + 1];
            foundNumber = true;
            break;
         }
         else if (letter < '0' || letter > '9')
         {
            name.resize(letterIndex + 1);
            number = &fullName[letterIndex + 1];
            foundNumber = true;
            break;
         }
      }

      if (!foundNumber)
      {
         // If we get here, it means our proxy's name is just a number.
         name = "";
         number = fullName;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string Map::NumberToString(int number)
   {
      std::string numString = dtUtil::ToString(number);

      // If we have a number value, convert it to be 2 digits at least.
      int letterCount = numString.length();
      for (int letterIndex = 0; letterIndex < letterCount; letterIndex++)
      {
         char letter = numString[letterIndex];

         // Ignore all leading zero's
         if (letter != '0')
         {
            numString = &numString[letterIndex];
            break;
         }
      }

      while (numString.length() < 2)
      {
         numString.insert(numString.begin(), '0');
      }

      return numString;
   }
}
