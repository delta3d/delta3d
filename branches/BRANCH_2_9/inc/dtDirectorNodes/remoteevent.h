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

#ifndef DIRECTOR_REMOTE_EVENT_NODE
#define DIRECTOR_REMOTE_EVENT_NODE

#include <dtDirector/eventnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

#include <dtCore/stringactorproperty.h>

namespace dtDirector
{
   /**
    * This is the base class for all event nodes.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class NODE_LIBRARY_EXPORT RemoteEvent : public EventNode
   {
      class ParamData;

   public:

      /**
       * Constructs the Node.
       */
      RemoteEvent();

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
       * Accessors for the event name.
       */
      void SetEventName(const std::string& eventName);
      const std::string& GetEventName() const;

      /**
       * Accessors for the parameter list property.
       */
      void SetParameterIndex(int index);
      void SetParameter(const ParamData& value);
      ParamData GetParameter() const;
      ParamData GetDefaultParameter() const;

      void SetParamName(const std::string& value);
      std::string GetParamName() const;

      void SetParamValue(const std::string& value);
      std::string GetParamValue() const;

      void SetParameterList(const std::vector<ParamData>& value);
      std::vector<ParamData> GetParameterList() const;

   protected:
      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~RemoteEvent();

   private:
      class ParamData
      {
      public:
         ParamData(int index)
         {
            name = "Param " + dtUtil::ToString<int>(index);

            displayProp = NULL;
         }
         ~ParamData(){}

         void SetValue(const std::string& inValue)
         {
            value = inValue;
         }

         const std::string& GetValue() const
         {
            return value;
         }

         dtCore::RefPtr<dtCore::ActorProperty> displayProp;
         std::string name;
         std::string value;
      };

      void UpdateParameterLinks();

      std::string mEventName;

      int mOrignalValueCount;
      std::vector<ParamData> mParameterList;
      int mParameterIndex;
   };
}

#endif
