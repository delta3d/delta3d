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

#ifndef DIRECTOR_NODE
#define DIRECTOR_NODE


#include <string>
#include <vector>

#include <dtDirector/export.h>
#include <dtDirector/nodetype.h>

#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>
#include <dtDirector/valuelink.h>

#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>

#include <dtUtil/stringutils.h>

#include <dtDAL/propertycontainer.h>


namespace dtDirector
{
   /**
    * This is the base class for all node objects.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT Node : public dtDAL::PropertyContainer
   {
   public:

      /**
       * Constructs the Node.
       */
      Node();

      /**
       * Initializes the Node.
       *
       * @param[in]  nodeType  The node type.
       */
      virtual void Init(const NodeType& nodeType);

      /**
       * Creates a copy of this node and returns it.  The method uses the
       * node manager to create the clone and then iterates though the
       * current state of this node's property set and copies their values
       * to the newly created clone.
       *
       * @return The cloned node.
       */
      virtual dtCore::RefPtr<Node> Clone();

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
       *
       * @param[in]  simDelta  The simulation time step.
       * @param[in]  delta     The real time step.
       */
      virtual void Update(float simDelta, float delta);

      /**
       * Retrieves the type of this node.
       *
       * @return  The type of this node.
       */
      const NodeType& GetType();

      /**
       * Retrieves the ID of the node.
       *
       * @return  The unique ID.
       */
      const dtCore::UniqueId& GetID() const {return mID;}

      /**
       * Sets the ID of the node.
       *
       * @param[in]  id  The ID.
       */
      const void SetID(const dtCore::UniqueId& id) {mID = id;}

      /**
       * Retrieves the comment for the node.
       *
       * @return  The comment of the node.
       */
      const std::string& GetComment() const;

      /**
       * Sets the comment for the node.
       *
       * @param[in]  comment  The comment.
       */
      void SetComment(const std::string& comment);

      /**
       * Retrieves the disabled status of the node.
       *
       * @return  True if the node is disabled.
       */
      bool GetDisabled() const;

      /**
       * Sets the disabled status of the node.
       *
       * @param[in]  disabled  True to disable.
       */
      void SetDisabled(bool disabled);

      /**
       * Retrieves the total number of values linked to a property.
       *
       * @param[in]  name  The name of the property.
       *
       * @return     The count.
       */
      virtual int GetPropertyCount(const std::string& name = "Value");

      /**
       * Retrieves a property of the given name.  This is overloaded
       * to provide functionality of redirected properties (from the
       * use of ValueLink's).
       *
       * @param[in]  name   The name of the property.
       * @param[in]  index  The property index, in case of multiple linking.
       *
       * @return     A pointer to the property, NULL if none found.
       *
       * @note  All properties used within nodes should be retrieved
       *         via this method instead of directly to ensure that
       *         the desired property is being used.
       */
      virtual dtDAL::ActorProperty* GetProperty(const std::string& name = "Value", int index = 0);

      /**
       * This method is provided for ease of use, it will
       * retrieve a property's value and convert it to the type of
       * your choice.
       *
       * @param[in]  name   The name of the property.
       * @param[in]  index  The property index, in case of multiple linking.
       *
       * @note  All properties used within nodes should be retrieved
       *         via these methods instead of directly to ensure that
       *         the desired property is being used.
       */
      template<typename T>
      T GetPropertyValue(const std::string& name = "Value", int index = 0)
      {
         T result = 0;
         dtDAL::ActorProperty* prop = GetProperty(name, index);
         if (prop)
         {
            std::string val = prop->ToString();
            result = dtUtil::ToType<T>(val);
         }
         return result;
      }

      std::string GetPropertyString(const std::string& name = "Value", int index = 0)
      {
         dtDAL::ActorProperty* prop = GetProperty(name, index);
         if (prop) return prop->ToString();
         return "";
      }

      /**
       * This method is provided for ease of use, it will
       * allow you to set the value of a property.
       *
       * @param[in]  value  The new value to set.
       * @param[in]  name   The name of the property.
       * @param[in]  index  The property index, in case of multiple linking.
       *                     Use -1 to set all linked properties.
       *
       * @note  All properties used within nodes should be retrieved
       *         via these methods instead of directly to ensure that
       *         the desired property is being used.
       */
      template<typename T>
      void SetPropertyValue(T value, const std::string& name = "Value", int index = -1)
      {
         if (index == -1)
         {
            int count = GetPropertyCount(name);
            for (index = 0; index < count; index++)
            {
               dtDAL::ActorProperty* prop = GetProperty(name, index);
               if (prop)
               {
                  std::string val = dtUtil::ToString(value);
                  prop->FromString(val);
               }
            }
         }
         else
         {
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               std::string val = dtUtil::ToString(value);
               prop->FromString(val);
            }
         }
      }

      void SetPropertyString(const std::string& value, const std::string& name = "Value", int index = -1)
      {
         if (index == -1)
         {
            int count = GetPropertyCount(name);
            for (index = 0; index < count; index++)
            {
               dtDAL::ActorProperty* prop = GetProperty(name, index);
               if (prop)
               {
                  prop->FromString(value);
               }
            }
         }
         else
         {
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               prop->FromString(value);
            }
         }
      }

      /**
       * Retrieves the input list.
       */
      std::vector<InputLink>& GetInputs() {return mInputs;}

      /**
       * Retrieves the output list.
       */
      std::vector<OutputLink>& GetOutputs() {return mOutputs;}

      /**
       * Retrieves the value list.
       */
      std::vector<ValueLink>& GetValues() {return mValues;}

      /**
       * Allow access to the NodePluginRegistry.
       */
      friend class NodePluginRegistry;

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~Node();

      /// Links
      std::vector<InputLink>  mInputs;
      std::vector<OutputLink> mOutputs;
      std::vector<ValueLink>  mValues;

   private:

      /**
       * Hidden Copy Constructor.
       *
       * @param[in]  Node  The node to copy from.
       */
      Node(const Node&);

      /**
       *	Hidden Assignment operator.
       *
       * @param[in]  left   This.
       * @param[in]  right  The node to copy from.
       */
      Node& operator=(const Node&);

      /**
       *	Sets the types of the node.
       * This can only be called from the NodeManager.
       *
       * @param[in]  type  The type of the node.
       */
      void SetType(const NodeType& type);


      // Properties.
      dtCore::UniqueId   mID;
      std::string        mComment;

      bool               mDisabled;

      dtCore::RefPtr<const NodeType>   mType;
   };
}

#endif
