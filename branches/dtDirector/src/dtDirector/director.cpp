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
   //////////////////////////////////////////////////////////////////////////
   DirectorGraphData::DirectorGraphData(Director* director)
      : mParent(NULL)
      , mEnabled(true)
      , mName("Macro")
      , mComment("")
      , mDirector(director)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraphData::BuildPropertyMap(bool isParent)
   {
      AddProperty(new dtDAL::BooleanActorProperty(
         "Enabled", "Enabled",
         dtDAL::BooleanActorProperty::SetFuncType(this, &DirectorGraphData::SetEnabled),
         dtDAL::BooleanActorProperty::GetFuncType(this, &DirectorGraphData::GetEnabled),
         "Enabled status of the graph."));

      AddProperty(new dtDAL::StringActorProperty(
         "Name", "Name",
         dtDAL::StringActorProperty::SetFuncType(this, &DirectorGraphData::SetName),
         dtDAL::StringActorProperty::GetFuncType(this, &DirectorGraphData::GetName),
         "The Name of the Director graph."));

      AddProperty(new dtDAL::StringActorProperty(
         "Comment", "Comment",
         dtDAL::StringActorProperty::SetFuncType(this, &DirectorGraphData::SetComment),
         dtDAL::StringActorProperty::GetFuncType(this, &DirectorGraphData::GetComment),
         "Comment"));

      // Only sub graphs have a position.
      if (!isParent)
      {
         AddProperty(new dtDAL::Vec2ActorProperty(
            "Position", "Position",
            dtDAL::Vec2ActorProperty::SetFuncType(this, &DirectorGraphData::SetPosition),
            dtDAL::Vec2ActorProperty::GetFuncType(this, &DirectorGraphData::GetPosition),
            "The Position of the Director graph in its parent.", "UI"));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorGraphData::Update(float simDelta, float delta)
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

      for (int graphIndex = 0; graphIndex < (int)mSubGraphs.size(); graphIndex++)
      {
         DirectorGraphData* graph = mSubGraphs[graphIndex];
         if (graph && graph->GetEnabled()) graph->Update(simDelta, delta);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   Node* DirectorGraphData::GetNode(const dtCore::UniqueId& id)
   {
      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mEventNodes[index]->GetID() == id)
         {
            return mEventNodes[index];
         }
      }

      count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mActionNodes[index]->GetID() == id)
         {
            return mActionNodes[index];
         }
      }

      count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         if (mValueNodes[index]->GetID() == id)
         {
            return mValueNodes[index];
         }
      }

      count = (int)mSubGraphs.size();
      for (int index = 0; index < count; index++)
      {
         DirectorGraphData* graph = mSubGraphs[index];
         if (graph)
         {
            Node* node = graph->GetNode(id);
            if (node) return node;
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   bool DirectorGraphData::AddNode(Node* node)
   {
      ValueNode* valueNode = dynamic_cast<ValueNode*>(node);
      if (valueNode)
      {
         mValueNodes.push_back(valueNode);
         return true;
      }

      EventNode* eventNode = dynamic_cast<EventNode*>(node);
      if (eventNode)
      {
         mEventNodes.push_back(eventNode);
         return true;
      }

      ActionNode* actionNode = dynamic_cast<ActionNode*>(node);
      if (actionNode)
      {
         mActionNodes.push_back(actionNode);
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<EventNode> > DirectorGraphData::GetInputNodes()
   {
      std::vector<dtCore::RefPtr<EventNode> > nodes;

      // Search all event nodes for input event nodes.
      int count = (int)mEventNodes.size();
      for (int index = 0; index < count; index++)
      {
         EventNode* node = mEventNodes[index].get();
         if (node && node->GetType().GetFullName() == "Core.Input")
         {
            bool bFound = false;
            int sortCount = (int)nodes.size();
            for (int sortIndex = 0; sortIndex < sortCount; sortIndex++)
            {
               EventNode* sortNode = nodes[sortIndex];
               if (sortNode && sortNode->GetPosition().y() >= node->GetPosition().y())
               {
                  bFound = true;
                  nodes.insert(nodes.begin() + sortIndex, node);
                  break;
               }
            }

            if (!bFound) nodes.push_back(node);
         }
      }

      return nodes;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<ActionNode> > DirectorGraphData::GetOutputNodes()
   {
      std::vector<dtCore::RefPtr<ActionNode> > nodes;

      // Search all action nodes for output action nodes.
      int count = (int)mActionNodes.size();
      for (int index = 0; index < count; index++)
      {
         ActionNode* node = mActionNodes[index].get();
         if (node && node->GetType().GetFullName() == "Core.Output")
         {
            bool bFound = false;
            int sortCount = (int)nodes.size();
            for (int sortIndex = 0; sortIndex < sortCount; sortIndex++)
            {
               ActionNode* sortNode = nodes[sortIndex];
               if (sortNode && sortNode->GetPosition().y() >= node->GetPosition().y())
               {
                  bFound = true;
                  nodes.insert(nodes.begin() + sortIndex, node);
                  break;
               }
            }

            if (!bFound) nodes.push_back(node);
         }
      }

      return nodes;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<ValueNode> > DirectorGraphData::GetExternalValueNodes()
   {
      std::vector<dtCore::RefPtr<ValueNode> > nodes;

      // Search all value nodes for external value nodes.
      int count = (int)mValueNodes.size();
      for (int index = 0; index < count; index++)
      {
         ValueNode* node = mValueNodes[index].get();
         if (node && node->GetType().GetFullName() == "Core.External Value")
         {
            bool bFound = false;
            int sortCount = (int)nodes.size();
            for (int sortIndex = 0; sortIndex < sortCount; sortIndex++)
            {
               ValueNode* sortNode = nodes[sortIndex];
               if (sortNode && sortNode->GetPosition().x() >= node->GetPosition().x())
               {
                  bFound = true;
                  nodes.insert(nodes.begin() + sortIndex, node);
                  break;
               }
            }

            if (!bFound) nodes.push_back(node);
         }
      }

      return nodes;
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   Director::Director()
      : mModified(false)
      , mGraph(new DirectorGraphData(this))
      , mMap(NULL)
   {
      mLogger = &dtUtil::Log::GetInstance();
   }

   //////////////////////////////////////////////////////////////////////////
   Director::~Director()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Init(dtDAL::Map* map)
   {
      Clear();

      SetMap(map);

      BuildPropertyMap();

      mGraph->BuildPropertyMap(true);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Clear()
   {
      // First clear all our current nodes.
      mGraph->mEventNodes.clear();
      mGraph->mActionNodes.clear();
      mGraph->mValueNodes.clear();
      mGraph->mSubGraphs.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::CreateDebugScript()
   {
      Clear();

      SetName("Debug Script");
      SetDescription("Debug Script for Testing purposes.");
      SetAuthor("Jeff P. Houde");
      SetComment("This is a code generated test script.");

      dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();

      // Create a primary event node.
      dtCore::RefPtr<EventNode> primaryEvent = dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Named Event", "General", mGraph).get());
      primaryEvent->SetPosition(osg::Vec2(50, 50));

      // Create an outside value node.
      dtCore::RefPtr<ValueNode> outsideValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", mGraph).get());
      outsideValue->SetPosition(osg::Vec2(200, 200));

      // Create a sub graph.
      DirectorGraphData* subGraph = new DirectorGraphData(this);
      subGraph->BuildPropertyMap();
      subGraph->mParent = mGraph;
      mGraph->mSubGraphs.push_back(subGraph);
      subGraph->mName = "Sub Graph";
      subGraph->SetPosition(osg::Vec2(200, 50));

      // Create an input node.
      dtCore::RefPtr<EventNode> inputNode = dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Input", "Core", subGraph).get());
      inputNode->SetPosition(osg::Vec2(50, 50));

      // Create an output node.
      dtCore::RefPtr<ActionNode> outputNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Output", "Core", subGraph).get());
      outputNode->SetPosition(osg::Vec2(600, 50));

      // Create an external value node.
      dtCore::RefPtr<ValueNode> extValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("External Value", "Core", subGraph).get());
      extValue->SetName("External Connected");
      extValue->SetPosition(osg::Vec2(500, 300));

      dtCore::RefPtr<ValueNode> extValue2 = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("External Value", "Core", subGraph).get());
      extValue2->SetName("External Not Connected");
      extValue2->SetPosition(osg::Vec2(600, 300));

      // Create our action node.
      dtCore::RefPtr<ActionNode> actionNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Binary Operation", "General", subGraph).get());
      actionNode->SetPosition(osg::Vec2(200, 50));

      // Create some value nodes.
      dtCore::RefPtr<ValueNode> valueA = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());
      dtCore::RefPtr<ValueNode> valueB = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());
      dtCore::RefPtr<ValueNode> resultValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());

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

         // Connect the output of action node with the output node.
         actionNode->GetOutputLink("Out")->Connect(outputNode->GetInputLink("In"));

         // Connect our A and B values to the action node.
         actionNode->GetValueLink("A")->Connect(valueA);
         actionNode->GetValueLink("B")->Connect(valueB);

         // Connect our result value to the action node.
         actionNode->GetValueLink("Result")->Connect(resultValue);

         // Connect our external value node to the action node.
         actionNode->GetValueLink("Result")->Connect(extValue);

         // Connect our external value node to our outside value node.
         extValue->GetValueLinks()[0].Connect(outsideValue);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::LoadScript(const std::string& scriptFile)
   {
      Clear();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

      DirectorParser* parser = new DirectorParser();
      if (parser)
      {
         try
         {
            parser->Parse(this, mMap.get(), "scripts/" + scriptFile + ".dtDir");
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
      mGraph->Update(simDelta, delta);
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
      return mGraph->GetNode(id);
   }
}
