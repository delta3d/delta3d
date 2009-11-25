/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#include <dtDirector/director.h>
#include <dtDirector/nodemanager.h>

#include <dtDirector/directorxml.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/project.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Director::Director()
      : mModified(false)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   Director::~Director()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Init()
   {
      BuildPropertyMap();

      dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();

      mEventNodes.push_back(dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Named Event", "General").get()));

      mActionNodes.push_back(dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Binary Operation", "General").get()));

      mValueNodes.push_back(dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get()));
      mValueNodes.push_back(dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get()));
      mValueNodes.push_back(dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get()));
      mValueNodes.push_back(dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get()));

      {
         mValueNodes[0]->SetPropertyValue(10);
         mValueNodes[1]->SetPropertyValue(15);

         // Connect all the nodes together.
         mEventNodes[0]->GetOutputs()[0].Connect(&mActionNodes[0]->GetInputs()[2]);

         mActionNodes[0]->GetValues()[0].Connect(mValueNodes[0].get());
         mActionNodes[0]->GetValues()[1].Connect(mValueNodes[1].get());
         mActionNodes[0]->GetValues()[2].Connect(mValueNodes[2].get());
         mActionNodes[0]->GetValues()[2].Connect(mValueNodes[3].get());

         // Trigger the event.
         mEventNodes[0]->Trigger(0);

         Update(0, 0);
         Update(0, 0);
         Update(0, 0);
         Update(0, 0);

         int firstResult = mValueNodes[2]->GetPropertyValue<int>();
         int secondResult = mValueNodes[3]->GetPropertyValue<int>();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::LoadScript(const std::string& scriptFile)
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::SaveScript(const std::string& scriptFile)
   {
      DirectorWriter* writer = new DirectorWriter();
      if (writer)
      {
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

         try
         {
            writer->Save(*this, "scripts/" + scriptFile + ".dtDir");
         }
         catch (const dtUtil::Exception& e)
         {
            std::string error = "Unable to parse " + scriptFile + " with error " + e.What();
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            fileUtils.PopDirectory();
            throw e;
         }

         fileUtils.PopDirectory();
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::BuildPropertyMap()
   {
      AddProperty(new dtDAL::StringActorProperty(
         "Name", "Name",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetName),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetName),
         "The Name of the Director script.", "Data"));

      AddProperty(new dtDAL::StringActorProperty(
         "Description", "Description",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetDescription),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetDescription),
         "The Description of the Director script.", "Data"));

      AddProperty(new dtDAL::StringActorProperty(
         "Author", "Author",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetAuthor),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetAuthor),
         "The Author of the Director script.", "Data"));

      AddProperty(new dtDAL::StringActorProperty(
         "Comment", "Comment",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetComment),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetComment),
         "Comment", "Data"));

      AddProperty(new dtDAL::StringActorProperty(
         "Copyright", "Copyright",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetCopyright),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetCopyright),
         "Copyright information.", "Data"));
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Update(float simDelta, float delta)
   {
      // Update all Event nodes.
      for (int nodeIndex = 0; nodeIndex < (int)mEventNodes.size(); nodeIndex++)
      {
         mEventNodes[nodeIndex]->Update(simDelta, delta);
      }

      // Update all Action nodes.
      for (int nodeIndex = 0; nodeIndex < (int)mActionNodes.size(); nodeIndex++)
      {
         mActionNodes[nodeIndex]->Update(simDelta, delta);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::InsertLibrary(unsigned pos, const std::string& name, const std::string& version) 
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
         mLibraryVersionMap.insert(make_pair(name, version));
         alreadyExists = false;
      }

      for (std::vector<std::string>::iterator i = mLibraries.begin(); i != mLibraries.end(); ++i) 
      {
         if (*i == name) 
         {
            mLibraries.erase(i);
            break;
         }
      }

      if (pos < mLibraries.size())
         mLibraries.insert(mLibraries.begin() + pos, name);
      else
         mLibraries.push_back(name);

      mModified = true;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::AddLibrary(const std::string& name, const std::string& version) 
   {
      InsertLibrary(mLibraries.size(), name, version);
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::RemoveLibrary(const std::string& name) 
   {
      std::map<std::string, std::string>::iterator oldMap = mLibraryVersionMap.find(name);

      if (oldMap != mLibraryVersionMap.end())
         mLibraryVersionMap.erase(oldMap);
      else
         return false;

      for (std::vector<std::string>::iterator i = mLibraries.begin(); i != mLibraries.end(); ++i) 
      {
         if (*i == name) 
         {
            mLibraries.erase(i);
            break;
         }
      }

      mModified = true;
      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::HasLibrary(const std::string& name) const
   {
      return mLibraryVersionMap.find(name) != mLibraryVersionMap.end();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::vector<std::string>& Director::GetAllLibraries() const
   {
      return mLibraries;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string Director::GetLibraryVersion(const std::string& mName) const
   {
      std::map<std::string, std::string>::const_iterator i = mLibraryVersionMap.find(mName);
      if (i == mLibraryVersionMap.end())
         return std::string("");

      return i->second;
   }
}
