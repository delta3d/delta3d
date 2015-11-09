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

#include <dtCore/timer.h>

#include <dtCore/project.h>
#include <dtCore/actorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirector/nodemanager.h>
#include <dtDirector/directortypefactory.h>

#include <dtUtil/exception.h>


#define SAFETY_TIMER 0.1f

namespace dtDirector
{
   dtCore::UniqueId Director::mPlayer = dtCore::UniqueId("");
   std::map<std::string, std::vector<ValueNode*> > Director::mGlobalValues;
   bool Director::mApplyingGlobalValue = false;

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   Director::Director()
      : mImmediateMode(false)
      , mCurrentThread(-1)
      , mThreadID(0)
      , mQueueingThreads(false)
      , mIsVisibleInInspector(true)
      , mMap(NULL)
      , mModified(false)
      , mStarted(false)
      , mLoading(false)
      , mDebugging(false)
      , mShouldStep(false)
      , mGraph(NULL)
      , mLogNodes(false)
      , mGameManager(NULL)
      , mMessageGMComponent(NULL)
      , mParent(NULL)
      , mHasDeprecatedProperty(false)
      , mIsImported(false)
      , mMasterNodeFreeIndex(-1)
      , mMasterGraphFreeIndex(-1)
      , mEnabled(true)
      , mActive(true)
   {
      mScriptOwner = "";

      mResource = dtCore::ResourceDescriptor::NULL_RESOURCE;

      DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
      if (factory)
      {
         factory->RegisterScriptInstance(this);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   Director::~Director()
   {
      DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
      if (factory)
      {
         factory->UnRegisterScriptInstance(this);
      }

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
   }

   dtCore::RefPtr<dtCore::ObjectType> Director::DIRECTOR_TYPE(new dtCore::ObjectType("Director", "dtDirector"));
   const dtCore::ObjectType& Director::GetObjectType() const { return *Director::DIRECTOR_TYPE; }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Director> Director::Clone(Director* parent)
   {
      dtCore::RefPtr<Director> newDirector = NULL;

      DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
      if (factory)
      {
         std::string scriptType = GetScriptType();
         newDirector = factory->CreateDirector(scriptType);
         if (newDirector)
         {
            newDirector->Init(mGameManager, mMap);
            newDirector->mScriptName = mScriptName;
            newDirector->mScriptOwner = mScriptOwner;
            newDirector->mResource = mResource;
            newDirector->mModified = mModified;
            newDirector->mStarted = false;
            newDirector->mLoading = true;
            newDirector->mDebugging = false;
            newDirector->mShouldStep = false;
            newDirector->mLibraries = mLibraries;
            newDirector->mLibraryVersionMap = mLibraryVersionMap;
            newDirector->mLogNodes = mLogNodes;
            newDirector->mMissingNodeTypes = mMissingNodeTypes;
            newDirector->mMissingLibraries = mMissingLibraries;
            newDirector->mMissingImportedScripts = mMissingImportedScripts;
            newDirector->mHasDeprecatedProperty = mHasDeprecatedProperty;
            newDirector->SetActive(mActive);
            newDirector->CopyPropertiesFrom(*this);
            newDirector->SetImported(IsImported());
            newDirector->SetParent(parent);

            // Clone the imported scripts.
            int count = (int)mImportedScriptList.size();
            for (int index = 0; index < count; ++index)
            {
               Director* imported = mImportedScriptList[index];
               if (imported)
               {
                  dtCore::RefPtr<Director> importedClone = imported->Clone(parent? parent: newDirector.get());
                  if (importedClone)
                  {
                     newDirector->mImportedScriptList.push_back(importedClone);
                     importedClone->SetParent(newDirector);
                  }
               }
            }

            GetGraphRoot()->Clone(newDirector);

            newDirector->mLoading = false;
         }
      }

      return newDirector;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Init(dtGame::GameManager* gm, dtCore::Map* map)
   {
      mGameManager = gm;

      Clear();

      if (gm)
      {
         // First check if this component has been created already.
         mMessageGMComponent = dynamic_cast<dtDirector::MessageGMComponent*>(gm->GetComponentByName(dtDirector::MessageGMComponent::TYPE->GetName()));

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
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::LoadDefaultLibraries()
   {
      AddLibrary("dtDirectorNodes");
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Clear()
   {
      ClearThreads();

      // Reset our graph data.
      mGraph = new DirectorGraph(this);
      mGraph->BuildPropertyMap(true);
      SetName("Untitled");
      SetComment("");
      SetDescription("");
      SetAuthor("");
      SetCopyright("");

      mResource = dtCore::ResourceDescriptor::NULL_RESOURCE;

      mLibraries.clear();
      mLibraryVersionMap.clear();

      mImportedScriptList.clear();

      mModified = false;
      mMissingNodeTypes.clear();
      mMissingLibraries.clear();
      mMissingImportedScripts.clear();
      mHasDeprecatedProperty = false;

      LoadDefaultLibraries();

      mScriptName = "";
      mStarted = false;
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

      mDebugging = false;
      mShouldStep = false;
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
      mThreads.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::IsCachedInstance() const
   {
      if (GetParent())
      {
         return GetParent()->IsCachedInstance();
      }

      dtDirector::DirectorTypeFactory* factory = dtDirector::DirectorTypeFactory::GetInstance();
      if (factory)
      {
         const dtDirector::Director* cache = factory->GetCachedScript(GetScriptName());
         if (cache == this)
         {
            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::IsVisibleInInspector() const
   {
      return mIsVisibleInInspector;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetParent(Director* parent)
   {
      if (parent == this)
      {
         return;
      }

      if (mParent.valid())
      {
         mParent->RemoveChild(this);
         mIsVisibleInInspector = true;
      }

      mParent = parent;

      if (mParent.valid())
      {
         mParent->AddChild(this);
         mIsVisibleInInspector = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::AddChild(Director* child)
   {
      if (!child || child == this)
      {
         return;
      }

      int count = (int)mChildren.size();
      for (int index = 0; index < count; ++index)
      {
         if (mChildren[index].get() == child)
         {
            return;
         }
      }

      mChildren.push_back(child);
      child->mParent = this;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::RemoveChild(Director* child)
   {
      if (!child || child == this)
      {
         return;
      }

      int count = (int)mChildren.size();
      for (int index = 0; index < count; ++index)
      {
         if (mChildren[index].get() == child)
         {
            child->mParent = NULL;
            mChildren.erase(mChildren.begin() + index);
            return;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<dtCore::ObserverPtr<Director> >& Director::GetChildren() const
   {
      return mChildren;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDirector::Director* Director::ImportScript(const std::string& scriptResource)
   {
      int count = (int)mImportedScriptList.size();
      for (int index = 0; index < count; ++index)
      {
         // If this script is already inherited, then we can do nothing and bail out.
         Director* imported = mImportedScriptList[index];
         if (imported && imported->GetResource().GetResourceIdentifier() == scriptResource)
         {
            return NULL;
         }
      }

      // If we get here, we want to attempt to load our inherited script and
      // add it to our list.
      dtCore::ResourceDescriptor rd(scriptResource);

      DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
      if (factory)
      {
         try
         {
            std::string path = dtCore::Project::GetInstance().GetResourcePath(rd);

            dtCore::RefPtr<dtDirector::Director> importedScript = factory->LoadScript(path, GetGameManager(), GetMap(), true, this);
            if (importedScript.valid())
            {
               // If the imported script is not a valid script type, we cannot support
               // importing this script.
               if (GetScriptType() != importedScript->GetScriptType())
               {
                  std::string error = std::string("Unable to import ") + rd.GetDisplayName().c_str() + ":  Script type is not compatible.";
                  LOG_WARNING(error.c_str());
                  return NULL;
               }

               importedScript->SetImported(true);
               importedScript->SetScriptOwner(GetScriptOwner());
               importedScript->SetNodeLogging(GetNodeLogging());
               importedScript->SetResource(rd);

               if (HasStarted())
               {
                  importedScript->OnStart();
               }

               RecurseImportScriptGraphs(importedScript->GetGraphRoot(), GetGraphRoot());

               mImportedScriptList.push_back(importedScript);

               return importedScript;
            }
         }
         catch (const dtUtil::Exception& e)
         {
            std::string error = std::string("Unable to import ") + rd.GetDisplayName().c_str() + " with error " + e.What().c_str();
            LOG_WARNING(error.c_str());
            return NULL;
         }
      }

      // if we failed to add this script for any reason, return false.
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::RemoveImportedScript(const std::string& scriptResource)
   {
      int count = (int)mImportedScriptList.size();
      for (int index = 0; index < count; ++index)
      {
         Director* script = mImportedScriptList[index];

         if (script && scriptResource == script->GetResource().GetResourceIdentifier())
         {
            // Cleanup unused graphs that were part of this imported script.
            RecurseRemoveUnusedImportedGraphs(script->GetGraphRoot());

            script->SetParent(NULL);
            mImportedScriptList.erase(mImportedScriptList.begin() + index);
            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDirector::Director* Director::GetImportedScript(const std::string& scriptResource)
   {
      int count = (int)mImportedScriptList.size();
      for (int index = 0; index < count; ++index)
      {
         Director* script = mImportedScriptList[index];

         if (script && scriptResource == script->GetResource().GetResourceIdentifier())
         {
            return script;
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<dtCore::RefPtr<dtDirector::Director> >& Director::GetImportedScriptList() const
   {
      return mImportedScriptList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& Director::GetID() const
   {
      return mID;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::Map* Director::GetMap()
   {
      return mMap;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetMap(dtCore::Map* map)
   {
      mMap = map;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::LoadScript(const std::string& scriptFile)
   {
      DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
      if (factory)
      {
         factory->LoadScript(this, scriptFile);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::SaveScript(const std::string& scriptFile)
   {
      DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
      if (factory)
      {
         factory->SaveScript(this, scriptFile);
      }
   }

   /////////////////////////////////////////////////////////////////
   const std::set<std::string>& Director::GetMissingNodeTypes()
   {
      return mMissingNodeTypes;
   }

   /////////////////////////////////////////////////////////////////
   const std::vector<std::string>& Director::GetMissingLibraries()
   {
      return mMissingLibraries;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<std::string>& Director::GetMissingImportedScripts()
   {
      return mMissingImportedScripts;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::HasDeprecatedProperty() const
   {
      return mHasDeprecatedProperty;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Director::StateData Director::GetState() const
   {
      StateData data;

      int count = (int)mThreads.size();
      for (int index = 0; index < count; ++index)
      {
         GetThreadState(data.threads, mThreads[index]);
      }

      GetValueState(data.values, GetGraphRoot());

      count = (int)mChildren.size();
      for (int index = 0; index < count; ++index)
      {
         GetValueState(data.values, mChildren[index].get());
      }

      return data;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::RestoreState(const StateData& state)
   {
      mThreads.clear();

      int count = (int)state.threads.size();
      for (int index = 0; index < count; ++index)
      {
         RestoreThreadState(state.threads[index], mThreads);
      }

      count = (int)state.values.size();
      for (int index = 0; index < count; ++index)
      {
         const StateValueData& data = state.values[index];
         Node* node = RecurseFindNode(data.id, this);
         if (node)
         {
            ValueNode* valueNode = node->AsValueNode();
            if (valueNode)
            {
               valueNode->SetFormattedValue(data.value);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::BuildPropertyMap()
   {
      AddProperty(new dtCore::StringActorProperty(
         "Name", "Name",
         dtCore::StringActorProperty::SetFuncType(this, &Director::SetName),
         dtCore::StringActorProperty::GetFuncType(this, &Director::GetName),
         "The Name of the Director script."));

      AddProperty(new dtCore::StringActorProperty(
         "Description", "Description",
         dtCore::StringActorProperty::SetFuncType(this, &Director::SetDescription),
         dtCore::StringActorProperty::GetFuncType(this, &Director::GetDescription),
         "The Description of the Director script."));

      AddProperty(new dtCore::StringActorProperty(
         "Author", "Author",
         dtCore::StringActorProperty::SetFuncType(this, &Director::SetAuthor),
         dtCore::StringActorProperty::GetFuncType(this, &Director::GetAuthor),
         "The Author of the Director script."));

      AddProperty(new dtCore::StringActorProperty(
         "Comment", "Comment",
         dtCore::StringActorProperty::SetFuncType(this, &Director::SetComment),
         dtCore::StringActorProperty::GetFuncType(this, &Director::GetComment),
         "Comment"));

      AddProperty(new dtCore::StringActorProperty(
         "Copyright", "Copyright",
         dtCore::StringActorProperty::SetFuncType(this, &Director::SetCopyright),
         dtCore::StringActorProperty::GetFuncType(this, &Director::GetCopyright),
         "Copyright information."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::OnStart()
   {
      // Init imported scripts first.
      int count = (int)mImportedScriptList.size();
      for (int index = 0; index < count; ++index)
      {
         Director* imported = mImportedScriptList[index];
         if (imported)
         {
            imported->OnStart();
         }
      }

      if (mStarted)
      {
         return;
      }

      // Notify all nodes that the script is started.
      std::vector<Node*> nodes;
      GetAllNodes(nodes);
      count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         Node* node = nodes[index];
         if (node) node->OnStart();
      }

      mStarted = true;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::Update(float simDelta, float delta)
   {
      // On the first update, make sure we notify all nodes that are have started.
      if (!HasStarted())
      {
         OnStart();
      }

      if (mNotifier.valid())
      {
         mNotifier->Update(mDebugging, mShouldStep);
      }

      // Update all threads.
      for (mCurrentThread = 0; mCurrentThread < (int)mThreads.size(); mCurrentThread++)
      {
         UpdateThread(mThreads[mCurrentThread], simDelta, delta);
         if (mCurrentThread < 0 || mCurrentThread >= (int)mThreads.size())
         {
            continue;
         }

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

      if (mNotifier.valid() && mShouldStep)
      {
         mNotifier->OnStepDebugging();
      }

      mShouldStep = false;
   }

   //////////////////////////////////////////////////////////////////////////
   int Director::BeginThread(Node* node, int index, bool reverseQueue, bool immediate)
   {
      // Always create threads on the proxy if able.
      if (GetParent())
      {
         return GetParent()->BeginThread(node, index);
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
         return -1;
      }

      std::vector<ThreadData>* threadList = &mThreads;
      int* curThread = &mCurrentThread;

      int threadID = -1;
      if (mCurrentThread > -1)
      {
         threadID = mThreads[mCurrentThread].id;
      }

      while (*curThread > -1)
      {
         ThreadData& t = (*threadList)[*curThread];
         if (!t.stack.empty())
         {
            StackData& s = t.stack[t.stack.size()-1];
            curThread = &s.currentThread;
            threadList = &s.subThreads;

            // If this is the current running stack item, and it does
            // not have an active running node, use this instead
            // of creating a new sub-thread.
            if (*curThread == -1 && !s.node && node)
            {
               s.node = node;
               s.index = index;
               s.first = true;
               s.finished = false;
               return t.id;
            }
         }
         else break;
      }

      if (!threadList) return -1;

      if (threadList == &mThreads)
      {
         threadID = mThreadID++;
      }

      ThreadData data;
      data.id = threadID;

      StackData stack;
      stack.node = node;
      stack.index = index;
      stack.first = true;
      stack.finished = false;
      stack.data = NULL;
      stack.currentThread = -1;

      data.stack.push_back(stack);
      threadList->push_back(data);

      if (mStarted && !mImmediateMode && *curThread == -1 && !mDebugging && immediate)
      {
         dtCore::Timer_t time = dtCore::Timer::Instance()->Tick();

         mImmediateMode = true;
         *curThread = threadList->size() - 1;

         bool continued = false;

         do
         {
            continued = false;

            if (mNotifier.valid())
            {
               mNotifier->Update(mDebugging, mShouldStep);
            }

            continued |= UpdateThread((*threadList)[*curThread], 0.0f, 0.0f);

            // If this thread has no more stacks in its thread, we can
            // remove it.
            if (!(*threadList)[*curThread].stack.empty())
            {
               CleanThread((*threadList)[*curThread].stack.back());
            }
            else
            {
               threadList->erase(threadList->begin() + *curThread);
               continued = false;
            }

            mShouldStep = false;

            float delta = dtCore::Timer::Instance()->DeltaSec(time, dtCore::Timer::Instance()->Tick());
            if (delta > SAFETY_TIMER)
            {
               break;
            }
         }
         while (continued);

         *curThread = -1;

         mImmediateMode = false;
      }

      return threadID;
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
      stack.finished = false;
      stack.data = NULL;
      stack.currentThread = -1;

      std::vector<ThreadData>* threadList = &mThreads;
      int* curThread = &mCurrentThread;

      while (*curThread > -1)
      {
         ThreadData& t = (*threadList)[*curThread];
         if (t.stack[t.stack.size()-1].currentThread == -1)
         {
            t.stack.push_back(stack);

            break;
         }
         else
         {
            StackData& s = t.stack[t.stack.size()-1];
            curThread = &s.currentThread;
            threadList = &s.subThreads;
         }
      }

      if (*curThread > -1 && mStarted && !mImmediateMode && !mDebugging)
      {
         dtCore::Timer_t time = dtCore::Timer::Instance()->Tick();

         mImmediateMode = true;
         bool continued = false;

         do
         {
            continued = false;

            if (mNotifier.valid())
            {
               mNotifier->Update(mDebugging, mShouldStep);
            }

            continued |= UpdateThread((*threadList)[*curThread], 0.0f, 0.0f);

            // If this thread has no more stacks in its thread, we can
            // remove it.
            if (!(*threadList)[*curThread].stack.empty())
            {
               CleanThread((*threadList)[*curThread].stack.back());
            }
            else
            {
               threadList->erase(threadList->begin() + *curThread);
               *curThread--;
               continued = false;
            }

            mShouldStep = false;

            float delta = dtCore::Timer::Instance()->DeltaSec(time, dtCore::Timer::Instance()->Tick());
            if (delta > SAFETY_TIMER)
            {
               break;
            }
         }
         while (continued);

         mImmediateMode = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::IsRunning(int id)
   {
      if (id == -1)
      {
         return !mThreads.empty();
      }
      else
      {
         int count = (int)mThreads.size();
         for (int index = 0; index < count; ++index)
         {
            if (id == mThreads[index].id)
            {
               return true;
            }
         }
      }

      return false;
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
      //if (libraryType == "Core")
      //{
      //   return true;
      //}

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetPlayer(const dtCore::UniqueId& player)
   {
      mPlayer = player;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId Director::GetPlayer() const
   {
      return mPlayer;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::BaseActorObject* Director::GetPlayerActor() const
   {
      if (mGameManager)
      {
         return mGameManager->FindActorById(mPlayer);
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetScriptOwner(const dtCore::UniqueId& owner)
   {
      mScriptOwner = owner;

      // Tell any scripts that we are imported from about this change as well.
      int count = (int)mImportedScriptList.size();
      for (int index = 0; index < count; ++index)
      {
         Director* script = mImportedScriptList[index];
         if (script)
         {
            script->SetScriptOwner(owner);
         }
      }

      // Also iterate through all child-scripts and tell them of this.
      count = (int)mChildren.size();
      for (int index = 0; index < count; ++index)
      {
         Director* script = mChildren[index].get();
         if (script)
         {
            script->SetScriptOwner(owner);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId Director::GetScriptOwner() const
   {
      return mScriptOwner;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::BaseActorObject* Director::GetScriptOwnerActor() const
   {
      if (mGameManager)
      {
         return mGameManager->FindActorById(mScriptOwner);
      }
      return NULL;
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

      if (old != mLibraryVersionMap.end())
      {
         old->second = version;
      }
      else
      {
         mLibraryVersionMap.insert(make_pair(name, version));
      }

      bool alreadyExists = false;
      for (std::vector<std::string>::iterator i = mLibraries.begin(); i != mLibraries.end(); ++i)
      {
         if (*i == name)
         {
            mLibraries.erase(i);
            alreadyExists = true;
            break;
         }
      }

      if (pos < mLibraries.size())
         mLibraries.insert(mLibraries.begin() + pos, name);
      else
         mLibraries.push_back(name);

      return !alreadyExists;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::AddLibrary(const std::string& name, const std::string& version /*= ""*/)
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

   ////////////////////////////////////////////////////////////////////////////////
   const DirectorGraph* Director::GetGraphRoot() const
   {
      return mGraph.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetGraphRoot(DirectorGraph* root)
   {
      mGraph = root;
   }

   //////////////////////////////////////////////////////////////////////////
   DirectorGraph* Director::GetGraph(const ID& id, bool includeImportedScripts)
   {
      if (includeImportedScripts)
      {
         ID graphID = id;
         graphID.index = -1;
         int count = (int)mImportedScriptList.size();
         for (int index = 0; index < count; ++index)
         {
            Director* imported = mImportedScriptList[index];
            if (imported)
            {
               DirectorGraph* graph = imported->GetGraph(graphID, true);

               if (graph)
               {
                  return graph;
               }
            }
         }
      }

      int count = (int)mMasterGraphList.size();
      if (id.index > -1 && id.index < count)
      {
         if (mMasterGraphList[id.index].graph &&
             mMasterGraphList[id.index].graph->GetID().id == id.id)
         {
            return mMasterGraphList[id.index].graph;
         }
      }
      // If the index is not available, then we have to iterate and search
      // every graph.
      else
      {
         for (int index = 0; index < count; ++index)
         {
            if (mMasterGraphList[index].graph &&
               mMasterGraphList[index].graph->GetID().id == id.id)
            {
               return mMasterGraphList[index].graph;
            }
         }
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   Node* Director::GetNode(const ID& id, bool includeImportedScripts)
   {
      if (includeImportedScripts)
      {
         int count = (int)mImportedScriptList.size();
         for (int index = 0; index < count; ++index)
         {
            Director* imported = mImportedScriptList[index];
            if (imported)
            {
               Node* node = imported->GetNode(id, true);

               if (node)
               {
                  return node;
               }
            }
         }
      }

      int count = (int)mMasterNodeList.size();
      if (id.index > -1 && id.index < count)
      {
         if (mMasterNodeList[id.index].node &&
            mMasterNodeList[id.index].node->GetID().id == id.id)
         {
            return mMasterNodeList[id.index].node;
         }
      }
      // If the index is not available, then we have to iterate and search
      // every graph.
      else
      {
         for (int index = 0; index < count; ++index)
         {
            if (mMasterNodeList[index].node &&
               mMasterNodeList[index].node->GetID().id == id.id)
            {
               return mMasterNodeList[index].node;
            }
         }
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   const Node* Director::GetNode(const ID& id, bool includeImportedScripts) const
   {
      if (includeImportedScripts)
      {
         int count = (int)mImportedScriptList.size();
         for (int index = 0; index < count; ++index)
         {
            Director* imported = mImportedScriptList[index];
            if (imported)
            {
               Node* node = imported->GetNode(id, true);

               if (node)
               {
                  return node;
               }
            }
         }
      }

      int count = (int)mMasterNodeList.size();
      if (id.index > -1 && id.index < count)
      {
         if (mMasterNodeList[id.index].node &&
            mMasterNodeList[id.index].node->GetID().id == id.id)
         {
            return mMasterNodeList[id.index].node;
         }
      }
      // If the index is not available, then we have to iterate and search
      // every graph.
      else
      {
         for (int index = 0; index < count; ++index)
         {
            if (mMasterNodeList[index].node &&
               mMasterNodeList[index].node->GetID().id == id.id)
            {
               return mMasterNodeList[index].node;
            }
         }
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::GetNodes(const std::string& name, const std::string& category, std::vector<Node*>& outNodes, bool searchImportedScripts /*= false*/)
   {
      mGraph->GetNodes(name, category, outNodes, true, searchImportedScripts);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::GetNodes(const std::string& name, const std::string& category, const std::string& property, const std::string& value, std::vector<Node*>& outNodes, bool searchImportedScripts /*= false*/)
   {
      mGraph->GetNodes(name, category, property, value, outNodes, true, searchImportedScripts);
   }

   //////////////////////////////////////////////////////////////////////////
   void Director::GetAllNodes(std::vector<Node*>& outNodes, bool includeImportedScripts)
   {
      mGraph->GetAllNodes(outNodes, true, includeImportedScripts);
   }

   //////////////////////////////////////////////////////////////////////////
   ValueNode* Director::GetValueNode(const std::string& name)
   {
      return mGraph->GetValueNode(name);
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::DeleteGraph(const ID& id)
   {
      return mGraph->DeleteGraph(id);
   }

   //////////////////////////////////////////////////////////////////////////
   bool Director::DeleteNode(const ID& id)
   {
      Node* node = GetNode(id);
      if (node && node->GetGraph())
      {
         return node->GetGraph()->DeleteNode(id);
      }

      return false;
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
            mShouldStep = true;
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

   ////////////////////////////////////////////////////////////////////////////////
   void Director::CleanIDs()
   {
      int count = (int)mMasterNodeList.size();
      int lastIndex = count - 1;
      for (int index = 0; index < count; ++index)
      {
         MasterNodeListData& data = mMasterNodeList[index];

         // As soon as we find a free index we want to find a later used
         // index and move it there.
         if (!data.node)
         {
            for (; lastIndex > index; --lastIndex)
            {
               Node* node = mMasterNodeList[lastIndex].node;
               if (node)
               {
                  MasterListRemoveNode(node);
                  MasterListAddNode(node, index);
                  break;
               }
            }

            if (lastIndex <= index)
            {
               break;
            }
         }
      }

      count = (int)mMasterGraphList.size();
      lastIndex = count - 1;
      for (int index = 0; index < count; ++index)
      {
         MasterGraphListData& data = mMasterGraphList[index];

         // As soon as we find a free index we want to find a later used
         // index and move it there.
         if (!data.graph)
         {
            for (; lastIndex > index; --lastIndex)
            {
               DirectorGraph* graph = mMasterGraphList[lastIndex].graph;
               if (graph)
               {
                  MasterListRemoveGraph(graph);
                  MasterListAddGraph(graph, index);
                  break;
               }
            }

            if (lastIndex <= index)
            {
               break;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::SetEnabled(bool enabled)
   {
      mEnabled = enabled;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::IsEnabled(bool checkRecursively /*= true*/) const
   {
      bool enabled = mEnabled;
      if (checkRecursively && GetParent() != NULL)
      {
         enabled = GetParent()->IsEnabled() && enabled;
      }
      return enabled;
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
         if (0 <= stack.currentThread && stack.currentThread < (int)stack.subThreads.size() &&
             stack.subThreads[stack.currentThread].stack.empty())
         {
            stack.subThreads.erase(stack.subThreads.begin() + stack.currentThread);
            stack.currentThread--;
         }
      }
      stack.currentThread = -1;

      // Threads always update the first item in the stack,
      // all other stack items are "sleeping".
      if ((!IsDebugging() || mShouldStep) && stack.node.valid() && !stack.finished)
      {
         Node* currentNode = stack.node.get();
         int   input       = stack.index;
         bool  first       = stack.first;
         stack.first = false;

         // If the update result is true, then we want to immediately
         // create a new thread on any new events.  Otherwise, our first
         // new thread will be a continuation of the current active thread.
         mQueueingThreads = true;
         bool notFinished = currentNode->Update(simDelta, delta, input, first, stack.data);
         stack.finished = !notFinished;

         // Only continue updating if our current node is not a latent one
         // or if the latent node has finished running.
         if (!currentNode->AsLatentNode() || stack.finished)
         {
            continued = true;
         }

         // If we are just starting this action node and it is latent,
         // register this node for messages.
         if (!currentNode->AsEventNode())
         {
            if (first && notFinished)
            {
               currentNode->RegisterMessages();
            }
            else if (!first && !notFinished)
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
                     BeginThread(input->GetOwner(), inputIndex, true);
                  }
               }
            }
         }

         ProcessUpdatedNode(currentNode, first, continued, input, outputs);

         // Process our queued threads.
         mQueueingThreads = false;

         // We need to store a temporary copy of the current thread queue
         // and clear the global copy because during the execution of
         // the queue there is a chance that other threads created during
         // this process will cause other queues to happen, and we don't
         // want these separate queues to conflict with each other.
         std::vector<ThreadQueue> threadQueue = mThreadQueue;
         mThreadQueue.clear();

         int count = (int)threadQueue.size();

         // Continue immediate mode if we have more items queued up.
         if (count > 0)
         {
            continued |= mImmediateMode;
         }

         for (int index = 0; index < count; index++)
         {
            ThreadQueue& queue = threadQueue[index];

            if (queue.isStack)
            {
               PushStack(queue.node, queue.input);
            }
            else
            {
               BeginThread(queue.node, queue.input);
            }
         }

         // If the current node is not latent, then
         // we can clear the node referenced by this
         // thread.
         if (stackIndex < (int)data.stack.size() &&
             data.stack[stackIndex].finished)
         {
            Node* node = data.stack[stackIndex].node.get();

            if (data.stack[stackIndex].data)
            {
               std::string nodeName = node->GetTypeName();
               if (!node->GetName().empty())
               {
                  nodeName += std::string(": ") + node->GetName();
               }

               LOG_ERROR("Director: Custom thread stack data pointer was not de-allocated after processing node " + nodeName);
            }

            data.stack[stackIndex].node = NULL;
         }
      }

      if (mNotifier.valid())
      {
         // Go through all other stack items and make sure our notifier
         // keeps these nodes glowing.
         for (int index = 0; index < (int)data.stack.size(); ++index)
         {
            if (data.stack[index].node.valid() &&
               !data.stack[index].finished &&
               !data.stack[index].first)
            {
               mNotifier->OnNodeExecution(data.stack[index].node.get(),
                  "", std::vector<std::string>());

               if (IsDebugging())
               {
                  mNotifier->BreakNode(data.stack[index].node.get());
               }
            }
         }
      }

      // If we did not continue the current stack, and we don't have any more
      // sub-threads in this stack.
      if (stackIndex < (int)data.stack.size() &&
         !data.stack[stackIndex].node &&
         data.stack[stackIndex].subThreads.empty())
      {
         // Pop this stack from the list.
         data.stack.erase(data.stack.begin() + stackIndex);
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

            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ALWAYS, "dtDirector::Director::UpdateThread", 531, message);
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
   std::string Director::GetValueKey(ValueNode* value)
   {
      if (!value || !value->GetProperty())
      {
         return "";
      }

      std::string keyName = value->GetName() + " [";
      keyName += value->GetType().GetFullName() + "]";
      return keyName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::RegisterGlobalValue(ValueNode* value)
   {
      if (!value || !value->GetProperty())
      {
         return false;
      }

      std::string keyName = GetValueKey(value);
      std::map<std::string, std::vector<ValueNode*> >::iterator iter = mGlobalValues.find(keyName);
      if (iter == mGlobalValues.end())
      {
         std::vector<ValueNode*> nodes;
         nodes.push_back(value);
         mGlobalValues[keyName] = nodes;
         return true;
      }
      else
      {
         std::vector<ValueNode*>& nodes = mGlobalValues[keyName];

         int count = (int)nodes.size();
         if (count > 0)
         {
            // If we are registering a value node to a global value that
            // already exists, make sure it is immediately updated with
            // the current global value.
            ValueNode* first = nodes[0];
            if (first)
            {
               mApplyingGlobalValue = true;
               value->SetFormattedValue(first->GetFormattedValue());
               value->SetFormattedInitialValue(first->GetFormattedInitialValue());
               mApplyingGlobalValue = false;
            }

            for (int index = 0; index < count; ++index)
            {
               if (value == nodes[index])
               {
                  return false;
               }
            }
         }

         nodes.push_back(value);
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::UnRegisterGlobalValue(ValueNode* value)
   {
      if (!value || !value->GetProperty())
      {
         return false;
      }

      std::string keyName = GetValueKey(value);
      std::map<std::string, std::vector<ValueNode*> >::iterator iter = mGlobalValues.find(keyName);
      if (iter == mGlobalValues.end())
      {
         return false;
      }
      else
      {
         std::vector<ValueNode*>& nodes = mGlobalValues[keyName];
         int count = (int)nodes.size();
         for (int index = 0; index < count; ++index)
         {
            if (value == nodes[index])
            {
               nodes.erase(nodes.begin() + index);
               return true;
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::OnValueChanged(ValueNode* value)
   {
      if (!value)
      {
         return;
      }

      if (GetNotifier())
      {
         GetNotifier()->OnValueChanged(value);
      }

      // If this value is global, apply the new value to all other global
      // value nodes that reference the same global value.
      if (value->GetGlobal() && value->GetProperty() && !mApplyingGlobalValue)
      {
         mApplyingGlobalValue = true;

         std::string newVal = value->GetFormattedValue();

         std::string keyName = GetValueKey(value);
         std::map<std::string, std::vector<ValueNode*> >::iterator iter = mGlobalValues.find(keyName);
         if (iter != mGlobalValues.end())
         {
            std::vector<ValueNode*>& nodes = mGlobalValues[keyName];
            int count = (int)nodes.size();
            for (int index = 0; index < count; ++index)
            {
               ValueNode* node = nodes[index];
               if (node && node != value)
               {
                  node->SetFormattedValue(newVal);
               }
            }
         }

         mApplyingGlobalValue = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::OnInitialValueChanged(ValueNode* value)
   {
      if (!value)
      {
         return;
      }

      // If this value is global, apply the new value to all other global
      // value nodes that reference the same global value.
      if (value->GetGlobal() && value->GetProperty() && !mApplyingGlobalValue)
      {
         mApplyingGlobalValue = true;

         std::string newVal = value->GetFormattedInitialValue();

         std::string keyName = GetValueKey(value);
         std::map<std::string, std::vector<ValueNode*> >::iterator iter = mGlobalValues.find(keyName);
         if (iter != mGlobalValues.end())
         {
            std::vector<ValueNode*>& nodes = mGlobalValues[keyName];
            int count = (int)nodes.size();
            for (int index = 0; index < count; ++index)
            {
               ValueNode* node = nodes[index];
               if (node && node != value)
               {
                  node->SetFormattedInitialValue(newVal);
               }
            }
         }

         mApplyingGlobalValue = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::MasterListAddNode(Node* node, int index)
   {
      if (!node)
      {
         return false;
      }

      if (index <= -1)
      {
         // Make more room in the master node list if we need to.
         if (mMasterNodeFreeIndex == -1)
         {
            MasterNodeListData data;
            data.index = (int)mMasterNodeList.size();
            data.node = node;
            data.next = -1;

            node->mID.index = data.index;

            mMasterNodeList.push_back(data);
            return true;
         }
         // If we have any free nodes available, use it.
         else if (mMasterNodeFreeIndex > -1 && mMasterNodeFreeIndex < (int)mMasterNodeList.size())
         {
            int nextFree = mMasterNodeList[mMasterNodeFreeIndex].next;

            mMasterNodeList[mMasterNodeFreeIndex].node = node;
            mMasterNodeList[mMasterNodeFreeIndex].next = -1;

            node->mID.index = mMasterNodeFreeIndex;

            // Iterate to the next available free node.
            mMasterNodeFreeIndex = nextFree;

            return true;
         }
      }
      else
      {
         // If the desired index is beyond of the list size, expand the list.
         while (index >= (int)mMasterNodeList.size())
         {
            MasterNodeListData data;
            data.index = (int)mMasterNodeList.size();
            data.node = NULL;
            data.next = mMasterNodeFreeIndex;

            mMasterNodeList.push_back(data);

            mMasterNodeFreeIndex = data.index;
         }

         // If the desired index is within our current master list size...
         if (index < (int)mMasterNodeList.size())
         {
            // If the desired index is being used, move the used node somewhere else.
            if (mMasterNodeList[index].node)
            {
               Node* movingNode = mMasterNodeList[index].node;
               int moveToIndex = mMasterNodeFreeIndex;
               if (moveToIndex == -1)
               {
                  moveToIndex = (int)mMasterNodeList.size();
               }

               // Remove this node and re-add it to another index position.
               MasterListRemoveNode(movingNode);
               MasterListAddNode(movingNode, moveToIndex);
            }

            // If the desired index is free, then place this node at that position.
            if (!mMasterNodeList[index].node)
            {
               mMasterNodeList[index].node = node;
               node->mID.index = index;

               // Make sure we remove this index from the free list.
               int lastFreeIndex = -1;
               int freeIndex = mMasterNodeFreeIndex;
               while (freeIndex > -1)
               {
                  if (freeIndex == index)
                  {
                     if (lastFreeIndex > -1)
                     {
                        mMasterNodeList[lastFreeIndex].next = mMasterNodeList[index].next;
                     }
                     else if (mMasterNodeFreeIndex == index)
                     {
                        mMasterNodeFreeIndex = mMasterNodeList[mMasterNodeFreeIndex].next;
                     }

                     mMasterNodeList[index].next = -1;
                     break;
                  }
                  lastFreeIndex = freeIndex;
                  freeIndex = mMasterNodeList[freeIndex].next;
               }
               return true;
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::MasterListRemoveNode(Node* node)
   {
      if (!node)
      {
         return false;
      }

      int index = node->mID.index;

      if (index > -1 && index < (int)mMasterNodeList.size() &&
         mMasterNodeList[index].node == node)
      {
         mMasterNodeList[index].node = NULL;
         mMasterNodeList[index].next = mMasterNodeFreeIndex;
         mMasterNodeFreeIndex = index;

         node->mID.index = -1;
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::MasterListAddGraph(DirectorGraph* graph, int index)
   {
      if (!graph)
      {
         return false;
      }

      if (index <= -1)
      {
         // Make more room in the master graph list if we need to.
         if (mMasterGraphFreeIndex == -1)
         {
            MasterGraphListData data;
            data.index = (int)mMasterGraphList.size();
            data.graph = graph;
            data.next = -1;

            graph->mID.index = data.index;

            mMasterGraphList.push_back(data);
            return true;
         }
         // If we have any free graphs available, use it.
         else if (mMasterGraphFreeIndex > -1 && mMasterGraphFreeIndex < (int)mMasterGraphList.size())
         {
            int nextFree = mMasterGraphList[mMasterGraphFreeIndex].next;

            mMasterGraphList[mMasterGraphFreeIndex].graph = graph;
            mMasterGraphList[mMasterGraphFreeIndex].next  = -1;

            graph->mID.index = mMasterGraphFreeIndex;

            // Iterate to the next available free graph.
            mMasterGraphFreeIndex = nextFree;

            return true;
         }
      }
      else
      {
         // If the desired index is beyond of the list size, expand the list.
         while (index >= (int)mMasterGraphList.size())
         {
            MasterGraphListData data;
            data.index = (int)mMasterGraphList.size();
            data.graph = NULL;
            data.next = mMasterGraphFreeIndex;

            mMasterGraphList.push_back(data);

            mMasterGraphFreeIndex = data.index;
         }

         // If the desired index is within our current master list size...
         if (index < (int)mMasterGraphList.size())
         {
            // If the desired index is being used, move the used graph somewhere else.
            if (mMasterGraphList[index].graph)
            {
               DirectorGraph* movingGraph = mMasterGraphList[index].graph;
               int moveToIndex = mMasterGraphFreeIndex;
               if (moveToIndex == -1)
               {
                  moveToIndex = (int)mMasterGraphList.size();
               }

               // Remove this node and re-add it to another index position.
               MasterListRemoveGraph(movingGraph);
               MasterListAddGraph(movingGraph, moveToIndex);
            }

            // If the desired index is already free, then place this node at that position.
            if (!mMasterGraphList[index].graph)
            {
               mMasterGraphList[index].graph = graph;
               graph->mID.index = index;

               // Make sure we remove this index from the free list.
               int lastFreeIndex = -1;
               int freeIndex = mMasterGraphFreeIndex;
               while (freeIndex > -1)
               {
                  if (freeIndex == index)
                  {
                     if (lastFreeIndex > -1)
                     {
                        mMasterGraphList[lastFreeIndex].next = mMasterGraphList[index].next;
                     }
                     else if (mMasterGraphFreeIndex == index)
                     {
                        mMasterGraphFreeIndex = mMasterGraphList[mMasterGraphFreeIndex].next;
                     }

                     mMasterGraphList[index].next = -1;
                     break;
                  }
                  lastFreeIndex = freeIndex;
                  freeIndex = mMasterGraphList[freeIndex].next;
               }
               return true;
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::MasterListRemoveGraph(DirectorGraph* graph)
   {
      if (!graph)
      {
         return false;
      }

      int index = graph->mID.index;

      if (index > -1 && index < (int)mMasterGraphList.size() &&
         mMasterGraphList[index].graph == graph)
      {
         mMasterGraphList[index].graph = NULL;
         mMasterGraphList[index].next  = mMasterGraphFreeIndex;
         mMasterGraphFreeIndex = index;

         graph->mID.index = -1;
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::GetThreadState(std::vector<Director::StateThreadData>& threads, const ThreadData& thread) const
   {
      StateThreadData data;
      data.id = thread.id;

      int count = (int)thread.stack.size();
      for (int index = 0; index < count; ++index)
      {
         const StackData& stack = thread.stack[index];

         StateStackData stackData;
         if (stack.node.valid())
         {
            stackData.id = stack.node->GetID();
         }
         else
         {
            stackData.id.clear();
         }
         stackData.index = stack.index;
         stackData.finished = stack.finished;

         int subCount = (int)stack.subThreads.size();
         for (int subIndex = 0; subIndex < subCount; ++subIndex)
         {
            GetThreadState(stackData.subThreads, stack.subThreads[subIndex]);
         }

         data.stack.push_back(stackData);
      }

      threads.push_back(data);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::GetValueState(std::vector<StateValueData>& values, const Director* child) const
   {
      if (!child || !child->GetGraphRoot())
      {
         return;
      }

      GetValueState(values, child->GetGraphRoot());

      int count = (int)child->mChildren.size();
      for (int index = 0; index < count; ++index)
      {
         Director* subChild = child->mChildren[index].get();
         if (subChild)
         {
            GetValueState(values, subChild);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::GetValueState(std::vector<StateValueData>& values, const DirectorGraph* graph) const
   {
      const std::vector<dtCore::RefPtr<ValueNode> >& valueNodes = graph->GetValueNodes();
      int count = (int)valueNodes.size();
      for (int index = 0; index < count; ++index)
      {
         ValueNode* node = valueNodes[index];
         // We want to ignore value link and reference nodes because
         // we don't actually want to set those values.
         if (node && node->GetType().GetFullName() != "Core.Reference" &&
            node->GetType().GetNodeType() != dtDirector::NodeType::LINK_NODE &&
            node->GetType().GetNodeType() != dtDirector::NodeType::MUTATOR_NODE)
         {
            StateValueData data;
            data.id = node->GetID();
            data.value = node->GetFormattedValue();
            values.push_back(data);
         }
      }

      count = (int)graph->GetSubGraphs().size();
      for (int index = 0; index < count; ++index)
      {
         const DirectorGraph* subGraph = graph->GetSubGraphs()[index];
         if (subGraph)
         {
            GetValueState(values, subGraph);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::RestoreThreadState(const StateThreadData& threadState, std::vector<ThreadData>& threads)
   {
      ThreadData thread;
      thread.id = threadState.id;

      int count = (int)threadState.stack.size();
      for (int index = 0; index < count; ++index)
      {
         const StateStackData& stackData = threadState.stack[index];

         StackData stack;
         stack.node = RecurseFindNode(stackData.id, this);
         stack.index = stackData.index;
         stack.first = true;
         stack.finished = stackData.finished;
         stack.currentThread = -1;
         int stackCount = (int)stackData.subThreads.size();
         for (int stackIndex = 0; stackIndex < stackCount; ++stackIndex)
         {
            RestoreThreadState(stackData.subThreads[stackIndex], stack.subThreads);
         }

         thread.stack.push_back(stack);
      }

      threads.push_back(thread);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Node* Director::RecurseFindNode(const ID& id, Director* script)
   {
      if (!script || id.id.ToString().empty())
      {
         return NULL;
      }

      Node* node = script->GetNode(id);
      if (node)
      {
         return node;
      }

      int count = (int)script->mChildren.size();
      for (int index = 0; index < count; ++index)
      {
         Director* child = script->mChildren[index].get();
         if (child)
         {
            node = RecurseFindNode(id, child);
            if (node)
            {
               return node;
            }
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Director::RecurseImportScriptGraphs(DirectorGraph* src, DirectorGraph* dst)
   {
      if (!src || !dst)
      {
         return;
      }

      int count = (int)src->GetSubGraphs().size();
      for (int index = 0; index < count; ++index)
      {
         DirectorGraph* subSrc = src->GetSubGraphs()[index];
         if (subSrc)
         {
            ID id;
            id.id = subSrc->GetID().id;

            // If a proxy for this graph doesn't already exist, create it.
            DirectorGraph* subDst = GetGraph(id);
            if (!subDst)
            {
               subDst = dst->AddGraph();
               if (subDst)
               {
                  subDst->SetImported(true);
                  subDst->SetReadOnly(true);
                  subDst->SetID(subSrc->GetID().id);

                  subDst->CopyPropertiesFrom(*subSrc);
               }
            }

            // Now recurse its children graphs.
            if (subDst)
            {
               RecurseImportScriptGraphs(subSrc, subDst);
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Director::RecurseRemoveUnusedImportedGraphs(DirectorGraph* graph)
   {
      if (!graph)
      {
         return false;
      }

      bool result = false;
      int count = (int)graph->GetSubGraphs().size();
      for (int index = 0; index < count; ++index)
      {
         // Start by recursing to the last children in the tree.
         result |= RecurseRemoveUnusedImportedGraphs(graph->GetSubGraphs()[index]);
      }

      // If this graph has no children, or the children graphs were removed...
      if (count == 0 || result)
      {
         // Check if a copy of this graph exists in the main script.
         ID id = graph->GetID();
         id.index = -1;

         DirectorGraph* mainGraph = GetGraph(id);

         // If we don't find a graph, return true so previous recursions know
         // it is safe to remove their graphs.
         if (!mainGraph)
         {
            return true;
         }
         // If we have a copy of this graph, then check if it is empty.
         else if (mainGraph &&
            mainGraph->GetEventNodes().empty() &&
            mainGraph->GetActionNodes().empty() &&
            mainGraph->GetValueNodes().empty() &&
            mainGraph->GetSubGraphs().empty())
         {
            // If the graph is empty, we want to remove it and then return true
            // so the previous recursions know it is safe to remove their graphs.
            DeleteGraph(mainGraph->GetID());
            return true;
         }

         return false;
      }
      return true;
   }
}
