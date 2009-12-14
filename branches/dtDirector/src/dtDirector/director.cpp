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
   Director::Director()
      : mModified(false)
      , mGraph(NULL)
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
      if (!mGraph.valid()) mGraph = new DirectorGraph(this);

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
      outsideValue->SetPosition(osg::Vec2(300, 200));

      // Create a sub graph.
      DirectorGraph* subGraph = new DirectorGraph(this);
      subGraph->BuildPropertyMap();
      subGraph->mParent = mGraph;
      mGraph->mSubGraphs.push_back(subGraph);
      subGraph->mName = "Macro";
      subGraph->SetPosition(osg::Vec2(300, 50));

      // Create an input node.
      dtCore::RefPtr<EventNode> inputNode = dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Input Link", "Core", subGraph).get());
      inputNode->SetPosition(osg::Vec2(50, 50));

      // Create an output node.
      dtCore::RefPtr<ActionNode> outputNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Output Link", "Core", subGraph).get());
      outputNode->SetPosition(osg::Vec2(600, 50));

      // Create an external value node.
      dtCore::RefPtr<ValueNode> extValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Value Link", "Core", subGraph).get());
      extValue->SetValueName("External Connected");
      extValue->SetPosition(osg::Vec2(500, 300));

      dtCore::RefPtr<ValueNode> extValue2 = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Value Link", "Core", subGraph).get());
      extValue2->SetValueName("External Not Connected");
      extValue2->SetPosition(osg::Vec2(600, 300));

      // Create our action node.
      dtCore::RefPtr<ActionNode> actionNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Binary Operation", "General", subGraph).get());
      actionNode->SetPosition(osg::Vec2(200, 50));

      // Create some value nodes.
      dtCore::RefPtr<ValueNode> valueA = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());
      dtCore::RefPtr<ValueNode> valueB = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());
      dtCore::RefPtr<ValueNode> resultValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());

      valueA->SetValueName("Int A");
      valueA->SetPosition(osg::Vec2(200, 300));
      valueB->SetValueName("Int B");
      valueB->SetPosition(osg::Vec2(300, 300));
      resultValue->SetValueName("Result Int");
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
      int count = (int)mThreads.size();
      for (int index = 0; index < count; index++)
      {
         ThreadData& data = mThreads[index];
         if (data.node)
         {
            Node* currentNode = data.node;

            // If the update result is true, then we want to immediately
            // create a new thread on any new events.  Otherwise, our first
            // new thread will be a continuation of the current active thread.
            bool makeNewThread = currentNode->Update(simDelta, delta, data.index);

            // Check for activated outputs and create new threads for them.
            int outputCount = (int)currentNode->GetOutputLinks().size();
            for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
            {
               OutputLink* output = &currentNode->GetOutputLinks()[outputIndex];
               if (int activeCount = output->Test())
               {
                  int linkCount = (int)output->GetLinks().size();
                  for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                  {
                     InputLink* input = output->GetLinks()[linkIndex];
                     if (!input) continue;

                     // Disabled nodes are ignored.
                     if (!input->GetOwner()->GetEnabled()) continue;

                     int inputCount = (int)input->GetOwner()->GetInputLinks().size();
                     int inputIndex = 0;
                     for (inputIndex = 0; inputIndex < inputCount; inputIndex++)
                     {
                        if (input == &input->GetOwner()->GetInputLinks()[inputIndex])
                        {
                           break;
                        }
                     }

                     if (inputIndex < inputCount)
                     {
                        for (int activeIndex = 0; activeIndex < activeCount; activeIndex++)
                        {
                           // Create a new thread.
                           if (makeNewThread)
                           {
                              BeginThread(input->GetOwner(), inputIndex);
                           }
                           // If we are continuing the current thread, continue it.
                           else
                           {
                              data.node = input->GetOwner();
                              data.index = inputIndex;

                              // From now on, all new active outputs create their own threads.
                              makeNewThread = true;
                           }
                        }
                     }
                  }
               }
            }

            // If we have not made a new thread yet, it means we need to remove
            // the current.
            if (!makeNewThread)
            {
               mThreads.erase(mThreads.begin() + index);
               index--;
               count--;
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::BeginThread(Node* node, int index)
   {
      ThreadData data;
      data.node = node;
      data.index = index;
      mThreads.push_back(data);
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
   DirectorGraph* Director::GetGraph(const dtCore::UniqueId& id)
   {
      return mGraph->GetGraph(id);
   }

   //////////////////////////////////////////////////////////////////////////
   Node* Director::GetNode(const dtCore::UniqueId& id)
   {
      return mGraph->GetNode(id);
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::DeleteGraph(const dtCore::UniqueId& id)
   {
      return mGraph->DeleteGraph(id);
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::DeleteNode(const dtCore::UniqueId& id)
   {
      return mGraph->DeleteNode(id);
   }
}
