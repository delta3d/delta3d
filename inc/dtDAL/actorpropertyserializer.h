/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * David Guthrie
 * Extracted actor property functionality from mapxml by Jeff P. Houde
 */

#ifndef DELTA_ACTOR_PROPERTY_SERIALIZER
#define DELTA_ACTOR_PROPERTY_SERIALIZER

#include <dtCore/uniqueid.h>

#include <dtDAL/export.h>
#include <dtDAL/basexml.h>
#include <dtDAL/basexmlhandler.h>
#include <dtDAL/namedgroupparameter.h>
#include <dtDAL/propertycontainer.h>

#include <stack>

namespace dtDAL
{
   class Map;
   class AbstractParameter;
   class ActorProperty;
   class DataType;
   class MapContentHandler;
   class ArrayActorPropertyBase;
   class ContainerActorProperty;
   class BasePropertyContainerActorProperty;

   struct SerializerRuntimeData
   {
      SerializerRuntimeData();

      dtCore::RefPtr<PropertyContainer> mPropertyContainer;
      dtCore::RefPtr<ActorProperty> mActorProperty;
      DataType* mActorPropertyType;
      DataType* mParameterTypeToCreate;

      std::string mParameterNameToCreate;
      std::string mDescriptorDisplayName;

      bool mHasDeprecatedProperty;
      bool mInActorProperty;
      bool mInGroupProperty;
      int  mInArrayProperty;     // Since arrays can be nested, we need to keep track of how deep we are.
      int  mInContainerProperty; // Since containers can be nested, we need to keep track of how deep we are.

      std::stack<dtCore::RefPtr<NamedParameter> > mParameterStack;

      void Reset()
      {
         mInActorProperty = false;

         mActorProperty = NULL;
         mActorPropertyType = NULL;

         ClearParameterValues();
      }

      void ClearParameterValues()
      {
         mInGroupProperty = false;
         mInArrayProperty = 0;
         mInContainerProperty = 0;
         while (!mParameterStack.empty())
         {
            mParameterStack.pop();
         }

         mParameterNameToCreate.clear();
         mParameterTypeToCreate = NULL;
         mDescriptorDisplayName.clear();
      }

   };

   /**
    * @class ActorPropertySerializer
    * @brief Writes a map out to an XML file
    */
   class DT_DAL_EXPORT ActorPropertySerializer
   {
   public:

      /**
       * Constructor.
       */
      ActorPropertySerializer(BaseXMLWriter* writer);
      ActorPropertySerializer(BaseXMLHandler* parser);

      /**
       * Destructor.
       */
      virtual ~ActorPropertySerializer();

      /**
       * Sets the map.
       */
      void SetMap(Map* map);

      /// Sets the property container to work on.  This will be set the current one on the internal stack.
      void SetCurrentPropertyContainer(dtDAL::PropertyContainer* pc);

      /**
       * Resets the serializer.
       */
      void Reset();
      
      /**
       * Writes a property.
       */
      void WriteProperty(const ActorProperty& property);

      /**
       * Handler when an element is started.
       *
       * @param[in]  localname  The local name of the element.
       *
       * @return     True if the element belongs to a property.
       */
      bool ElementStarted(const XMLCh* const localname);

      /**
       * Handler when an element is ended.
       *
       * @param[in]  localname  The local name of the element.
       *
       * @return     True if the element belongs to a property.
       */
      bool ElementEnded(const XMLCh* const localname);

      /**
       * Handler to parse the data of an element.
       *
       * @param[in]  chars  The characters of the element data.
       *
       * @return     True if the characters belong to a property.
       */
      bool Characters(BaseXMLHandler::xmlCharString& topEl, const XMLCh* const chars);

      //processes the mGroupProperties multimap to set GroupActorProperties.
      void AssignGroupProperties();

      //processes the mActorLinking multimap to set ActorActorProperties.
      void LinkActors();

      /**
       * Returns whether or not the map had a temporary property in it.
       */
      bool HasDeprecatedProperty() const { return mData.top().mHasDeprecatedProperty; }

   private:

      inline SerializerRuntimeData& Top() { return mData.top(); }

      /**
       * Writes a Vector actor property.
       */
      template <typename VecType>
      void WriteVec(const VecType& vec, char* numberConversionBuffer, const size_t bufferMax);

      /**
       * Writes a color actor property.
       */
      template <typename Type>
      void WriteColorRGBA(const Type& holder, char* numberConversionBuffer, size_t bufferMax);

      /**
       * Writes an array actor property.
       */
      void WriteArray(const ArrayActorPropertyBase& arrayProp, char* numberConversionBuffer, size_t bufferMax);

      /**
       * Writes a container actor property.
       */
      void WriteContainer(const ContainerActorProperty& arrayProp, char* numberConversionBuffer, size_t bufferMax);

      /**
       * Writes a single property container.
       */
      void WritePropertyContainer(const BasePropertyContainerActorProperty& propConProp);

      /**
       * Writes a simple actor property.
       */
      void WriteSimple(const AbstractParameter& holder);

      /**
       * Writes a parameter for a group actor property.
       */
      void WriteParameter(const NamedParameter& param);

      /**
       * Ends an actor property group element.
       */
      void EndActorPropertyGroupElement();

      //Called from characters when the state says we are inside a parameter of a group actor property.
      void ParameterCharacters(BaseXMLHandler::xmlCharString& topEl, const XMLCh* const chars);

      //Creates a named parameter based on the name and type last parsed and pushes it to the top of the stack.
      void CreateAndPushParameter();

      //parses the text data from the xml and stores it in the property.
      void ParsePropertyData(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, dtDAL::DataType*& dataType, dtDAL::ActorProperty* actorProperty);

      //parses the text data from the xml and stores it in the property.
      void ParseParameterData(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue);

      //parses one item out of the xml and stores it in the proper element of the osg Vec#.
      template <typename VecType>
      void ParseVec(BaseXMLHandler::xmlCharString& topEl, const std::string& dataValue, VecType& vec, size_t vecSize);

      void EnterPropertyContainer();

      //parses the data for an array property.
      void ParseArray(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, ArrayActorPropertyBase* arrayProp);

      //parses the data for a container property.
      void ParseContainer(BaseXMLHandler::xmlCharString& topEl, std::string& dataValue, ContainerActorProperty* arrayProp);

      //decides on a property's datatype base on the name of the element.
      DataType* ParsePropertyType(const XMLCh* const localname, bool errorIfNotFound = true);

      // Workaround for properties where empty data would result in the property not being set.
      void NonEmptyDefaultWorkaround();

      void EndActorPropertyParameterElement();

      void EndActorPropertyElement();

      //returns true if a property in the actor is the same as the XML expects and adjusts the value.
      bool IsPropertyCorrectType(dtDAL::DataType*& dataType, dtDAL::ActorProperty* actorProperty);

      dtCore::RefPtr<Map> mMap;

      BaseXMLWriter*  mWriter;
      BaseXMLHandler* mParser;

      // To support nested property containers, we need a stack of runtime data.
      std::stack<SerializerRuntimeData> mData;

      //The data for group parameters is like linked actors, it needs to be set only after the map is done loading
      //so things are not in a bad state.
      std::multimap<dtDAL::PropertyContainer*, std::pair<std::string, dtCore::RefPtr<dtDAL::NamedGroupParameter> > > mGroupParameters;

      //data for actor linking is not completely available until all actors have been created, so it
      //is stored until the end.
      std::multimap<dtDAL::PropertyContainer*, std::pair<std::string, dtCore::UniqueId> > mActorLinking;

   };
}

#endif
