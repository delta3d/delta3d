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

#include <dtDAL/actorproperty.h>
#include <dtDAL/project.h>
#include <dtDAL/stringactorproperty.h>

#include <dtDirector/directorxml.h>
#include <dtDirector/nodemanager.h>

#include <dtUtil/datapathutils.h>

#include <osgDB/FileNameUtils>

namespace dtDirector
{
   IMPLEMENT_MANAGEMENT_LAYER(DirectorInstance)

   ////////////////////////////////////////////////////////////////////////////////
   DirectorInstance::DirectorInstance(Director* director, const std::string& name)
      : mDirector(director)
   {
      RegisterInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorInstance::~DirectorInstance()
   {
      DeregisterInstance(this);
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   Director::Director()
      : mCurrentThread(-1)
      , mQueueingThreads(false)
      , mRecording(false)
      , mRecordTime(0.0f)
      , mMap(NULL)
      , mModified(false)
      , mStarted(false)
      , mDebugging(false)
      , mShouldStep(false)
      , mGraph(NULL)
      , mLogNodes(false)
      , mLogger(NULL)
      , mGameManager(NULL)
      , mMessageGMComponent(NULL)
      , mParent(NULL)
      , mActive(true)
   {
      mPlayer = "";
      mLogger = &dtUtil::Log::GetInstance();

      mResource = dtDAL::ResourceDescriptor::NULL_RESOURCE;

      mBaseInstance = new DirectorInstance(this);
   }

   //////////////////////////////////////////////////////////////////////////
   Director::~Director()
   {
      Clear();

      if (mMessageGMComponent.valid() && mGameManager)
      {
         // If we're the last director running, remove the Message component from the GM
         if (mMessageGMComponent->referenceCount() == 1)
         {
            mGameManager->RemoveComponent(*mMessageGMComponent);
         }
         mMessageGMComponent = NULL;
      }

      mBaseInstance = NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Init(dtGame::GameManager* gm, dtDAL::Map* map)
   {
      mGameManager = gm;

      Clear();

      if (gm)
      {
         // First check if this component has been created already.
         mMessageGMComponent = dynamic_cast<dtDirector::MessageGMComponent*>(gm->GetComponentByName("DirectorMessageGMComponent"));

         // If it hasn't, then create one.
         if (!mMessageGMComponent.valid())
         {
            mMessageGMComponent = new dtDirector::MessageGMComponent();
            gm->AddComponent(*mMessageGMComponent);
         }
      }

      SetMap(map);

      LoadDefaultLibraries();

      BuildPropertyMap();

      mGraph->BuildPropertyMap(true);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::LoadDefaultLibraries()
   {
      AddLibrary("dtDirectorNodes", "");
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Clear()
   {
      ClearThreads();

      // Reset our graph data.
      mGraph = new DirectorGraph(this);
      SetName("Director Script");
      SetComment("");
      SetDescription("");
      SetAuthor("");
      SetCopyright("");

      mResource = dtDAL::ResourceDescriptor::NULL_RESOURCE;

      mLibraries.clear();
      mLibraryVersionMap.clear();

      LoadDefaultLibraries();

      mScriptName = "";
      if (mBaseInstance.valid())
      {
         mBaseInstance->SetName("Director: None");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string Director::GetDefaultPropertyKey() const
   {
      return "Director";
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::SetNotifier(DirectorNotifier* notifier)
   {
      if (GetParent())
      {
         GetParent()->SetNotifier(notifier);
         return;
      }

      mNotifier = notifier;
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorNotifier* Director::GetNotifier() const
   {
      if (GetParent())
      {
         return GetParent()->GetNotifier();
      }

      return mNotifier;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::ClearThreads()
   {
      // Save out any currently recorded data if it exists.
      if (mRecording && !mScriptName.empty())
      {
         SaveRecording(mScriptName);
      }
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

            // Now free our thread memory.
            delete thread;
         }
      }
      threads.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetParent(Director* parent)
   {
      mParent = parent;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::Map* Director::GetMap()
   {
      return mMap;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetMap(dtDAL::Map* map)
   {
      mMap = map;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtUtil::Log* Director::GetLogger()
   {
      return mLogger;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::LoadScript(const std::string& scriptFile)
   {
      Clear();

      if (scriptFile.empty()) return false;

      std::string fileName = osgDB::getNameLessExtension(scriptFile) + ".dtdir";

      bool hasContext = dtDAL::Project::GetInstance().IsContextValid();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      if (hasContext)
      {
         fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());
      }
      else
      {
         fileName = dtUtil::FindFileInPathList(fileName);
      }

      dtCore::RefPtr<DirectorParser> parser = new DirectorParser();
      if (parser)
      {
         try
         {
            parser->Parse(this, mMap.get(), fileName);
         }
         catch (const dtUtil::Exception& e)
         {
            std::string error = "Unable to parse " + scriptFile + " with error " + e.What();
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__, error.c_str());
            if (hasContext)
            {
               fileUtils.PopDirectory();
            }
            throw e;
         }

         if (hasContext)
         {
            fileUtils.PopDirectory();
         }
         mModified = parser->HasDeprecatedProperty();
         mScriptName = fileName;

         if (mBaseInstance.valid())
         {
            mBaseInstance->SetName(std::string("Director: ") +
               osgDB::getStrippedName(scriptFile));
         }

         return true;
      }

      if (hasContext)
      {
         fileUtils.PopDirectory();
      }
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::SaveScript(const std::string& scriptFile)
   {
      DirectorWriter* writer = new DirectorWriter();
      if (writer)
      {
         std::string fileName = osgDB::getNameLessExtension(scriptFile) + ".dtdir";

         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

         try
         {
            writer->Save(this, fileName);
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
      // On the first update, make sure we notify all nodes that are have started.
      if (!mStarted)
      {
         // Notify all nodes that the script is started.
         std::vector<Node*> nodes;
         GetAllNodes(nodes);
         int count = (int)nodes.size();
         for (int index = 0; index < count; index++)
         {
            Node* node = nodes[index];
            if (node) node->OnStart();
         }

         mStarted = true;
      }

      mRecordTime += delta;

      if (mNotifier.valid())
      {
         mNotifier->Update(!(mDebugging && !mShouldStep));
      }

      bool continued = false;

      //do
      {
         continued = false;

         // Update all threads.
         for (mCurrentThread = 0; mCurrentThread < (int)mThreads.size(); mCurrentThread++)
         {
            continued |= UpdateThread(mThreads[mCurrentThread], simDelta, delta);

            // If this thread has no more stacks in its thread, we can
            // remove it.
            if (mThreads[mCurrentThread].stack.empty())
            {
               mThreads.erase(mThreads.begin() + mCurrentThread);
               mCurrentThread--;
            }
         }

         // We reset the current thread value so any new threads created outside
         // of the update will generate a brand new main thread.
         mCurrentThread = -1;

         CleanThreads();
      }
      //while (continued);

      if (mNotifier.valid())
      {
         if (mShouldStep)
         {
            mNotifier->OnStepDebugging();
         }
      }

      mShouldStep = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::BeginThread(Node* node, int index, bool immediate, bool reverseQueue)
   {
      // Always create threads on the proxy if able.
      if (GetParent())
      {
         GetParent()->BeginThread(node, index, immediate);
         return;
      }

      // If we are queuing threads now, add the new thread data to the queue
      // for later.
      if (mQueueingThreads)
      {
         ThreadQueue queue;
         queue.node = node;
         queue.input = index;
         queue.isStack = false;

         if (reverseQueue)
         {
            mThreadQueue.insert(mThreadQueue.begin(), queue);
         }
         else
         {
            mThreadQueue.push_back(queue);
         }
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

      bool continued = false;

      do
      {
         continued = false;
         if (mStarted && immediate && mCurrentThread == -1 && mThreads.size() > 0)
         {
            mCurrentThread = mThreads.size() - 1;

            continued |= UpdateThread(mThreads[mCurrentThread], 0.0f, 0.0f);

            // If this thread has no more stacks in its thread, we can
            // remove it.
            if (mThreads[mCurrentThread].stack.empty())
            {
               mThreads.erase(mThreads.begin() + mCurrentThread);
            }

            CleanThreads();

            mCurrentThread = -1;
         }
      }
      while (continued);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::PushStack(Node* node, int index)
   {
      // Always push stacks on the proxy director if able.
      if (GetParent())
      {
         GetParent()->PushStack(node, index);
         return;
      }

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
   bool Director::IsRunning()
   {
      return !mThreads.empty();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetName(const std::string& name)
   {
      mGraph->SetName(name);
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Director::GetName()
   {
      return mGraph->GetName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetDescription(const std::string& description)
   {
      mDescription = description;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string& Director::GetDescription()
   {
      return mDescription;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetAuthor(const std::string& author)
   {
      mAuthor = author;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string& Director::GetAuthor()
   {
      return mAuthor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetComment(const std::string& comment)
   {
      mGraph->SetComment(comment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string& Director::GetComment()
   {
      return mGraph->GetComment();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetCopyright(const std::string& copyright)
   {
      mCopyright = copyright;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string& Director::GetCopyright()
   {
      return mCopyright;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetCreateDateTime(const std::string& time)
   {
      mCreationTime = time;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string& Director::GetCreateDateTime()
   {
      return mCreationTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetNodeLogging(bool enabled)
   {
      mLogNodes = enabled;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::GetNodeLogging()
   {
      return mLogNodes;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string Director::GetScriptType() const
   {
      return "Scenario";
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::IsLibraryTypeSupported(const std::string& libraryType) const
   {
      if (libraryType == "Core")
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetPlayer(const dtCore::UniqueId& player)
   {
      mPlayer = player;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId Director::GetPlayer()
   {
      return mPlayer;
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
   bool Director::IsRecording()
   {
      return mRecording;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::SaveRecording(const std::string& filename)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());

      bool result = false;
      FILE* file = fopen((osgDB::getNameLessExtension(filename) + ".dtdirreplay").c_str(), "wb");
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
      FILE* file = fopen((osgDB::getNameLessExtension(filename) + ".dtdirreplay").c_str(), "rb");
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
   bool Director::InsertLibrary(unsigned pos, const std::string& name, const std::string& version)
   {
      // Attempt to load the library.
      if (!NodeManager::GetInstance().IsInRegistry(name))
      {
         if (!NodeManager::GetInstance().LoadNodeRegistry(name))
         {
            return false;
         }
      }

      std::string libraryType = NodeManager::GetInstance().GetNodeLibraryType(name);
      if (!IsLibraryTypeSupported(libraryType))
      {
         return false;
      }

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

      return true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::AddLibrary(const std::string& name, const std::string& version)
   {
      return InsertLibrary(mLibraries.size(), name, version);
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

   ////////////////////////////////////////////////////////////////////////////////
   DirectorGraph* Director::GetGraphRoot()
   {
      return mGraph.get();
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
      mGraph->GetNodes(name, category, property, value, outNodes);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::GetAllNodes(std::vector<Node*>& outNodes)
   {
      mGraph->GetAllNodes(outNodes);
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

   ////////////////////////////////////////////////////////////////////////////////
   void Director::ToggleDebugEnabled(bool enabled)
   {
      if (GetParent())
      {
         GetParent()->ToggleDebugEnabled(enabled);
      }

      if (mNotifier.valid())
      {
         if (!mDebugging && enabled)
         {
            mNotifier->OnBeginDebugging();
         }
         else if (mDebugging && !enabled)
         {
            mNotifier->OnEndDebugging();
         }
      }

      mDebugging = enabled;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::IsDebugging() const
   {
      if (GetParent())
      {
         return GetParent()->IsDebugging();
      }

      return mDebugging;
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void Director::StepDebugger()
   {
      if (GetParent())
      {
         GetParent()->StepDebugger();
      }

      mShouldStep = true;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::UpdateThread(ThreadData& data, float simDelta, float delta)
   {
      // If there is no stack, this thread is finished.
      if (data.stack.empty()) return false;

      int stackIndex = data.stack.size() - 1;
      if (stackIndex < 0) return false;

      StackData& stack = data.stack[stackIndex];

      bool continued = false;

      // Update all the sub-threads in the stack.
      for (stack.currentThread = 0; stack.currentThread < (int)stack.subThreads.size(); stack.currentThread++)
      {
         continued |= UpdateThread(stack.subThreads[stack.currentThread], simDelta, delta);

         if (stack.subThreads[stack.currentThread].stack.empty())
         {
            stack.subThreads.erase(stack.subThreads.begin() + stack.currentThread);
            stack.currentThread--;
         }
      }
      stack.currentThread = -1;

      // Threads always update the first item in the stack,
      // all other stack items are "sleeping".
      if ((!IsDebugging() || mShouldStep) && stack.node.valid())
      {
         Node* currentNode = stack.node.get();
         int   input       = stack.index;
         bool  first       = stack.first;
         stack.first = false;

         // If the update result is true, then we want to immediately
         // create a new thread on any new events.  Otherwise, our first
         // new thread will be a continuation of the current active thread.
         mQueueingThreads = true;
         bool isLatentNode = currentNode->Update(simDelta, delta, input, first);
         continued = !isLatentNode;

         // If we are just starting this action node and it is latent,
         // register this node for messages.
         if (!currentNode->AsEventNode())
         {
            if (first && isLatentNode)
            {
               currentNode->RegisterMessages();
            }
            else if (!first && !isLatentNode)
            {
               currentNode->UnRegisterMessages();
            }
         }

         // Check for activated outputs and create new threads for them.
         std::vector<OutputLink*> outputs;
         int outputCount = (int)currentNode->GetOutputLinks().size();
         for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
         {
            OutputLink* output = &currentNode->GetOutputLinks()[outputIndex];
            if (output->Test())
            {
               // Check for redirection of the output.
               if (output->GetRedirectLink()) output = output->GetRedirectLink();

               outputs.push_back(output);

               int linkCount = (int)output->GetLinks().size();
               for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
               {
                  InputLink* input = output->GetLinks()[linkIndex];
                  if (!input) continue;

                  // Disabled nodes are ignored.
                  if (!input->GetOwner()->IsEnabled()) continue;

                  // Check for redirection of the input.
                  if (input->GetRedirectLink())
                  {
                     input = input->GetRedirectLink();

                     // Disabled nodes are ignored.
                     if (!input->GetOwner()->IsEnabled()) continue;
                  }

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
                     BeginThread(input->GetOwner(), inputIndex, false, true);
                  }
               }
            }
         }

         ProcessUpdatedNode(currentNode, first, continued, input, outputs);

         // Process our queued threads.
         mQueueingThreads = false;
         int count = (int)mThreadQueue.size();
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

         // If the current node is not latent, then
         // we can clear the node referenced by this
         // thread.
         if (!isLatentNode)
         {
            data.stack[stackIndex].node = NULL;
         }
      }

      // If we did not continue the current stack, and we don't have any more
      // sub-threads in this stack.
      if (!data.stack[stackIndex].node &&
         data.stack[stackIndex].subThreads.empty())
      {
         // Pop this stack from the list.
         data.stack.erase(data.stack.begin() + stackIndex);

         // If we do not have any remaining stack items, we can remove this thread.
         if (data.stack.empty())
         {
            return false;
         }
      }

      return continued;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::CleanThreads()
   {
      for (int threadIndex = 0; threadIndex < (int)mThreads.size(); ++threadIndex)
      {
         if (mThreads[threadIndex].stack.size())
         {
            StackData& stack = mThreads[threadIndex].stack.back();
            if (mThreads[threadIndex].stack.size() == 1 && !stack.node)
            {
               mThreads.insert(mThreads.end(),
                  stack.subThreads.begin(),
                  stack.subThreads.end());

               mThreads[threadIndex].stack.pop_back();
            }
            else
            {
               CleanThread(stack);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::CleanThread(StackData& data)
   {
      for (int threadIndex = 0; threadIndex < (int)data.subThreads.size(); ++threadIndex)
      {
         if (data.subThreads[threadIndex].stack.size())
         {
            StackData& stack = data.subThreads[threadIndex].stack.back();
            if (data.subThreads[threadIndex].stack.size() == 1 && !stack.node)
            {
               data.subThreads.insert(data.subThreads.end(),
                  stack.subThreads.begin(),
                  stack.subThreads.end());

               data.subThreads[threadIndex].stack.pop_back();
            }
            else
            {
               CleanThread(stack);
            }
         }
      }
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
            if (continued)
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
         else if (continued)
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

      if (mNotifier.valid())
      {
         std::string inputName;
         if (input < (int)node->GetInputLinks().size())
         {
            InputLink* link = &node->GetInputLinks()[input];
            if (link)
            {
               inputName = link->GetName();
            }
         }

         std::vector<std::string> outputNames;
         for (int index = 0; index < (int)outputs.size(); ++index)
         {
            OutputLink* link = outputs[index];
            if (link)
            {
               outputNames.push_back(link->GetName());
            }
         }

         mNotifier->OnNodeExecution(node, inputName, outputNames);

         // If we are debugging, then we need to pause every new node.
         if (IsDebugging())
         {
            mNotifier->BreakNode(node);
         }
         // Toggle debugging mode on a break point.
         else if (mNotifier->ShouldBreak(node))
         {
            ToggleDebugEnabled(true);
            mNotifier->BreakNode(node, true);
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

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<Director::RecordThreadData*> Director::GetRecordingData()
   {
      return mRecordThreads;
   }
}
