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
 * Author: Eric R. Heine
 */

#ifndef GET_VECTOR_VALUES_ACTION_NODE
#define GET_VECTOR_VALUES_ACTION_NODE

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/actionnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT GetVectorValuesAction: public ActionNode
   {
   public:

      /**
       * Constructor.
       */
      GetVectorValuesAction();

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
       * Accessors for property values.
       */
      void SetVector(const std::string& value);
      std::string GetVector() const;

      void SetX(const std::string& value);
      std::string GetX() const;

      void SetY(const std::string& value);
      std::string GetY() const;

      void SetZ(const std::string& value);
      std::string GetZ() const;

      void SetW(const std::string& value);
      std::string GetW() const;

   protected:

      /**
       * Destructor.
       */
      ~GetVectorValuesAction();

   private:

      std::string mVector;

      dtCore::RefPtr<dtCore::ActorProperty> mXProp;
      dtCore::RefPtr<dtCore::ActorProperty> mYProp;
      dtCore::RefPtr<dtCore::ActorProperty> mZProp;
      dtCore::RefPtr<dtCore::ActorProperty> mWProp;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // GET_VECTOR_VALUES_ACTION_NODE
