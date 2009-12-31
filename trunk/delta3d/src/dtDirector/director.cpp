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
      , mQueueingThreads(false)
      , mRecording(false)
      , mRecordTime(0.0f)
      , mLogNodes(false)
   {
      mPlayer = "";
      mLogger = &dtUtil::Log::GetInstance();
   }

   //////////////////////////////////////////////////////////////////////////
   Director::~Director()
   {
      Clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Init(dtDAL::Map* map)
   {
      if (!mGraph.valid())
      {
         mGraph = new DirectorGraph(this);
         mGraph->SetName("Director Script");
      }

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

      mThreads.clear();

      ClearRecordingData(mRecordThreads);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::ClearRecordingData(std::vector<RecordThreadData*>& threads)
   {
      // Iterate through each thread.
      int threadCount = (int)threads.size();
      for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
      {
         RecordThreadData* thread = threads[threadIndex];
         if (thread)
         {
            // Iterate through all of the nodes in this thread.
            int nodeCount = (int)thread->nodes.size();
            for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
            {
               RecordNodeData& node = thread->nodes[nodeIndex];

               // Recursively clear the sub threads first.
               ClearRecordingData(node.subThreads);
            }

            // No free our thread memory.
            delete thread;
         }
      }
      threads.clear();
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
      mRecordTime += delta;

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
      // If we are queuing threads now, add the new thread data to the queue
      // for later.
      if (mQueueingThreads)
      {
         ThreadQueue queue;
         queue.node = node;
         queue.input = index;
         queue.isStack = false;
         mThreadQueue.push_back(queue);
         return;
      }

      std::vector<ThreadData>* threadList = &mThreads;
      int curThread = mCurrentThread;

      RecordThreadData* recordThread = NULL;

      while (curThread > -1)
      {
         ThreadData& t = (*threadList)[curThread];
         if (!t.stack.empty())
         {
            StackData& s = t.stack[t.stack.size()-1];
            curThread = s.currentThread;
            threadList = &s.subThreads;
            recordThread = t.recordThread;

            // If this is the current running stack item, and it does
            // not have an active running node, use this instead
            // of creating a new sub-thread.
            if (curThread == -1 && !s.node && node)
            {
               s.node = node;
               s.index = index;
               return;
            }
         }
         else break;
      }

      if (!threadList) return;

      ThreadData data;
      StackData stack;
      stack.node = node;
      stack.index = index;
      stack.first = true;
      stack.currentThread = -1;

      data.recordThread = NULL;

      // If we are recording, start recording this new thread.
      if (mRecording)
      {
         RecordThreadData* newRecordThread = new RecordThreadData();
         data.recordThread = newRecordThread;

         if (recordThread)
         {
            if (!recordThread->nodes.empty())
            {
               RecordNodeData& recordNode = recordThread->nodes[recordThread->nodes.size()-1];
               recordNode.subThreads.push_back(newRecordThread);
            }
         }
         else
         {
            mRecordThreads.push_back(newRecordThread);
         }
      }

      data.stack.push_back(stack);
      threadList->push_back(data);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::PushStack(Node* node, int index)
   {
      // If we are queuing threads now, add the new stack data to the queue
      // for later.
      if (mQueueingThreads)
      {
         ThreadQueue queue;
         queue.node = node;
         queue.input = index;
         queue.isStack = true;
         mThreadQueue.push_back(queue);
         return;
      }

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

   ////////////////////////////////////////////////////////////////////////////////
   void Director::StartRecording()
   {
      mRecording = true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::PauseRecording()
   {
      mRecording = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::StopRecording()
   {
      mRecording = false;

      ClearRecordingData(mRecordThreads);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::SaveRecording(const std::string& filename)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

      bool result = false;
      FILE* file = NULL;
      fopen_s(&file, (std::string("directors/") + filename + ".dtDirReplay").c_str(), "wb");
      if (file)
      {
         result = WriteRecordThreads(file, mRecordThreads);
         fclose(file);
      }

      fileUtils.PopDirectory();
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::LoadRecording(const std::string& filename)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

      bool result = false;
      FILE* file = NULL;
      fopen_s(&file, (std::string("directors/") + filename + ".dtDirReplay").c_str(), "rb");
      if (file)
      {
         // First clear all current recording data.
         ClearRecordingData(mRecordThreads);

         result = ReadRecordThreads(file, mRecordThreads);
         fclose(file);
      }

      fileUtils.PopDirectory();
      return result;
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
         mQueueingThreads = true;
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

         // Process our queued threads.
         mQueueingThreads = false;
         count = (int)mThreadQueue.size();
         for (int index = 0; index < count; index++)
         {
            ThreadQueue& queue = mThreadQueue[index];

            if (queue.isStack)
            {
               PushStack(queue.node, queue.input);
            }
            else
            {
               BeginThread(queue.node, queue.input);
            }
         }
         mThreadQueue.clear();

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
      // If we are recording, then record this node.
      if (mRecording)
      {
         bool doRecord = false;

         // If this is the first execution of this node, we always want to record this.
         // otherwise, only record if this node is triggering any outputs.
         if (first || outputs.size())
         {
            doRecord = true;
         }

         if (doRecord)
         {
            // Find the current thread recording.
            std::vector<ThreadData>* threadList = &mThreads;
            int curThread = mCurrentThread;

            RecordThreadData* recordThread = NULL;

            while (curThread > -1)
            {
               ThreadData& t = (*threadList)[curThread];
               if (!t.stack.empty())
               {
                  StackData& s = t.stack[t.stack.size()-1];
                  curThread = s.currentThread;
                  threadList = &s.subThreads;
                  recordThread = t.recordThread;
               }
               else break;
            }

            if (recordThread)
            {
               RecordNodeData nodeData;
               nodeData.time = mRecordTime;
               nodeData.nodeID = node->GetID();
               nodeData.input = "";

               if (input > -1 && input < (int)node->GetInputLinks().size())
               {
                  InputLink& link = node->GetInputLinks()[input];
                  nodeData.input = link.GetName();
               }

               int count = (int)outputs.size();
               for (int index = 0; index < count; index++)
               {
                  OutputLink* link = outputs[index];
                  if (link) nodeData.outputs.push_back(link->GetName());
               }

               recordThread->nodes.push_back(nodeData);
            }
         }
      }

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

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::WriteRecordThreads(FILE* file, std::vector<RecordThreadData*>& threads)
   {
      if (!file) return false;

      int count = (int)threads.size();
      fwrite(&count, sizeof(int), 1, file);

      for (int index = 0; index < count; index++)
      {
         std::vector<RecordNodeData>& nodes = threads[index]->nodes;
         if (!WriteRecordNodes(file, nodes))
         {
            return false;
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::ReadRecordThreads(FILE* file, std::vector<RecordThreadData*>& threads)
   {
      if (!file) return false;

      int count = 0;
      fread(&count, sizeof(int), 1, file);

      for (int index = 0; index < count; index++)
      {
         RecordThreadData* threadData = new RecordThreadData();
         if (!ReadRecordNodes(file, threadData->nodes))
         {
            return false;
         }

         threads.push_back(threadData);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::WriteRecordNodes(FILE* file, std::vector<RecordNodeData>& nodes)
   {
      if (!file) return false;

      int count = (int)nodes.size();
      fwrite(&count, sizeof(int), 1, file);

      for (int index = 0; index < count; index++)
      {
         RecordNodeData& node = nodes[index];

         fwrite(&node.time, sizeof(float), 1, file);

         if (!WriteString(file, node.nodeID.ToString()))
         {
            return false;
         }

         if (!WriteString(file, node.input))
         {
            return false;
         }

         int outCount = (int)node.outputs.size();
         fwrite(&outCount, sizeof(int), 1, file);

         for (int outIndex = 0; outIndex < outCount; outIndex++)
         {
            if (!WriteString(file, node.outputs[outIndex]))
            {
               return false;
            }
         }

         if (!WriteRecordThreads(file, node.subThreads))
         {
            return false;
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::ReadRecordNodes(FILE* file, std::vector<RecordNodeData>& nodes)
   {
      if (!file) return false;

      int count = 0;
      fread(&count, sizeof(int), 1, file);

      for (int index = 0; index < count; index++)
      {
         RecordNodeData node;

         fread(&node.time, sizeof(float), 1, file);

         std::string token;
         if (!ReadString(file, token))
         {
            return false;
         }
         node.nodeID = token;

         if (!ReadString(file, node.input))
         {
            return false;
         }

         int outCount = 0;
         fread(&outCount, sizeof(int), 1, file);

         for (int outIndex = 0; outIndex < outCount; outIndex++)
         {
            std::string outName;
            if (!ReadString(file, outName))
            {
               return false;
            }

            node.outputs.push_back(outName);
         }

         if (!ReadRecordThreads(file, node.subThreads))
         {
            return false;
         }

         nodes.push_back(node);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::WriteString(FILE* file, const std::string& str)
   {
      if (!file) return false;

      size_t len = str.length();
      fwrite(&len, sizeof(size_t), 1, file);

      if (len <= 0) return true;
      fwrite(str.c_str(), sizeof(char), len, file);

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::ReadString(FILE* file, std::string& str)
   {
      if (!file) return false;

      size_t len;
      fread(&len, sizeof(size_t), 1, file);

      if (len > 0)
      {
         str.resize(len);
         fread(&str[0], sizeof(char), len, file);
      }

      return true;
   }
}
