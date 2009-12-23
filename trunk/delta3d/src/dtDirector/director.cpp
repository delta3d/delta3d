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
      , mCurrentThread(-1)
      , mLogNodes(false)
   {
      mPlayer = "";
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

   ////////////////////////////////////////////////////////////////////////////
   //void Director::CreateDebugScript()
   //{
   //   Clear();

   //   SetName("Debug Script");
   //   SetDescription("Debug Script for Testing purposes.");
   //   SetAuthor("Jeff P. Houde");
   //   SetComment("This is a code generated test script.");

   //   dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();

   //   // Create a primary event node.
   //   dtCore::RefPtr<EventNode> primaryEvent = dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Named Event", "General", mGraph).get());
   //   primaryEvent->SetPosition(osg::Vec2(50, 50));

   //   // Create an outside value node.
   //   dtCore::RefPtr<ValueNode> outsideValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", mGraph).get());
   //   outsideValue->SetPosition(osg::Vec2(300, 200));

   //   // Create a sub graph.
   //   DirectorGraph* subGraph = new DirectorGraph(this);
   //   subGraph->BuildPropertyMap();
   //   subGraph->mParent = mGraph;
   //   mGraph->mSubGraphs.push_back(subGraph);
   //   subGraph->mName = "Macro";
   //   subGraph->SetPosition(osg::Vec2(300, 50));

   //   // Create an input node.
   //   dtCore::RefPtr<EventNode> inputNode = dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Input Link", "Core", subGraph).get());
   //   inputNode->SetPosition(osg::Vec2(50, 50));

   //   // Create an output node.
   //   dtCore::RefPtr<ActionNode> outputNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Output Link", "Core", subGraph).get());
   //   outputNode->SetPosition(osg::Vec2(600, 50));

   //   // Create an external value node.
   //   dtCore::RefPtr<ValueNode> extValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Value Link", "Core", subGraph).get());
   //   extValue->SetValueName("External Connected");
   //   extValue->SetPosition(osg::Vec2(500, 300));

   //   dtCore::RefPtr<ValueNode> extValue2 = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Value Link", "Core", subGraph).get());
   //   extValue2->SetValueName("External Not Connected");
   //   extValue2->SetPosition(osg::Vec2(600, 300));

   //   // Create our action node.
   //   dtCore::RefPtr<ActionNode> actionNode = dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Binary Operation", "General", subGraph).get());
   //   actionNode->SetPosition(osg::Vec2(200, 50));

   //   // Create some value nodes.
   //   dtCore::RefPtr<ValueNode> valueA = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());
   //   dtCore::RefPtr<ValueNode> valueB = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());
   //   dtCore::RefPtr<ValueNode> resultValue = dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General", subGraph).get());

   //   valueA->SetValueName("Int A");
   //   valueA->SetPosition(osg::Vec2(200, 300));
   //   valueB->SetValueName("Int B");
   //   valueB->SetPosition(osg::Vec2(300, 300));
   //   resultValue->SetValueName("Result Int");
   //   resultValue->SetPosition(osg::Vec2(400, 300));

   //   // Give some default values for our A and B.
   //   valueA->SetInt(10);
   //   valueB->SetInt(15);

   //   // Connect nodes together.
   //   {
   //      // Now connect our primary event to the input of our sub-graph.
   //      primaryEvent->GetOutputLink("Out")->Connect(inputNode->GetInputLink("In"));

   //      // Connect the output of our sub-graph to the action node.
   //      inputNode->GetOutputLink("Out")->Connect(actionNode->GetInputLink("Multiply"));

   //      // Connect the output of action node with the output node.
   //      actionNode->GetOutputLink("Out")->Connect(outputNode->GetInputLink("In"));

   //      // Connect our A and B values to the action node.
   //      actionNode->GetValueLink("A")->Connect(valueA);
   //      actionNode->GetValueLink("B")->Connect(valueB);

   //      // Connect our result value to the action node.
   //      actionNode->GetValueLink("Result")->Connect(resultValue);

   //      // Connect our external value node to the action node.
   //      actionNode->GetValueLink("Result")->Connect(extValue);

   //      // Connect our external value node to our outside value node.
   //      extValue->GetValueLinks()[0].Connect(outsideValue);
   //   }
   //}

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
            parser->Parse(this, mMap.get(), "directors/" + scriptFile + ".dtDir");
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
            writer->Save(this, "directors/" + scriptFile + ".dtDir");
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
      // Update all threads.
      int count = (int)mThreads.size();
      for (mCurrentThread = 0; mCurrentThread < count; mCurrentThread++)
      {
         if (!UpdateThread(mThreads[mCurrentThread], simDelta, delta))
         {
            // Pop the first callstack item from the thread.
            mThreads.erase(mThreads.begin() + mCurrentThread);
            mCurrentThread--;
            count--;
         }
      }

      // We reset the current thread value so any new threads created outside
      // of the update will generate a brand new main thread.
      mCurrentThread = -1;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::BeginThread(Node* node, int index)
   {
      std::vector<ThreadData>* threadList = &mThreads;
      int curThread = mCurrentThread;

      while (curThread > -1)
      {
         ThreadData& t = (*threadList)[curThread];
         if (!t.stack.empty())
         {
            StackData& s = t.stack[t.stack.size()-1];
            curThread = s.currentThread;
            threadList = &s.subThreads;

            // If this is the current running stack item, and it does
            // not have an active running node, use this instead
            // of creating a new sub-thread.
            if (curThread == -1 && !s.node)
            {
               s.node = node;
               s.index = index;
               return;
            }
         }
         else break;
      }

      if (!curThread) return;

      ThreadData data;
      StackData stack;
      stack.node = node;
      stack.index = index;
      stack.first = true;
      stack.currentThread = -1;
      data.stack.push_back(stack);

      threadList->push_back(data);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::PushStack(Node* node, int index)
   {
      StackData stack;
      stack.node = node;
      stack.index = index;
      stack.first = true;
      stack.currentThread = -1;

      std::vector<ThreadData>* threadList = &mThreads;
      int curThread = mCurrentThread;

      while (curThread > -1)
      {
         ThreadData& t = (*threadList)[curThread];
         if (t.stack[t.stack.size()-1].currentThread == -1)
         {
            t.stack.push_back(stack);
            return;
         }
         else if (!t.stack.empty())
         {
            StackData& s = t.stack[t.stack.size()-1];
            curThread = s.currentThread;
            threadList = &s.subThreads;
         }
         else break;
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
   void Director::GetNodes(const std::string& name, const std::string& category, std::vector<Node*>& outNodes)
   {
      mGraph->GetNodes(name, category, outNodes);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::GetNodes(const std::string& name, const std::string& category, const std::string& property, const std::string& value, std::vector<Node*>& outNodes)
   {
      std::vector<Node*> nodes;
      mGraph->GetNodes(name, category, nodes);

      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = nodes[index];
         if (!node) continue;

         dtDAL::ActorProperty* prop = node->GetProperty(property);
         if (prop && prop->ToString() == value)
         {
            outNodes.push_back(node);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   ValueNode* Director::GetValueNode(const std::string& name)
   {
      return mGraph->GetValueNode(name);
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

   //////////////////////////////////////////////////////////////////////////
   bool Director::UpdateThread(ThreadData& data, float simDelta, float delta)
   {
      // If there is no stack, this thread is finished.
      if (data.stack.empty()) return false;

      int stackIndex = data.stack.size() - 1;
      if (stackIndex < 0) return false;

      StackData& stack = data.stack[stackIndex];

      // Update all the sub-threads in the stack.
      int count = (int)stack.subThreads.size();
      for (stack.currentThread = 0; stack.currentThread < count; stack.currentThread++)
      {
         if (!UpdateThread(stack.subThreads[stack.currentThread], simDelta, delta))
         {
            stack.subThreads.erase(stack.subThreads.begin() + stack.currentThread);
            stack.currentThread--;
            count--;
         }
      }
      stack.currentThread = -1;

      bool makeNewThread = false;

      // Threads always update the first item in the stack,
      // all other stack items are "sleeping".
      if (stack.node)
      {
         Node* currentNode = stack.node;
         int   input       = stack.index;
         bool  first       = stack.first;
         stack.first = false;

         // If the update result is true, then we want to immediately
         // create a new thread on any new events.  Otherwise, our first
         // new thread will be a continuation of the current active thread.
         makeNewThread = currentNode->Update(simDelta, delta, input, first);
         bool continued = makeNewThread;

         // Check for activated outputs and create new threads for them.
         std::vector<OutputLink*> outputs;
         int outputCount = (int)currentNode->GetOutputLinks().size();
         for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
         {
            OutputLink* output = &currentNode->GetOutputLinks()[outputIndex];
            if (output->Test())
            {
               outputs.push_back(output);

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
                     // Create a new thread.
                     if (makeNewThread)
                     {
                        BeginThread(input->GetOwner(), inputIndex);
                     }
                     // If we are continuing the current thread, continue it.
                     else
                     {
                        StackData& stack = data.stack[stackIndex];
                        stack.node = input->GetOwner();
                        stack.index = inputIndex;
                        stack.first = true;

                        // From now on, all new active outputs create their own threads.
                        makeNewThread = true;
                     }
                  }
               }
            }
         }

         ProcessUpdatedNode(currentNode, first, continued, input, outputs);

         // If we did not continue this current thread, stop it.
         if (!makeNewThread)
         {
            data.stack[stackIndex].node = NULL;
         }
      }

      // If we did not continue the current stack, and we don't have any more
      // sub-threads in this stack.
      if (!makeNewThread && data.stack[stackIndex].subThreads.empty())
      {
         // Pop this stack from the list.
         data.stack.erase(data.stack.begin() + stackIndex);

         // If we do not have any remaining stack items, we can remove this thread.
         if (data.stack.empty())
         {
            return false;
         }
      }

      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::ProcessUpdatedNode(Node* node, bool first, bool continued, int input, std::vector<OutputLink*> outputs)
   {
      // If this node is flagged to log its comment, log it.
      if (GetNodeLogging() && node->GetNodeLogging())
      {
         std::string message;
         // If this is the first execution of this node.
         if (first)
         {
            // If we are only updating this node once.
            if (!continued)
            {
               message = "Executed ";
            }
            // If we are going to keep the thread on this node.
            else
            {
               message = "Began ";
            }
         }
         // If this is not the first execution of this node, then only
         // print the log if this node is finishing.
         else if (!continued)
         {
            message = "Finished ";
         }
         // If we have triggered any outputs, then we need to log it.
         else if (outputs.size())
         {
            message = "Updated ";
         }

         // If we are logging a message.
         if (!message.empty())
         {
            if (input < (int)node->GetInputLinks().size())
            {
               InputLink* link = &node->GetInputLinks()[input];
               if (link)
               {
                  message += "Input (" + link->GetName() + ") on ";
               }
            }

            message += "Node \'" + node->GetType().GetFullName();
            if (!node->GetComment().empty())
            {
               message += " - " + node->GetComment() + "\' ";
            }
            else message += "\'";

            if (outputs.size())
            {
               message += "and Output (";

               int count = (int)outputs.size();
               for (int index = 0; index < count; index++)
               {
                  message += outputs[index]->GetName();

                  if (index < count - 1) message += ", ";
               }
               message += ")";
            }

            mLogger->LogMessage(dtUtil::Log::LOG_ALWAYS, "dtDirector::Director::UpdateThread", 531, message);
         }
      }
   }
}
