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

#ifndef DIRECTOR_DIRECTOR
#define DIRECTOR_DIRECTOR

#include <dtDirector/export.h>
#include <dtDirector/directorgraph.h>
#include <dtDirector/messagegmcomponent.h>
#include <dtDirector/directornotifier.h>
#include <dtDirector/node.h>

#include <dtCore/map.h>

#include <dtGame/gamemanager.h>

#include <dtCore/base.h>
#include <dtCore/observerptr.h>
#include <dtUtil/getsetmacros.h>
#include <dtCore/resourcedescriptor.h>

#include <cstdio>

namespace dtUtil
{
   class Log;
}


/**
 * Provides a node-based hierarchy for script processing.
 */
namespace dtDirector
{
   /**
    * This is the base class for all director scripts.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT Director: public dtCore::PropertyContainer
   {
   private:
      struct ThreadData;
      struct StackData;
      struct StateThreadData;
      struct StateValueData;
   public:

      struct StateData
      {
         std::vector<StateThreadData> threads;
         std::vector<StateValueData>  values;
      };

   public:

      static dtCore::RefPtr<dtCore::ObjectType> DIRECTOR_TYPE;
      /**
       * Constructs the Director.
       */
      Director();


      /*override*/ const dtCore::ObjectType& GetObjectType() const;

      /**
       * Clones an instance of this script.
       */
      dtCore::RefPtr<Director> Clone(Director* parent = NULL);

      /**
       * Initializes the Director.
       *
       * @param[in]  map  The current map.
       */
      virtual void Init(dtGame::GameManager* gm = NULL, dtCore::Map* map = NULL);

      /**
       * Loads our default set of node libraries.
       */
      virtual void LoadDefaultLibraries();

      /**
       * Clears all data in this Director script.
       */
      virtual void Clear();

      /**
       * Accesses the Director Notifier.
       */
      void SetNotifier(DirectorNotifier* notifier);
      DirectorNotifier* GetNotifier() const;

      /**
       * Clears all running threads to stop all current nodes.
       */
      void ClearThreads();

      /**
       * Retrieves whether this script is a cached instance.
       */
      bool IsCachedInstance() const;

      /**
       *   Retrieves whether this script should be visible within Inspector.
       */
      bool IsVisibleInInspector() const;

      void SetVisibleInInspector(bool visible) {mIsVisibleInInspector = visible;}

      /**
       * Retrieves whether the script has started.
       */
      bool HasStarted() const {return mStarted;}

      /**
       * Retrieves whether the script is currently being loaded.
       */
      bool IsLoading() const {return mLoading;}

      /**
       * Retrieves the Director Proxy if it has one.
       *
       * @return  The Director Proxy.
       */
      Director* GetParent() {return mParent.get();}
      const Director* GetParent() const {return mParent.get();}

      /**
       * Sets the Director Proxy.
       *
       * @param[in]  proxy  The Director Proxy.
       */
      void SetParent(Director* parent);

      /**
       * Adds a child director script.
       *
       * @param[in]  child  The child to add.
       */
      virtual void AddChild(Director* child);

      /**
       * Removes a child director script.
       *
       * @param[in]  child  The child to remove.
       */
      void RemoveChild(Director* child);

      /**
       * Retrieves the children list.
       */
      const std::vector<dtCore::ObserverPtr<Director> >& GetChildren() const;

      /**
       *   Imports a script into this one.
       *
       * @param[in]  scriptResource  The resource of the script to import.
       *
       * @return     Returns true on success.
       */
      Director* ImportScript(const std::string& scriptResource);

      /**
       *   Removes a script that was imported.
       *
       * @param[in]  scriptResource  The resource of the imported script to remove.
       *
       * @return     Returns true on success.
       */
      bool RemoveImportedScript(const std::string& scriptResource);

      /**
       *   Retrieves the imported script.
       *
       * @param[in]  scriptResource  The resource of the imported script to retrieve.
       *
       * @return     Returns the script if found.
       */
      Director* GetImportedScript(const std::string& scriptResource);

      /**
       *   Retrieves the list of inherited scripts.
       */
      const std::vector<dtCore::RefPtr<Director> >& GetImportedScriptList() const;

      /**
       * Retrieves the script ID.
       *
       * @return  Script ID.
       */
      const dtCore::UniqueId& GetID() const;

      /**
       * Retrieves the map.
       *
       * @return  The map.
       */
      dtCore::Map* GetMap();

      /**
       * Sets the map.
       *
       * @param[in]  map  The current map.
       */
      void SetMap(dtCore::Map* map);

      /**
       * Retrieves the Messaging GM component.
       *
       * @return  The Messaging GM component.
       */
      dtDirector::MessageGMComponent* GetMessageGMComponent() {return mMessageGMComponent;}

      /**
       * Retrieves the Game Manager.
       *
       * @return  The Game Manager.
       */
      dtGame::GameManager* GetGameManager() {return mGameManager;}

      /**
      * Retrieves the currently loaded script.
      */
      const std::string& GetScriptName() const {return mScriptName;}
      void SetScriptName(const std::string& name) {mScriptName = name;}

      /**
       * Access to the script resource descriptor.
       */
      void SetResource(const dtCore::ResourceDescriptor& resource) {mResource = resource;}
      dtCore::ResourceDescriptor GetResource() const {return mResource;}

      /**
       * Loads a Director script.  An exception will occur on error.
       *
       * @param[in]  scriptFile  The name of the script file to load.
       */
      void LoadScript(const std::string& scriptFile);

      /**
       * Saves a Director script.  An exception will occur on error.
       *
       * @param[in]  scriptFile  The name of the script file to save.
       */
      void SaveScript(const std::string& scriptFile);

      /**
       * Retrieves all missing node types.
       */
      const std::set<std::string>& GetMissingNodeTypes();

      /**
       * Retrieves all missing libraries.
       */
      const std::vector<std::string>& GetMissingLibraries();

      /**
       *   Retrieves all missing imported scripts.
       */
      const std::vector<std::string>& GetMissingImportedScripts();

      /**
       * Retrieves whether any loaded properties were deprecated.
       */
      bool HasDeprecatedProperty() const;

      /**
       * Retrieves the current state of the script.
       */
      StateData GetState() const;

      /**
       * Restores the state of the script.
       */
      void RestoreState(const StateData& state);

      /**
       * This method is called in init, which instructs the director
       * to create its properties.  Methods implementing this should
       * be sure to call their parent class's buildPropertyMap method to
       * ensure all properties in the proxy inheritance hierarchy are
       * correctly added to the property map.
       *
       * @see GetDeprecatedProperty to handle old properties that need
       *       to be removed.
       */
      virtual void BuildPropertyMap();

      /**
       *   Initializes the script when it starts.
       */
      virtual void OnStart();

      /**
       * Updates the Director.
       *
       * @param[in]  simDelta  The simulation time step.
       * @param[in]  delta     The real time step.
       */
      virtual void Update(float simDelta, float delta);

      /**
       * Starts a new update thread.
       *
       * @param[in]  node          The starting node to update.
       * @param[in]  index         The index of the input being activated.
       * @param[in]  reverseQueue  If true, will push the new thread to the front of the queue.
       * @param[in]  immediate     True to attempt to immediately execute the thread.
       *
       * @return     The ID if the new thread created.
       */
      int BeginThread(Node* node, int index, bool reverseQueue = false, bool immediate = true);

      /**
       * Pushes a new item to the thread stack.
       *
       * @param[in]  node   The new stacks starting node.
       * @param[in]  index  The new stacks started input.
       */
      void PushStack(Node* node, int index);

      /**
       * Retrieves whether there are any active threads running.
       *
       * @param[in]  id  The id of the thread (See Director::BeginThread
       *                 and EventNode::Trigger methods to retrieve this
       *                 id value) or -1 for any thread.
       *
       * @return     True if the thread or script is active.
       */
      bool IsRunning(int id = -1);

      /**
       * Accessors for the name of the script.
       */
      void SetName(const std::string& name);
      const std::string& GetName();

      /**
       * Accessors for the description of the script.
       */
      void SetDescription(const std::string& description);
      std::string& GetDescription();

      /**
       * Accessors for the Author of the script.
       */
      void SetAuthor(const std::string& author);
      std::string& GetAuthor();

      /**
       * Accessors for the comment of the script.
       */
      void SetComment(const std::string& comment);
      std::string& GetComment();

      /**
       * Accessors for the copyright information of the script.
       */
      void SetCopyright(const std::string& copyright);
      std::string& GetCopyright();

      /**
       * Accessors for the scripts creation time.
       */
      void SetCreateDateTime(const std::string& time);
      std::string& GetCreateDateTime();

      /**
       * Accessors for the node logging flag.
       */
      void SetNodeLogging(bool enabled);
      bool GetNodeLogging();

      /**
       * Retrieves the script type.
       */
      virtual std::string GetScriptType() const;

      /**
       * Checks whether a given library type is supported by this script.
       *
       * @param[in]  libraryType  The libraries type.
       *
       * @return     True if the library type is supported.
       */
      virtual bool IsLibraryTypeSupported(const std::string& libraryType) const;

      /**
       * Sets the player actor.
       *
       * @param[in]  player  The player.
       */
      void SetPlayer(const dtCore::UniqueId& player);

      /**
       * Retrieves the player.
       */
      dtCore::UniqueId GetPlayer() const;
      dtCore::BaseActorObject* GetPlayerActor() const;

      /**
       * Sets the owner of this script.
       *
       * @param[in]  owner  The script owner.
       */
      void SetScriptOwner(const dtCore::UniqueId& owner);

      /**
       * Retrieves the owner of this script.
       */
      dtCore::UniqueId GetScriptOwner() const;
      dtCore::BaseActorObject* GetScriptOwnerActor() const;

      /**
       *   Sets this script as imported.
       */
      void SetImported(bool imported) {mIsImported = imported;}
      bool IsImported() const {return mIsImported;}

      /**
       * Inserts a node library with the given name at the given position.
       * If a library of the given name is already listed, the version
       * will be updated and the order adjusted to match the iterator.
       *
       * @param[in]  pos      The position to place the library.
       * @param[in]  name     The name of the library
       * @param[in]  version  The version string of the library.
       *
       * @return     True if the library was inserted properly.
       */
      bool InsertLibrary(unsigned pos, const std::string& name, const std::string& version);

      /**
       * This will add the given library to list.  If a library by the
       * given name is already listed, the library will be placed at the
       * end and the version will be update to the one given.
       *
       * @param[in]  name     The name of the library.
       * @param[in]  version  The version string of the library.
       *
       * @return     True if the library was inserted properly.
       */
      bool AddLibrary(const std::string& name, const std::string& version = "");

      /**
       * Removes a library from the list.
       *
       * @param[in]  name  The name of the library to remove.
       *
       * @return     True if an item was actually removed.
       */
      bool RemoveLibrary(const std::string& name);

      /**
       * Retrieves whether a library is listed.
       *
       * @param[in]  name  The name of the library.
       *
       * @return     True if this map loads the given library.
       */
      bool HasLibrary(const std::string& name) const;

      /**
       * Retrieves all node libraries referenced in this script.
       */
      const std::vector<std::string>& GetAllLibraries() const;
      const std::string GetLibraryVersion(const std::string& mName) const;

      /**
       * Retrieves the graph data.
       */
      DirectorGraph* GetGraphRoot();
      const DirectorGraph* GetGraphRoot() const;

      /**
       * Sets the graph root.
       */
      void SetGraphRoot(DirectorGraph* root);

      /**
       * Retrieves a graph of the given id.
       *
       * @param[in]  id  The id of the graph.
       *
       * @return     A pointer to the graph or NULL if not found.
       */
      DirectorGraph* GetGraph(const ID& id, bool includeImportedScripts = false);

      /**
       * Retrieves a node of the given the id.
       *
       * @param[in]  id                      The id of the node.
       * @param[in]  includeImportedScripts  True to find nodes within all imported scripts as well.
       *
       * @return     A pointer to the node or NULL if not found.
       */
      Node* GetNode(const ID& id, bool includeImportedScripts = false);
      const Node* GetNode(const ID& id, bool includeImportedScripts = false) const;

      /**
       * Retrieves a list of nodes that are of a certain type.
       *
       * @param[in]   name                   The type name of the node.
       * @param[in]   category               The type category of the node.
       * @param[out]  outNodes               A list of nodes found.
       * @param[in]   searchImportedScripts  Whether we should look through imported scripts or not.
       */
      void GetNodes(const std::string& name, const std::string& category, std::vector<Node*>& outNodes, bool searchImportedScripts = false);

      /**
       * Retrieves a list of nodes that are of a certain type,
       * and contain a property with a given value.
       *
       * @param[in]   name                   The type name of the node.
       * @param[in]   category               The type category of the node.
       * @param[in]   property               The name of the property to find.
       * @param[in]   value                  The value of the property.
       * @param[out]  outNodes               A list of nodes found.
       * @param[in]   searchImportedScripts  Whether we should look through imported scripts or not.
       */
      void GetNodes(const std::string& name, const std::string& category, const std::string& property, const std::string& value, std::vector<Node*>& outNodes, bool searchImportedScripts = false);

      /**
       * Retrieves a list of all nodes in the Director.
       *
       * @param[out]  outNodes                A list of nodes found.
       * @param[in]   includeImportedScripts  True to include all nodes from imported scripts.
       */
      void GetAllNodes(std::vector<Node*>& outNodes, bool includeImportedScripts = false);

      /**
       * Retrieves a value node with the given name.
       * @note  If there are more than one value with the
       *         same name, only the first one will be returned.
       *
       * @param[in]  name  The name of the value to find.
       *
       * @return     A pointer to the value node, NULL if not found.
       */
      ValueNode* GetValueNode(const std::string& name);

      /**
       * Deletes a graph.
       *
       * @param[in]  graph  The graph to remove.
       *
       * @return     True if the graph was found and removed.
       */
      bool DeleteGraph(const ID& id);

      /**
       * Deletes a node.
       *
       * @param[in]  node  The node to remove.
       *
       * @return     True if the node was found and removed.
       */
      bool DeleteNode(const ID& id);

      /**
       *   Retrieves whether this script has been modified during the loading process.
       */
      bool IsModified() const {return mModified;}

      /**
       * Toggles the debug mode.
       */
      void ToggleDebugEnabled(bool enabled);
      bool IsDebugging() const;

      void StepDebugger();

      /**
       *   Cleans up all ID indexes.
       */
      void CleanIDs();

      /**
       * Sets whether this script is enabled or not.
       *
       * @param[in]  enabled  Whether this director script is enabled or not.
       */
      void SetEnabled(bool enabled);

      /**
       * Checks if this script is enabled.
       *
       * @param[in]  checkRecursively  Whether we should check if all our parent scripts are enabled or not as well
       * @return  Returns whether this script is enabled or not.
       */
      bool IsEnabled(bool checkRecursively = true) const;

   protected:

      /**
       *   Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~Director();

      /**
       * Updates a thread.
       *
       * @param[in]  data       The thread data.
       * @param[in]  simDelta   The simulation time step.
       * @param[in]  delta      The real time step.
       *
       * @return     Returns false if the thread should stop.
       */
      bool UpdateThread(ThreadData& data, float simDelta, float delta);

      /**
       * Cleans up empty threads.
       */
      void CleanThreads();

      /**
       * Recursively cleans up empty threads.
       *
       * @param[in]  data  The stack data to clean.
       */
      void CleanThread(StackData& data);

      /**
       * Processes an updated node.
       *
       * @param[in]  node       The node.
       * @param[in]  first      True if this is the nodes first update.
       * @param[in]  continued  True if this node will continue to update.
       * @param[in]  input      The input index that was fired.
       * @param[in]  outputs    A list of outputs that were activated.
       */
      void ProcessUpdatedNode(Node* node, bool first, bool continued, int input, std::vector<OutputLink*> outputs);

      /**
       * Retrieves the global value key name.
       */
      std::string GetValueKey(ValueNode* value);

      /**
       * Handles the registration of a value node to be global.
       *
       * @return  False if the value node could not be registered.
       */
      bool RegisterGlobalValue(ValueNode* value);
      bool UnRegisterGlobalValue(ValueNode* value);

      /**
       * Event handler when a value has changed.
       */
      void OnValueChanged(ValueNode* value);
      void OnInitialValueChanged(ValueNode* value);

   private:

      /**
       *   Adds or removes a node from the node master list.
       *
       * @param[in]  node   The node to add or remove.
       * @param[in]  index  A desired index to place this node if possible.
       */
      bool MasterListAddNode(Node* node, int index = -1);
      bool MasterListRemoveNode(Node* node);

      /**
       *   Adds or removes a graph from the graph master list.
       *
       * @param[in]  graph  The graph to add or remove.
       */
      bool MasterListAddGraph(DirectorGraph* graph, int index = -1);
      bool MasterListRemoveGraph(DirectorGraph* graph);

      void GetThreadState(std::vector<StateThreadData>& threads, const ThreadData& thread) const;
      void GetValueState(std::vector<StateValueData>& values, const Director* child) const;
      void GetValueState(std::vector<StateValueData>& values, const DirectorGraph* graph) const;

      void RestoreThreadState(const StateThreadData& threadState, std::vector<ThreadData>& threads);
      Node* RecurseFindNode(const ID& id, Director* script);

      void RecurseImportScriptGraphs(DirectorGraph* src, DirectorGraph* dst);
      bool RecurseRemoveUnusedImportedGraphs(DirectorGraph* graph);

      // State Stack Data.
      struct StateStackData
      {
         ID    id;
         int   index;
         bool  finished;

         std::vector<StateThreadData> subThreads;
      };

      // State Thread Data.
      struct StateThreadData
      {
         int id;
         std::vector<StateStackData> stack;
      };

      struct StateValueData
      {
         ID id;
         std::string      value;
      };

      // Thread Queue.
      struct ThreadQueue
      {
         Node* node;
         int   input;
         bool  isStack;
      };

      // Thread stacks.
      struct StackData
      {
         dtCore::ObserverPtr<Node> node;
         int   index;
         bool  first;
         bool  finished;

         void* data;

         std::vector<ThreadData> subThreads;
         int currentThread;
      };

      // Execution threads.
      struct ThreadData
      {
         int                    id;
         std::vector<StackData> stack;
      };

      // Thread Data.
      bool mImmediateMode;

      std::vector<ThreadData> mThreads;
      int mCurrentThread;
      int mThreadID;

      std::vector<ThreadQueue> mThreadQueue;
      bool mQueueingThreads;

      // Other Data.
      static dtCore::UniqueId mPlayer;
      dtCore::UniqueId mScriptOwner;

      static std::map<std::string, std::vector<ValueNode*> > mGlobalValues;
      static bool mApplyingGlobalValue;

      // Core Info.
      dtCore::UniqueId mID;
      std::string mScriptName;
      std::string mDescription;
      std::string mAuthor;
      std::string mCopyright;
      std::string mCreationTime;

      bool mIsVisibleInInspector;

      dtCore::ResourceDescriptor mResource;

      dtCore::RefPtr<dtCore::Map> mMap;
      bool        mModified;
      bool        mStarted;
      bool        mLoading;

      bool        mDebugging;
      bool        mShouldStep;

      std::vector<std::string> mLibraries;
      std::map<std::string, std::string> mLibraryVersionMap;

      dtCore::RefPtr<DirectorGraph> mGraph;

      bool           mLogNodes;

      dtGame::GameManager* mGameManager;
      dtCore::RefPtr<dtDirector::MessageGMComponent> mMessageGMComponent;

      dtCore::ObserverPtr<Director> mParent;
      std::vector<dtCore::ObserverPtr<Director> > mChildren;

      std::vector<dtCore::RefPtr<Director> > mImportedScriptList;

      dtCore::RefPtr<DirectorNotifier> mNotifier;

      std::set<std::string> mMissingNodeTypes;
      std::vector<std::string> mMissingLibraries;
      std::vector<std::string> mMissingImportedScripts;
      bool mHasDeprecatedProperty;
      bool mIsImported;

      struct MasterNodeListData
      {
         int index;
         Node* node;
         int next;
      };

      struct MasterGraphListData
      {
         int index;
         DirectorGraph* graph;
         int next;
      };

      std::vector<MasterNodeListData>  mMasterNodeList;
      std::vector<MasterGraphListData> mMasterGraphList;

      int                              mMasterNodeFreeIndex;
      int                              mMasterGraphFreeIndex;

      bool mEnabled;

      //friend class DirectorGraph;
      friend class Node;
      friend class ValueNode;
      friend class ArrayValueNode;
      friend class DirectorGraph;
      friend class DirectorTypeFactory;

      // Switch to enable/disable this director
      DT_DECLARE_ACCESSOR_INLINE(bool, Active)
   };
   typedef dtCore::RefPtr<Director> DirectorPtr;
}

#endif // DIRECTOR_DIRECTOR
