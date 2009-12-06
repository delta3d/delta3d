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
      mLogger = &dtUtil::Log::GetInstance();
   }

   //////////////////////////////////////////////////////////////////////////
   Director::~Director()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Init()
   {
      BuildPropertyMap();
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::CreateDebugScript()
   {
      SetName("Debug Script");

      dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();

      // Create a primary event node.
      dtCore::RefPtr<EventNode> primaryEvent = dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Named Event", "General").get());
      mGraph.mEventNodes.push_back(primaryEvent);
      primaryEvent->SetPosition(osg::Vec2(50, 50));

      // Create an outside value node.
      dtCore::RefPtr<ValueNode> outsideValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());
      mGraph.mValueNodes.push_back(outsideValue);

      // Create a sub graph.
      mGraph.mSubGraphs.push_back(DirectorGraphData());
      DirectorGraphData& subGraph = mGraph.mSubGraphs[0];
      subGraph.mName = "Sub Graph";

      // Create an input node.
      dtCore::RefPtr<EventNode> inputNode = dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Input", "Core").get());
      subGraph.mEventNodes.push_back(inputNode);
      inputNode->SetPosition(osg::Vec2(50, 50));

      // Create an output node.
      dtCore::RefPtr<ActionNode> outputNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Output", "Core").get());
      subGraph.mActionNodes.push_back(outputNode);
      outputNode->SetPosition(osg::Vec2(600, 50));

      // Create an external value node.
      dtCore::RefPtr<ValueNode> extValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("External Value", "Core").get());
      subGraph.mValueNodes.push_back(extValue);
      extValue->SetName("External Connected");
      extValue->SetPosition(osg::Vec2(500, 300));

      dtCore::RefPtr<ValueNode> extValue2 = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("External Value", "Core").get());
      subGraph.mValueNodes.push_back(extValue2);
      extValue2->SetName("External Not Connected");
      extValue2->SetPosition(osg::Vec2(600, 300));

      // Create our action node.
      dtCore::RefPtr<ActionNode> actionNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Binary Operation", "General").get());
      subGraph.mActionNodes.push_back(actionNode);
      actionNode->SetPosition(osg::Vec2(200, 50));

      // Create some value nodes.
      dtCore::RefPtr<ValueNode> valueA = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());
      dtCore::RefPtr<ValueNode> valueB = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());
      dtCore::RefPtr<ValueNode> resultValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());
      subGraph.mValueNodes.push_back(valueA);
      subGraph.mValueNodes.push_back(valueB);
      subGraph.mValueNodes.push_back(resultValue);

      valueA->SetName("Int A");
      valueA->SetPosition(osg::Vec2(200, 300));
      valueB->SetName("Int B");
      valueB->SetPosition(osg::Vec2(300, 300));
      resultValue->SetName("Result Int");
      resultValue->SetPosition(osg::Vec2(400, 300));

      // Give some default values for our A and B.
      valueA->SetInt(10);
      valueB->SetInt(15);

      // Connect nodes together.
      {
         // Now connect our primary event to the input of our sub-graph.
         primaryEvent->GetOutputLink("Out")->Connect(inputNode->GetInputLink("In"));

         // Connect the output of our sub-graph to the action node.
         inputNode->GetOutputLink("Out")->Connect(actionNode->GetInputLink("Multiply"));
         inputNode->GetOutputLink("Out")->Connect(actionNode->GetInputLink("Add"));

         // Connect the output of action node with the output node.
         actionNode->GetOutputLink("Out")->Connect(outputNode->GetInputLink("In"));

         //// Connect our A and B values to the action node.
         //actionNode->GetValueLink("A")->Connect(valueA);
         //actionNode->GetValueLink("B")->Connect(valueB);

         //// Connect our result value to the action node.
         //actionNode->GetValueLink("Result")->Connect(resultValue);

         //// Connect our external value node to the action node.
         //actionNode->GetValueLink("Result")->Connect(extValue);

         // Connect our external value node to our outside value node.
         extValue->GetValueLinks()[0].Connect(outsideValue);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::LoadScript(const std::string& scriptFile, dtDAL::Map* map)
   {
      // First clear all our current nodes.
      mGraph.mSubGraphs.clear();
      mGraph.mEventNodes.clear();
      mGraph.mActionNodes.clear();
      mGraph.mValueNodes.clear();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

      DirectorParser* parser = new DirectorParser();
      if (parser)
      {
         try
         {
            parser->Parse(this, map, "scripts/" + scriptFile + ".dtDir");
         }
         catch (const dtUtil::Exception& e)
         {
            std::string error = "Unable to parse " + scriptFile + " with error " + e.What();
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            fileUtils.PopDirectory();
            throw e;
         }

         fileUtils.PopDirectory();
         mModified = parser->HasDeprecatedProperty();
         return true;
      }

      fileUtils.PopDirectory();
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
            writer->Save(this, "scripts/" + scriptFile + ".dtDir");
         }
         catch (const dtUtil::Exception& e)
         {
            std::string error = "Unable to parse " + scriptFile + " with error " + e.What();
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            fileUtils.PopDirectory();
            throw e;
         }

         fileUtils.PopDirectory();

         mModified = false;
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
         "The Name of the Director script."));

      AddProperty(new dtDAL::StringActorProperty(
         "Description", "Description",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetDescription),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetDescription),
         "The Description of the Director script."));

      AddProperty(new dtDAL::StringActorProperty(
         "Author", "Author",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetAuthor),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetAuthor),
         "The Author of the Director script."));

      AddProperty(new dtDAL::StringActorProperty(
         "Comment", "Comment",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetComment),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetComment),
         "Comment"));

      AddProperty(new dtDAL::StringActorProperty(
         "Copyright", "Copyright",
         dtDAL::StringActorProperty::SetFuncType(this, &Director::SetCopyright),
         dtDAL::StringActorProperty::GetFuncType(this, &Director::GetCopyright),
         "Copyright information."));
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Update(float simDelta, float delta)
   {
      mGraph.Update(simDelta, delta);
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

   //////////////////////////////////////////////////////////////////////////
   Node* Director::GetNode(const dtCore::UniqueId& id)
   {
      return mGraph.GetNode(id);
   }
}
