/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011 MOVES Institute
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

#ifndef SPAWN_ACTOR_ACTION_NODE
#define SPAWN_ACTOR_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/actionnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>
#include <dtCore/baseactorobject.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/booleanactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT SpawnActorAction: public ActionNode
   {
   public:

      /**
       * Constructor.
       */
      SpawnActorAction();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       * @param[in]  graph     The graph that owns this node.
       */
      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);

      /**
       * This method is called in init, which instructs the node
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
       * This function queries the proxy with any properties not
       * found in the property list. If a property was previously
       * removed from the proxy, but is still important to load,
       * then this function should return a temporary property of
       * the appropriate type to be used when loading the map.
       *
       * @param[in]  name  The name of the property queried for.
       *
       * @return           A temporary property, or NULL if
       *                   none is needed.
       */
      virtual dtCore::RefPtr<dtCore::ActorProperty> GetDeprecatedProperty(const std::string& name);

      /**
       * Updates the node.
       * @note  Parent implementation will auto activate any trigger
       *        with the "Out" label by default.
       *
       * @param[in]  simDelta     The simulation time step.
       * @param[in]  delta        The real time step.
       * @param[in]  input        The index to the input that is active.
       * @param[in]  firstUpdate  True if this input was just activated,
       *
       * @return     True if the current node should remain active.
       */
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      /**
       * Determines whether a value link on this node can connect
       * to a given value.
       *
       * @param[in]  link   The link.
       * @param[in]  value  The value to connect to.
       *
       * @return     True if a connection can be made.
       */
      virtual bool CanConnectValue(ValueLink* link, ValueNode* value);

      /**
       * This event is called by value nodes that are linked via
       * value links when that value has changed.
       *
       * @param[in]  linkName  The name of the value link that is changing.
       */
      virtual void OnLinkValueChanged(const std::string& linkName);

      /**
       * Updates the template proxy and the properties visible
       * in the current node.
       */
      void UpdateTemplate();

      /**
       * Accessors for property values.
       */
      void SetActorType(const std::string& value);
      std::string GetActorType() const;
      std::vector<std::string> GetActorTypeList() const;
      dtCore::PropertyContainer* GetActorContainer();

      void SetActorName(const std::string& value);
      std::string GetActorName() const;

      void SetGhost(bool value);
      bool GetGhost() const;

      void SetSpawnLocation(const osg::Vec3& value);
      osg::Vec3 GetSpawnLocation() const;

      void SetOutActor(const dtCore::UniqueId& value);
      dtCore::UniqueId GetOutActor();

   protected:

      /**
       * Destructor.
       */
      ~SpawnActorAction();

   private:

      std::string mActorType;
      osg::Vec3   mSpawnLocation;

      dtCore::RefPtr<dtCore::BaseActorObject>      mTemplateActor;
      dtCore::RefPtr<dtCore::BooleanActorProperty> mGhostProp;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // SPAWN_ACTOR_ACTION_NODE
